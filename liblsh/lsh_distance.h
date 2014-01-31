#ifndef LSH_DISTANCE_H
#define LSH_DISTANCE_H
/**
 * this whole library is not re-entrant. you may use it only like this:
 *
 * it = lsh_initialize(...);
 * while (lsh_iterate(it));
 *
 * eg. you can't have multiple lsh_iterator objects around.
 *
 * :) tough life, ha?
 */

/**
 * levenshtein distance between two strings.
 * O(strlen(t1) * strlen(t2));
 */
int lsh_distance(const char *t1, const char *t2);

/**
 * banded levenshtein distance between two strings.
 * O(n * K)
 */
int lsh_distance_K(const char *t1, const char *t2, int K);

/**
 * this is the iterator state. it is reverse iterator, when you get
 * a value from lsh_initialize you can read whether last letters
 * are aligned together or not. more precisely:
 *
 * if take[k] is true, then take letter from k-th word.
 * if take[k] is false, then make a gap in k-th word.
 *
 * it will never be !take[0] && !take[1], eg. both gaps.
 */
struct lsh_iterator {
  int take[2];
  int _row, _col;
};

/**
 * calculate edit distance between two objects of length len1 and len2.
 * penalty for aligning obj1[i1] with obj2[i2] is given with callback.
 * callback will be called O(len * bandwidth) times.
 *
 * implementation assumes that first symbol in both objs are gap symbols.
 * in other words, if implementation wants to know what is the penalty
 * of aligning gap with obj2[1] it will invoke penalty(0, 2)
 *
 * if bandwidth is 0, then bandwidth will be max(len1, len2), eg. no
 * bandwidth optimization.
 *
 * if result is not NULL, edit distance will be written to it.
 */
struct lsh_iterator*
lsh_initialize(
    int len1,
    int len2,
    int(*penalty)(int, int),
    int bandwidth,
    int *result);

/**
 * this function is equivalent to one above except it expects mcmp call back
 * which is multiple penalty:
 *
 * mcmp(int row, int col_from, int col_to, int *out);
 *
 * it should populate *out array with col_to - col_from numbers that are
 * penalty values.
 *
 * that function should behave like this:
 *
 * void cmp_multi(int i1, int i2_lo, int i2_hi, int *out) {
 *   for (;i2_lo != i2_hi; ++i2_lo) {
 *     *(out++) = penalty(i1, i2_lo);
 *    }
 * }
 */
struct lsh_iterator*
lsh_initialize_mcmp(
    int len1,
    int len2,
    int(*penalty)(int, int),
    void(*mcmp)(int, int, int, int*),
    int bandwidth,
    int *result);

/**
 * even more sick interface to penalties calculation. int** will be given which
 * will mmcmp function point to the existing array of penalty values.
 */
struct lsh_iterator*
lsh_initialize_mmcmp(
    int len1,
    int len2,
    int(*penalty)(int, int),
    void(*mcmp)(int, int, int**),
    int bandwidth,
    int *result);

/**
 * iterate towards the begining of objects. if this function returns
 * true, it means that you should iterate more.
 *
 * make sure you iterate till the end. otherwise, free iterator yourself.
 */
int lsh_iterate(struct lsh_iterator *s);

/**
 * clears internally allocated buffers. you can cal lsh_initialize after
 * this again, but then we will have to do more mallocs.
 */
void lsh_cleanup();

#endif // LSH_DISTANCE_H
