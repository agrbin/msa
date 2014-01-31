#ifndef ACGT_CONNECTED_STRING_H
#define ACGT_CONNECTED_STRING_H
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>
#include <list>

namespace acgt {

/**
 * This class will iterate over concated strings as it was only one string.
 *
 * vector<pair<const T*, size_t> > strs = {{"aaaaaa", 6}, {"cgtcgt", 6}};
 * ConnectedString s(strs);
 *
 * cout << string(s.begin(), s.end()) << endl;
 *
 * echoes
 *
 * "aaaaaacgtcgt"
 *
 * Iteration is O(1), no additional memory is allocated.
 */
template<class T>
class ConnectedString {
 public:
  typedef std::pair<const T*, size_t> string_t;
  typedef const std::vector<string_t> fragments_t;

  class iterator_traits {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef const T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;

    inline T operator*() const;
    inline bool operator!= (const iterator_traits&) const;
    inline bool operator== (const iterator_traits&) const;

   protected:
    off_t frag_it_;
    const T *str_;
    size_t to_read_;
  };

  class iterator : public iterator_traits {
   public:
    inline iterator operator++();
    inline iterator operator++(int);
    inline off_t offset() const {return offset_;}
    iterator(fragments_t &fragments, size_t size, bool start);
   private:
    off_t offset_;
    void next_word();
    fragments_t &fragments_;
  };

  class reverse_iterator : public iterator_traits {
   public:
    inline reverse_iterator operator++();
    inline reverse_iterator operator++(int);
    inline off_t offset() const {return offset_;}
    reverse_iterator(fragments_t &fragments, size_t size, bool start);
   private:
    off_t offset_;
    void prev_word();
    fragments_t &fragments_;
  };

  friend iterator;
  friend reverse_iterator;

  ConnectedString(fragments_t fragments)
    : fragments_(fragments),
      size_(0) {
    for (const auto& t : fragments) {
      assert(t.first);
      size_ += t.second;
    }
  }

  inline size_t size() const {
    return size_;
  }

  ConnectedString<T>::iterator begin() const;
  ConnectedString<T>::iterator end() const;
  ConnectedString<T>::reverse_iterator rbegin() const;
  ConnectedString<T>::reverse_iterator rend() const;

 private:
  fragments_t fragments_;
  size_t size_;
};

/** traits **/

template<class T>
inline T ConnectedString<T>::iterator_traits::operator*() const {
  return *this->str_;
}

template<class T>
inline typename ConnectedString<T>::iterator
ConnectedString<T>::iterator::operator++(int) {
  iterator it = (*this);
  ++(*this);
  return it;
}

template<class T>
inline typename ConnectedString<T>::reverse_iterator
ConnectedString<T>::reverse_iterator::operator++(int) {
  reverse_iterator it = (*this);
  ++(*this);
  return it;
}

template<class T>
inline typename ConnectedString<T>::iterator
ConnectedString<T>::iterator::operator++() {
  assert(this->to_read_);
  ++this->str_;
  ++offset_;
  --this->to_read_;
  if (!this->to_read_) { // rare
    next_word();
  }
  return *this;
}

template<class T>
inline typename ConnectedString<T>::reverse_iterator
ConnectedString<T>::reverse_iterator::operator++() {
  assert(this->to_read_);
  --this->str_;
  --offset_;
  --this->to_read_;
  if (!this->to_read_) { // rare
    prev_word();
  }
  return *this;
}

template<class T>
inline bool
ConnectedString<T>::iterator_traits::operator!= (
    const iterator_traits &rhs) const {
  return !(*this == rhs);
}

template<class T>
inline bool
ConnectedString<T>::iterator_traits::operator== (
    const iterator_traits &rhs) const {
  return this->str_ == rhs.str_;
}

template<class T>
ConnectedString<T>::iterator::iterator(
      fragments_t &fragments, size_t size, bool start)
    : offset_(start ? 0 : size),
      fragments_(fragments) {
  if (start && fragments_.size()) {
    this->frag_it_ = 0;
    this->to_read_ = fragments_[this->frag_it_].second;
    this->str_ = fragments_[this->frag_it_].first;
    next_word(); // if first word is empty.
  } else {
    this->frag_it_ = fragments.size();
    this->str_ = NULL;
    this->to_read_ = 0;
  }
}

template<class T>
void
ConnectedString<T>::iterator::next_word() {
  while (!this->to_read_ && this->str_) {
    ++this->frag_it_;
    this->to_read_ = this->frag_it_ < (off_t) fragments_.size() ?
      fragments_[this->frag_it_].second : 0;
    this->str_ = this->frag_it_ < (off_t) fragments_.size() ?
      fragments_[this->frag_it_].first : NULL;
  }
}

template<class T>
typename ConnectedString<T>::iterator
ConnectedString<T>::begin() const {
  return iterator(fragments_, size_, true);
}

template<class T>
typename ConnectedString<T>::iterator
ConnectedString<T>::end() const {
  return iterator(fragments_, size_, false);
}

/* ------------ REVERSE! --------------- */

template<class T>
ConnectedString<T>::reverse_iterator::reverse_iterator(
      fragments_t &fragments, size_t size, bool start)
    : offset_(start ? size : 0),
      fragments_(fragments) {
  if (start && fragments_.size()) {
    this->frag_it_ = fragments_.size() - 1;
    this->to_read_ = fragments_[this->frag_it_].second;
    this->str_ = fragments_[this->frag_it_].first + this->to_read_ - 1;
    prev_word(); // if first word is empty.
  } else {
    this->frag_it_ = -1;
    this->str_ = NULL;
    this->to_read_ = 0;
  }
}

template<class T>
void
ConnectedString<T>::reverse_iterator::prev_word() {
  while (!this->to_read_ && this->str_) {
    --this->frag_it_;
    this->to_read_ = this->frag_it_ >= 0 ?
      fragments_[this->frag_it_].second : 0;
    this->str_ = this->frag_it_ >= 0 ?
      fragments_[this->frag_it_].first + this->to_read_ - 1 : NULL;
  }
}

template<class T>
typename ConnectedString<T>::reverse_iterator
ConnectedString<T>::rbegin() const {
  return reverse_iterator(fragments_, size_, true);
}

template<class T>
typename ConnectedString<T>::reverse_iterator
ConnectedString<T>::rend() const {
  return reverse_iterator(fragments_, size_, false);
}

} // namespace
#endif // ACGT_CONNECTED_STRING_H
