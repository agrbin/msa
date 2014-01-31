#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lsh_distance.h"

#define maxn 100
#define ntests 1000

int n1, n2;
char t1[2*maxn], t2[2*maxn];
int K;

int cmp(int i1, int i2) {
  return (t1[i1] != t2[i2]) * (1 + i1 % (i2 + 1));
}

void generate() {
  int i;
  for (;;) {
    n1 = rand() % maxn + 1;
    n2 = rand() % maxn + 1;
    K = n1 / 10 + 1;
    if (n1 - n2 > K || n2 - n1 > K) continue;
    break;
  }
  for (i = 0; i < n1; ++i)
    t1[i] = 'a' + rand() % 26;
  for (i = 0; i < n2; ++i)
    t2[i] = (rand() % 10 > 2 && i + 3 < n1) ? t1[i + rand() % 4]
      : 'a' + rand() % 26;
  t1[n1] = 0;
  t2[n2] = 0;
}


void test() {
  int result;
  struct lsh_iterator* it;
  generate();

  it = lsh_initialize(n1, n2, cmp, K, &result);
  const char *it1 = t1 + n1 - 1;
  const char *it2 = t2 + n2 - 1;

  int dist = 0;
  int id1 = n1 - 1;
  int id2 = n2 - 1;

  while (lsh_iterate(it)) {
    dist += cmp(it->take[0] ? id1 + 1 : 0, it->take[1] ? id2 + 1 : 0);
    id1 -= it->take[0];
    id2 -= it->take[1];
    continue;
    printf("%c%c\n",
        it->take[0] ? *it1-- : '-',
        it->take[1] ? *it2-- : '-');
  }

  assert(result == dist);
}

int main() {
  int j;
  for (j = 0; j < 1; ++j) {
    K = (j+1) % 30;
    test();
  }
  return 0;
}
