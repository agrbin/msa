#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

int n;
string alphabet = "acgt";

void mutate(string &t) {
  string novi;
  for (size_t j = 0; j < t.size(); ++j) {
    if (rand() % 100 < 15) {
      int a = rand() % 15;
      if (a < 11) {
        // insertion
        novi.push_back(alphabet[rand() % 4]);
        novi.push_back(t[j]);
      } else if (a < 13) {
        // deletion
        continue;
      } else {
        // subst
        novi.push_back(alphabet[rand() % 4]);
      }
    } else {
      novi.push_back(t[j]);
    }
  }
  t = novi;
}

int main() {
  srand(time(0) *getpid());
  scanf("%d", &n);
  string a;
  for (int j = 0; j < n; ++j) {
    a += alphabet[rand() % 4];
  }
  string b = a;
  mutate(a);
  mutate(b);
  cout << a << endl << b << endl;
  return 0;
}
