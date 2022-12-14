// ising2D.c 2次元イジングモデルの物理量の計算を行うプログラム

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define L 300
#define N L*L
#define J 1
#define kB 1
#define STEP 100 // 初期依存性をなくすためのループ(モンテカルロステップ)
#define LOOP 200 // 物理量を計測するループ

double energy(int **array);
double magnetic(int **array);
void metropolis3D(int **array, double kt, double *en, double *mag);
int diffEnergy(int **array, int x, int y);
void outputSpin(int **array, int kt);
void outputEnergy(FILE *enfile, int times, double en);

int main(void){
    // 変数の宣言
    int i, j, k, elemen, times, flag;
    int **array;
    int **Init;
    double en, mag, en2, mag2, heat, mag_suscep, aveEN, aveMAG, aveEN2, aveMAG2, t, kt, kt2;
    long double sumEN, sumMAG;

    // 配列の確保
    array = (int **)malloc(L*sizeof(int *));
    Init = (int **)malloc(L*sizeof(int *));
    for(i=0;i<L;i++){
        array[i] = (int *)malloc(L*sizeof(int));
        Init[i] = (int *)malloc(L*sizeof(int));
    }

    // データを書き込むファイルを開く
    FILE *energyfile, *magneticfile, *heatfile, *mag_suscepfile;
    energyfile = fopen("output/energy2D.dat", "w");
    magneticfile = fopen("output/magnetic2D.dat", "w");
    heatfile = fopen("output/heat2D.dat", "w");
    mag_suscepfile = fopen("output/mag_suscep2D.dat", "w");

    // 乱数のSEED値の設定
    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("サンプリング:%d回\n",LOOP*N);
    
    // 初期配列(秩序)
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            Init[i][j] = 1;
        }
    }

    // 温度変化させるループ
    for(t=0.01;t<=10.00;t+=0.01){
        // 初期配列
        for(i=0;i<L;i++){
            for(j=0;j<L;j++){
                array[i][j] = Init[i][j];
            }
        }

        printf("\r現在kt=%fを処理中\n",kt);

        

        // 物理量の平均値の初期化
        sumEN = 0;
        sumMAG = 0;
        en2 = 0;
        mag2 = 0;
        kt = kB*t;
        kt2 = kB*t*t;
        
        // エネルギーと磁化の計算
        en = energy(array);
        mag = magnetic(array);

        // 熱平衡かさせるための事前動作
        for(times=0;times<STEP;times++){
            for(i=0;i<N;i++){
                metropolis3D(array, kt, &en, &mag);
            }
        }
        
        // 物理量を計算するループ
        for(times=0;times<LOOP*N;times++){
            metropolis3D(array, kt, &en, &mag);
            // 物理量の総和の計算
            sumEN += en;
            sumMAG += mag;
            en2 += en*en;
            mag2 += mag*mag;
        }
        
        aveEN = sumEN/(LOOP*N);
        aveMAG = sumMAG/(LOOP*N);
        aveEN2  = en2/(LOOP*N);
        aveMAG2 = mag2/(LOOP*N);

        heat = (aveEN2 - (aveEN*aveEN))/kt2;
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
        // outputSpin(array, (int)kt);

        // 計算結果を標準出力にも表示
        printf("\r\033[Kエネルギー:%f\n",aveEN);
        printf("\r\033[K磁化:%f\n",aveMAG);
        printf("\r\033[K比熱:%f\n",heat);
        printf("\r\033[K帯磁率:%f\n",mag_suscep);
        printf("\033[%dA",5);
        fflush(stdout);
    }
    printf("\033[%dB\n",4);

    // ファイルと配列の解放
    fclose(energyfile);
    fclose(magneticfile);
    fclose(heatfile);
    fclose(mag_suscepfile);
    for(i=0;i<L;i++){
        free(array[i]);
        free(Init[i]);
    }
    free(array);
    free(Init);

    printf("data saved in energy2D.dat magnetic2D.dat heat2D.dat mag_suscep2D.dat\n");

    return 0;

}


// エネルギーを計算する関数
double energy(int **array){
    int i, j;
    double sum = 0.0;

    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            sum += array[i][j]*(array[((i==L-1)?0:i+1)][j] + array[i][((j==L-1)?0:j+1)]);
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
void metropolis3D(int **array, double kt, double *en, double *mag){
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

// スピンを反転した際のエネルギー差を計算する関数
int diffEnergy(int **array, int x, int y){
    int sum;
    sum = array[((x==L-1)?0:x+1)][y] + array[((x==0)?L-1:x-1)][y] + array[x][((y==L-1)?0:y+1)] + array[x][((y==0)?L-1:y-1)];

    return 2*J*array[x][y]*sum;
}

// スピン配置を出力する関数
void outputSpin(int **array, int kt){
    FILE *spinfile;
    int i,j;
    char str[1000];
    sprintf(str, "output/spin/spin%d.dat", kt);
    spinfile = fopen(str, "w");
    for(i=0;i<L;i++){
        for(j=0;j<L;j++){
            if(array[i][j] == 1){
                fprintf(spinfile, "+");
            }else if(array[i][j] == -1){
                fprintf(spinfile, "-");
            }
        }
        fprintf(spinfile, "\n");
    }
    fclose(spinfile);
}

// エネルギーの値を出力する関数
void outputEnergy(FILE *enfile, int times, double en){
    fprintf(enfile, "%d %f\n", times, en);
}