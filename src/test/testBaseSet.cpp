#include "BaseSet.hpp"

#include <iostream>
#include <cassert>

#include "test.h"
#include "Base.h"

using namespace acgt;
using namespace std;

void make(BaseSet &b, string t) {
  for (auto c : t) {
    b.add(Base::humanToInt(c));
  }
}

int dist_ro_c(string a, string b) {
  BaseSet  _b; make(_b, b);
  assert(a.size() == 1);
  return _b.ro_c(Base::humanToInt(a[0]));
}

int dist_ro_a(string a, string b) {
  BaseSet  _b; make(_b, b);
  assert(a.size() == 1);
  return _b.ro_a(Base::humanToInt(a[0]));
}

char consensus(string t) {
  BaseSet _t; make(_t, t);
  return Base::intToHuman(_t.getConsensus());
}

int score(string t) {
  BaseSet _t; make(_t, t);
  return _t.getScore();
}

void test1() {
  EQ(dist_ro_c("a", "a") , 0);
  EQ(dist_ro_c("a", "aaaa") , 0);
  EQ(dist_ro_c("a", "aaac") , 0);
  EQ(dist_ro_c("a", "aagc") , 0);
  EQ(dist_ro_c("a", "aagg") , 0);
  EQ(dist_ro_c("a", "aggg") ,50);
  EQ(dist_ro_c("-", "--aa") , 0);
  EQ(dist_ro_c("-", "-aaa") ,50);
  EQ(dist_ro_c("c", "cg") , 0);
  EQ(dist_ro_c("c", "t") ,50);
}

void test2() {
  EQ(dist_ro_a("a", "a") , 0);
  EQ(dist_ro_a("a", "c") , 50);
  EQ(dist_ro_a("a", "ac") , 25);
  EQ(dist_ro_a("a", "aaac") , 12);
  EQ(dist_ro_a("a", "aaaa-") , 10);
  EQ(dist_ro_a("g", "ggcgg-gggg") , 10);
  EQ(dist_ro_a("g", "cccgccgc--") , 40);
}

void test3() {
  EQ(consensus("aaa-") , 'A');
  EQ(consensus("aa-") , 'A');
  EQ(score("aa-") , 1);
  EQ(score("aaaaaaa") , 0);
}

int main() {
  test1();
  test2();
  test3();
  return 0;
}
