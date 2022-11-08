// ising5-1.c 2次元イジングモデルの物理量の計算を行うプログラム
// ising5.cのエネルギー計算を改良したやつ
// メトロポリス法

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define L 200
#define N L*L
#define J 1
#define k 1
#define STEP L
#define LOOP 2000000

double energy(int **array);
double magnetic(int **array);
void metropolis2D(int **array, double kt, double *en, double *mag);
int diffEnergy(int **array, int x, int y);

int main(void){
    int i, j, elemen, t, flag;
    int **array;
    double en, mag, en2, mag2, heat, mag_suscep;
    double aveEN, aveMAG, aveEN2 , aveMAG2, sumEN, sumMAG, kt, kt2;

    // 配列の確保
    array = (int **)malloc(L*sizeof(int *));
    for(i=0;i<L;i++){
        array[i] = (int *)malloc(L*sizeof(int));
    }

    // データを書き込むファイルを開く
    FILE *energyfile, *magneticfile, *heatfile, *mag_suscepfile;
    energyfile = fopen("output/energy2D.dat", "w");
    magneticfile = fopen("output/magnetic2D.dat", "w");
    heatfile = fopen("output/heat2D.dat", "w");
    mag_suscepfile = fopen("output/mag_suscep2D.dat", "w");


    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("サンプリング:%d回\n",LOOP);
    
    // for(i=0;i<L;i++){
    //     for(j=0;j<L;j++){
    //         array[i][j] = 1;
    //     }
    // }

    // 温度変化させるループ
    for(kt=0.01;kt<=10.00;kt+=0.01){
        //初期配列(低温)
        for(i=0;i<L;i++){
            for(j=0;j<L;j++){
                array[i][j] = 1;
            }
        }
        // for(i=0;i<L;i++){
        //     for(j=0;j<L;j++){
        //         if(rand()%2){
        //             array[i][j] = 1;
        //         }else{
        //             array[i][j] = -1;
        //         }
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
                metropolis2D(array, kt, &en, &mag);
            }
        }        
        
        // 物理量を計算するループ
        for(t=0;t<LOOP;t++){
            metropolis2D(array, kt, &en, &mag);
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
        printf("\033[%dA",4);
        fflush(stdout);
    }
    printf("\033[%dB\n",3);

    // ファイルと配列の解放
    fclose(energyfile);
    fclose(magneticfile);
    fclose(heatfile);
    fclose(mag_suscepfile);
    for(i=0;i<L;i++){
        free(array[i]);
    }
    free(array);

    printf("data saved in energy2D.dat magnetic2D.dat heat2D.dat\n");

    return 0;

}


// エネルギーを計算する関数
double energy(int **array){
    int i, j;
    double sum = 0.0;

    for(i=0;i<(L);i++){
        for(j=0;j<(L);j++){
            sum += array[i][j]*(array[i][(j+1)%L] + array[(i+1)%L][j]);
        }
    }
    return (-J*sum);
}

// 磁化を計算する関数
double magnetic(int **array){
    int i,j,ans;
    ans = 0;
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            ans += array[i][j];
        }
    }
    return ans;
}

// スピン配列を状態遷移させるための処理
void metropolis2D(int **array, double kt, double *en, double *mag){
    int x, y;
    double diffen;
    // ランダムに選択した1点を反転させた時のエネルギーの差を計算する処理
    x = rand()%L;
    y = rand()%L;
    diffen = (double)diffEnergy(array, x, y);

    // メトロポリス法によるスピンが反転するかの判定
    if((diffen<=0) || (exp((-diffen/kt)) >= (double)rand()/RAND_MAX)){
        //スピンの反転と物理量の計算
        array[x][y] *= -1;
        *en += diffen;
        if(array[x][y]==1){
            *mag += 2;
        }else if(array[x][y]==-1){
            *mag += -2;
        }
    }
}

// スピンを反転した際のエネルギーを計算する関数
int diffEnergy(int **array, int x, int y){
    int sum;
    sum = array[(x+1)%L][y] + array[abs(x-1)%L][y] + array[x][(y+1)%L] + array[x][abs(y-1)%L];
    return 2*J*array[x][y]*sum;
}