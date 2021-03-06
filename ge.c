#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "generator.c"
#include "hash.c"

#define MAX_GNA_LENGTH 30
#define N 800			// Population size
#define NGEN 20 			// Number of generations
// #define CXPB 0.9 			// Cross over probability
// #define MUTPR 1.0 			// Mutation probability
#define GMP 0.3 			// Gen mutation probability
int rMGL;
int selectedPopulation[N][MAX_GNA_LENGTH];
int totalPopulation[N*3][MAX_GNA_LENGTH];
uint32_t selectedPopulationEval[N];
uint32_t totalPopulationEval[3*N];
int best[MAX_GNA_LENGTH];
uint32_t bestEval = UINT32_MAX;

long long mutTimeStat;
long long crossTimeStat;
long long selTimeStat;
long long evalTimeStat;
long long diskTimeStat;
long long mapTimeStat;
int incount;
int hashhit;

char totalPopulationExpr[2*N][exprMAXSIZE];
struct table * htable;

int findRangeUsingBinarySearch(uint64_t* arr, int size, int val) {
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

void arrncpy(uint32_t *destArr, uint32_t *sourceArr, int num)
{
	for(int i=0; i<num; i++)
	{	
		destArr[i] = sourceArr[i];
	}
}

void singlePointCrossOver() {
	uint32_t invert[N];
	uint64_t evaluationSteps[N];
	uint64_t sum = 0;
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
	qsort(idxArr, 3*N, sizeof(uint32_t), cmpfunc);
	
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
	qsort(idxArr, 3*N, sizeof(uint32_t), cmpfunc);

	int invert[3*N];
	uint64_t evaluationSteps[3*N];
	uint64_t sum = 0;
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
	uint32_t tempEval[N];
	arrncpy(tempEval, selectedPopulationEval, N);
	arrncpy(tempSel[0], selectedPopulation[0], N*MAX_GNA_LENGTH);

	int sidxArr[N];
	for(int i=0; i<N; i++)
		idxArr[i] = i;
	qsort(idxArr, N, sizeof(uint32_t), cmpfuncS);
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

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return (((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec))/1000000;
}

void evalPop()
{
	struct timespec start, end;
	printf("Mapping time.....");
	fflush(stdout);
	// time_t begin = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &start);
	char prog[exprMAXSIZE+38];
	char allprog[(exprMAXSIZE+38) * 2 * N];
	strcpy(allprog, "");
	for(int i=0; i<2*N; i++)
	{
		char expr[exprMAXSIZE];
		strcpy(expr, "<e><o><e>");
		if(genExpr(expr, totalPopulation[i], MAX_GNA_LENGTH))
		{
			totalPopulationEval[i] = lookup(htable, expr);
			if(totalPopulationEval[i] == -1)
			{
			strcpy(totalPopulationExpr[i], expr);
			sprintf(prog, "int exec%d(int x, int y)\n{\nreturn %s;\n}\n", i, expr);
			strcat(allprog,prog);
			}
			else
			{
				hashhit++;
			}
			
		}
		else 
			// sprintf(prog, "int exec%d(int x, int y)\n{\nreturn %s;\n}\n", i, "65535");
			totalPopulationEval[i] = UINT32_MAX/2;
	}
	// time_t end = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Done, time spent = %ld\n", timespecDiff(&end,&start));
	mapTimeStat+=timespecDiff(&end, &start);

	if(!strlen(allprog))
	{
		printf("Nothing new to evaluate\n");
	}
	else
	{
		printf("Disk I\\O.....");
		fflush(stdout);
		// begin = clock_gettime();
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		FILE *fp;
		char filePath[30];
		sprintf(filePath, "temp/generated_all.c");
		
		fp = fopen(filePath, "w+");
		fputs(allprog, fp);
		fclose(fp);

		char gccCall[100];
		sprintf(gccCall, "gcc -fPIC -O -shared temp/generated_all.c -o temp/individual_all.so");
		system(gccCall);

		void *handle;
		char indvPath[30];
		sprintf(indvPath, "temp/individual_all.so");
		handle = dlopen(indvPath, RTLD_LAZY);
		if (!handle) {
			/* fail to load the library */
			fprintf(stderr, "Error: %s\n", dlerror());
		}

		// end = clock_gettime();
		clock_gettime(CLOCK_MONOTONIC, &end);
		printf("Done, time spent = %ld\n", timespecDiff(&end,&start));
		diskTimeStat+=timespecDiff(&end, &start);
		printf("Eval time.....");
		fflush(stdout);
		// begin = clock_gettime();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(int i=0; i<2*N; i++)
		{
			if(totalPopulationEval[i] == -1)
			{
				totalPopulationEval[i]=evaluate_F(i, handle)+strlen(totalPopulationExpr[i]);
				insert(htable, totalPopulationExpr[i], totalPopulationEval[i]);
				incount++;
			}
		}
		dlclose(handle);
		// end = clock_gettime();
		clock_gettime(CLOCK_MONOTONIC, &end);
		printf("Done, time spent = %ld\n", timespecDiff(&end,&start));
		evalTimeStat+=timespecDiff(&end, &start);
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
	
}




int main(int argc, char** argv)
{
	// init_genrand64(clock_gettime());
	initTestData();
	htable = createTable(2*N);
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
		// showInd(totalPopulation[i],MAX_GNA_LENGTH, totalPopulationEval[i]);
	}
	evalPop();
	int idxArr[N];
	for(int i=0; i<N; i++)
		idxArr[i] = i;
	qsort(idxArr, N, sizeof(uint32_t), cmpfunc);
	for(int i=0; i<N; i++)
	{
		arrncpy(selectedPopulation[i], totalPopulation[idxArr[i]], MAX_GNA_LENGTH);
		selectedPopulationEval[i] = totalPopulationEval[idxArr[i]];	
	}

	for(int i=0; i<NGEN; i++)
	{
	struct timespec start, end;
	printf("------------------\n");
	printf("Iteration %d\n", i);
	// crossover them
	printf("Crossover time......");
	fflush(stdout);
	// time_t begin = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &start);
	singlePointCrossOver();
	// time_t end = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Done, spent time = %ld\n", timespecDiff(&end, &start));
	crossTimeStat+=timespecDiff(&end, &start);
	// mutate
	printf("Mutation time......");
	fflush(stdout);
	// begin = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &start);
	mutation();
	// end = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Done, spent time = %ld\n", timespecDiff(&end, &start));
	mutTimeStat+=timespecDiff(&end, &start);
	evalPop();
	// evaluate and do selection
	printf("Selection time......");
	fflush(stdout);
	// begin = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &start);
	// tSelection();
	selection();
	// end = clock_gettime();
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Done, spent time = %ld\n", timespecDiff(&end, &start));
	selTimeStat+=timespecDiff(&end, &start);
	printf("------------------\n");
	}
	printf("Best: ");
	showInd(best, MAX_GNA_LENGTH, bestEval);
	printf("Avg mutation time: %lld\n", mutTimeStat/NGEN);
	printf("Avg crossover time: %lld\n", crossTimeStat/NGEN);
	printf("Avg selection time: %lld\n", selTimeStat/NGEN);
	printf("Avg evaluation time: %lld\n", evalTimeStat/NGEN);
	printf("Avg disk i\\o time: %lld\n", diskTimeStat/NGEN);
	printf("Avg mapping time: %lld\n", mapTimeStat/NGEN);
	printf("HashHits: %d,  Hash inserts: %d\n", hashhit, incount);
	printf("GE var params: N=%d; NPoints=%d\n", N, EVALPOINTSNUM);
	return 0;
}