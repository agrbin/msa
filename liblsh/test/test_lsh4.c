#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lsh_distance.h"

#define maxn 100
#define ntests 1000

#define PRINTA

#define min(a,b) ((a) < (b) ? (a) : (b))

int n1, n2;
int dp[maxn][maxn];
char t1[maxn], t2[maxn];

int solve() {
  int i, j;
  for (i = 0; i <= n1; ++i)
    dp[i][0] = i;

  for (i = 0; i <= n2; ++i)
    dp[0][i] = i;

  for (i = 1; i <= n1; ++i)
    for (j = 1; j <= n2; ++j) {
      if (t1[i-1] == t2[j-1]) {
        dp[i][j] = dp[i-1][j-1];
      } else {
        dp[i][j] = dp[i-1][j-1] + 1;
      }
      dp[i][j] = min(dp[i][j], dp[i-1][j] + 1);
      dp[i][j] = min(dp[i][j], dp[i][j-1] + 1);
    }
  return dp[n1][n2];
}

void generate() {
  int i;
  n1 = rand() % maxn + 1;
  n2 = rand() % maxn + 1;
  for (i = 0; i < n1; ++i)
    t1[i] = 'a' + rand() % 4;
  for (i = 0; i < n2; ++i)
    t2[i] = (rand() % 10 > 2 && i + 3 < n1) ? t1[i + rand() % 4]
      : 'a' + rand() % 4;
  t1[n1] = 0;
  t2[n2] = 0;
}

typedef struct lsh_iterator* itptr;

int cmp(int i, int j) {
  return i && j ? t1[i-1] != t2[j-1] : 1;
}

void test() {
  int i, b, res, lres;
  itptr it;
  for (i = 0; i < ntests; ++i) {
    generate();
    /*
    memcpy(t1, "babbbaabdb", 10); n1 = 10;
    memcpy(t2, "abbaaaaa", 8); n2 = 8;
    */
    lres = maxn * 3;
    for (b = 1; b < n1 + n2; b += (n1/20)+1) {
      it = lsh_initialize(n1, n2, cmp, b, &res);
      if (res > lres) {
        printf("FAIL\n");
        exit(0);
      }
      while (lsh_iterate(it));
      lres = res;
    }
  }
}

int main() {
  srand(getpid());
  test();
  lsh_cleanup();
  return 0;
}
