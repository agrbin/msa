#include <iostream>

#ifndef NDEBUG
#define EQ(a, b) do {\
  if ((a) != (b)) {\
    std::cerr << "EQ doesn't hold in "; \
    std::cerr << __FILE__ << ':' << __LINE__ << endl; \
    std::cerr << #a " = " << a << std::endl; \
    std::cerr << #b " = " << b << std::endl; \
    exit(1); \
  }} while (0)
#else
#define EQ
#endif
