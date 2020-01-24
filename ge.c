#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "generator.c"

#define MAX_GNA_LENGTH 30
#define N 1000				// Population size
#define NGEN 100 			// Number of generations
// #define CXPB 0.9 			// Cross over probability
// #define MUTPR 1.0 			// Mutation probability
#define GMP 0.3 			// Gen mutation probability
int rMGL;
int selectedPopulation[N][MAX_GNA_LENGTH];
int totalPopulation[N*3][MAX_GNA_LENGTH];
int selectedPopulationEval[N];
int totalPopulationEval[3*N];
int best[MAX_GNA_LENGTH];
int bestEval = UINT16_MAX;

int findRangeUsingBinarySearch(int* arr, int size, int val) {
	// printf("%d - size \n", size);
	// printf("%f - log2(size) \n", log2(size));
	// printf("Random value is %d\n", val);
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

void arrncpy(int *destArr, int *sourceArr, int num)
{
	for(int i=0; i<num; i++)
	{	
		destArr[i] = sourceArr[i];
	}
}

void singlePointCrossOver() {
	int invert[N];
	int evaluationSteps[N];
	long long sum = 0;
	for(int i = 0; i < N; i++) {
		// printf("%d\n", selectedPopulationEval[i]);
		invert[i] = selectedPopulationEval[N-1] / selectedPopulationEval[i];
		sum += invert[i];
		evaluationSteps[i] = sum;
		// printf("%d\n", evaluationSteps[i]);
	}

	for (int i = 0; i < N; i++) {
		int firstParentIndex = findRangeUsingBinarySearch(evaluationSteps, N, ((uint32_t) genrand64_int64()) % (evaluationSteps[N-1] - 1) + 1);
		
		int secondParentIndex = 0;
		
		// do {
			secondParentIndex = findRangeUsingBinarySearch(evaluationSteps, N, ((uint32_t) genrand64_int64()) % (evaluationSteps[N-1] - 1) + 1);	
		// } while(firstParentIndex==secondParentIndex);
		
		// printf("firstParentIndex is %d\n", firstParentIndex);
		// printf("secondParentIndex is %d\n", secondParentIndex);

		// int xPoint = (int)genrand64_int64() % (MAX_GNA_LENGTH/4) 
		// 			- (int)genrand64_int64() % (MAX_GNA_LENGTH/4) 
		// 			+ (int)genrand64_int64() % (MAX_GNA_LENGTH/4) 
		// 			- (int)genrand64_int64() % (MAX_GNA_LENGTH/4);
		// xPoint = abs(xPoint);
		int xPoint = (uint16_t)genrand64_int64() % MAX_GNA_LENGTH;
		// printf("%d\n", xPoint);
		arrncpy(totalPopulation[i*2], selectedPopulation[firstParentIndex], xPoint);
		arrncpy(&totalPopulation[i*2][xPoint], &selectedPopulation[secondParentIndex][xPoint], MAX_GNA_LENGTH - xPoint);		
		// totalPopulationEval[i*2] = evInd(totalPopulation[i*2], MAX_GNA_LENGTH, i*2);

		arrncpy(totalPopulation[i*2+1], selectedPopulation[secondParentIndex], xPoint);
		arrncpy(&totalPopulation[i*2+1][xPoint], &selectedPopulation[firstParentIndex][xPoint], MAX_GNA_LENGTH - xPoint);		
		// totalPopulationEval[i*2+1] = evInd(totalPopulation[i*2+1], MAX_GNA_LENGTH, i*2+1);
		arrncpy(totalPopulation[2*N+i], selectedPopulation[i], MAX_GNA_LENGTH);
		totalPopulationEval[2*N + i] = selectedPopulationEval[i];
	}
}

int cmpfunc (const void * a, const void * b) {
   return ( totalPopulationEval[*(int*)a] - totalPopulationEval[*(int*)b] );
}

int cmpfuncS (const void * a, const void * b) {
   return ( selectedPopulationEval[*(int*)a] - selectedPopulationEval[*(int*)b] );
}



void selection()
{
	int idxArr[3*N];
	for(int i=0; i<3*N; i++)
		idxArr[i] = i;
	qsort(idxArr, 3*N, sizeof(int), cmpfunc);
	
	for(int i = 0; i < N; i++) {
		arrncpy(selectedPopulation[i], totalPopulation[idxArr[i]], MAX_GNA_LENGTH);
		selectedPopulationEval[i] = totalPopulationEval[idxArr[i]];
		// showInd(selectedPopulation[i],MAX_GNA_LENGTH, selectedPopulationEval[i]);
		
	}
	if(totalPopulationEval[idxArr[0]] < bestEval)
		{
			bestEval = totalPopulationEval[idxArr[0]];
			arrncpy(best, totalPopulation[idxArr[0]], MAX_GNA_LENGTH);
		}
}

void tSelection()
{
	int idxArr[3*N];
	for(int i=0; i<3*N; i++)
		idxArr[i] = i;
	qsort(idxArr, 3*N, sizeof(int), cmpfunc);

	int invert[3*N];
	int evaluationSteps[3*N];
	long long sum = 0;
	for(int i = 0; i < 3*N; i++) {
		// printf("%d\n", selectedPopulationEval[i]);
		invert[i] = totalPopulationEval[idxArr[3*N-1]] / totalPopulationEval[idxArr[i]];
		sum += invert[i];
		evaluationSteps[i] = sum;
		// printf("%d\n", evaluationSteps[i]);
	}
	int tournatemtSize = 5;
	for(int i = 0; i < N; i++) {
		int bestIdx = findRangeUsingBinarySearch(evaluationSteps, 3*N, ((uint16_t) genrand64_int64()) % (evaluationSteps[3*N-1] - 1) + 1);
		bestIdx = idxArr[bestIdx];
		int index;
		for(int j = 1; j < tournatemtSize; j++)
		{
			index = findRangeUsingBinarySearch(evaluationSteps, 3*N, ((uint16_t) genrand64_int64()) % (evaluationSteps[3*N-1] - 1) + 1);
			if(totalPopulationEval[idxArr[index]]>totalPopulationEval[bestIdx])
				bestIdx =  idxArr[index];
		}

		arrncpy(selectedPopulation[i], totalPopulation[bestIdx], MAX_GNA_LENGTH);
		selectedPopulationEval[i] = totalPopulationEval[bestIdx];
		showInd(selectedPopulation[i],MAX_GNA_LENGTH, selectedPopulationEval[i]);
		
	}
	int tempSel[N][MAX_GNA_LENGTH];
	int tempEval[N];
	arrncpy(tempEval, selectedPopulationEval, N);
	arrncpy(tempSel[0], selectedPopulation[0], N*MAX_GNA_LENGTH);

	int sidxArr[N];
	for(int i=0; i<N; i++)
		idxArr[i] = i;
	qsort(idxArr, N, sizeof(int), cmpfuncS);
	for(int i=0; i<N; i++)
	{
		arrncpy(selectedPopulation[i], tempSel[idxArr[i]], MAX_GNA_LENGTH);
		selectedPopulationEval[i] = tempEval[idxArr[i]];
	}


	if(selectedPopulationEval[0] < bestEval)
		{
			bestEval = selectedPopulationEval[0];
			arrncpy(best, selectedPopulation[0], MAX_GNA_LENGTH);
		}
}

void evalPop()
{
	char prog[exprMAXSIZE+38];
	char allprog[(exprMAXSIZE+38) * 2 * N];
	strcpy(allprog, "");
	for(int i=0; i<2*N; i++)
	{
		char expr[exprMAXSIZE];
		strcpy(expr, "<e><o><e>");
		if(genExpr(expr, totalPopulation[i], MAX_GNA_LENGTH))
			sprintf(prog, "int exec%d(int x, int y)\n{\nreturn %s;\n}\n", i, expr);
		else 
			sprintf(prog, "int exec%d(int x, int y)\n{\nreturn %s;\n}\n", i, "65535");
		strcat(allprog,prog);
		totalPopulationEval[i] = strlen(expr);
	}

	FILE *fp;
	char* filePath = malloc(30 * sizeof(char));
	sprintf(filePath, "temp/generated_all.c");
	
	fp = fopen(filePath, "w+");
	fputs(allprog, fp);
	fclose(fp);

	char* gccCall = malloc(100 * sizeof(char));
	sprintf(gccCall, "gcc -fPIC -O -shared temp/generated_all.c -o temp/individual_all.so");
	system(gccCall);

	for(int i=0; i<2*N; i++)
	{
		totalPopulationEval[i]+=evaluate_F(i);
	}
}


void mutation()
{
	for(int i=0; i<2*N; i++)
	{
		// if((uint16_t) genrand64_int64()%UINT16_MAX * 1.0 < (UINT16_MAX * MUTPR))
			{
				for(int j=0; j<MAX_GNA_LENGTH; j++)
				{
					if((uint16_t) genrand64_int64() * 1.0 < (UINT16_MAX * GMP))
						totalPopulation[i][j] = (uint16_t) genrand64_int64();
				}
				// totalPopulationEval[i] = evInd(totalPopulation[i], MAX_GNA_LENGTH, i);
			}
	}
	evalPop();
}


int main(int argc, char** argv)
{
	// init_genrand64(time(NULL));
	initTestData();
	rMGL = sqrt(sqrt(MAX_GNA_LENGTH * 1.0));
	// create random instances
	printf("Creating random instances\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < MAX_GNA_LENGTH; j++) {
			totalPopulation[i][j] = abs((int) genrand64_int64()); 
			// printf("%d \n", selectedPopulation[i][j]);
		}
		// selectedPopulationEval[i] = i*2;
		// totalPopulationEval[i] = evInd(totalPopulation[i], MAX_GNA_LENGTH, i);
		showInd(totalPopulation[i],MAX_GNA_LENGTH, totalPopulationEval[i]);
	}
	evalPop();
	int idxArr[N];
	for(int i=0; i<N; i++)
		idxArr[i] = i;
	qsort(idxArr, N, sizeof(int), cmpfunc);
	for(int i=0; i<N; i++)
	{
		arrncpy(selectedPopulation[i], totalPopulation[idxArr[i]], MAX_GNA_LENGTH);
		selectedPopulationEval[i] = totalPopulationEval[idxArr[i]];	
	}

	for(int i=0; i<NGEN; i++)
	{
	printf("------------------\n");
	printf("Iteration %d\n", i);
	// crossover them
	printf("Crossover time\n");
	singlePointCrossOver();
	// mutate
	printf("Mutation time\n");
	mutation();
	// evaluate and do selection
	printf("Selection time\n");
	// tSelection();
	selection();
	printf("------------------\n");
	}
	printf("Best: ");
	showInd(best, MAX_GNA_LENGTH, bestEval);

	return 0;
}