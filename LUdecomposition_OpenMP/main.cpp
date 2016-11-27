

//#include "stdafx.h"	/* used in VS */
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <iomanip>
#include <omp.h>	/* OpenMP header */
#include <sys/time.h>

using namespace std;

int main(int argc, char* argv[]) {

  /* Timestamp variables */
  time_t start,end;
  double start_time, end_time;
  struct timeval tz;
  struct timezone tx;

  /* Pocet Threadu */
  int threadx = 2;
  omp_set_nested(0);
  omp_set_dynamic(1);
  omp_set_num_threads(threadx);

  /* Velikost Matice */
  int n = 4;

  printf("Size of matrix :%d\n\n", n);
  float *x = new float [n]; //--- initiation matrix X
  float *y = new float [n]; //--- initiation matrix y
  float sum1 =0, sum2=0, sum3=0, sum4=0, sum=0;


  float **a; //------------------- Inicilizace pole pro matici A
  a = new float *[n];

  for (int i = 0; i<n; i++) {
    a[i] = new float [n];
  }


  float *b= new float [n]; //------ Inicilizace pole B

  cout<<"SOUCET RADKU :\n";
  for (int i=0; i<(n); i++ ) {
    for (int j=0; j<(n); j++ ) {
      if (i==j)
      a[i][j] = n;
      else
      a[i][j] = 1;
    }

  b [i]= (2*n)-1;
  cout<<" = "<<b [i]<<endl ;
  }

  //-------------------------------------------------------------- Upper A matrix initiation

  /* Windows timer start */
  int start_s=clock();

  float **upperA ;
  upperA= new float *[n];

  for (int i = 0; i<n; i++) {
    upperA[i] = new float [n];
  }

  cout<<"\n\n\n";
  for (int i=0; i<(n); i++ ) {
    for (int j=0; j<(n); j++ ) {
      if (i>j)
      upperA[i][j] = 0;
    }
  }

  a[2][2] = 7;

    cout<<"INPUT MATRIX :\n";
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) cout <<a[i][j]<<" ";
    cout<<endl;
  }

  //------------------------------------------------ initiate lower A

  float ** lowerA;
  lowerA = new float *[n];
  //cout<<"\n\n\n";
  for (int i = 0; i<n; i++) {
    lowerA[i] = new float [n];
  }

  for (int i=0; i<(n); i++ ) {
    for (int j=0; j<(n); j++ ) {
      if (i==j)
      lowerA[i][j] = 1;
      else if (i<j)
      lowerA[i][j] = 0;
    }

  }

 //------------------------------calculate upper A
 for (int i=0; i<(n); i++ ) {
   #pragma omp parallel for private ( sum1)
   for(int j=i; j<(n); j++ ) {
   sum1=0;

   #pragma omp parallel for  reduction (+: sum1)
   for (int e=0; e<(n); e++ ){if (e!=i) {
     sum1=sum1 + (lowerA[i][e]* upperA[e][i]);}
     //cout <<upperA[i][j]<<" ";
   }

   sum1=a[i][j]-sum1;
   upperA[i][j]=sum1;
   //cout<<upperA[i][j]<<" "; //<<" (sum "<<sum1<<" )"
   sum1=0;

   }

   if(i+1<n) {
   #pragma omp parallel for private(sum1)
   for(int j=i+1; j<n; j++) {
     sum1=0;
     #pragma omp parallel for  reduction (+: sum1)
     for (int e=0; e<(n); e++ ){if (e!=i) {
       sum1=sum1 + (lowerA[i][e]* upperA[e][i]);}
       //cout <<upperA[i][j]<<" ";
     }

     lowerA[j][i]=(a[j][i]-sum1)/upperA[i][i];
     sum1=0;

    }

   }
  }// end for

  //---------------------------calculate lower B
  //-------------------------------------------------------------- Calculate for y[n]
  //cout<<"\n\n";

  double reduct=0;
  for (int i = 0; i<n; i ++) {
    #pragma omp parallel for reduction(+:sum)
    for (int j=0; j<n; j++) {
      if (i!=j)
      sum = sum +(lowerA[i][j]*y[j]);
    }

    y[i] = (b[i]- sum)/ lowerA[i][i];
    sum = 0;
  }

  //-------------------------------------------------------------- Calculate for x[n], the final answer

  for (int i = (n-1); i>-1; i --) {
    #pragma omp parallel for  reduction (+: sum)
    for (int j=(n-1); j>-1; j--) {
      if (i!=j)
	sum = sum +(upperA[i][j]*x[j]);
    }

    x[i] = (y[i]- sum)/ upperA[i][i];
    sum = 0;
  }



  /* time stop */
  int stop_s=clock();
  cout << "Execute time: " <<(stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 <<" miliseconds"<< endl;


  cout<<"L :\n";
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) cout <<lowerA[i][j]<<" ";
    cout<<endl;
  }
  cout<<"U :\n";
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      cout <<upperA[i][j]<<" ";
    }

  cout<<endl;
  }
}

