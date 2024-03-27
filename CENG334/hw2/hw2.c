#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "hw2_output.h"
#include <semaphore.h>

int **A;
int **B;
int **C;
int **D;
int **J;
int **L;
int **R;
int N,M,K;

sem_t **J_sem;
sem_t **L_sem;

void *addition1(void *arg){
    int row = *(int *)arg;
    free(arg);
    for(int j = 0 ; j < M ; j++){
        J[row][j] = A[row][j] + B[row][j];
        hw2_write_output(0, row + 1, j + 1, J[row][j]);
        sem_post(&J_sem[row][j]);
    }
    pthread_exit(NULL);
}

void *addition2(void *arg){
    int row = *(int *)arg;
    free(arg);
    for(int j = 0 ; j < K ; j++){
        L[row][j] = C[row][j] + D[row][j];
        hw2_write_output(1, row + 1, j + 1, L[row][j]);
        sem_post(&L_sem[row][j]);
    }
    pthread_exit(NULL);
}

void *multiplication(void *arg){
    int row = *(int *)arg;
    free(arg);
    for(int col = 0 ; col < K ; col++){
        int sum = 0;
        for(int i = 0 ; i < M ; i++){
            sem_wait(&J_sem[row][i]);
            sem_wait(&L_sem[i][col]);
            sum += J[row][i] * L[i][col];
            sem_post(&J_sem[row][i]);
            sem_post(&L_sem[i][col]);
        }
        R[row][col] = sum;
        hw2_write_output(2, row + 1, col + 1, R[row][col]);
    }
    pthread_exit(NULL);
}

void readInput() {
    scanf("%d %d", &N, &M);
    A = (int **) malloc(sizeof(int *) * N);
    for (int i = 0; i < N; i++) {
        A[i] = (int *) malloc(sizeof(int) * M);
        for (int j = 0; j < M; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    scanf("%d %d", &N, &M);
    B = (int **) malloc(sizeof(int *) * N);
    for (int i = 0; i < N; i++) {
        B[i] = (int *) malloc(sizeof(int) * M);
        for (int j = 0; j < M; j++) {
            scanf("%d", &B[i][j]);
        }
    }


    scanf("%d %d", &M, &K);
    C = (int **) malloc(sizeof(int *) * M);
    for (int i = 0; i < M; i++) {
        C[i] = (int *) malloc(sizeof(int) * K);
        for (int j = 0; j < K; j++) {
            scanf("%d", &C[i][j]);
        }
    }

    scanf("%d %d", &M, &K);
    D = (int **) malloc(sizeof(int *) * M);
    for (int i = 0; i < M; i++) {
        D[i] = (int *) malloc(sizeof(int) * K);
        for (int j = 0; j < K; j++) {
            scanf("%d", &D[i][j]);
        }
    }
}

void constructJLRMatrices(){
    J = (int **)malloc(sizeof(int *) * N);
    for(int i = 0 ; i < N ; i++){
        J[i] = (int *)malloc(sizeof(int) * M);
    }
    L = (int **)malloc(sizeof(int *) * M);
    for(int i = 0 ; i < M ; i++){
        L[i] = (int *)malloc(sizeof(int) * K);
    }
    R = (int **)malloc(sizeof(int *) * N);
    for(int i = 0 ; i < N ; i++){
        R[i] = (int *)malloc(sizeof(int) * K);
    }
}

void destroySemaphores(){
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < M ; j++){
            sem_destroy(&J_sem[i][j]);
        }
    }

    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < K ; j++){
            sem_destroy(&L_sem[i][j]);
        }
    }
}

void freeAll(){
    for(int i = 0 ; i < N ; i++){
        free(A[i]);
        free(B[i]);
        free(J[i]);
        free(R[i]);
        free(J_sem[i]);
    }
    free(A);
    free(B);
    free(J);
    free(R);
    free(J_sem);

    for(int i = 0 ; i < M ; i++){
        free(C[i]);
        free(D[i]);
        free(L[i]);
        free(L_sem[i]);
    }
    free(C);
    free(D);
    free(L);
    free(L_sem);

}


int main() {
    // hw2_init_output
    hw2_init_output();

    // Reading inputs
    readInput();

    // Construct J, L and R matrices
    constructJLRMatrices();

    // Allocate semaphores
    J_sem = (sem_t **)malloc(sizeof(sem_t *) * N);
    for(int i = 0 ; i < N ; i++){
        J_sem[i] = (sem_t *)malloc(sizeof(sem_t) * M);
    }
    L_sem = (sem_t **)malloc(sizeof(sem_t *) * M);
    for(int i = 0 ; i < M ; i++){
        L_sem[i] = (sem_t *)malloc(sizeof(sem_t) * K);
    }


    // initialize synchronization primitives
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < M ; j++){
            sem_init(&J_sem[i][j], 0, 0);
        }
    }
    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < K ; j++){
            sem_init(&L_sem[i][j], 0, 0);
        }
    }

    // Creating threads
    // Create threads for the first operation (A + B = J)
    pthread_t threads[N + M + N];
    for(int i = 0 ; i < N ; i++){
        int *arg =(int *)malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&(threads[i]), NULL, addition1, arg);
    }

    // Create threads for the second operation (C + D = L)
    for(int i = N ;  i < (N + M) ; i++){
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i - N;
        pthread_create(&(threads[i]), NULL, addition2, arg);
    }

    // Create threads for the multiplication operation (J * L  = R)
    for(int i = N + M; i < (N + M + N) ; i++){
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i - N - M;
        pthread_create(&(threads[i]), NULL, multiplication, arg);
    }

    // Wait for threads to finish
    for(int i = 0 ; i < (N + M + N) ; i++) {
        pthread_join(threads[i], NULL);
    }

    // Output the final result matrix R
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < K ; j++){
            printf("%d", R[i][j]);
            if(j == (K-1)){
                printf("\n");
            }else{
                printf(" ");
            }
        }
    }

    // Destroy semaphores
    destroySemaphores();
    // Free the created 2D arrays
    freeAll();
    return 0;
}
