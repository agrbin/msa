#include "PairwiseAligner.hpp"

#include <iostream>
#include <cassert>

using namespace acgt;
using namespace std;

int mypenal(char a, char b) {return a != b;}

void testReal(string a, string b) {
  char dummy = 0;
  for (int bandwidth_ = 1; bandwidth_ < 100; bandwidth_ += 10) {
    PairwiseAligner<char>(
      vector<char>(a.begin(), a.end()),
      vector<char>(b.begin(), b.end()),
      mypenal,
      bandwidth_,
      &dummy
    );
  }
}

void testAlign(string a, string b) {
  int dist = 0;
  SimplePairwiseAligner align(a, b, &dist);
  for (size_t j = 0; j < a.size(); ++j) {
    assert(a[j] != '-' || b[j] != '-');
    if (a[j] != b[j]) --dist;
  }
  assert(dist == 0);
  testReal(a, b);
}

void manual() {
  int dist = 0;
  string a = "abcfffdegf", b = "abcdegfff";
  SimplePairwiseAligner align(a, b, &dist);
  cout << a << endl << b << endl;
}

int main() {
  //manual();
  //return 0;
  testAlign("anton", "aton");
  testAlign("", "a");
  testAlign("a", "");
  testAlign("aaaaaaaa", "");
  testAlign("aaaaaaaa", "aaaaaaaa");
  testAlign("aaa", "bbb");
  testAlign("babc", "sdfkjsdf");
  return 0;
}

