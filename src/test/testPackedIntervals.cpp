#include "PackedIntervals.h"

#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "test.h"

const size_t inf = 0x3f3f3f3f;

using namespace acgt;
using namespace std;

void validSolution(
    pair< size_t, vector<size_t> > sol,
    vector< pair<off_t, off_t> > intervals,
    int radius,
    size_t expected) {
  vector<size_t> sorted = sol.second;
  sort(sorted.begin(), sorted.end());
  EQ(sol.second.size(), intervals.size());
  if (radius == 0 && expected != inf) {
    EQ(expected, sol.first);
  }
  if (!sorted.size()) return;
  EQ(sorted.front(), 0);
  EQ(sol.first, sorted.back() + 1);
  for (auto &i : intervals) {
    i.first -= radius; i.second += radius;
  }
  for (size_t grupa = 0; grupa < sol.first; ++grupa) {
    for (size_t i1 = 0; i1 < intervals.size(); ++i1) {
      for (size_t i2 = 0; i2 < intervals.size(); ++i2) {
        if (i1 == i2) continue;
        if (sol.second[i1] != sol.second[i2]) continue;
        auto int1 = intervals[i1], int2 = intervals[i2];
        if (max(int1.first, int2.first) < min(int1.second, int2.second)) {
          cout << "FAIL\n";
          cout << "u grupi " << grupa << " intervali " << i1 << " i " << i2 <<
            " se sijeku\n";
          cout << "[" << int1.first << "," << int1.second << ">\n";
          cout << "[" << int2.first << "," << int2.second << ">\n";
          cout << "input:\n";
          for (auto kk : intervals) {
            cout << "[" << kk.first << "," << kk.second << ">\n";
          }
          exit(1);
        }
      }
    }
  }
}

void test1(int radius) {
  PackedIntervals P(radius);
  vector<pair<off_t, off_t>> v;
  v = {};
  validSolution(P.dispatch(v), v, radius, 0);
  v = {{0, 5}};
  validSolution(P.dispatch(v), v, radius, 1);
  v = {{0, 5}, {6, 10}};
  validSolution(P.dispatch(v), v, radius, 1);
  v = {{0, 5}, {5, 10}};
  validSolution(P.dispatch(v), v, radius, 1);
  v = {{0, 5}, {4, 10}};
  validSolution(P.dispatch(v), v, radius, 2);
  v = {{0, 5}, {4, 10}, {5, 10}};
  validSolution(P.dispatch(v), v, radius, 2);
  for (int k = 0; k < 60; ++k) {
    v = {};
    for (int j = 0; j < k; ++j) {
      off_t x = rand() % k + 10;
      v.emplace_back(x, x + rand() % (k / 20 + 5));
    }
    validSolution(P.dispatch(v), v, radius, inf);
  }
}

int main() {
  test1(0);
  test1(5);
  test1(10);
  return 0;
}
