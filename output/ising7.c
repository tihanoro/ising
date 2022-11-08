// ising6.c 1次元イジングモデルの物理量の計算を行うプログラム

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 100
#define J 1
#define Z 2
// #define k 1
#define LOOP 10000

double energy(int *array);
double magnetic(int *array);
void metropolis3D(int *array, double kt, double *en, double *mag);
double diffEnergy(int *array, int x, double *mag);

int main(void){
    // 変数の宣言
    int i, j, k, elemen, t, flag;
    int *array;
    double en, mag, en2, mag2, heat, mag_suscep, aveEN, aveMAG, aveEN2, aveMAG2, kt, kt2;
    long double sumEN, sumMAG;

    // 配列の確保
    array = (int *)malloc(N*sizeof(int));

    // データを書き込むファイルを開く
    FILE *energyfile, *magneticfile, *heatfile, *mag_suscepfile;
    energyfile = fopen("output/energyHF.dat", "w");
    magneticfile = fopen("output/magneticHF.dat", "w");
    heatfile = fopen("output/heatHF.dat", "w");
    mag_suscepfile = fopen("output/mag_suscepHF.dat", "w");

    // 乱数のSEED値の設定
    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("サンプリング:%d回\n",LOOP);
    
    // for(i=0;i<L;i++){
    //     for(j=0;j<L;j++){
    //         for(k=0;k<L;k++){
    //             array[i][j][k] = 1;
    //         }
            
    //     }
    // }

    // 温度変化させるループ
    for(kt=0.01;kt<=30.00;kt+=0.01){
        // 初期配列(秩序)
        for(i=0;i<N;i++){
            array[i] = 1;
        }

        // 初期配列(無秩序)
        // for(i=0;i<N;i++){
        //     if(rand()%2){
        //         array[i] = 1;
        //     }else{
        //         array[i] = -1;
        //     }
        // }

        printf("\r現在kt=%fを処理中\n",kt);

        

        // 物理量の平均値の初期化
        sumEN = 0;
        sumMAG = 0;
        en2 = 0;
        mag2 = 0;
        kt2 = kt*kt;
        
        // 状態を変化させる処理
        en = energy(array);
        mag = magnetic(array);

        // 熱平衡かさせるための事前動作
        for(t=0;t<30;t++){
            for(i=0;i<N;i++){
                metropolis3D(array, kt, &en, &mag);
            }
        }        
        
        // 物理量を計算するループ
        for(t=0;t<LOOP;t++){
            metropolis3D(array, kt, &en, &mag);
            // 物理量の総和の計算
            sumEN += en;
            sumMAG += mag;
            en2 += en*en;
            mag2 += mag*mag;
        }
        aveEN = sumEN/LOOP;
        aveMAG = sumMAG/LOOP;
        aveEN2  = en2/LOOP;
        aveMAG2 = mag2/LOOP;

        heat = (aveEN2 - (aveEN*aveEN))*k/kt2;
        mag_suscep = (aveMAG2 - (aveMAG*aveMAG))/kt;

        // aveEN /= N;
        // aveMAG /= N;
        heat /= N;
        mag_suscep /= N;

        // 計算結果をファイルに記述する関数
        fprintf(energyfile, "%f %f\n", kt, aveEN);
        fprintf(magneticfile, "%f %f\n", kt, aveMAG);
        fprintf(heatfile, "%f %f\n", kt, heat);
        fprintf(mag_suscepfile, "%f %f\n", kt, mag_suscep);

        // 計算結果を標準出力にも表示
        printf("\r\033[Kエネルギー:%f\n",aveEN);
        printf("\r\033[K磁化:%f\n",aveMAG);
        printf("\r\033[K比熱:%f\n",heat);
        printf("\r\033[K帯磁率:%f\n",mag_suscep);
        printf("\033[%dA",5);
        fflush(stdout);
    }
    printf("\033[%dB\n",3);

    // ファイルと配列の解放
    fclose(energyfile);
    fclose(magneticfile);
    fclose(heatfile);
    fclose(mag_suscepfile);
    free(array);

    printf("data saved\n");

    return 0;

}


// エネルギーを計算する関数
double energy(int *array){
    int i,j;
    int sum = 0;

    for(i=0;i<N;i++){
        for(j=i+1;j<N;j++){
            sum += array[i]*array[j%N];
        }
    }
    return (-J*sum);
}

// 磁化を計算する関数
double magnetic(int *array){
    int i,j,k,ans;
    ans = 0;
    for(i=0;i<N;i++){
        ans += array[i];
    }
    return ans;
}

// スピン配列を状態遷移させるための処理
void metropolis3D(int *array, double kt, double *en, double *mag){
    int x, y, z;
    double diffen;
    // ランダムに選択した1点を反転させた時のエネルギーの差を計算する処理
    x = rand()%N;
    diffen = (double)diffEnergy(array, x, mag);
    // メトロポリス法によるスピンが反転するかの判定
    if((diffen<=0) || (exp((-diffen/kt)) >= (double)rand()/RAND_MAX)){
        //スピンの反転と物理量の計算
        array[x] *= -1;
        *en += diffen;
        if(array[x]==1){
            *mag += 2;
        }else if(array[x]==-1){
            *mag += -2;
        }
    }
}

// スピンを反転した際のエネルギー差を計算する関数
double diffEnergy(int *array, int x, double *mag){
    int i;
    double sum, j;

    sum = (*mag) - array[x];

    j = J*Z/(2*N);
    return 2*j*array[x]*sum;
}