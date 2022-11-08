// ising.c 一次元イジングモデルのシミュレーションを行うプログラム
// メトロポリス法

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 100
#define kt 1.0
#define J 1

double energy(int array[]);

int main(void){
    int i, j, element, array[N];
    int t,x;
    double olden, newen;

    FILE *output, *output1, *output2;
    output = fopen("spin.dat", "w");

    srand((unsigned)time(NULL));

    // 初期配列を配値(高温)
    // for(i=0;i<N;i++){
    //     if(rand()%2){
    //         array[i] = 1;
    //     }else{
    //         array[i] = -1;
    //     }
    // }

   // 初期配列(低温)
   for(i=0;i<N;i++){
       array[i] = -1;
   }
    

    // 状態を変化させる処理
    for(t=0;t<=1000;t++){

        // 全部のスピンの反転を調査する処理
        //for(i=0;i<=N;i++){
            olden = energy(array);
            element = (double)rand()/RAND_MAX*N;
            // element = i;
            array[element] *= -1;
            newen = energy(array);

            // もしエネルギーがnew>oldのとき、exp(dE/kt)の確率で変化を受容
            if((newen>olden) && (exp((-newen + olden)/kt) <= (double)rand()/RAND_MAX)){
                array[element]  *= -1;
            }
        //}
        // 状態を出力する処理
        for(x=0;x<N;x++){
            fprintf(output, "%d %d %d\n", t, x, array[x]);
        }
        fprintf(output, "\n");
    }
    fclose(output);

    printf("data saved in spin.dot\n");

    return 0;

}

double energy(int array[]){
    int i;
    double sum = 0.0;

    for(i=0;i<(N-1);i++){
        sum += array[i]*array[i+1];
    }
    sum += array[N-1]*array[0];

    return (-J*sum);
}