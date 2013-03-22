#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

/*
 * This program use the cpuid instruction to gather information from
 * the CPU features. The --dump switch can be used to show also all gathered
 * register values, instead of only values recognized as a specific feature
 */

/* CPUID register can be standard or extended, they have different bases */
#define STD_BASE 0x0
#define EXT_BASE 0x80000000

/* Case-insensitive option to enable dump of register values */
#define DUMP "--dump"

/* Documentation pointer to be showed during each execution */
char *header =
    "CPUID is documented in the Intel(R) 64 and IA-32 Software Developer\n"
    "Manual Volume 2A Instruction Set A-M [doc #253666 on intel.com]\n";

/* Feature mapping between bits and their description */
typedef struct feature_s {
	unsigned int mask;
	char *msg;
} feature_t;

/* To avoid using max/count variables, arrays must end as below */

/* Features found at EAX register read from STD 0x6 */
feature_t std_eax_0x6[] = {
	{ 0x00000001, "DIGTEMP", },	/* [0] */
	{ 0x00000002, "TRBOBST", },	/* [1] */
	{ 0x00000004, "ARAT", },        /* [2] */
	{ -1, NULL, },
};

/* Features found at ECX register read from STD 0x1 */
feature_t std_ecx_0x1[] = {
	{ 0x00000001, "SSE3", },	/* [0] */
	{ 0x00000002, "PCLMULQDQ", },	/* [1] */
	{ 0x00000004, "DTES64", },	/* [2] */
	{ 0x00000008, "MONITOR", },	/* [3] */
	{ 0x00000010, "DS-CPL", },	/* [4] */
	{ 0x00000020, "VMX", },	        /* [5] */
	{ 0x00000040, "SMX", },	        /* [6] */
	{ 0x00000080, "EIST", },	/* [7] */
	{ 0x00000100, "TM2", },	        /* [8] */
	{ 0x00000200, "SSSE3", },	/* [9] */
	{ 0x00000400, "CNXT-ID", },	/* [10] */
	{ 0x00001000, "FMA", },         /* [12] */
	{ 0x00002000, "CMPXCHG16B", },	/* [13] */
	{ 0x00004000, "XTPR", },	/* [14] */
	{ 0x00008000, "PDCM", },	/* [15] */
	{ 0x00040000, "DCA", },	        /* [18] */
	{ 0x00080000, "SSE4.1", },	/* [19] */
	{ 0x00100000, "SSE4.2",	},      /* [20] */
	{ 0x00200000, "x2APIC",	},      /* [21] */
	{ 0x00400000, "MOVBE", },	/* [22] */
	{ 0x00800000, "POPCNT", },	/* [23] */
	{ 0x02000000, "AES", },	        /* [25] */
	{ 0x04000000, "XSAVE", },       /* [26] */
	{ 0x08000000, "OSXSAVE", },     /* [27] */
	{ 0x10000000, "AVX", },         /* [28] */
	{ -1, NULL, },
};

/* Features found at ECX register read from STD 0x1 */
feature_t std_ecx_0x6[] = {
	{ 0x00000001, "MPERF-APERF-MSR", }, /* [0] */
	{ -1, NULL, },
};

/* Features found at EDX register read from STD 0x1 */
feature_t std_edx_0x1[] = {
	{ 0x00000001, "FPU", },	  /* [0] */
	{ 0x00000002, "VME", },	  /* [1] */
	{ 0x00000004, "DE", },	  /* [2] */
	{ 0x00000008, "PSE", },	  /* [3] */
	{ 0x00000010, "TSC", },	  /* [4] */
	{ 0x00000020, "MSR", },	  /* [5] */
	{ 0x00000040, "PAE", },	  /* [6] */
	{ 0x00000080, "MCE", },	  /* [7] */
	{ 0x00000100, "CX8", },	  /* [8] */
	{ 0x00000200, "APIC", },  /* [9] */
	{ 0x00000800, "SEP", },	  /* [11] */
	{ 0x00001000, "MTRR", },  /* [12] */
	{ 0x00002000, "PGE", },	  /* [13] */
	{ 0x00004000, "MCA", },	  /* [14] */
	{ 0x00008000, "CMOV", },  /* [15] */
	{ 0x00010000, "PAT", },	  /* [16] */
	{ 0x00020000, "PSE36", }, /* [17] */
	{ 0x00040000, "PSN", },	  /* [18] */
	{ 0x00080000, "CLFSH", }, /* [19] */
	{ 0x00200000, "DS", },	  /* [21] */
	{ 0x00400000, "ACPI", },  /* [22] */
	{ 0x00800000, "MMX", },	  /* [23] */
	{ 0x01000000, "FXSR", },  /* [24] */
	{ 0x02000000, "SSE", },	  /* [25] */
	{ 0x04000000, "SSE2", },  /* [26] */
	{ 0x08000000, "SS", },	  /* [27] */
	{ 0x10000000, "HTT", },	  /* [28] */
	{ 0x20000000, "TM", },	  /* [29] */
	{ 0x80000000, "PBE", },	  /* [31] */
	{ -1, NULL, },
};

/* Features found at ECX register read from EXT 0x1 */
feature_t ext_ecx_0x1[] = {
	{ 0x00000001, "LAHF-SAHF", },	/* [0] */
	{ -1, NULL, },
};

/* Features found at EDX register read from EXT 0x1 */
feature_t ext_edx_0x1[] = {
	{ 0x00000800, "SYSCALL", }, /* [11] */
	{ 0x00100000, "XD", },	    /* [20] */
	{ 0x08000000, "RDTSCP",	},  /* [27] */
	{ 0x20000000, "EM64T", },   /* [29] */
	{ -1, NULL, },
};

/* Features found at EDX register read from EXT 0x7 */
feature_t ext_edx_0x7[] = {
	{ 0x00000100, "INVTSC",	}, /* [8] */
	{ -1, NULL, },
};

/* Structure used to store register values */
typedef struct regs_s {
	int eax, ebx, ecx, edx;
} regs_t;

/* Print feature found on this register value according to the mapping */
static void show(const char *msg, int reg, const feature_t * feature)
{
	int i = 0;

	if (NULL != msg)
		printf("%s", msg);

	while (NULL != feature[i].msg) {

		if (reg & feature[i].mask)
			printf("%s ", feature[i].msg);
		i++;
	}
	printf("\n");
}

/* Assembler wrapper, read data registers located at the requested address */
static int get(regs_t *regs, int base)
{
	int res = -1;
	int eax, ebx, ecx, edx;

	if (NULL != regs) {

		asm("mov %4, %%eax;"
		    "cpuid;"
		    "mov %%eax, %0;"
		    "mov %%ebx, %1;"
		    "mov %%ecx, %2;"
		    "mov %%edx, %3;"
		    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
		    : "r"(base)
		    : "%eax", "%ebx", "%ecx", "%edx");

		regs->eax = eax;
		regs->ebx = ebx;
		regs->ecx = ecx;
		regs->edx = edx;

		res = 0;
	}

	return res;
}

/* Load registers from base address up to max */
static regs_t *load(int base, int max)
{
	int res;
	regs_t *regs = calloc(max - base, sizeof(regs_t));

	if (NULL != regs) {

		int i;
		for (i = base + 1; i < max; i++) {

			res = get(&(regs[i - base]), i);
			if (res == -1) {
				free(regs);
				regs = NULL;
				goto out;
			}
		}
	}
 out:
	return regs;
}

/*
 * Gets max address for both STD and EXT CPUID registers using get(), then
 * extract their values using load(), then show their mapping into features
 */
int main(int argc, char **argv)
{
	int res = -1;

	/* to store register values */
	regs_t *std_regs = NULL;
	regs_t *ext_regs = NULL;

	/* to store max address */
	int std_max;
	int ext_max;
	regs_t *regs = calloc(1, sizeof(regs_t));
	if (NULL == regs)
		goto out;

	/* get max STD address */
	res = get(regs, STD_BASE);
	if (-1 == res)
		goto out;
	std_max = regs->eax;

	/* get max EXT address */
	res = get(regs, EXT_BASE);
	if (-1 == res)
		goto out;
	ext_max = regs->eax;

	/* load STD/EXT registers into tables */
	std_regs = load(STD_BASE, std_max);
	if (NULL == std_regs)
		goto out;

	ext_regs = load(EXT_BASE, ext_max);
	if (NULL == ext_regs)
		goto out;

	/* show known features according to mappings above */
	printf("\n%s\n", header);
	show("CPUID STD 1 ECX: ", std_regs[1].ecx, std_ecx_0x1);
	show("CPUID STD 1 EDX: ", std_regs[1].edx, std_edx_0x1);
	show("CPUID STD 6 EAX: ", std_regs[6].eax, std_eax_0x6);
	show("CPUID STD 6 ECX: ", std_regs[6].ecx, std_ecx_0x6);
	printf("\n");
	show("CPUID EXT 6 ECX: ", ext_regs[1].ecx, ext_ecx_0x1);
	show("CPUID EXT 1 EDX: ", ext_regs[1].edx, ext_edx_0x1);
	show("CPUID EXT 7 EDX: ", ext_regs[7].edx, ext_edx_0x7);
	printf("\n");

	/* if dump was requested, show all values without translation */
	if (argc == 2 && !strncasecmp(argv[1], DUMP, sizeof(DUMP))) {

		int i;
		for (i = 0; i < std_max; i++) {
			printf("CPUID STD %d ALL: %#x %#x %#x %#x\n",
			       i,
			       std_regs[i].eax,
			       std_regs[i].ebx,
			       std_regs[i].ecx,
			       std_regs[i].edx);
		}
		printf("\n");

		for (i = 0; i < (ext_max - EXT_BASE); i++) {
			printf("CPUID EXT %d ALL: %#x %#x %#x %#x\n",
			       i,
			       ext_regs[i].eax,
			       ext_regs[i].ebx,
			       ext_regs[i].ecx,
			       ext_regs[i].edx);
		}
		printf("\n");
	}
 out:
	free(regs);
	free(std_regs);
	free(ext_regs);

	return res;
}
