#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "generator.c"

const uint64_t MAX_GNA_LENGTH = 100;
const uint64_t N = 100; 	// Population size
const uint64_t NGEN = 500; 	// Number of populations
const float CXPB = 0.9; 	// Cross over probability
const float MUTPR = 0.1; 	// Mutation probability


int main(int argc, char** argv)
{
	init_genrand64(time(NULL));

	// create random instances
	int population[N][MAX_GNA_LENGTH];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < MAX_GNA_LENGTH; j++) {
			population[i][j] = genrand64_int64(); 
			printf("%d \n", population[i][j]);
		}
		printf("////////");
	}

	// crossover them


	// mutate
	// evaluate and do selection

	
	return 0;
}