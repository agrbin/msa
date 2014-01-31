#include "LayoutFile.h"

#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

using std::fstream;
using std::string;
using std::vector;
using std::pair;
using std::ostringstream;

namespace acgt {

LayoutFile::LayoutFile(string filename) :
    filename_(filename) { }

LayoutFile::~LayoutFile() { }

bool LayoutFile::isPrefix(int num,
                          string &line,
                          const string &what) {
  if (line.substr(0, what.size()) == what) {
    line = line.substr(what.size());
    return true;
  }
  ostringstream oss;
  oss << filename_ << ":" << num
      << " expected " << what;
  error_ = oss.str();
  return false;
}

bool LayoutFile::initialize() {
  fstream fin(filename_);
  if (!fin) {
    error_ = "can't open layout file";
    return false;
  }
  string line;
  enum state_t {E_LAY, E_TLE, E_clr,
                E_off, E_src, E_end, E_end_TLE};
  state_t state = E_LAY;
  vector<record> *reads;
  record *current = NULL;
  int num = 0;
  while (getline(fin, line)) {
    ++num;
    switch (state) {
    case E_LAY:
      if (!isPrefix(num, line, "{LAY")) return 0;
      contigs_.emplace_back();
      reads = &contigs_.back();
      state = E_TLE;
      break;
    case E_TLE:
      if (!isPrefix(num, line, "{TLE")) return 0;
      reads->emplace_back();
      current = &reads->back();
      state = E_clr;
      break;
    case E_clr:
      if (!isPrefix(num, line, "clr:")) return 0;
      sscanf(line.c_str(), "%d,%d", &current->lo, &current->hi);
      state = E_off;
      break;
    case E_off:
      if (!isPrefix(num, line, "off:")) return 0;
      sscanf(line.c_str(), "%d", &current->offset);
      state = E_src;
      break;
    case E_src:
      if (!isPrefix(num, line, "src:")) return 0;
      sscanf(line.c_str(), "%d", &current->read_id);
      state = E_end;
      break;
    case E_end:
      if (!isPrefix(num, line, "}")) return 0;
      state = E_end_TLE;
      break;
    case E_end_TLE:
      if (line == "}") {
        state = E_LAY;
      } else {
        reads->emplace_back();
        current = &reads->back();
        state = E_clr;
      }
      break;
    }
  }
  return true;
}

string LayoutFile::getError() const {
  return error_;
}

const std::vector< std::vector<LayoutFile::record> >
  LayoutFile::getContigs() const {
  return contigs_;
}


} // namespace acgt
