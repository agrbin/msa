/**
 * move 1 means that we take letter from first string.
 *
 * moves in original matrix:
 *
 * 0 1
 *  \|
 * 2-x
 *
 * moves in banded matrix:
 *
 *   0 1
 *   |/
 * 2-x
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lsh_distance.h"

#define INF 0x3f3f3f3f

static int(*penalty)(int, int);
static void(*mpenalty)(int, int, int, int*);
static void(*mmpenalty)(int, int, int**);

static int n1, n2, K, double_K_plus_1;

/* 2*K+1 */
static int *dp_write = NULL;
static int *dp_read = NULL;
static int *cmpvals = NULL;
static int allocated_width = 0;

/* penalty values for gap : obj2[0..n2> */
static int *cmp_0_all = NULL;
static int cmp_0_all_len = 0;

/* n1*(2*K+1) */
static char *rc = NULL;
static int allocated_rcs = 0;
#define RC(x, y) (rc[(x*double_K_plus_1) + y])

/*
 this library is not re-entrant because it uses static buffers.
 this flag will be set to 1 if there is iteration in progress.
*/
static int busy = 0;

void adjust_bandwidth() {
  /* lets have n1 as a smaller number. */
  int diff = n2 > n1 ? n2 - n1 : n1 - n2;
  /* if we don't have bandwidth .. well, fuck it, max(n1, n2). */
  if (!K) {
    K = 1 + (n2 > n1 ? n2 : n1);
  }
  /* make sure that bandwidth is large enough to read the result. */
  if (diff > K) {
    K = diff;
  }
}

void check_allocation() {
  if (allocated_width < (double_K_plus_1)) {
    dp_write = (int*) realloc(dp_write, (double_K_plus_1) * sizeof(int));
    dp_read = (int*) realloc(dp_read, (double_K_plus_1) * sizeof(int));
    cmpvals = (int*) realloc(cmpvals, (double_K_plus_1) * sizeof(int));
    allocated_width = (double_K_plus_1);
  }
  if (allocated_rcs < n1 * (double_K_plus_1)) {
    rc = (char*) realloc(rc, n1 * (double_K_plus_1));
    allocated_rcs = n1 * (double_K_plus_1);
  }
  if (cmp_0_all_len < n2) {
    cmp_0_all = (int*) realloc(cmp_0_all, n2 * sizeof(int));
    cmp_0_all_len = n2;
  }
}

void swap_dps() {
  int *dp_temp = dp_read;
  dp_read = dp_write;
  dp_write = dp_temp;
}

void initialize_cmp_0_all() {
  int k;
  for (k = 0; k < n2; ++k) {
    cmp_0_all[k] = penalty(0, k);
  }
}

void edit_distance_haste(int row) {
  int k, p_row_0 = penalty(row, 0);
  int *cmpvals_org = NULL;
  assert(row);

  /* take penalty values, or prepopulate if mpenalty is unavailable */
  if (mmpenalty != NULL) {
    cmpvals_org = cmpvals;
    mmpenalty(row, row - K, &cmpvals);
  } else if (mpenalty != NULL) {
    mpenalty(row, row - K, row + K + 1, cmpvals);
  } else {
    for (k = 0; k < double_K_plus_1; ++k) {
      cmpvals[k] = penalty(row, row + k - K);
    }
  }

  /* init last element in band with move 0. */
  {
    int k = 2 * K;
    dp_write[k] = dp_read[k] + cmpvals[k];
    RC(row, k) = 0;
  }
  /* for each element in band except last, choose move 0 or 1. */
  for (k = 0; k < 2 * K; ++k) {
    int penal0 = dp_read[k] + cmpvals[k],
        penal1 = dp_read[k+1] + p_row_0;

    dp_write[k] = penal0 < penal1 ? penal0 : penal1;
    RC(row,k) = !(penal0 < penal1);
  }
  /* for each element except first in band try to choose move 2. */
  for (k = 1; k < double_K_plus_1; ++k) {
    int penal = dp_write[k-1] + cmp_0_all[row + k - K];
    if (penal < dp_write[k]) {
      dp_write[k] = penal;
      RC(row,k) = 2;
    }
  }
  if (cmpvals_org != NULL) {
    cmpvals = cmpvals_org;
  }
  swap_dps();
}


/**
 * in this function we don't use mpenalty but we could.
 */
void edit_distance_careful(int row) {
  int k;
  for (k = 0; k < double_K_plus_1; ++k) {
    int col = row + k - K;
    /* if out of box continue. */
    if (col < 0 || row >= n1 || col >= n2) {
      continue;
    }
    /* if this is starting state, return answer */
    if (!row && !col) {
      dp_write[k] = 0;
      RC(row,k) = 0;
      continue;
    }
    /* move 0 is possible if row > 0 and col > 0 */
    if (row && col) {
      /*
      if (row == 2) {
        printf("%d,%d -> %d\n", row, col, penalty(row, col));
      }
      */
      dp_write[k] = dp_read[k] + penalty(row, col);
      RC(row,k) = 0;
    } else {
      dp_write[k] = INF;
    }
    /* move 1 is possible if row > 0 and not on the right edge. */
    if (row && k != 2 * K) {
      int penal = dp_read[k+1] + penalty(row, 0);
      if (dp_write[k] > penal) {
        dp_write[k] = penal;
        RC(row,k) = 1;
      }
    }
    /* move 2 is possible if col > 0 */
    if (k && col > 0) {
      int penal = dp_write[k-1] + penalty(0, col);
      if (dp_write[k] > penal) {
        dp_write[k] = penal;
        RC(row,k) = 2;
      }
    }
  }
#ifdef LSH_DEBUG
  for (k = 0; k < row; ++k) printf("  ");
  for (k = 0; k < double_K_plus_1; ++k) {
    if (dp_write[k] == INF) printf("x ");
    else printf("%d ", dp_write[k]);
  }
  printf("\n");
#endif
  swap_dps();
}


void output() {
#ifdef LSH_DEBUG
  int i1, k, i2;
  for (i1 = 0; i1 < n1; ++i1) {
    for (k = 0; k < i1; ++k) printf("  ");
    for (i2 = 0; i2 < 2 * K + 1; ++i2) {
      if (RC(i1,i2) == (char) INF) printf("x ");
      else printf("%d ", RC(i1,i2));
    }
    printf("\n");
  }
  printf("\n");
#endif
}

void edit_distance() {
  int row;
  initialize_cmp_0_all();
  if (n2 - K > K + 1) {
    for (row = 0; row < K + 1; ++row)
      edit_distance_careful(row);
    for (row = K + 1; row < n2 - K; ++row)
      edit_distance_haste(row);
    for (row = n2 - K; row < n1; ++row)
      edit_distance_careful(row);
  } else {
    for (row = 0; row < n1; ++row)
      edit_distance_careful(row);
  }
}

void lsh_cleanup() {
  free(rc);
  free(dp_write);
  free(dp_read);
  free(cmp_0_all);
  free(cmpvals);
  allocated_rcs = allocated_width = cmp_0_all_len = 0;
}

int lsh_iterate(struct lsh_iterator *s) {
  if (!s->_row && s->_col == K) {
    free(s);
    busy = 0;
    return 0;
  }
  switch (RC(s->_row, s->_col)) {
    case 0:
      s->_row--;
      s->take[0] = s->take[1] = 1;
      return 1;
    case 1:
      s->_row--; s->_col++;
      s->take[0] = 1; s->take[1] = 0;
      return 1;
    case 2:
      s->_col--;
      s->take[0] = 0; s->take[1] = 1;
      return 1;
    default:
      assert(0);
  }
}

struct lsh_iterator*
lsh_initialize(int len1, int len2, int(*penalty_)(int, int),
    int bandwidth, int *result)
{
  struct lsh_iterator* ret;
  assert (!busy);
  busy = 1;
  n1 = len1 + 1, n2 = len2 + 1;
  /* this is ugly comunication between lsh_initialize_mcmp and lsh_initialize
   * to check if we have mpenalty
   */
  if (bandwidth < 0) {
    bandwidth *= -1;
  } else {
    mpenalty = NULL;
    mmpenalty = NULL;
  }
  penalty = penalty_;
  K = bandwidth;
  adjust_bandwidth();
  double_K_plus_1 = 2*K + 1;
  check_allocation();
  edit_distance();
  output();
  if (result != NULL) {
    *result = dp_read[n2 - n1 + K];
  }
  ret = (struct lsh_iterator*) malloc(sizeof(struct lsh_iterator));
  ret->_row = n1 - 1;
  ret->_col = n2 - n1 + K;
  return ret;
}


struct lsh_iterator*
lsh_initialize_mcmp(int len1, int len2, int(*penalty_)(int, int),
    void(*mcmp)(int, int, int, int*), int bandwidth, int *result)
{
  mpenalty = mcmp;
  return lsh_initialize(len1, len2, penalty_, -bandwidth, result);
}

struct lsh_iterator*
lsh_initialize_mmcmp(int len1, int len2, int(*penalty_)(int, int),
    void(*mcmp)(int, int, int**), int bandwidth, int *result)
{
  mmpenalty = mcmp;
  return lsh_initialize(len1, len2, penalty_, -bandwidth, result);
}

static const char *__t1, *__t2;

int dummy_penalty(int i, int j) {
  return i && j ? __t1[i-1] != __t2[j-1] : 1;
}

int lsh_distance(const char *t1, const char *t2) {
  int result;
  struct lsh_iterator* it;
  __t1 = t1, __t2 = t2;
  it = lsh_initialize(strlen(t1) + 1, strlen(t2) + 1,
      dummy_penalty, 0, &result);
  /* just make sure that path is valid.
   we could've done free(it) also and busy=0*/
  while(lsh_iterate(it));
  return result;
}


int lsh_distance_K(const char *t1, const char *t2, int _K) {
  int result;
  struct lsh_iterator* it;
  __t1 = t1, __t2 = t2;
  it = lsh_initialize(strlen(t1) + 1, strlen(t2) + 1,
      dummy_penalty, _K, &result);
  /* just make sure that path is valid.
   we could've done free(it) also and busy=0 */
  while(lsh_iterate(it));
  return result;
}

