#include<stdio.h>
#include<stdlib.h>

int main(void){
    int i,j,seed;
    seed = 123456;
    for(i=0;i<10;i++){
        srand(seed);
        for(j=0;j<10;j++){
            printf("%d, ", rand());
        }
        printf("\n");
    }
}