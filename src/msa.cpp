#include "MultipleAligner.h"
#include "FastaFile.h"
#include "LayoutFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

using namespace acgt;
using namespace std;

void usage(char *path) {
  printf(
    "usage: %s [-p] [-e EPSILON] [-m MAX_RADIUS] reads-fasta layout-afg\n"
    "\t-p is no-pack, disable packing non-interscted reads to same rows\n"
    "\t-e (=0.001) EPSILON, "
      "band size in edit-distance as percent of maximum offset\n"
    "\t-m (=300) MAX_RADIUS, maximum band size to use\n", path);
  exit(1);
}

void scan_args(int argc, char **argv,
    float *epsilon, int *max_radius, bool *pack) {
  int opt;
  while ((opt = getopt(argc, argv, "pe:m:")) != -1) {
    switch (opt) {
      case 'p':
        *pack = 0;
        break;
      case 'e':
        sscanf(optarg, "%f", epsilon);
        break;
      case 'm':
        sscanf(optarg, "%d", max_radius);
        break;
      default:
        usage(*argv);
    }
  }
}

int doit(
    float epsilon,
    int max_radius,
    bool pack,
    const char *fasta_file,
    const char *layout_file,
    const char *cons_file = "consensus.fasta") {
  FastaFile fasta(fasta_file);
  FastaFile output(cons_file);
  LayoutFile layout(layout_file);
  // erase contents of cons_file
  fclose(fopen(cons_file, "w"));

  if (!fasta.initialize()) {
    fprintf(stderr, "%s", fasta.getError().c_str());
    return 1;
  }

  if (!layout.initialize()) {
    fprintf(stderr, "%s", layout.getError().c_str());
    return 1;
  }

  auto contigs = layout.getContigs();
  for (const auto& contig : contigs) {
    printf("processing new contig with %ld reads..\n", contig.size());
    MultipleAligner MA(epsilon, max_radius, pack);
    for (auto read : contig) {
      auto read_str = fasta.getRead(read.read_id);
      int size = read.hi - read.lo; // can be neg!
      if (read.hi < read.lo) {
        swap(read.lo, read.hi);
      }
      MA.addSequence(
          read_str.first + read.lo,
          size,
          read.offset);
    }
    output.appendRead(MA.getConsensus(NULL));
  }
  return 0;
}

int main(int argc, char **argv) {
  float epsilon = 0.001;
  int max_radius = 300;
  bool pack = true;
  char *path = *argv;
  {
    scan_args(argc, argv, &epsilon, &max_radius, &pack);
    argv += optind;
    argc -= optind;
  }
  if (argc != 2) {
    fprintf(stderr, "expected two arguments after options.\n\n");
    usage(path);
  }
  return doit(epsilon, max_radius, pack,
      argv[0], argv[1]);
}

