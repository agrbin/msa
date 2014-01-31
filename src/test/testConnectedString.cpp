#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "ConnectedString.hpp"

using namespace acgt;
using namespace std;

void test1(string a, string b) {
  auto a1 = pair<const char*, size_t> (a.c_str(), a.size());
  auto a2 = pair<const char*, size_t> (b.c_str(), b.size());
  vector<pair<const char*, size_t>> v = {a1, a2};
  ConnectedString<char> t(v);
  string clone = "";
  for (ConnectedString<char>::iterator it = t.begin(); it != t.end(); ++it) {
    clone.push_back(*it);
  }
  assert(clone == a + b);
  assert(t.size() == a.size() + b.size());
}

void test2(string a, string b) {
  auto a1 = pair<const char*, size_t> (a.c_str(), a.size());
  auto a2 = pair<const char*, size_t> (b.c_str(), b.size());
  vector<pair<const char*, size_t>> v = {a1, a2};
  ConnectedString<char> t(v);
  string clone = "";
  for (ConnectedString<char>::reverse_iterator it = t.rbegin(); it != t.rend(); ++it){
    clone.push_back(*it);
  }
  reverse(clone.begin(), clone.end());
  assert(clone == a + b);
  assert(t.size() == a.size() + b.size());
}

void test3(string a) {
  for (int j = 0; j < 10; ++j) {
    auto a1 = pair<const char*, size_t> (a.c_str(), a.size());
    vector<pair<const char*, size_t>> v(j, a1);
    ConnectedString<char> t(v);
    string clone = "";
    for (ConnectedString<char>::iterator it = t.begin(); it != t.end(); ++it) {
      clone.push_back(*it);
    }
    string acpy = "";
    for (int k = 0; k < j; ++k) {
      acpy += a;
    }
    assert(clone == acpy);
    assert(t.size() == a.size() * j);
  }
}

void test_all(string a, string b) {
  test1(a, b);
  test2(a, b);
  test3(a);
  test3(b);
}

int main() {
  test_all("anton", "pas");
  test_all("a", "pas");
  test_all("a", "p");
  test_all("", "p");
  test_all("", "");
  test_all("p", "");
  return 0;
}
