#ifndef ACGT_GAPPED_LINE_H
#define ACGT_GAPPED_LINE_H
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>
#include <list>

#include "ConnectedString.hpp"
#include "Base.h"

namespace acgt {

/**
 * All chars here are uint8_t, encoded like in Base.h. We will use '.' and '-'
 * encodings.
 *
 * GappedLine is structure that will allow you to iterate over multiple
 * strings with gaps and add new gaps allong the way. Original gaps will be
 * encoded with '.' in output, and new gaps with '-'. For example, if you
 * have two string "aaaaaa" on offset 3 and "cgtcgt" on offset 10, your line
 * looks like this:
 *
 * vector<pair<const uint8_t*, size_t> > strs = {{"aaaaaa", 6}, {"cgtcgt", 6}};
 * vector<off_t> offsets = {3, 10};
 * GappedLine gl(strs, offsets);
 *
 * gl.toString() == "...aaaaaa.cgtcgt"
 *
 * You can iterate over the line:
 *
 * auto it = gl.begin();
 * for (int j = 0; j < 5; ++j) ++it;
 *
 * and you can add gap:
 *
 * it.addGap(5);
 *
 * now the line looks like:
 *
 * "...aa-----aaaa.cgtcgt"
 *
 * Also, you can iterate in reverse direction (rbegin(), rend()), and all
 * operations are O(1).
 * This class will not take ownership over all the uint8_t*.
 */
class GappedLine {
 public:
  struct gap {
    off_t offset;
    size_t length;
    bool added;
    gap (off_t offset, size_t length, bool added = false)
      : offset(offset), length(length), added(added) { }
  };
  typedef std::list<gap> gaps_t;
  typedef std::pair<const uint8_t*, size_t> string_t;
  typedef ConnectedString<uint8_t> ConnectedSeq;

  class iterator_traits {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef const uint8_t value_type;
    typedef ptrdiff_t difference_type;
    typedef const uint8_t* pointer;
    typedef const uint8_t& reference;

    iterator_traits(GappedLine &line,
                    bool in_gap, size_t gaps_to_read);
    inline bool inGap() const;

   protected:
    GappedLine &line_;
    bool in_gap_;
    size_t gaps_to_read_;
  };

  class iterator : public iterator_traits {
   public:
    iterator(GappedLine &line, bool start);

    // can I read one more please?
    inline iterator operator++(int);
    inline iterator operator++();
    inline uint8_t operator* () const;
    inline bool operator!= (const iterator &rhs) const;
    inline bool operator== (const iterator &rhs) const;
    void addGap(size_t size);

   private:
    void emplaceGap(size_t size);
    ConnectedSeq::iterator wit_;
    GappedLine::gaps_t::iterator git_;
    const GappedLine::gaps_t::iterator gaps_start_;
  };

  // implementation of class reverse_iterator is identical to
  // iterator class except of emplaceGap method. that's why
  // we didn't bother to template this out.
  // dynamic polymorphism is avoided because of perf.
  class reverse_iterator : public iterator_traits {
   public:
    reverse_iterator(GappedLine &line, bool start);

    // can I read one more please?
    inline reverse_iterator operator++(int);
    inline reverse_iterator operator++();
    inline uint8_t operator* () const;
    inline bool operator!= (const reverse_iterator &rhs) const;
    inline bool operator== (const reverse_iterator &rhs) const;
    void addGap(size_t size);

   private:
    void emplaceGap(size_t size);
    ConnectedSeq::reverse_iterator wit_;
    GappedLine::gaps_t::reverse_iterator git_;
    const GappedLine::gaps_t::reverse_iterator gaps_start_;
  };

  friend class GappedLine::iterator;
  friend class GappedLine::reverse_iterator;

  GappedLine(
      const std::vector<string_t>,
      const std::vector<off_t>);

  iterator begin();
  iterator end();
  reverse_iterator rbegin();
  reverse_iterator rend();

  size_t size() const;
  void debug() const;

 private:
  // sum of lengths in words_ and gaps_
  size_t size_;
  ConnectedSeq words_;
  gaps_t gaps_;
};

inline bool GappedLine::iterator_traits::inGap() const {
  return in_gap_;
}

inline GappedLine::iterator
GappedLine::iterator::operator++(int) {
  iterator ret = *this;
  ++(*this);
  return ret;
}

inline GappedLine::reverse_iterator
GappedLine::reverse_iterator::operator++(int) {
  reverse_iterator ret = *this;
  ++(*this);
  return ret;
}

inline GappedLine::iterator
GappedLine::iterator::operator++() {
  if (in_gap_) {
    --gaps_to_read_;
    if (!gaps_to_read_) { // rare
      in_gap_ = false;
      ++git_;
      // ovdje git_ mozda postaje nevalidan,
      // ali onda je i i sigurno kraj jer znamo da je
      // jedan gap na kraju.
    }
  } else {
    ++wit_;
    // ovdje je git_ sigurno validan jer na kraju imamo
    // gap.
    if (git_->offset == wit_.offset()) {
      in_gap_ = true;
      gaps_to_read_ = git_->length;
      if (!gaps_to_read_) {
        ++ git_;
        in_gap_ = false;
      }
    }
  }
  return *this;
}

// only difference is ++git_ => --+git_
inline GappedLine::reverse_iterator
GappedLine::reverse_iterator::operator++() {
  if (in_gap_) {
    --gaps_to_read_;
    if (!gaps_to_read_) { // rare
      in_gap_ = false;
      ++git_;
      // ovdje git_ mozda postaje nevalidan,
      // ali onda je i i sigurno kraj jer znamo da je
      // jedan gap na kraju/na pocetku
    }
  } else {
    ++wit_;
    // ovdje je git_ sigurno validan jer na kraju imamo
    // gap.
    if (git_->offset == wit_.offset()) {
      in_gap_ = true;
      gaps_to_read_ = git_->length;
      if (!gaps_to_read_) {
        ++ git_;
        in_gap_ = false;
      }
    }
  }
  return *this;
}

inline uint8_t
GappedLine::iterator::operator* () const {
  return in_gap_ ? (git_->added ? Base::IN_GAP : Base::OUT_GAP) : *wit_;
}

inline uint8_t
GappedLine::reverse_iterator::operator* () const {
  return in_gap_ ? (git_->added ? Base::IN_GAP : Base::OUT_GAP) : *wit_;
}

inline bool
GappedLine::iterator::operator!= (const GappedLine::iterator &rhs) const {
  return !(*this == rhs);
}

inline bool
GappedLine::reverse_iterator::operator!= (
    const GappedLine::reverse_iterator &rhs) const {
  return !(*this == rhs);
}

inline bool
GappedLine::iterator::operator== (const iterator &rhs) const {
  return wit_ == rhs.wit_ &&
    in_gap_ == rhs.in_gap_ &&
    gaps_to_read_ == rhs.gaps_to_read_;
}

inline bool
GappedLine::reverse_iterator::operator== (const reverse_iterator &rhs) const {
  return wit_ == rhs.wit_ &&
    in_gap_ == rhs.in_gap_ &&
    gaps_to_read_ == rhs.gaps_to_read_;
}

} // namespace
#endif // ACGT_GAPPED_LINE_H
