#include "GappedLine.hpp"
#include "GappedLineBrute.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "Base.h"
#include "Random.h"
#include "test.h"

using namespace acgt;
using namespace std;

vector< vector<uint8_t> > hole;

GappedLine make(vector<pair<const char*,size_t>> v, vector<off_t> o) {
  vector<pair<const uint8_t*,size_t> > n;
  for (auto c : v) {
    hole.emplace_back(c.second);
    vector<uint8_t> &v = hole.back();
    for (size_t j = 0; j < c.second; ++j)
      v[j] = Base::humanToInt(c.first[j]);
    v.push_back(0);
    n.push_back(make_pair(&v[0], c.second));
  }
  return GappedLine(n, o);
}

string stringify(GappedLine &gl) {
  string ret = "";
  for (auto it = gl.begin(); it != gl.end(); ++it) {
    ret.push_back(Base::intToHuman(*it));
  }
  string rev = "";
  for (auto it = gl.rbegin(); it != gl.rend(); ++it) {
    rev.push_back(Base::intToHuman(*it));
  }
  reverse(rev.begin(), rev.end());
  EQ(rev, ret);
  return ret;
}

void test1() {
  const char *a = "ACGTG";
  const char *b = "GGG";
  vector<off_t> offs;
  vector<pair<const char*,size_t> > v;
  v.emplace_back(a, strlen(a));
  v.emplace_back(b, strlen(b));
  v.emplace_back("a", 0);
  v.emplace_back("a", 1);
  offs.push_back(2);
  offs.push_back(2 + strlen(a) + 2);
  offs.push_back(15);
  offs.push_back(15);
  auto gl = make(v, offs);
  string res = stringify(gl);
  EQ(res,"..ACGTG..GGG...A");
}

void test2() {
  vector<off_t> offs = {0};
  vector<pair<const char*, size_t> > v = {{"ACGTG", 5}};
  string t1;
  auto gl = make(v, offs);
  auto it = gl.begin();
  it.addGap(1);
  t1.push_back(Base::intToHuman(*(it++)));
  t1.push_back(Base::intToHuman(*(it++)));
  it.addGap(1);
  t1.push_back(Base::intToHuman(*(it++)));
  t1.push_back(Base::intToHuman(*(it++)));
  it.addGap(1);
  t1.push_back(Base::intToHuman(*(it++)));
  t1.push_back(Base::intToHuman(*(it++)));
  while (it != gl.end()) t1.push_back(Base::intToHuman(*(it++)));
  string t2 = stringify(gl);
  EQ(t1.size(), t2.size());
  EQ(t1, t2);
  assert(gl.size() == t2.size());
}

void test3() {
  auto gl = make({}, {});
  auto it = gl.begin();
  EQ("", stringify(gl));
  it.addGap(0);
  EQ("", stringify(gl));
  it.addGap(5);
  EQ(".....", stringify(gl));
}

void test4() {
  vector<off_t> offs = {0};
  vector<pair<const char*, size_t> > v = {{"ACGTG", 5}};
  string t1;
  auto gl = make(v, offs);

  auto it = gl.rbegin();
  it.addGap(5);
  for (int j = 0; j < 6; ++j) ++it;
  it.addGap(3);
  for (int j = 0; j < 6; ++j) ++it;
  it.addGap(4);
  for (int j = 0; j < 5; ++j) ++it;
  it.addGap(10);
  it.addGap(5);
  for (auto it = gl.begin(); it != gl.end(); ++it) {
    it.addGap(1);
    ++it;
  }
  for (auto it = gl.rbegin(); it != gl.rend(); ++it) {
    it.addGap(1);
    ++it;
  }
  stringify(gl);
}

void test5() {
  vector<off_t> offs = {0};
  vector<pair<const char*, size_t> > v = {{"ACGTG", 5}};
  GappedLine gl = make(v, offs);
  auto it = gl.rbegin();
  it.addGap(1);
  ++it; ++it;
  it.addGap(1);
  ++it; ++it;
  it.addGap(1);
  EQ("ACG-T-G.", stringify(gl));
}

void test6() {
  vector<off_t> offs = {0, 8};
  vector<pair<const char*, size_t> > v = {{"ACGTG", 5}, {"GTGCA",5}};
  GappedLine gl = make(v, offs);

  auto it = gl.begin();
  it.addGap(1);
  ++it; ++it;
  it.addGap(1);
  ++it; ++it;
  it.addGap(1);

  auto rit = gl.rbegin();
  rit.addGap(1);
  ++rit; ++rit;
  rit.addGap(1);
  ++rit; ++rit;
  rit.addGap(1);

  string ret = "";
  for (auto it = gl.begin(); it != gl.end(); ++it) {
    ret.push_back(Base::intToHuman(*it));
  }
  reverse(ret.begin(), ret.end());
  EQ(ret, stringify(gl));
}

// N number of words
// K>1 max size of ward
// G>1 max size of gap between words.
void test7(int N, int K, int G) {
  vector<off_t> offs;
  vector<pair<const char*, size_t> > words;
  vector<string> holdOn;
  off_t offset = 0;
  for (int j = 0; j < N; ++j) {
    string word = Random::randomSeq(rand() % K);
    holdOn.push_back(word);
    words.push_back(make_pair<const char*, size_t>(word.c_str(), word.size()));
    offs.push_back(offset);
    offset += word.size() + 1 + rand() % G;
  }
  GappedLine gl = make(words, offs);
  GappedLineBrute bgl(words, offs);
  EQ(stringify(gl), bgl.toString());
  {
    auto it1 = gl.begin();
    auto it2 = bgl.begin();
    while (it1 != gl.end()) {
      if (rand() % 3 == 0) {
        int gg = 1 + (rand() % G);
        it1.addGap(gg);
        it2.addGap(gg);
        while (gg--) ++it1, ++it2;
      }
      ++it1, ++it2;
    }
  }
  EQ(stringify(gl), bgl.toString());
  {
    auto it1 = gl.rbegin();
    auto it2 = bgl.rbegin();
    while (it1 != gl.rend()) {
      if (rand() % 3 == 0) {
        int gg = rand() % G;
        it1.addGap(gg);
        it2.addGap(gg);
        while (gg--) ++it1, ++it2;
      }
      ++it1, ++it2;
    }
  }
  /*
  cout << stringify(gl) << endl;
  cout << bgl.toString() << endl << endl;
  */
  EQ(stringify(gl), bgl.toString());
}

void test7() {
  for (int j = 0; j < 100; ++j) {
    test7(0, 10, 3);
    test7(2, 10, 3);
    test7(10, 10, 3);
    test7(20, 10, 1);
    test7(20, 1, 1);
  }
  test7(100, 200, 10);
}

void test8() {
  vector<off_t> offs = {5};
  vector<pair<const char*, size_t> > v = {{"ACGTG", 5}};
  GappedLine gl = make(v, offs);
  assert(gl.begin().inGap());
  gl.begin().addGap(-5);
  assert(!gl.begin().inGap());
  EQ(stringify(gl), "ACGTG");
}

void test9() {
  vector<off_t> offs = {3,10};
  vector<pair<const char*, size_t> > v = {{"AAAAAA", 6}, {"CGTCGT", 6}};
  GappedLine gl = make(v, offs);
  auto it = gl.begin();
  for (int j = 0; j < 5; ++j, ++it);
  it.addGap(5);
  EQ(stringify(gl), "...AA-----AAAA.CGTCGT");
}

void test10() {
  vector<off_t> offs = {10,3};
  vector<pair<const char*, size_t> > v = {{"AAAAAA", 6}, {"CGTCGT", 6}};
  GappedLine gl = make(v, offs);
  auto it = gl.begin();
  for (int j = 0; j < 5; ++j, ++it);
  it.addGap(5);
  EQ(stringify(gl), "...AA-----AAAA.CGTCGT");
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  return 0;
}

