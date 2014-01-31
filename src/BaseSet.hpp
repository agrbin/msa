#ifndef ACGT_BASE_SET_H
#define ACGT_BASE_SET_H

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "Base.h"
#include <cassert>

namespace acgt {

struct BaseSet {
  // count and sum of all.
  int sum;
  uint8_t count[Base::NBASES];

  // we keep track of max_count and where_mask
  // only for bases > 0
  uint8_t max_count;
  uint8_t where_mask;

  BaseSet();
  BaseSet(uint8_t base);

  void clear();

  void add(uint8_t base);

  int getScore() const;
  uint8_t getConsensus() const;
  uint8_t getConsensusStrict() const;

  int ro_c(uint8_t base) const;
  int ro_a(uint8_t base) const;
  int distanceFromSimple(uint8_t base) const;

  static const BaseSet simples[6];
};

inline void BaseSet::clear() {
  memset(this, 0, sizeof (BaseSet));
  where_mask = ((1 << (Base::NBASES - 1)) - 1) << 1;
}

inline void BaseSet::add(uint8_t base) {
  ++count[base];
  if (!base) {
    return;
  }
  ++sum;
  if (count[base] > max_count) {
    max_count = count[base];
    where_mask = 1 << base;
  } else if (count[base] == max_count) {
    where_mask |= 1 << base;
  }
}

inline int BaseSet::ro_c(uint8_t base) const {
  return 50 * !((1 << base) & where_mask);
}

inline int BaseSet::ro_a(uint8_t base) const {
  int score = 0;
  for (int j = 1; j < Base::NBASES; ++j) {
    if (j == base) continue;
    score += count[j];
  }
  return 50 * score / sum;
}

inline int
BaseSet::distanceFromSimple(uint8_t base) const {
  if (!base || !sum) return 0;
  return ro_a(base) + ro_c(base);
}

} // namespace
#endif
