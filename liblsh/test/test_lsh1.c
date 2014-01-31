/*
this test tests unbounded edit distance.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lsh_distance.h"

#define maxn 50
#define ntests 100000

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
  n1 = rand() % maxn;
  n2 = rand() % maxn;
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
    r = lsh_distance(t1, t2);
    if (r != solve()) {
      printf("%s\n%s\n", t1, t2);
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
