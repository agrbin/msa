#include "Random.h"

#include <string>

#include "Base.h"

using std::string;

namespace acgt {

string
Random::randomSeq(size_t n) {
  string t;
  while (n--) t.push_back(Base::humanAlphabet[2 + rand() % (Base::NBASES - 2)]);
  return t;
}

bool
Random::randomBool(float p) {
  if (!p) return false;
  return rand() <= ((float) RAND_MAX * p);
}

void
Random::mutate(
    string &t,
    float pIns,
    float pDel,
    float pSub)
{
  string mutated;
  for (size_t j = 0; j < t.size(); ++j) {
    if (randomBool(pIns)) {
      mutated.push_back(Base::humanAlphabet[rand() % Base::NBASES]);
    }
    if (randomBool(pSub)) {
      mutated.push_back(Base::humanAlphabet[rand() % Base::NBASES]);
      continue;
    }
    if (!randomBool(pDel)) {
      mutated.push_back(t[j]);
    }
  }
  t = mutated;
}


} // namespace
