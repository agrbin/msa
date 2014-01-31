#include "FastaFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <stdio.h>
#include <string>

using std::ofstream;
using std::string;
using std::vector;
using std::pair;

namespace acgt {

FastaFile::FastaFile(string filename) :
    filename_(filename),
    file_(NULL),
    appended_reads_(0)
   { }

FastaFile::~FastaFile() {
  if (file_) {
    munmap((void*) file_, file_size_);
  }
}

bool FastaFile::appendRead(const string t) {
  ofstream fout(filename_, ofstream::app);
  if (!fout) {
    return false;
  }
  fout << ">" << (++appended_reads_) << std::endl;
  for (size_t j = 0; j < t.size(); ++j) {
    if (j && j % 60 == 0) fout << std::endl;
    fout << t[j];
  }
  fout << std::endl;
  return true;
}

size_t FastaFile::fileSize(int fd) {
  off_t fsize;
  fsize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  return fsize;
}

bool FastaFile::initialize() {
  return loadFasta(filename_.c_str()) &&
    parseFasta();
}

bool FastaFile::loadFasta(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    error_ = "error opening fasta file";
    return false;
  }
  file_size_ = fileSize(fd);
  file_ = (const char*) mmap(
      NULL, file_size_,
      PROT_READ, MAP_SHARED,
      fd, 0);
  if (file_ == MAP_FAILED) {
    error_ = "error loading fasta file (mmap failed)";
    return false;
  }
  return true;
}

bool FastaFile::parseFasta() {
  bool begin_line = true, in_header = false;
  pair<off_t, size_t> to_insert(-1, -1);
  for (size_t j = 0; j < file_size_; ++j) {
    if (begin_line && file_[j] == '>') {
      to_insert.second = j - to_insert.first;
      reads_.push_back(to_insert);
      begin_line = false;
      in_header = true;
      continue;
    }
    if (in_header && file_[j] == '\n') {
      to_insert.first = j + 1;
      in_header = false;
      begin_line = true;
      continue;
    }
    begin_line = (file_[j] == '\n');
  }
  reads_.push_back(to_insert);
  return true;
}

string FastaFile::getError() const {
  return error_;
}

pair<const char*, size_t>
FastaFile::getRead(int id) const {
  assert(id > 0 && id < (int) reads_.size());
  return pair<const char*, size_t>(
      file_ + reads_[id].first, reads_[id].second);
}

} // namespace acgt
