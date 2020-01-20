#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "generator.c"

const uint64_t MAX_GNA_LENGTH = 5;
const uint64_t N = 1; 	// Population size
const uint64_t NGEN = 500; 	// Number of populations
const float CXPB = 0.9; 	// Cross over probability
const float MUTPR = 0.1; 	// Mutation probability

int selectedPopulation[N][MAX_GNA_LENGTH];
int totalPopulation[N*3][MAX_GNA_LENGTH];
int slectedPopulationEval[N];

void singlePointCrossOver(void){

}

int main(int argc, char** argv)
{
	init_genrand64(time(NULL));

	// create random instances
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < MAX_GNA_LENGTH; j++) {
			selectedPopulation[i][j] = genrand64_int64(); 
			printf("%d \n", selectedPopulation[i][j]);
		}
		slectedPopulationEval[i] = evInd(selectedPopulation[i], MAX_GNA_LENGTH, i);
		printf("////////");
	}

	// evaluate initial instances



	// crossover them


	// mutate
	// evaluate and do selection

	
	return 0;
}