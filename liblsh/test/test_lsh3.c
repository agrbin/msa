/*
 * testing that bounded lsh works.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lsh_distance.h"

#define maxn 30
#define ntests 100000

#define min(a,b) ((a) < (b) ? (a) : (b))

int n1, n2;
int dp[2*maxn][2*maxn];
char t1[2*maxn], t2[2*maxn];
int K;

int solve() {
  int i, j;
  for (i = 0; i <= n1; ++i)
    dp[i][0] = i;

  for (i = 0; i <= n2; ++i)
    dp[0][i] = i;

  for (i = 1; i <= n1; ++i)
    for (j = 1; j <= n2; ++j) {
      if (j - i > K || i - j > K) {
        dp[i][j] = 1e9; continue;
      }
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
  int i, r;
  for (i = 0; i < ntests; ++i) {
    generate();
    if (n1 - n2 > K || n2 - n1 > K) continue;
    r = lsh_distance_K(t1, t2, K);
    if (r != solve()) {
      printf("%s\n%s\n", t1, t2);
      printf("K:%d\n", K);
      printf("lsh: %d\n", r);
      printf("brt: %d\n\n", solve());
      printf("FAIL!\n");
      exit(0);
    }
  }
}

int main() {
  srand(getpid());
  test();
  lsh_cleanup();
  return 0;
}
