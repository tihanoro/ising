// ising1D.c 1次元イジングモデルの物理量の計算を行うプログラム

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 10000
#define J 1
#define kB 1
#define STEP 100 // 初期依存性をなくすためのループ(モンテカルロステップ)
#define LOOP 200 // 物理量を計測するループ

// progress barを作成するための定義
#define T_MIN 0.01
#define T_MAX 20.00
#define T_STEP 0.01

double energy(int *array);
double magnetic(int *array);
void metropolis3D(int *array, double kt, double *en, double *mag, int s, int d);
int diffEnergy(int *array, int x);
void outputSpin(int *array, int kt);
void outputEnergy(FILE *enfile, int times, double en);

int main(void){
    // 変数の宣言
    int i, j, k, elemen, times, flag;
    int *array;
    int *Init;
    double en, mag, en2, mag2, heat, mag_suscep, aveEN, aveMAG, aveEN2, aveMAG2, t, kt, kt2;
    long double sumEN, sumMAG;

    // 配列の確保
    array = (int *)malloc(N*sizeof(int));
    Init = (int *)malloc(N*sizeof(int));

    // データを書き込むファイルを開く
    FILE *energyfile, *magneticfile, *heatfile, *mag_suscepfile;
    energyfile = fopen("output/energy1D.dat", "w");
    magneticfile = fopen("output/magnetic1D.dat", "w");
    heatfile = fopen("output/heat1D.dat", "w");
    mag_suscepfile = fopen("output/mag_suscep1D.dat", "w");

    // 乱数のSEED値の設定
    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("サンプリング:%d回\n",LOOP*N);
    
    // 初期配列(秩序)
    for(i=0;i<N;i++){
        Init[i] = 1;
    }

    // 温度変化させるループ
    for(t=0.01;t<=10.00;t+=0.01){
        // 初期配列
        for(i=0;i<N;i++){
            array[i] = Init[i];
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
                metropolis3D(array, kt, &en, &mag, 0, N);
            }
        }
        
        // 物理量を計算するループ
        times = 0;
        while(times<LOOP*N){
            int d = rand()%(N/2);
            int x = rand()%N;
            for(i=0;i<d;i++){
                metropolis3D(array, kt, &en, &mag, x, d);
                // 物理量の総和の計算
                sumEN += en;
                sumMAG += mag;
                en2 += en*en;
                mag2 += mag*mag;
                times++;
            }
        }
        
        aveEN = sumEN/times;
        aveMAG = sumMAG/times;
        aveEN2  = en2/times;
        aveMAG2 = mag2/times;

        heat = (aveEN2 - (aveEN*aveEN))/kt2;
        mag_suscep = (aveMAG2 - (aveMAG*aveMAG))/kt;

        // aveEN /= N;
        aveMAG /= N;
        heat /= N;
        mag_suscep /= N;

        // 計算結果をファイルに記述する関数
        fprintf(energyfile, "%f %f\n", kt/J, aveEN);
        fprintf(magneticfile, "%f %f\n", kt/J, aveMAG);
        fprintf(heatfile, "%f %f\n", kt/J, heat);
        fprintf(mag_suscepfile, "%f %f\n", kt/J, mag_suscep);
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

    free(array);
    free(Init);

    printf("data saved in energy1D.dat magnetic1D.dat heat1D.dat mag_suscep1D.dat\n");

    return 0;

}


// エネルギーを計算する関数
double energy(int *array){
    int i;
    double sum = 0.0;

    for(i=0;i<N;i++){
        sum += array[i]*array[((i==N-1)?0:i+1)];
    }
    return (-J*sum);
}

// 磁化を計算する関数
double magnetic(int *array){
    int i,ans;
    ans = 0;
    for(i=0;i<N;i++){
        ans += array[i];
    }
    return ans;
}

// スピン配列を状態遷移させるための処理
void metropolis3D(int *array, double kt, double *en, double *mag, int s, int d){
    int x, y;
    double diffen;
    // ランダムに選択した1点を反転させた時のエネルギーの差を計算する処理
    x = (rand()%d + s)%N;
    diffen = (double)diffEnergy(array, x);

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
int diffEnergy(int *array, int x){
    int sum;
    sum = array[((x==N-1)?0:x+1)] + array[((x==0)?N-1:x-1)];

    return 2*J*array[x]*sum;
}

// スピン配置を出力する関数
void outputSpin(int *array, int kt){
    FILE *spinfile;
    int i,j;
    char str[1000];
    sprintf(str, "output/spin/spin%d.dat", kt);
    spinfile = fopen(str, "w");
    for(i=0;i<N;i++){
        if(array[i] == 1){
            fprintf(spinfile, "+");
        }else if(array[i] == -1){
            fprintf(spinfile, "-");
        }
    }
    fclose(spinfile);
}

// エネルギーの値を出力する関数
void outputEnergy(FILE *enfile, int times, double en){
    fprintf(enfile, "%d %f\n", times, en);
}