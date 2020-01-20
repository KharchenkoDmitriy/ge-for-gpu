#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "generator.c"

const uint64_t MAX_GNA_LENGTH = 100;
const uint64_t N = 1000;

int main(int argc, char** argv)
{
	// create random instances
	// crossover them
	// mutate
	// evaluate and do selection

	init_genrand64(time(NULL));
	printf("%llu \n", genrand64_int64());
	return 0;
}