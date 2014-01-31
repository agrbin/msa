#include "GappedLine.hpp"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace acgt {

using std::vector;
using std::pair;
using std::cout;
using std::endl;
using std::sort;

GappedLine::GappedLine(
      const vector<string_t> fragments,
      const vector<off_t> offsets) :
    size_(0),
    words_(fragments) {
  assert(offsets.size() == fragments.size());
  // sort fragments by offsets
  vector<size_t> ord(offsets.size());
  for (size_t i = 0; i < ord.size(); ++i) ord[i] = i;
  sort(ord.begin(), ord.end(), [&offsets] (size_t a, size_t b) {
    return offsets[a] < offsets[b];
  });
  // create gaps
  size_t cum_offset = 0;
  for (size_t j = 0; j < fragments.size(); ++j) {
    gaps_.emplace_back(cum_offset, offsets[ord[j]] - size_);
    cum_offset += fragments[ord[j]].second;
    size_ = offsets[ord[j]] + fragments[ord[j]].second;
  }
  gaps_.emplace_back(cum_offset, 0);
  // merge all gaps_ pointing to same position.
  gaps_t::iterator last_it;
  for (auto it = gaps_.begin(); it != gaps_.end(); ++it) {
    if (it != gaps_.begin()) {
      assert(last_it->offset <= it->offset);
      if (last_it->offset == it->offset) {
        it->length += last_it->length;
        gaps_.erase(last_it);
      }
    }
    last_it = it;
  }
}

void GappedLine::debug() const {
  cout << "size: " << size_ << endl;
  for (auto &t : gaps_) {
    cout << t.offset << ' ' << t.length << endl;
  }
  cout << endl;
}

size_t GappedLine::size() const {
  return size_;
}

GappedLine::iterator GappedLine::begin() {
  return iterator(*this, true);
}

GappedLine::iterator GappedLine::end() {
  return iterator(*this, false);
}

GappedLine::reverse_iterator GappedLine::rbegin() {
  return reverse_iterator(*this, true);
}

GappedLine::reverse_iterator GappedLine::rend() {
  return reverse_iterator(*this, false);
}

GappedLine::iterator_traits::iterator_traits(
    GappedLine &line, bool in_gap, size_t gaps_to_read) :
  line_(line),
  in_gap_(in_gap),
  gaps_to_read_(gaps_to_read)
{ }

GappedLine::iterator::iterator(GappedLine &line, bool start) :
  iterator_traits(
    line,
    start ? true : false,
    start ? line.gaps_.front().length : 0),
  wit_(start ? line.words_.begin() : line.words_.end()),
  git_(start ? line.gaps_.begin() : line.gaps_.end()),
  gaps_start_(line.gaps_.begin())
{
  if (start && !gaps_to_read_) {
    in_gap_ = false;
    ++git_;
  }
}

GappedLine::reverse_iterator::reverse_iterator(GappedLine &line, bool start) :
  iterator_traits(
    line,
    start ? true : false,
    start ? line.gaps_.back().length : 0 ),
  wit_(start ? line.words_.rbegin() : line.words_.rend()),
  git_(start ? line.gaps_.rbegin() : line.gaps_.rend()),
  gaps_start_(line.gaps_.rbegin())
{
  if (start && !gaps_to_read_) {
    in_gap_ = false;
    ++git_;
  }
}

void GappedLine::iterator::addGap(size_t size) {
  if (!size) {
    return;
  }
  line_.size_ += size;
  // if currently in gap, just extend that gap.
  if (in_gap_) {
    gaps_to_read_ += size;
    git_->length += size;
    if (!gaps_to_read_) {
      ++ git_;
      in_gap_ = false;
    }
  } else {
    bool dont_insert = false;
    // otherwise we are definetly in the gap now, and
    // gaps to read is size.
    in_gap_ = true;
    gaps_to_read_ = size;
    // just need to updated gaps_ list accordingly.
    // special case if we just went out from a gap,
    // return us inside.
    if (git_ != gaps_start_) {
      --git_;
      if (git_->offset == wit_.offset()) {
        git_->length += size;
        dont_insert = true;
      } else {
        ++git_;
      }
    }
    if (!dont_insert) {
      emplaceGap(size);
    }
  }
}

void GappedLine::reverse_iterator::addGap(size_t size) {
  if (!size) {
    return;
  }
  line_.size_ += size;
  // if currently in gap, just extend that gap.
  if (in_gap_) {
    gaps_to_read_ += size;
    git_->length += size;
    if (!gaps_to_read_) {
      ++ git_;
      in_gap_ = false;
    }
  } else {
    bool dont_insert = false;
    // otherwise we are definetly in the gap now, and
    // gaps to read is size.
    in_gap_ = true;
    gaps_to_read_ = size;
    // just need to updated gaps_ list accordingly.
    // special case if we just went out from a gap,
    // return us inside.
    if (git_ != gaps_start_) {
      --git_;
      if (git_->offset == wit_.offset()) {
        git_->length += size;
        dont_insert = true;
      } else {
        ++git_;
      }
    }
    if (!dont_insert) {
      emplaceGap(size);
    }
  }
}

void GappedLine::iterator::emplaceGap(size_t size) {
  line_.gaps_.emplace(git_, wit_.offset(), size, true);
  --git_;
}

void GappedLine::reverse_iterator::emplaceGap(size_t size) {
  line_.gaps_.emplace(git_.base(), wit_.offset(), size, true);
}


} // namespace
