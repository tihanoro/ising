#include<stdio.h>
#include<stdlib.h>

#define L 200
#define N L*L
#define J 1

int diffEnergy(int **array, int x, int y);

int main(void){
    int i, j;
    int **array;
    array = (int**)malloc(sizeof(int *)*L);
    for(i=0;i<L;i++){
        array[i] = (int*)malloc(sizeof(int)*L);
    }
    srand(12345);
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            if(rand()%2){
                array[i][j] = 1;
            }else{
                array[i][j] = -1;
            }
        }
    }
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            diffEnergy(array, i, j);
            printf("%d %d\r",i,j);
        }
    }
    return 0;
}

int diffEnergy(int **array, int x, int y){
    int sum;
    sum = array[(x+1)%L][y] + array[abs((x-1)%L)][y] + array[x][(y+1)%L] + array[x][abs((y-1)%L)];
    return 2*J*array[x][y]*sum;
}