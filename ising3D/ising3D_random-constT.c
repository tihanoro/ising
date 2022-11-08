// ising6.c 3次元イジングモデルの物理量の計算を行うプログラム

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define L 20
#define N L*L*L
#define J 1
#define STEP 1000
#define LOOP 1000

double energy(int ***array);
double magnetic(int ***array);
void metropolis3D(int ***array, double kt, double *en, double *mag);
int diffEnergy(int ***array, int x, int y, int z);
void outputSpin(int ***array, int kt);

int main(void){
    // 変数の宣言
    int i, j, k, elemen, t, flag;
    int ***array;
    int ***Init;
    double en, mag, en2, mag2, heat, mag_suscep, aveEN, aveMAG, aveEN2, aveMAG2, kt2;
    double kt = 0.1;
    long double sumEN, sumMAG;

    // 配列の確保
    array = (int ***)malloc(L*sizeof(int **));
    Init = (int ***)malloc(L*sizeof(int **));
    for(i=0;i<L;i++){
        array[i] = (int **)malloc(L*sizeof(int *));
        Init[i] = (int **)malloc(L*sizeof(int *));
        for(j=0;j<L;j++){
            array[i][j] = (int *)malloc(L*sizeof(int));
            Init[i][j] = (int *)malloc(L*sizeof(int));
        }
    }

    // データを書き込むファイルを開く
    FILE *energyy, *magneticc, *heatt, *mag_suscepp;
    char energyStr[100], magneticStr[100], heatStr[100], mag_suscepStr[100];

    // 乱数のSEED値の設定
    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("サンプリング:%d回\n",LOOP);

    // 初期配列(秩序)
    // for(i=0;i<L;i++){
    //     for(j=0;j<L;j++){
    //         for(k=0;k<L;k++){
    //             array[i][j][k] = Init[i][j][k];
    //         }
    //     }
    // }

    // 初期配列(無秩序)
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                if(rand()%2){
                    Init[i][j][k] = 1;
                }else{
                    Init[i][j][k] = -1;
                }
            }
        }
    }

    // 温度変化させるループ
    for(kt=0.1;kt<=10;kt+=0.1){
        // 初期配列
        for(i=0;i<L;i++){
            for(j=0;j<L;j++){
                for(k=0;k<L;k++){
                    array[i][j][k] = Init[i][j][k];
                }
            }
        }

        printf("\r現在kt=%fを処理中\n",kt);

        

        // 物理量の平均値の初期化
        en2 = 0;
        mag2 = 0;
        kt2 = kt*kt;
        
        // 状態を変化させる処理
        en = energy(array);
        mag = magnetic(array);
        
        // データを書き込むファイルを開く
        sprintf(energyStr, "output/kt/energy-%f.dat",kt);
        sprintf(magneticStr, "output/kt/magnetic-%f.dat",kt);
        sprintf(heatStr, "output/kt/heat-%f.dat",kt);
        sprintf(mag_suscepStr, "output/kt/mag_suscep-%f.dat",kt);
        energyy = fopen(energyStr, "w");
        magneticc = fopen(magneticStr, "w");
        heatt = fopen(heatStr, "w");
        mag_suscepp = fopen(mag_suscepStr, "w");        
        
        // 物理量を計算するループ
        for(t=0;t<LOOP;t++){

            // 1モンテカルロステップ
            for(i=0;i<N;i++){
                metropolis3D(array, kt, &en, &mag);
                en2 = en*en;
                mag2 = mag*mag;
            }

            heat = (en2 - en*en)/(N*kt2);
            mag_suscep = (mag2 - mag*mag)/(N*kt2);

            // 計算結果をファイルに記述する関数
            fprintf(energyy, "%d %f\n", t, en);
            fprintf(magneticc, "%d %f\n", t, mag);
            fprintf(heatt, "%d %f\n", t, heat);
            fprintf(mag_suscepp, "%d %f\n", t, mag_suscep);

            // printf("\r\033[Kエネルギー:%f\n", en);
            // printf("\r\033[K磁化:%f\n", mag);
            // printf("\033[%dA",3);
            fflush(stdout);
        }
    }

    // ファイルと配列の解放
    fclose(energyy);
    fclose(magneticc);
    fclose(heatt);
    fclose(mag_suscepp);
    for(i=0;i<L;i++){
        free(Init[i]);
        free(array[i]);
    }
    free(array);
    free(Init);

    printf("data saved in energy3D.dat magnetic3D.dat heat3D.dat mag_suscep3D.dat\n");

    return 0;

}


// エネルギーを計算する関数
double energy(int ***array){
    int i, j, k;
    double sum = 0.0;

    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                sum += array[i][j][k]*(array[(i+1)%L][j][k] + array[i][(j+1)%L][k] + array[i][j][(k+1)%L]);
            }
        }
        
    }

    return (-J*sum);
}

// 磁化を計算する関数
double magnetic(int ***array){
    int i,j,k,ans;
    ans = 0;
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            for(k=0;k<L;k++){
                ans += array[i][j][k];
            }
        }
    }
    return ans;
}

// スピン配列を状態遷移させるための処理
void metropolis3D(int ***array, double kt, double *en, double *mag){
    int x, y, z;
    double diffen;
    // ランダムに選択した1点を反転させた時のエネルギーの差を計算する処理
    x = rand()%L;
    y = rand()%L;
    z = rand()%L;
    diffen = (double)diffEnergy(array, x, y, z);

    // メトロポリス法によるスピンが反転するかの判定
    if((diffen<=0) || (exp((-diffen/kt)) >= (double)rand()/RAND_MAX)){
        //スピンの反転と物理量の計算
        array[x][y][z] *= -1;
        *en += diffen;
        if(array[x][y][z]==1){
            *mag += 2;
        }else if(array[x][y][z]==-1){
            *mag += -2;
        }
    }
}

// スピンを反転した際のエネルギー差を計算する関数
int diffEnergy(int ***array, int x, int y, int z){
    int sum;
    sum = array[(x+1)%L][y][z] + array[abs((x-1)%L)][y][z] + array[x][(y+1)%L][z] + array[x][abs((y-1)%L)][z] + array[x][y][(z+1)%L] + array[x][y][abs((z-1)%L)];

    return 2*J*array[x][y][z]*sum;
}

// スピン配置を出力する関数
void outputSpin(int ***array, int kt){
    FILE *spinfile;
    int i,j,k;
    char str[1000];
    sprintf(str, "output/spin/spin%d.dat", kt);
    spinfile = fopen(str, "w");
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            if(array[0][i][j] == 1){
                fprintf(spinfile, "+");
            }else if(array[0][i][j] == -1){
                fprintf(spinfile, "-");
            }
        }
        fprintf(spinfile, "\n");
    }
}