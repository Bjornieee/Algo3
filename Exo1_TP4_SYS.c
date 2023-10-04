#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define N 5
int TPF[2];

int fils(){
    close(TPF[1]);
    ssize_t rd;
    int un;
    while((rd = read(TPF[0], (void *) &un, sizeof(int)))>0){
        printf("[FILS] : %d\n",(int) un);
    }
    if ((int) rd == -1) perror("read");
    exit(1);
}

void pere(){
    close(TPF[0]);
    for(int i=1;i<N;i++){
        (write(TPF[1],(void *)&i,sizeof(int)));
        sleep(1);
    }
    close(TPF[1]);
    int CIR;
    int a =wait(&CIR);
    printf("[PERE] : MON FILS %d est terminé avec le code %d",a , WEXITSTATUS(CIR));
}
int main() {
    if (pipe(TPF)==1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    switch(fork()){
        case -1: {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        case 0:
            fils();
        }
    pere();
}
