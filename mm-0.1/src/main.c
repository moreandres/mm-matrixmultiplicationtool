#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <syslog.h>
#include <cblas.h>

#include <mpi.h>
#include "config.h"
#include "utils.h"
#include "mm.h"

const char * argp_program_version = PACKAGE_STRING;
const char * argp_program_bug_address = PACKAGE_BUGREPORT;
static char argp_program_doc [] = "\nmm -- Matrix multiplication";
static char argp_args_doc[] = "SIZE METHOD";

static struct argp_option argp_options[] =
  {
    { "verbose", 'v', 0, 0, "Produce verbose output" },
    { "check", 'c', 0, 0, "Double check results" },
    { "block", 'b', "BLOCK", 0, "Use block size BLOCK"},
    { 0 }
  } ;

static error_t argp_parser(int key, char * arg, struct argp_state * state)
{
  int res = 0;
  struct argp_args * args = state->input;
  
  switch (key)
    {
    case 'v':
      args->verbose++;
      break;
    case 'c':
      args->check = 1;
      break;
    case 'b':
      args->block = atoi(arg);
      break;
    case ARGP_KEY_ARG:
      switch (state->arg_num)
	{
	case 0: 
	  args->size = atoi(arg);
	  break;
	case 1:
	  args->method = arg;
	  break;
	default: 
	  argp_usage(state);
	  break;
	}
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2)
	argp_usage(state);
      break;
    default:
      res = ARGP_ERR_UNKNOWN;
    }
  
  return res;
}

static struct argp argp = { argp_options, argp_parser, argp_args_doc, argp_program_doc };

int main(int argc, char * argv[])
{
  int res = -1;
  
  if (MPI_SUCCESS == (res = MPI_Init(&argc,&argv)))
    {
      struct argp_args * args = calloc(1,sizeof(struct argp_args));
      if (0 == (res = argp_parse(&argp, argc, argv, 0, 0, args)))
	{
	  if (!args->block)
	    {
	      int linesize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
	      args->block = linesize / sizeof(double);
	    }
	  
	  double * a = mm_init(args->size);
	  double * b = mm_init(args->size);
	  double * c = calloc(args->size * args->size, sizeof(double));
	
	  if (0 == (res = mm_process(args, a, b, c)))
	    {
	      if (args->check)
		{
		  double * tmp = calloc(args->size * args->size, sizeof(double));
		  
		  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
			      args->size, args->size, args->size,
			      1.0, a, args->size, 
			      b, args->size, 0.0, tmp, args->size);
		  
		  if (0 != (res = mm_check(c, tmp, args->size)))
		    printf("Comparison Failed\n");
		}
	    }
	}
      
      res = MPI_Finalize();
    }
  else
    {
      res = MPI_Abort(MPI_COMM_WORLD, res);
    }
  
  return res;
}

