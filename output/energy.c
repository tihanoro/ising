#include<stdio.h>
#include<stdlib.h>

#define L 20
#define J 1

double energy(int ***array){
    int i, j, k;
    double sum = 0.0;

    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                sum += array[i][j][k]*(array[((i==L-1)?0:i+1)][j][k] + array[i][((j==L-1)?0:j+1)][k] + array[i][j][((k==L-1)?0:k+1)]);
            }
        }
        
    }

    return (-J*sum);
}

int main(void){
    int ***array;
    int times;
    int i,j,k;
    double en;
    // 配列の確保
    array = (int ***)malloc(L*sizeof(int **));
    for(i=0;i<L;i++){
        array[i] = (int **)malloc(L*sizeof(int *));
        for(j=0;j<L;j++){
            array[i][j] = (int *)malloc(L*sizeof(int));
        }
    }
    FILE *aiueo;
    aiueo = fopen("energy.dat", "w");

    for(times=1;times<=100;times++){
        for(i=0;i<L;i++){
            for(j=0;j<L;j++){
                for(k=0;k<L;k++){
                    if(rand()%2){
                        array[i][j][k] = 1;
                    }else{
                        array[i][j][k] = -1;
                    }
                }
            }
        }
        en = energy(array);
        fprintf(aiueo, "%d %f\n", times, en);
    }
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                array[i][j][k] = 1;
            }
        }
    }
    en = energy(array);
    fprintf(aiueo, "last %f", en);
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                if((i+j+k)%2==0){
                    array[i][j][k] = 1;
                }else{
                    array[i][j][k] = -1;
                }
            }
        }
    }
    en = energy(array);
    fprintf(aiueo, "last %f", en);

    fclose(aiueo);
    free(array);

    return 0;
}