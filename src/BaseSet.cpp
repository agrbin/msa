#include "BaseSet.hpp"

#include "Base.h"

namespace acgt {

const BaseSet BaseSet::simples[6] = {
  BaseSet(Base::OUT_GAP),
  BaseSet(Base::IN_GAP),
  BaseSet(Base::A),
  BaseSet(Base::C),
  BaseSet(Base::G),
  BaseSet(Base::T)
};

BaseSet::BaseSet() {
  clear();
}

BaseSet::BaseSet(uint8_t base) {
  clear(); add(base);
}

int BaseSet::getScore() const {
  uint8_t cons = getConsensus();
  int score = 0;
  for (int j = 1; j < Base::NBASES; ++j) {
    if (j == cons) continue;
    score += count[j];
  }
  return score;
}

uint8_t BaseSet::getConsensusStrict() const {
  int k = 0, which = 0;
  for (uint8_t j = 2; j < Base::NBASES; ++j) {
    if ((1 << j) & where_mask) {
      k++;
      which = j;
    }
  }
  return k == 1 ? which : 0;
  assert(0);
}


uint8_t BaseSet::getConsensus() const {
  for (uint8_t j = 1; j < Base::NBASES; ++j) {
    if ((1 << j) & where_mask) {
      return j;
    }
  }
  assert(0);
}

} // namespace
