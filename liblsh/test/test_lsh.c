#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "lsh_distance.h"

const char *t1 = ".anton", *t2 = ".aton";

int cmp(int i1, int i2) {
  return t1[i1] != t2[i2];
}

int main() {
  int result;
  struct lsh_iterator* it = lsh_initialize(
      strlen(t1), strlen(t2), cmp, 1, &result);
  const char *it1 = t1 + strlen(t1) - 1;
  const char *it2 = t2 + strlen(t2) - 1;
  assert(result == 1);

  while (lsh_iterate(it)) {
    continue;
    printf("%c%c\n",
        it->take[0] ? *it1-- : '-',
        it->take[1] ? *it2-- : '-');
  }

  return 0;
}
