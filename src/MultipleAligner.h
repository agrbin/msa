#ifndef ACGT_MULTIPLE_ALIGNER_H
#define ACGT_MULTIPLE_ALIGNER_H

#include <string>
#include <vector>

#include "lsh_distance.h"
#include "GappedLine.hpp"
#include "PackedIntervals.h"

namespace acgt {

class MultipleAligner {
 public:
  MultipleAligner(
      double epsilon = 0.1,
      int max_radius = 1e9,
      bool pack = true);
  void addSequence(std::string str, off_t offset);
  void addSequence(const char *str, off_t offset);
  void addSequence(const char *str, int len, off_t offset);

  // will implicitly call align
  std::string getConsensus(int *scoreptr = NULL);
  void align();

 private:
  int getScore();
  void buildRows();
  void alignByRow(size_t row);
  void buildPackedCols(size_t row);
  void reconstructLsh(size_t row, struct lsh_iterator* it);
  void dumpMatrix();
  void stripGaps();
  void preCalculateDistances();
  void log(const char *fmt, ...);
  void fillBegins(std::vector<GappedLine::iterator>&);

  // algorithm params
  double epsilon_;
  int max_radius_;
  bool pack_;

  // input dataset.
  std::vector< std::vector<uint8_t> > storage_;
  std::vector<GappedLine::string_t> words_;
  std::vector<off_t> offsets_;

  // flag.
  bool align_called_;

  // internal data structures
  size_t radius_; // max anchoring distance
  size_t C_; // coverage, number of rows in matrix_;
  size_t N_; // width of each column in matrix_
  std::vector<GappedLine> matrix_; // all rows

  // packedDistLookup[i] pise koja je baza u trenutnom row-u na i-tom stupcu
  // odgovor moze biti 0, 1, 2, 3, 4, 5 gdje 0 odgovara '.', 1 '-', a
  // ostalo oacgt. ali to je zapravo to puta N_ + 1.
  //
  // packedDistances_ je vektor duljine 6 * N_ gdje u j-tom retku pise
  // udaljenost baze j do compositne baze u stupcu i.
  // sve u svemu penalty(i, j) =
  //  packedDistances_[ packedDistLookup_[i] + i ]
  std::vector<int> packedDistLookup_;
  std::vector<int> packedDistances_;

  static int penalty(int i1, int i2);
  static void mmulti_penalty(int i1, int lo_i2, int **out);
  static int *lookup;
  static int *dists;
};

} // namespace
#endif
