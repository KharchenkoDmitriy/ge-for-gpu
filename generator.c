#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "mtgen.c"

#define exprMAXSIZE 12

void genCode(char *expr, int indvNum)
{
	char *prog = malloc(200 * sizeof(char));
	sprintf(prog, "int exec(int x, int y)\n{\nreturn %s;\n}", expr);

	FILE *fp;
	char* filePath = malloc(30 * sizeof(char));
	sprintf(filePath, "temp/generated%d.c", indvNum);
	
	fp = fopen(filePath, "w+");
	fputs(prog, fp);
	fclose(fp);

	char* gccCall = malloc(100 * sizeof(char));
	sprintf(gccCall, "gcc -fPIC -O -shared temp/generated%d.c -o temp/individual%d.so", indvNum,indvNum);
	system(gccCall);
}

int runIndv(int indvNum, int x, int y)
{
	void *handle;
	int (*func_print_name)(int,int);

	char* indvPath = malloc(30 * sizeof(char));
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

int idealfunc(int x, int y)
{
	return x*y+1;
}

#define EVALPOINTSNUM 100
int evaluate(int indvNum)
{
	unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
	init_by_array64(init, length);
	int x,y;
	int dif = 0;
	for(int i=0; i < EVALPOINTSNUM; i++)
	{
		x = genrand64_int64()%100;
		y = genrand64_int64()%100;
		if(idealfunc(x,y) - runIndv(indvNum,x,y))
			dif++;
	}
	return dif;
}


// BNF grammar
int genExpr(char *expr, int* DNA, int DNASize)
{
	int * p_DNA = DNA;
	char * token = expr;
	printf("start expr: %s\n", token);
	while(token = strchr(expr, '<'))
	{
		int npos = token-expr;
		token[0] = 0;
		char left[exprMAXSIZE];
		char right[exprMAXSIZE];
		strcpy(right, &token[3]);
		strcpy(left, expr);
		printf("r: %s\n", right);
		printf("l: %s\n", left);
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
		printf("expr: %s\n", expr);
		if(p_DNA == &DNA[DNASize-1])
			p_DNA = DNA;
			else
			{
				p_DNA++;
			}
		  
	}
	return 1;
	
}

int evInd(int* dna, int dnaSize, int indNum)
{
	char expr[exprMAXSIZE];
	strcpy(expr, "<e>");
	if(!genExpr(expr, dna, dnaSize))
		return EVALPOINTSNUM;
	else
	{
		genCode(expr, indNum);
		return evaluate(indNum);
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