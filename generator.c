#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "mtgen.c"

#define exprMAXSIZE 200

void genCode(char *expr, int indvNum)
{
	char prog[38+exprMAXSIZE];
	sprintf(prog, "int exec(int x, int y)\n{\nreturn %s;\n}", expr);

	FILE *fp;
	char filePath[30];
	sprintf(filePath, "temp/generated%d.c", indvNum);
	
	fp = fopen(filePath, "w+");
	fputs(prog, fp);
	fclose(fp);

	char gccCall[100];
	sprintf(gccCall, "gcc -fPIC -O -shared temp/generated%d.c -o temp/individual%d.so", indvNum,indvNum);
	system(gccCall);
}

int runIndv(int indvNum, int x, int y)
{
	void *handle;
	int (*func_print_name)(int,int);

	char indvPath[30];
	sprintf(indvPath, "temp/individual%d.so", indvNum);
	handle = dlopen(indvPath, RTLD_LAZY);
	if (!handle) {
		/* fail to load the library */
		fprintf(stderr, "Error: %s\n", dlerror());
		return EXIT_FAILURE;
	}

	*(int**)(&func_print_name) = dlsym(handle, "exec");
	if (!func_print_name) {
		/* no such symbol */
		fprintf(stderr, "Error: %s\n", dlerror());
		dlclose(handle);
		return EXIT_FAILURE;
	}

	int res =  func_print_name(x,y);
	dlclose(handle);
	return res;
}

int runInd(int indvNum, int x, int y)
{
	void *handle;
	int (*func_print_name)(int,int);

	char indvPath[30];
	sprintf(indvPath, "temp/individual%d.so", indvNum);
	handle = dlopen(indvPath, RTLD_LAZY);
	if (!handle) {
		/* fail to load the library */
		fprintf(stderr, "Error: %s\n", dlerror());
		return EXIT_FAILURE;
	}

	*(int**)(&func_print_name) = dlsym(handle, "exec");
	if (!func_print_name) {
		/* no such symbol */
		fprintf(stderr, "Error: %s\n", dlerror());
		dlclose(handle);
		return EXIT_FAILURE;
	}

	int res =  func_print_name(x,y);
	dlclose(handle);
	return res;
}

int runInd_F(int indvNum, int x, int y)
{
	void *handle;
	int (*func_print_name)(int,int);

	char indvPath[30];
	sprintf(indvPath, "temp/individual_all.so");
	handle = dlopen(indvPath, RTLD_LAZY);
	if (!handle) {
		/* fail to load the library */
		fprintf(stderr, "Error: %s\n", dlerror());
		return EXIT_FAILURE;
	}
	char funcName[10];
	sprintf(funcName, "exec%d", indvNum);
	*(int**)(&func_print_name) = dlsym(handle, funcName);
	if (!func_print_name) {
		/* no such symbol */
		fprintf(stderr, "Error: %s\n", dlerror());
		dlclose(handle);
		return EXIT_FAILURE;
	}

	int res =  func_print_name(x,y);
	dlclose(handle);
	return res;
}

int idealfunc(int x, int y)
{
	return x*x + x*y+1;
}

#define EVALPOINTSNUM 10
int x[EVALPOINTSNUM];
int y[EVALPOINTSNUM];
void initTestData()
{
	for(int i=0; i < EVALPOINTSNUM; i++)
	{
		x[i] = (int)genrand64_int64()%20;
		y[i] = (int)genrand64_int64()%20;
	}
}
int evaluate(int indvNum)
{
	long long cumdif = 0;
	long dif = 0;
	for(int i=0; i < EVALPOINTSNUM; i++)
	{
		// if(idealfunc(x,y) - runIndv(indvNum,x,y))
			// dif++;
		dif = idealfunc(x[i],y[i]) - runIndv(indvNum,x[i],y[i]);
		cumdif += abs(dif);
		// if(!dif)
			// cumdif++;
	}
	// cumdif /= EVALPOINTSNUM;
	if(cumdif < 0) return 1;
	if(cumdif > UINT16_MAX) return UINT16_MAX;
	// cumdif = UINT32_MAX - cumdif;
	// if(cumdif < 0) cumdif = 0;
	// return cumdif + 1;
	return cumdif;
}

int evaluate_F(int indvNum)
{
	long long cumdif = 0;
	long dif = 0;
	for(int i=0; i < EVALPOINTSNUM; i++)
	{
		// if(idealfunc(x,y) - runIndv(indvNum,x,y))
			// dif++;
		dif = idealfunc(x[i],y[i]) - runInd_F(indvNum,x[i],y[i]);
		cumdif += abs(dif);
		// if(!dif)
			// cumdif++;
	}
	// cumdif /= EVALPOINTSNUM;
	if(cumdif < 0) return 1;
	if(cumdif > UINT16_MAX) return UINT16_MAX;
	// cumdif = UINT32_MAX - cumdif;
	// if(cumdif < 0) cumdif = 0;
	// return cumdif + 1;
	return cumdif;
}


// BNF grammar
int genExpr(char *expr, int* DNA, int DNASize)
{
	int * p_DNA = DNA;
	char * token = expr;
	// printf("start expr: %s\n", token);
	while(token = strchr(expr, '<'))
	{
		int npos = token-expr;
		token[0] = 0;
		char left[exprMAXSIZE];
		char right[exprMAXSIZE];
		strcpy(right, &token[3]);
		strcpy(left, expr);
		// printf("r: %s\n", right);
		// printf("l: %s\n", left);
		switch (token[1])
		{
		case 'e':
			switch (p_DNA[0]%2)
			{
			case 0:
				if(strlen(left)+strlen("(<e><o><e>)") > exprMAXSIZE)
					return 0;
				strcat(left, "(<e><o><e>)");
				break;
			case 1:
				strcat(left, "<v>");
				break;
			default:
				printf("invalid dna: %d\n", p_DNA[0]%2);
				break;
			}
			break;
		case 'v':
			switch (p_DNA[0]%3)
			{
			case 0:
				strcat(left, "x");
				break;
			case 1:
				strcat(left, "y");
				break;
			case 2:
				strcat(left, "1");
				break;
			default:
				printf("invalid dna: %d\n", p_DNA[0]%3);
				break;
			}            
			break;
		case 'o':
			switch (p_DNA[0]%3)
			{
			case 0:
				strcat(left, "+");
				break;
			case 1:
				strcat(left, "-");
				break;
			case 2:
				strcat(left, "*");
				break;
			default:
				printf("invalid dna: %d\n", p_DNA[0]%3);
				break;
			}   
			break;
		default:
			break;
		}
		if(strlen(left)+strlen(right) > exprMAXSIZE)
			return 0;
		strcat(left, right);
		strcpy(expr, left);
		// printf("expr: %s\n", expr);
		if(p_DNA == &DNA[DNASize-1])
			p_DNA = DNA;
			else
			{
				p_DNA++;
			}
		  
	}
	// printf("expr: %s\n", expr);
	return 1;
	
}

void showInd(int* dna, int dnaSize, int eval)
{
	char expr[exprMAXSIZE];
	strcpy(expr, "<e><o><e>");
	genExpr(expr, dna, dnaSize);
	printf("dna: ");
	for(int i=0; i< dnaSize; i++)
		printf("%d", dna[i]%6);
	printf(" ||| ");
	printf("expr: %s ||| ", expr);
	printf("eval: %d\n", eval);
}

int evInd(int* dna, int dnaSize, int indNum)
{
	char expr[exprMAXSIZE];
	strcpy(expr, "<e><o><e>");
	if(!genExpr(expr, dna, dnaSize))
		return UINT16_MAX;
	else
	{
		genCode(expr, indNum);
		int ev = evaluate(indNum) + strlen(expr);
		// printf("dna: ");
		// for(int i=0; i< dnaSize; i++)
		// 	printf("%d", dna[i]%6);
		// printf(" ||| ");
		// printf("expr: %s ||| ", expr);
		// printf("eval: %d\n", ev);
		return ev;
	}
}

// int main(int argc, char** argv)
// {
// 	int dna[10];
// 	dna[0] = 0;
// 	dna[1] = 0;
// 	dna[2] = 1;
// 	dna[3] = 0;
// 	dna[4] = 2;
// 	dna[5] = 1;
// 	dna[6] = 1;
// 	dna[7] = 0;
// 	dna[8] = 1;
// 	dna[9] = 2;
	
// 	char expr[exprMAXSIZE];
// 	// strcpy(expr, "<e>");
// 	// if(!genExpr(expr, dna, 10))
// 	// 	printf("bad DNA\n");
// 	// else
// 	// {
// 	// 	genCode(expr, 2);
// 	// 	printf("%d\n", evaluate(2));
// 	// }
// 	printf("%d\n", evInd(dna, 10, 2));

// 	return EXIT_SUCCESS;
	
// }