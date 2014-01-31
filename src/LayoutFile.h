#ifndef ACGT_LAYOUT_FILE_H
#define ACGT_LAYOUT_FILE_H

#include <string>
#include <vector>

namespace acgt {

class LayoutFile {

  struct record {
    int read_id;
    int lo, hi;
    int offset;
  };

 public:
  LayoutFile(std::string filename);
  ~LayoutFile();

  bool initialize();
  std::string getError() const;

  const std::vector< std::vector<record> >
    getContigs() const;

 private:
  bool isPrefix(int line_num,
                std::string &line,
                const std::string &what);

  std::string filename_;
  std::string error_;

  std::vector< std::vector<record> > contigs_;
};

} // namespace
#endif
