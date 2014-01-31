#include "FixedMatrix.hpp"

#include <iostream>

using namespace acgt;
using namespace std;

void testArray(size_t n, size_t bits) {
  vector<int> v(n);
  FixedArray f(n, bits);
  int max = (1ll << bits);

  // cout << "novi test " << n << ' ' << bits << endl;
  for (int j = 0; j < 10; ++j) {
    int val = rand() % max;
    int pos = rand() % n;
    // cout << "SET " << pos << " -> " << val << endl;
    v[pos] = val;
    f.set<int>(pos, val);
    pos = rand() % n;
    if (!(f.get<int>(pos) == v[pos])) {
      cout << "FAIL " << pos << ": " << n << ' ' << bits << " -> "
        << f.get<int>(pos) << ' ' << v[pos] << endl;
    }
  }
}

void manualMatrixTest() {
  {
    FixedMatrix f(10, 10, 3);
    f.set<int>(2, 2, 7);
    assert(f.get<int>(2, 2) == 7);
    assert(f.get<int>(-2, 2, 5) == 5);
    assert(f.get<int>(2, 10, 5) == 5);
  }
  {
    FixedMatrix f(1, 1, 32);
    f.set<int>(0, 0, -1);
    assert(f.get<int>(0, 0) == -1);
  }
}


int main() {
  for (int bits = 1; bits < 32; ++bits) {
    testArray(10000 - bits * 30, bits);
  }
  manualMatrixTest();
  return 0;
}
