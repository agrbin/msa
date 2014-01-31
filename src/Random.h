#ifndef ACGT_RANDOM_H
#define ACGT_RANDOM_H

#include <cstdlib>
#include <string>

namespace acgt {

class Random {
 public:

  /**
   * gets the random string over acgt alphabet with uniform probability
   * for each uint8_t.
   */
  static std::string randomSeq(size_t n);

  /**
   * this function returns true with probability p.
   */
  static bool randomBool(float p);

  /**
   * mutate original string over acgt alphabet with probabilities
   * for insertion, deletion and substitution.
   *
   * O(t.size())
   */
  static void mutate(std::string &t,
      float pIns, float pDel, float pSub);

};


} // namespace
#endif  // ACGT_RANDOM_H

