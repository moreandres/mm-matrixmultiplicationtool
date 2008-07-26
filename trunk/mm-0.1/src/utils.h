#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define BUG(fmt, args...) do { printf("%s:%d:%s() ", __FILE__, __LINE__, __FUNCTION__); \
    printf(fmt, ##args); printf("\n"); abort(); } while (0)
#define DBG(fmt, args...) do { printf("%s:%d:%s ", __FILE__, __LINE__, __FUNCTION__); \
    printf(fmt, ##args); printf("\n"); } while (0)
#define VERB(fmt, args...) do { ; printf(fmt, ##args); printf("\n"); } while (0)

struct argp_args
{
  int mpi_size;
  int mpi_rank;
  int check;
  int verbose;
  int size;
  int block;
  char * method;
};

#define FLOPS(x, t) ( ( (2 * x * x * x) - (x * x) ) ) / ( t * 1000000)

#endif /* UTILS_H */
