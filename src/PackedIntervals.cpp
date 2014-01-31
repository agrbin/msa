#include "PackedIntervals.h"

#include <iostream>
using namespace std;

using std::set;
using std::pair;
using std::vector;

namespace acgt {

PackedIntervals::PackedIntervals(int radius) :
    radius_(radius) {
}

bool PackedIntervals::cmp::operator() (size_t idx1, size_t idx2) const {
  return intervals_[idx1].second + radius_ <= intervals_[idx2].first - radius_;
}

pair<size_t, vector<size_t>>
PackedIntervals::dispatch(intervals_t &intervals) const {
  // build
  vector<iset> groups;
  for (size_t idx = 0; idx < intervals.size(); ++idx) {
    bool found = false;
    for (auto &set : groups) {
      if (!set.count(idx)) {
        set.insert(idx);
        found = true;
        break;
      }
    }
    if (!found) {
      groups.emplace_back(cmp(radius_, intervals));
      groups.back().insert(idx);
    }
  }
  // output
  pair<size_t, vector<size_t>> sol;
  sol.first = groups.size();
  sol.second.resize(intervals.size());
  for (size_t set_id = 0; set_id < groups.size(); ++set_id) {
    for (auto idx : groups[set_id]) {
      sol.second[idx] = set_id;
    }
  }
  return sol;
}


} // namespace
