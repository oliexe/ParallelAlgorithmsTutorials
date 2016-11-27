#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

double **make2dmatrix(long n);
void free2dmatrix(double ** M, long n);
void printmatrix(double **A, long n);

long matrix_size,version;
char algo;


void decomposeOpenMP(double **A, long n)
{
	printf("\nDECOMPOSE OPENMP CALLED\n");

	long i,j,k,rows,mymin,mymax;
	int pid=0;
	int nprocs;

#pragma omp parallel shared(A,n,nprocs) private(i,j,k,pid,rows,mymin,mymax)
	{
#ifdef _OPENMP
		nprocs=omp_get_num_threads();
#endif

#ifdef _OPENMP
		pid=omp_get_thread_num();
#endif

	//	printf("1. I am proc no %d out of %d\n",pid,nprocs);

		rows=n/nprocs;
		mymin=pid * rows;
		mymax=mymin + rows - 1;

		if(pid==nprocs-1 && (n-(mymax+1))>0)
			mymax=n-1;

		for(k=0;k<n;k++){
			if(k>=mymin && k<=mymax){
				//#pragma omp for schedule(static)
				for(j=k+1;j<n;j++){
					A[k][j] = A[k][j]/A[k][k];
				}
			}

#pragma omp barrier
			for(i=(((k+1) > mymin) ? (k+1) : mymin);i<=mymax;i++){
				//#pragma omp for schedule(static)
				for(j=k+1;j<n;j++){
					A[i][j] = A[i][j] - A[i][k] * A[k][j];
				}
			}
		}
	}
}



int checkVersion1(double **A, long n)
{
	long i, j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			if(A[i][j]!=1){
				return 0;
			}
	}
	return 1;
}

void initializeVersion1(double **A, long n)
{
	long i, j;
	for (i=0;i<n;i++){
		for (j=0;j<n;j++){
			if(i<=j )
				A[i][j]=i+1;
			else
				A[i][j]=j+1;

		}
	}
}



double **getMatrix(long size,long version)
{
	double **m=make2dmatrix(size);
    initializeVersion1(m,size);
}

int check(double **A, long size, long version){
	checkVersion1(A,size);

}

int main(){
    matrix_size=1000;
	long num_threads=8;
	omp_set_num_threads(num_threads);

	double **matrix=getMatrix(matrix_size,version);

	//printmatrix(matrix,matrix_size);

	/**
	 * Code to Time the LU decompose
	 */
	clock_t begin, end;
	double time_spent;
	begin = clock();

	decomposeOpenMP(matrix,matrix_size);

	end = clock();
	time_spent = ((double)(end - begin)) / CLOCKS_PER_SEC;

	printmatrix(matrix,matrix_size);


	printf("\n**********************************\n\n");
	printf("Algo selected :%s\n","OpenMP");
	printf("Size of Matrix :%lu \n",matrix_size);
	printf("Version Number : %lu\n",version);
	printf("Number of Procs : %lu\n",num_threads);
	printf("%s",check(matrix,matrix_size,version)==1? "DECOMPOSE SUCCESSFULL\n":"DECOMPOSE FAIL\n");
	printf("DECOMPOSE TIME TAKEN : %f seconds\n",time_spent);
	printf("\n**********************************\n\n");

	free2dmatrix(matrix,matrix_size);
	return 0;
}


double **make2dmatrix(long n)
{
	long i;
	double **m;
	m = (double**)malloc(n*sizeof(double*));
	for (i=0;i<n;i++)
		m[i] = (double*)malloc(n*sizeof(double));
	return m;
}

// only works for dynamic arrays:
void printmatrix(double **A, long n)
{
	printf("\n *************** MATRIX ****************\n\n");
	long i, j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
			printf("%f ",A[i][j]);
		printf("\n");
	}
}

void free2dmatrix(double ** M, long n)
{
	long i;
	if (!M) return;
	for(i=0;i<n;i++)
		free(M[i]);
	free(M);
}
