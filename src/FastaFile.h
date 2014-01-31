#ifndef ACGT_FASTA_FILE_H
#define ACGT_FASTA_FILE_H

#include <string>
#include <vector>

namespace acgt {

class FastaFile {
 public:
  FastaFile(std::string filename);
  ~FastaFile();

  bool appendRead(const std::string t);

  bool initialize();
  std::string getError() const;
  std::pair<const char*, size_t> getRead(int id) const;

 private:
  bool loadFasta(const char *filename);
  bool parseFasta();
  static size_t fileSize(int fd);

  std::string filename_;
  const char *file_;
  int appended_reads_;

  std::string error_;
  size_t file_size_;
  std::vector< std::pair<off_t, size_t> > reads_;
};

} // namespace
#endif
