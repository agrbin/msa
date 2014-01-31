#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <cstdio>

char buffer[1<<12];
int total = 0;

void count(const char *filename, char *me) {
  int size, ret = 0;
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "%s: %s: can't open file: %s",
        me, filename, strerror(errno));
    return;
  }
  while ((size = read(fd, buffer, sizeof buffer))) {
    if (size <= 0) {
      fprintf(stderr, "%s: %s: can't read from file: %s",
          me, filename, strerror(errno));
      return;
    }
    for (char *it = buffer; it != buffer + size; ret += *it++ == 'n');
  }
  printf("%d 'n' in %s.\n", ret, filename);
  total += ret;
  close(fd);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    count("/dev/stdin", *argv);
  } else {
    for (int i = 1; i < argc; ++i) {
      count(argv[i], *argv);
    }
  }
  if (argc > 2) {
    printf("\n%d 'n' in all files.\n", total);
  }
  return 0;
}
