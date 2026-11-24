#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void transpose_submit(int M,int N,int A[N][M],int B[M][N]);
void mul_submit(int M,int N,int A[N][M],int B[M][N],int C[N][N]);
int main(void) {
    srand(2530306);
    for(int test=0;test<200;test++) {
        int M=test==0?32:test==1?64:1+rand()%70;
        int N=test==0?32:test==1?64:1+rand()%70;
        int (*A)[M]=malloc(N*sizeof *A),(*B)[N]=malloc(M*sizeof *B);
        int (*T)[N]=malloc(M*sizeof *T),(*C)[N]=malloc(N*sizeof *C);
        int *saveA=malloc(N*sizeof *A),*saveB=malloc(M*sizeof *B);
        for(int i=0;i<N;i++) for(int k=0;k<M;k++) A[i][k]=rand()%21-10;
        for(int k=0;k<M;k++) for(int j=0;j<N;j++) B[k][j]=rand()%21-10;
        memcpy(saveA,A,N*sizeof *A);memcpy(saveB,B,M*sizeof *B);
        memset(C,0x7f,N*sizeof *C);
        transpose_submit(M,N,A,T);
        for(int i=0;i<N;i++) for(int j=0;j<M;j++) assert(T[j][i]==A[i][j]);
        mul_submit(M,N,A,B,C);
        for(int i=0;i<N;i++) for(int j=0;j<N;j++) {
            int v=0;for(int k=0;k<M;k++) v+=A[i][k]*B[k][j];
            assert(v==C[i][j]);
        }
        assert(!memcmp(saveA,A,N*sizeof *A));assert(!memcmp(saveB,B,M*sizeof *B));
        free(A);free(B);free(T);free(C);free(saveA);free(saveB);
    }
    puts("Matrix: 200 shapes, signed random inputs, unchanged inputs, ASan/UBSan PASS");
}
