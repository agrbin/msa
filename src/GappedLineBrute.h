#ifndef ACGT_GAPPED_LINE_BRUTE_H
#define ACGT_GAPPED_LINE_BRUTE_H
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>
#include <list>

namespace acgt {

class GappedLineBrute {
 public:
  typedef std::pair<const char*, size_t> string_t;

  class iterator_traits {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef const char value_type;
    typedef ptrdiff_t difference_type;
    typedef const char* pointer;
    typedef const char& reference;
    iterator_traits(
        std::list<char> &chars,
        std::list<char>::iterator it);

    char operator* () const;
    bool operator!= (const iterator_traits &rhs) const;
    bool operator== (const iterator_traits &rhs) const;
   protected:
    std::list<char> &chars_;
    std::list<char>::iterator it_;
  };

  class iterator : public iterator_traits {
   public:
    iterator(GappedLineBrute &line, bool start);
    iterator operator++(int);
    iterator operator++();
    void addGap(size_t size);
  };

  class reverse_iterator : public iterator_traits {
   public:
    reverse_iterator(GappedLineBrute &line, bool start);
    reverse_iterator operator++(int);
    reverse_iterator operator++();
    void addGap(size_t size);
  };

 public:
  friend class GappedLineBrute::iterator;
  friend class GappedLineBrute::reverse_iterator;

  GappedLineBrute(
      const std::vector<string_t>&,
      const std::vector<off_t>&);

  iterator begin();
  iterator end();
  reverse_iterator rbegin();
  reverse_iterator rend();

  size_t size() const;
  inline std::string toString() { // debug only!
    return std::string(begin(), end());
  }

 private:
  std::list<char> chars_;

};

} // namespace
#endif // ACGT_GAPPED_LINE_BRUTE_H
