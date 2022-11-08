// ising.c 一次元イジングモデルのシミュレーションを行うプログラム
// メトロポリス法を用いて物理量を計算するやつ
// kT-E と　kT-M と　kT-C

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 1000
// #define kt 100.0
#define J 1

double energy(int array[]);
double magnetic(int array[]);
void spin1D(int array[], double kt, double *newen, double *newmag, double *olden, double *oldmag);

int main(void){
    int i, j, element, array[N];
    int t,x;
    double olden, newen, oldmag, newmag;
    double aveEN, aveMAG, kt;

    FILE *output, *output1, *output2;
    // output = fopen("spin.dat", "w");
    output1 = fopen("energy.dat", "w");
    output2 = fopen("magnetic.dat", "w");

    srand((unsigned)time(NULL));

    // 温度変化させるループ
    for(kt=0.1;kt<=100;kt+=0.1){
        // 初期配列を配値(高温)
        /*
        for(i=0;i<N;i++){
            if(rand()%2){
                array[i] = 1;
            }else{
                array[i] = -1;
            }
        }
        */
        // 初期配列(低温)
        for(i=0;i<N;i++){
            array[i] = -1;
        }

        // 物理量の平均値の初期化
        aveEN = 0;
        aveMAG = 0;
            
        // 状態を変化させる処理
        newmag = magnetic(array);
        for(t=0;t<=1000;t++){

            spin1D(array, kt, &newen, &newmag, &olden, &oldmag);

            aveEN += newen;
            aveMAG += newmag;

            // // 状態を出力する処理
            // for(x=0;x<N;x++){
            //     fprintf(output, "%d %d %d\n", t, x, array[x]);
            // }
            // fprintf(output, "\n");
            // fprintf(output1, "%d %f\n", t, newen);
            // fprintf(output2, "%d %f\n", t, newmag);
        }

        aveEN = aveEN/N;
        aveMAG = aveMAG/N;

        fprintf(output1, "%f %f\n", kt, aveEN);
        fprintf(output2, "%f %f\n", kt, aveMAG);
    }
    // fclose(output);
    fclose(output1);
    fclose(output2);

    printf("data saved in spin.dat\n");

    return 0;

}

// エネルギーを計算する関数
double energy(int array[]){
    int i;
    double sum = 0.0;

    for(i=0;i<(N-1);i++){
        sum += array[i]*array[i+1];
    }
    sum += array[N-1]*array[0];

    return (-J*sum);
}

// 磁価を計算する関数
double magnetic(int array[]){
    int i,ans;
    ans = 0;
    for(i=0;i<N;i++){
        ans += array[i];
    }
    return ans;
}

// スピン配列を熱平衡化させるための処理
void spin1D(int array[], double kt, double *newen, double *newmag, double *olden, double *oldmag){
    double newen, olden, oldmag;
    int element;
    // ランダムに選択した1点を反転させる処理
    *olden = energy(array);
    *oldmag = *newmag;
    element = (double)rand()/RAND_MAX*N;
    array[element] *= -1;
    *newen = energy(array);
    if(array[element]){
        *newmag += 2;
    }else{
        *newmag -= 2;
    }

    // もしエネルギーがnew>oldのとき、exp(dE/kt)の確率で変化を受容
    if((*newen>*olden) && (exp(-*newen + *olden)/kt) <= (double)rand()/RAND_MAX){
        array[element]  *= -1;
        *newmag = *oldmag;
    }
}