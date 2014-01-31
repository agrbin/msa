#ifndef ACGT_BASE_H
#define ACGT_BASE_H

#include <cstdint>

namespace acgt {

struct Base {
  // mostly used for IO operations
  static const char *humanAlphabet;
  static uint8_t humanToInt(char base);
  static uint8_t humanToIntCareful(char base);
  static char intToHuman(uint8_t base);

  // class constants.
  static const uint8_t OUT_GAP;
  static const uint8_t IN_GAP;
  static const uint8_t A;
  static const uint8_t C;
  static const uint8_t G;
  static const uint8_t T;

  // max mabse
  static const int NBASES = 6;
};

} // namespace acgt
#endif  // ACGT_BASE_H
