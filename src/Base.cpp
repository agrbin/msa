#include "Base.h"

#include <cassert>

namespace acgt {

const uint8_t Base::OUT_GAP  = 0;
const uint8_t Base::IN_GAP   = 1;
const uint8_t Base::A        = 2;
const uint8_t Base::C        = 3;
const uint8_t Base::G        = 4;
const uint8_t Base::T        = 5;

const int Base::NBASES;

const char* Base::humanAlphabet = ".-ACGT";

uint8_t Base::humanToIntCareful(char base) {
  switch (base) {
  case 'a': case 'A': return 2;
  case 'c': case 'C': return 3;
  case 'g': case 'G': return 4;
  case 't': case 'T': return 5;
  default: return 0;
  }
}

uint8_t Base::humanToInt(char base) {
  switch (base) {
  case '.': return 0;
  case '-': return 1;
  case 'a': case 'A': return 2;
  case 'c': case 'C': return 3;
  case 'g': case 'G': return 4;
  case 't': case 'T': return 5;
  default:
    assert(!"no such base");
  }
}

char Base::intToHuman(uint8_t base) {
  assert(base < 6);
  return humanAlphabet[base];
}

}
