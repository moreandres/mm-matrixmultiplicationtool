#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <cblas.h>
#include <mpi.h>

#include "config.h"

#include "utils.h"
#include "mm.h"

int mm_method(struct argp_args * args)
{
  assert(args != NULL);

  int res = -1;

  char * methods[] = { "simple", "block", "transp", "cblas", "mpi", NULL };
  
  int i = 0;
  while (methods[i]!=NULL)
    {
      if (! strncasecmp(methods[i], args->method, sizeof(methods[i])))
	{
	  res = i;
	  break;
	}
      i++;
    }

  return res;
}

double * mm_init(int n)
{
  assert(n > 0);

  double * res = NULL;
  
  res = malloc(n * n * sizeof(double));

  srandom(time(NULL));
  
  int i,j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      res[i * n + j] = 1.0 * random() / RAND_MAX;
  
  return res;
}

int mm_mpi(double * a, double * b, double * c, int n)
{
  assert(a != NULL && b != NULL && c != NULL && n > 0);

  int res = -1;
  
  int size = -1;
  if (MPI_SUCCESS == (res = MPI_Comm_size(MPI_COMM_WORLD,&size)))
    {
      assert(0 == n % (size - 1));
      
      int rank = -1;
      if (MPI_SUCCESS == (res = MPI_Comm_rank(MPI_COMM_WORLD,&rank)))
	{
	  MPI_Status status;
  
	  if (0 == rank)
	    {
	      int i;
	      int rows = n / (size - 1);
	      int offset = 0;
	      for (i = 1; i < size; i++)
		{
		  int res = -1;
		  
		  res = MPI_Send (&a[offset * n + 0], rows * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		  res = MPI_Send (&b[0], n * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		  offset += rows;
		}
	      
	      for (i = 1; i < size; i++)
		{
		  offset = rows * (i - 1);
		  MPI_Recv (&c[offset * n + 0], rows * n, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
		}
	      
	      res = 0;
	    }
	  
	  if (0 < rank)
	    {
	      int rows = n / (size - 1);
	      
	      int start = rows * (rank - 1);
	      int end = rows * rank;
	      
	      res = MPI_Recv (&a[0], rows * n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
	      res = MPI_Recv (&b[0], n * n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
	      
	      int i,j,k;
	      for (i = start; i < end; i++)
		for (k = 0; k < n; k++)
		  for (j = 0; j < n; j++)
		    c[i * n + k] += a[i * n + j] * b[j * n + k];
	      
	      cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
			  rows, n, n,
			  1.0, a, n, 
			  b, n, 0.0, c, n);
	      
	      res = MPI_Send (&c[0], rows * n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	      res = MPI_Finalize();
	      exit(0);
	    }
	}
    }
  return res;
}

int mm_simple(double * a, double * b, double * c, int n)
{
  assert(a != NULL && b != NULL && c !=NULL && n > 0);
    
  int i,j,k;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
	c[i * n + j] += a[i * n + k] * b[k * n + j];

  return 0;
}

double wtime(void)
{
  double sec;
  struct timeval tv;
  
  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

int mm_check(double * a, double * b, int n)
{
  assert(a != NULL && b != NULL && n > 0);
  
  int i,j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      {
	if (fabs(a[i * n + j] - b[i * n + j]) > 0.000001)
	  {
	    return -1;
	  }
      }
  
  return 0;
}

int mm_cblas(double * a, double * b, double * c, int n)
{
  assert(a != NULL && b != NULL && c != NULL && n > 0);
  
  int res = 0;
  
   cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	       n, n, n,
	       1.0, a, n, 
	       b, n, 0.0, c, n);
   
  return res;
}

int mm_transp(double * a, double * b, double * c, int n)
{
  assert(a != NULL && b != NULL && c != NULL && n > 0);
  
  int res = -1;
  
  double * tmp = malloc(n * n * sizeof(double));
  
  int i,j,k;
  
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      tmp[i * n + j] = b[j * n + i];
  
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
	c[i * n + j] += a[i * n + k] * tmp[j * n + k];
  
  return res;
}

int mm_block_sub(double * a, double * b, double * c, int n, int block)
{
  assert(a != NULL && b != NULL && c != NULL);
  assert(n > 0 && block > 0);
  assert(0 == n % block);
  
  int i,j,k;
  for (i = 0; i < block; i++)
    for (j = 0; j < block; j++)
      for (k = 0; k < block; k++)
	c[i * n + j] += a[i * n + k] * b[k * n + j];
  
  return 0;
}

int mm_block(double * a, double * b, double * c, int n, int block)
{
  assert(a != NULL && b != NULL && c != NULL);
  assert(n > 0 && block >= 0);
  assert(0 == n % block);

  int res = -1;

  int i,j,k;
  for (i = 0; i < n; i += block)
    for (j = 0; j < n; j += block)
      for (k = 0; k < n; k += block)
	mm_block_sub(&a[i * n + k], &b[k * n + j], 
		     &c[i * n + j], n, block);
  
  
  return res;
}

int mm_process(struct argp_args * args, double * a, double * b, double * c)
{
  assert(args != NULL);
  
  int res = -1;
  double time = -1;
  
  int method = -1;
  if (-1 != (method = mm_method(args)))
    {
      
       time = wtime();
       
       switch (method)
	 {
	 case 0:
	   res = mm_simple(a, b, c, args->size);
	   break;
	 case 1:
	   res = mm_block(a, b, c, args->size, args->block);
	   break;
	 case 2:
	   res = mm_transp(a, b, c, args->size);
	   break;
	 case 3:
	   res = mm_cblas(a, b, c, args->size);
	   break;
	 case 4:
	   res = mm_mpi(a, b, c, args->size);
	   break;
	 default:
	   BUG("Method");
	   break;
	 }
       
       time = wtime() - time;
       
       int size;
       MPI_Comm_size(MPI_COMM_WORLD,&size);

       printf("%s,%d,%f,", args->method, args->size, time);
       printf("%f,%d,%d\n", FLOPS(1.0 * args->size, time), args->block, size);
    }
  else 
    {
      printf("Unsupported method\n");
    }
    
  return res;
}
