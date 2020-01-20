#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "generator.c"

#define MAX_GNA_LENGTH 5
#define N 5 				// Population size
#define NGEN 500 			// Number of generations
#define CXPB 0.9 			// Cross over probability
#define MUTPR 0.1 			// Mutation probability

int selectedPopulation[N][MAX_GNA_LENGTH];
int totalPopulation[N*3][MAX_GNA_LENGTH];
int slectedPopulationEval[N];

int findRangeUsingBinarySearch(int* arr, int size, int val) {
	// printf("%d - size \n", size);
	// printf("%f - log2(size) \n", log2(size));
	printf("Random value is %d\n", val);
	int from = 0;
	int to = size;
	while(to - from > 1) {

		int index = (from + to) / 2;
		
		if(val > arr[index])
			from = index;
		else
			to = index;
	}
	return to;
}

void singlePointCrossOver() {
	int evaluationSteps[N];
	int sum = 0;
	for(int i = 0; i < N; i++) {
		sum += slectedPopulationEval[i];
		evaluationSteps[i] = sum;
		printf("%d\n", evaluationSteps[i]);
	}

	for (int i = 0; i < N; i++) {
		int firstParentIndex = findRangeUsingBinarySearch(evaluationSteps, N, ((uint16_t) genrand64_int64()) % (evaluationSteps[N-1] - 1) + 1);
		
		int secondParentIndex = 0;
		
		do {
			secondParentIndex = findRangeUsingBinarySearch(evaluationSteps, N, ((uint16_t) genrand64_int64()) % (evaluationSteps[N-1] - 1) + 1);	
		} while(firstParentIndex==secondParentIndex);
		
		printf("firstParentIndex is %d\n", firstParentIndex);
		printf("secondParentIndex is %d\n", secondParentIndex);

		
	}
}

int main(int argc, char** argv)
{
	init_genrand64(time(NULL));

	// create random instances
	printf("Creating random instances\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < MAX_GNA_LENGTH; j++) {
			selectedPopulation[i][j] = genrand64_int64(); 
			printf("%d \n", selectedPopulation[i][j]);
		}
		slectedPopulationEval[i] = i*2;
		//slectedPopulationEval[i] = evInd(selectedPopulation[i], MAX_GNA_LENGTH, i);
	}

	// crossover them
	printf("Crossover time\n");
	singlePointCrossOver();

	// mutate
	// evaluate and do selection

	
	return 0;
}