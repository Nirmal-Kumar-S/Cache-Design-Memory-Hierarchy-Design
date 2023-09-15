#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <inttypes.h>
#include <math.h>
#include <set>
#include <iomanip>
#include "sim.h"

using namespace std;
/*  "argc" holds the number of command-line arguments.
"argv[]" holds the arguments themselves.

Example:
./sim 32 8192 4 262144 8 3 10 gcc_trace.txt
argc = 9
argv[0] = "./sim"
argv[1] = "32"
argv[2] = "8192"
... and so on
*/
int main (int argc, char *argv[]) {
FILE *fp;			// File pointer.
char *trace_file;		// This variable holds the trace file name.
cache_params_t params;	// Look at the sim.h header file for the definition of struct cache_params_t.
char rw;			// This variable holds the request's type (read or write) obtained from the trace.
uint32_t addr;		// This variable holds the request's address obtained from the trace.
// The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.

// Exit with an error if the number of command-line arguments is incorrect.
if (argc != 9) {
printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
exit(EXIT_FAILURE);
}

// "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
params.L1_SIZE   = (uint32_t) atoi(argv[2]);
params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
params.L2_SIZE   = (uint32_t) atoi(argv[4]);
params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
params.PREF_N    = (uint32_t) atoi(argv[6]);
params.PREF_M    = (uint32_t) atoi(argv[7]);
trace_file       = argv[8];

// Open the trace file for reading.
fp = fopen(trace_file, "r");
if (fp == (FILE *) NULL) {
// Exit with an error if file open failed.
printf("Error: Unable to open file %s\n", trace_file);
exit(EXIT_FAILURE);
}

// Print simulator configuration.
printf("===== Simulator configuration =====\n");
printf("BLOCKSIZE:  %u\n", params.BLOCKSIZE);
printf("L1_SIZE:    %u\n", params.L1_SIZE);
printf("L1_ASSOC:   %u\n", params.L1_ASSOC);
printf("L2_SIZE:    %u\n", params.L2_SIZE);
printf("L2_ASSOC:   %u\n", params.L2_ASSOC);
printf("PREF_N:     %u\n", params.PREF_N);
printf("PREF_M:     %u\n", params.PREF_M);
printf("trace_file: %s\n", trace_file);

char str[2];
int L2_flag_print = 0;
bool L1_prefetch_enable = false;
bool L2_prefetch_enable = false;
// Set prefetch enable flag
if(params.L2_SIZE > 0 && params.PREF_M > 0 && params.PREF_N > 0)
{
L2_prefetch_enable = true;
}
else if(params.L1_SIZE > 0 && params.PREF_M > 0 && params.PREF_N > 0)
{
L1_prefetch_enable = true;
}
// Create L1 cache
cache * L1 = NULL;
if(params.L1_SIZE > 0) {
L1 = new cache(params.L1_SIZE,params.L1_ASSOC, params.BLOCKSIZE,L1_prefetch_enable,params.PREF_N,params.PREF_M); //caculate # of sets for L1
}

//Create L2 cache
cache *L2 = NULL;
if(params.L2_SIZE > 0) {
L2 = new cache(params.L2_SIZE,params.L2_ASSOC,params.BLOCKSIZE,L2_prefetch_enable,params.PREF_N,params.PREF_M); //caculate # of sets for L2
L1->setL2_flag(L2);
// L1->setL2_flag(1);
L2_flag_print = 1;
}

// Read requests from the trace file and echo them back.
while (fscanf(fp, "%s %lx", str, &addr) != EOF) 
{	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
rw = str[0];
if (rw == 'r')
read_write_process('r', addr, L1);
else if (rw == 'w')
read_write_process('w', addr, L1);
else {
printf("Error: Unknown request type %c.\n", rw);
exit(EXIT_FAILURE);
}

///////////////////////////////////////////////////////
// Issue the request to the L1 cache instance here.
///////////////////////////////////////////////////////
}
print_cache(1,L1); //print L1 cache

//If L2 cache exists print L2
if(L2_flag_print) {
print_cache(0,L2);
L2_flag_print = 0;
}
if(L1_prefetch_enable)
{
print_prefetch(L1);
}
else if(L2_prefetch_enable)
{
print_prefetch(L2);
}
print_sim_results(L1, L2);
return(0);

}
