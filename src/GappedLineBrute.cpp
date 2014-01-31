#include <iostream>

#include "GappedLineBrute.h"

using std::cout;
using std::endl;

namespace acgt {

/* iterator_traits */

GappedLineBrute::iterator_traits::iterator_traits(
      std::list<char> &chars,
      std::list<char>::iterator it)
    : chars_(chars),
      it_(it) { }

char
GappedLineBrute::iterator_traits::operator* () const {
  assert(*it_);
  return *it_;
}

bool
GappedLineBrute::iterator_traits::operator!= (
    const iterator_traits &rhs) const {
  return !(*this == rhs);
}

bool
GappedLineBrute::iterator_traits::operator== (
    const iterator_traits &rhs) const {
  return it_ == rhs.it_;
}

/* iterator */

GappedLineBrute::iterator::iterator(
      GappedLineBrute &line, bool start)
    : iterator_traits(line.chars_, start ?
        (++line.chars_.begin()) : (--line.chars_.end())) {
}

GappedLineBrute::iterator
GappedLineBrute::iterator::operator++ (int) {
  iterator ret = *this;
  ++(*this);
  return ret;
}

GappedLineBrute::iterator
GappedLineBrute::iterator::operator++ () {
  ++it_;
  return *this;
}

void GappedLineBrute::iterator::addGap(size_t size) {
  bool first_gap_ = (*it_ == '.');
  first_gap_ |= (it_ == ++chars_.begin());
  first_gap_ |= (*(--it_) == '.');
  ++it_;
  chars_.insert(it_, size, first_gap_ ? '.' : '-');
  for (size_t j = 0; j < size; ++j, --it_);
}

/* reverse_iterator */

GappedLineBrute::reverse_iterator::reverse_iterator(
      GappedLineBrute &line, bool start)
    : iterator_traits(line.chars_, !start ?
         line.chars_.begin() : (--(--line.chars_.end()))) {
}

GappedLineBrute::reverse_iterator
GappedLineBrute::reverse_iterator::operator++ (int) {
  reverse_iterator ret = *this;
  ++(*this);
  return ret;
}

GappedLineBrute::reverse_iterator
GappedLineBrute::reverse_iterator::operator++ () {
  --it_;
  return *this;
}

void GappedLineBrute::reverse_iterator::addGap(size_t size) {
  bool first_gap_ = (*it_ == '.');
  first_gap_ |= (it_ == (--(--chars_.end())));
  first_gap_ |= (*(++it_) == '.');
  --it_;
  chars_.insert(++it_, size, first_gap_ ? '.' : '-');
  --it_;
}

/* gapped_line_brute */

GappedLineBrute::GappedLineBrute(
      const std::vector<string_t> &words,
      const std::vector<off_t> &offs) {
  size_t size = 2 + (words.size() ? words.back().second + offs.back() : 0);
  chars_.resize(size, '.');
  for (size_t j = 0; j < words.size(); ++j) {
    auto it = (++chars_.begin());
    for (int k = 0; k < offs[j]; ++k) ++it;
    for (size_t k = 0; k < words[j].second; ++k)
      *(it++) = words[j].first[k];
  }
  *chars_.begin() = '@';
  *(--chars_.end()) = '@';
}

GappedLineBrute::iterator GappedLineBrute::begin() {
  return iterator(*this, 1);
}

GappedLineBrute::iterator GappedLineBrute::end() {
  return iterator(*this, 0);
}

GappedLineBrute::reverse_iterator GappedLineBrute::rbegin() {
  return reverse_iterator(*this, 1);
}

GappedLineBrute::reverse_iterator GappedLineBrute::rend() {
  return reverse_iterator(*this, 0);
}

size_t GappedLineBrute::size() const {
  return chars_.size() - 2;
}


} // namespace

