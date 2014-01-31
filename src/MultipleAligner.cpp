#include "MultipleAligner.h"

#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <algorithm>

#include "lsh_distance.h"
#include "Base.h"
#include "BaseSet.hpp"

using std::string;
using std::vector;
using std::pair;

namespace acgt {

MultipleAligner::MultipleAligner(double epsilon,
                                 int max_radius,
                                 bool pack) :
    epsilon_(epsilon),
    max_radius_(max_radius),
    pack_(pack),
    align_called_(false)
{ }

void
MultipleAligner::addSequence(const char *str, off_t offset) {
  addSequence(str, strlen(str), offset);
}

void
MultipleAligner::addSequence(string str, off_t offset) {
  addSequence(str.c_str(), str.size(), offset);
}

void
MultipleAligner::addSequence(const char *str, int len, off_t offset) {
  bool was_neg = len < 0;
  if (len < 0) {
    len = -len;
  }
  storage_.emplace_back();
  vector<uint8_t> &t = storage_.back();
  t.reserve(len);
  for (int j = 0; j < len; ++j) {
    int base = Base::humanToIntCareful(str[j]);
    if (base) t.push_back(base);
  }
  if (was_neg) {
    std::reverse(t.begin(), t.end());
  }
  words_.emplace_back(&t[0], t.size());
  offsets_.push_back(offset);
}

string MultipleAligner::getConsensus(int *scoreptr) {
  if (!align_called_) align();
  vector<GappedLine::iterator> its;
  string sol;
  fillBegins(its);
  int score = 0;
  for (size_t i = 0; i < N_; ++i) {
    BaseSet all;
    for (auto &it : its) {
      all.add(*it++);
    }
    char x = Base::intToHuman(all.getConsensusStrict());
    if (x == '.') x = 'n';
    if (x != '-') {
      score += all.getScore();
      sol.push_back(x);
    }
  }
  if (scoreptr != NULL) {
    *scoreptr = score;
  }
  return sol;
}

int MultipleAligner::getScore() {
  int score;
  getConsensus(&score);
  return score;
}

void
MultipleAligner::fillBegins(vector<GappedLine::iterator> &its) {
  for (auto &row : matrix_) {
    its.push_back(row.begin());
  }
}

void MultipleAligner::log(const char* fmt, ...)
{
  std::va_list arg;
  va_start(arg, fmt);
  fprintf(stdout, "msa: ");
  vfprintf(stdout, fmt, arg);
  fprintf(stdout, "\n");
  va_end(arg);
}

void MultipleAligner::buildRows() {
  vector<PackedIntervals::interval_t> intervals(words_.size());
  size_t width = 0;
  for (size_t j = 0; j < words_.size(); ++j) {
    intervals[j].first = offsets_[j];
    intervals[j].second = offsets_[j] + words_[j].second;
    width = std::max(width, words_[j].second + offsets_[j]);
  }
  radius_ = std::max((int) (width * epsilon_), 1);
  radius_ = std::min((int) radius_, max_radius_);
  log("radius is %ld (maximum allowed shift)", radius_);
  PackedIntervals P(2*radius_ + (!pack_ * 1e6));
  auto ret = P.dispatch(intervals);
  C_ = ret.first;
  for (size_t j = 0; j < C_; ++j) {
    vector<GappedLine::string_t> row_words;
    vector<off_t> row_offsets;
    static uint8_t dummy = 0;
    for (size_t k = 0; k < ret.second.size(); ++k) {
      if (ret.second[k] == j) {
        row_words.emplace_back(words_[k]);
        row_offsets.emplace_back(offsets_[k]);
      }
    }
    row_words.emplace_back(&dummy, 0);
    row_offsets.emplace_back(width);
    matrix_.emplace_back(row_words, row_offsets);
  }
  N_ = width;
  log("matrix has %ld rows (max coverage), and %ld cols", C_, N_);
  log("initial score: %d", getScore());
  for (const auto &row : matrix_) {
    assert(row.size() == N_);
  }
}

void MultipleAligner::buildPackedCols(size_t row) {
  vector<GappedLine::iterator> its;
  packedDistLookup_.resize(N_ + 1);
  packedDistances_.resize((N_ + 1) * 6);
  fillBegins(its);
  // we need first element in packedCols to be gap, lsh_distance
  // assumes that. this will point to distances from '-' to others.
  packedDistLookup_[0] = 1 * (N_ + 1);
  for (int k = 0; k < 6; ++k) {
    packedDistances_[ k * (N_ + 1) + 0 ] =
      BaseSet(k).distanceFromSimple(Base::IN_GAP);
  }

  // build everything else.
  BaseSet composite;
  for (size_t i = 0; i < N_; ++i) {
    composite.clear();
    for (size_t k = 0; k < C_; ++k) {
      if (k != row) {
        composite.add(*its[k]++);
      }
    }
    packedDistLookup_[i + 1] = (*its[row]++) * (N_ + 1);
    for (int k = 0; k < 6; ++k) {
      packedDistances_[ k * (N_ + 1) + i + 1 ] =
        composite.distanceFromSimple(k);
    }
  }
  lookup = &packedDistLookup_[0];
  dists = &packedDistances_[0];
}

void MultipleAligner::dumpMatrix() {
  for (auto &row : matrix_) {
    for (auto c : row) {
      printf("%c", Base::intToHuman(c));
    }
    printf("\n");
  }
}

void MultipleAligner::alignByRow(size_t row) {
  // build packed cols
  // invoke edit distance
  // iterate.
  // log("realign by row %ld", row);
  buildPackedCols(row);
  // log("  packed cols built.");
  int res;
  struct lsh_iterator *lsh_it = lsh_initialize_mmcmp(
      N_, N_, penalty, mmulti_penalty, radius_, &res);
  // log("  edit distance done with result %d.", res);
  reconstructLsh(row, lsh_it);
  // log("  reconstruct complete, score: %d, dist %d.", getScore(), res);
}

void
MultipleAligner::reconstructLsh(size_t row, struct lsh_iterator *it) {
  vector<GappedLine::reverse_iterator> its[2];
  for (size_t j = 0; j < C_; ++j) {
    (j == row ? its[0] : its[1])
      .push_back(matrix_[j].rbegin());
  }
  N_ = 0;
  while (lsh_iterate(it)) {
    bool allInGap = true;
    for (size_t col = 0; col < 2; ++col) {
      if (!it->take[col]) {
        for (auto &it : its[col]) {
          it.addGap(1);
        }
      }
    }
    for (size_t col = 0; col < 2 && allInGap; ++col) {
      for (auto &it : its[col]) {
        if (!it.inGap()) {
          allInGap = false;
          break;
        }
      }
    }
    for (size_t col = 0; col < 2; ++col) {
      for (auto &it : its[col]) {
        if (allInGap) {
          it.addGap(-1);
        } else {
          ++it;
        }
      }
    }
    if (!allInGap) {
      ++N_;
    }
  }
  for (const auto &row : matrix_) {
    assert(row.size() == N_);
  }
}

int *MultipleAligner::lookup = NULL;
int *MultipleAligner::dists = NULL;

int MultipleAligner::penalty(int i1, int i2) {
  return dists[lookup[i1] + i2];
}

void MultipleAligner::mmulti_penalty(
    int i1, int lo_i2, int **out) {
  *out = dists + lookup[i1] + lo_i2;
}

void MultipleAligner::align() {
  if (align_called_) return;
  align_called_ = true;
  int seconds = time(0);
  log("%d sequences to align.", words_.size());
  buildRows();
  int score;
  do {
    score = getScore();
    log("iterating.. %d", getScore());
    for (size_t j = 0; j < C_; ++j) {
      alignByRow(j);
    }
  } while (score != getScore());
  log("final score: %d, seconds: %d", getScore(), time(0) - seconds);
}


} // namespace
