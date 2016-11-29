//pouze test podle YouTube tutorialu

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

double **creatematrix(long n);
void kill(double ** M, long n);
long size;

double **l;
double **u;
double **mat;

//LU Rozklad pro openMP
void LU(double **A)
{
    long n = size;
	printf("\nWait...\n");
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
		rows=n/nprocs;
		mymin=pid * rows;
		mymax=mymin + rows - 1;

		if(pid==nprocs-1 && (n-(mymax+1))>0)
			mymax=n-1;

		for(k=0;k<n;k++){
			if(k>=mymin && k<=mymax){
				for(j=k+1;j<n;j++){
					A[k][j] = A[k][j]/A[k][k];
				}
			}

                #pragma omp barrier
			for(i=(((k+1) > mymin) ? (k+1) : mymin);i<=mymax;i++){
				for(j=k+1;j<n;j++){
					A[i][j] = A[i][j] - A[i][k] * A[k][j];
				}
			}
		}
	}
}

//Tisk dvojrozmerneho pole
void print(double **matrix)
{
    printf("\n");
	int i =0, j=0;
    for (i=0;i<size;i++) {
        for(j=0;j<size;j++) {
            printf(" %f  ",matrix[i][j]);
        }
        printf("\n");
    }
}

// Alokace pameti pro matici
double **creatematrix(long size)
{
	long i;
	double **m;
	m = (double**)malloc(size*sizeof(double*));
	for (i=0;i<size;i++)
		m[i] = (double*)malloc(size*sizeof(double));
	return m;
}

// Smazat matici z pameti
void kill(double **matrix, long size)
{
	long i;
	if (!matrix) return;
	for(i=0;i<size;i++)
		free(matrix[i]);
	free(matrix);
}

//Inicializovat matice - naplni celou matici hodnotami
void init(double **matrix, long size)
{
	long i, j;
	for (i=0;i<size;i++){
		for (j=0;j<size;j++){
			if(i<=j )
				matrix[i][j]=i+1;
			else
				matrix[i][j]=j+1;
		}
	}
}

int main(int argc, char *argv[])
{
 int a;
 size = 1000;


 //threads 2-10
 for( a = 2; a <= 10; a = a + 1 ){
    double **matrix=creatematrix(size);
    long num_threads=a;
    omp_set_num_threads(num_threads);
	init(matrix,size);

	if(size<=10){
	    //print(matrix);
	}

	clock_t begin, end;
	double time_spent;
	begin = clock();

	LU(matrix);
	//check(matrix,size);

	end = clock();
	time_spent = ((double)(end - begin)) / CLOCKS_PER_SEC;
	printf("Size : %lu \n",size);
	printf("Threads : %lu\n",num_threads);
	printf("Time : %f sec\n",time_spent);

	kill(matrix,size);
    }

 return 0;
}
