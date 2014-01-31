#ifndef ACGT_PACKED_INTERVALS_H
#define ACGT_PACKED_INTERVALS_H

#include <iostream>
#include <vector>
#include <set>

namespace acgt {

/*
 * given a set of intervals, dispatch them into groups such that no
 * two intervals in one group have intersection.
 * this class will use heuristics to solve this problem.
 * problem is NP complete (k-coloring of a graph)
 *
 * if radius is given, all intervals will be enlarged by radius on both
 * sides prior to computation.
 *
 * all intervals are in format [lo, hi>
 */
class PackedIntervals {

 public:
  typedef std::pair<off_t, off_t> interval_t;
  typedef const std::vector<interval_t> intervals_t;

  PackedIntervals(int radius = 0);

  struct cmp {
    cmp(int radius, intervals_t &intervals) :
      radius_(radius), intervals_(intervals) { }
    bool operator() (size_t idx1, size_t idx2) const;
    int radius_;
    intervals_t &intervals_;
  };

  /**
   * main method.
   * auto sol = dispatch(intervals);
   * sol.first is number of groups.
   * sol.second is group ids for each interval
   *
   * sol.size() == intervals.size().
   * min(sol.second) == 0.
   * max(sol.second) == sol.first - 1.
   * intervals[i] will go to group sol[i].
   */
  std::pair< size_t, std::vector<size_t> >
  dispatch(const std::vector<interval_t> &intervals) const;

 private:
  typedef std::set<size_t, cmp> iset;
  const int radius_;
};

} // namespace acgt
#endif // ACGT_PACKED_INTERVALS_H
