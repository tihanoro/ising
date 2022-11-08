// ising.c 一次元イジングモデルのシミュレーションを行うプログラム
// メトロポリス法

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 1000
#define J 1
#define LOOP 100000

double energy(int array[]);
double magnetic(int array[]);
void metropolis1D(int array[], double kt, int *newen, int *newmag, int *olden, int *oldmag);

int main(void){
    int i, j, element, array[N];
    int t;
    int olden, newen, oldmag, newmag, en2, mag2;
    double heat, mag_suscep;
    double aveEN, aveMAG, aveEN2, aveMAG2, sumEN, sumMAG, kt, kt2;

    FILE *energyfile, *magneticfile, *heatfile, *fluctenfile;
    energyfile = fopen("energy1.dat", "w");
    magneticfile = fopen("magnetic1.dat", "w");
    heatfile = fopen("heat1.dat", "w");
    fluctenfile = fopen("fluctenfile.dat", "w");

    srand((unsigned)time(NULL));

    printf("N:%d\n",N);
    printf("繰り返し%d回\n",LOOP);
    
    for(i=0;i<N;i++){
        if(rand()%2){
            array[i] = 1;
        }else{
            array[i] = -1;
        }
    }

    // 温度変化させるループ
    for(kt=5.00;kt>0;kt-=0.01){

        printf("\r現在kt=%fを処理中\n",kt);

        // 初期配列(低温)
        // for(i=0;i<N;i++){
        //     array[i] = 1;
        // }
        

        // 物理量の平均値の初期化
        sumEN = 0;
        sumMAG = 0;
        en2 = 0;
        mag2 = 0;
        kt2 = kt*kt;
        
        
        // 状態を変化させる処理
        newen = energy(array);
        newmag = magnetic(array);

        // 熱平衡かさせるための事前動作
        for(i=0;i<10*N;i++){
            metropolis1D(array, kt, &newen, &newmag, &olden, & oldmag);
        }

        // 物理量を計算するループ
        for(t=0;t<LOOP;t++){

            metropolis1D(array, kt, &newen, &newmag, &olden, &oldmag);

            sumEN += newen;
            sumMAG += abs(newmag);
            en2 += newen*newen;
            mag2 += newmag*newmag;
        }

        aveEN = sumEN/LOOP;
        aveMAG = sumMAG/LOOP;
        aveEN2  = en2/LOOP;

        heat = (aveEN2 - (aveEN*aveEN))/kt2;
        mag_suscep = (aveMAG2 - (aveMAG*aveMAG))/kt;

        // aveEN /= N;
        // aveMAG /= N;
        // heat /= N;
        mag_suscep /= N;

        fprintf(energyfile, "%f %f\n", kt, aveEN);
        fprintf(magneticfile, "%f %f\n", kt, aveMAG);
        fprintf(heatfile, "%f %f\n", kt, heat);
        fprintf(fluctenfile, "%f %f\n", kt, mag_suscep);

        printf("\r\033[Kエネルギー:%f\n",aveEN);
        printf("\r\033[K磁化:%f\n",aveMAG);
        printf("\r\033[K比熱:%f\n",heat);
        printf("\033[%dA",4);
        fflush(stdout);
    }
    printf("\033[%dB\n",3);
    fclose(energyfile);
    fclose(magneticfile);
    fclose(heatfile);
    fclose(fluctenfile);

    printf("data saved in energy.dat magnetic.dat heat.dat\n");

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

double magnetic(int array[]){
    int i,ans;
    ans = 0;
    for(i=0;i<N;i++){
        ans += array[i];
    }
    return ans;
}

// スピン配列を状態遷移させるための処理
void metropolis1D(int array[], double kt, int *newen, int *newmag, int *olden, int *oldmag){
    int element;
    // ランダムに選択した1点を反転させる処理
    *olden = *newen;
    *oldmag = *newmag;
    // *olden = energy(array);
    // *oldmag = magnetic(array);
    element = (double)rand()/RAND_MAX*N;
    array[element] *= -1;
    *newen = energy(array);
    // if(array[element]){
    //     *newmag = *oldmag + 2;
    // }else{
    //     *newmag = *oldmag - 2;
    // }
    *newmag = magnetic(array);

    // もしエネルギーがnew>oldのとき、exp(dE/kt)の確率で変化を受容
    if((*newen>*olden) && (exp((-(*newen) + *olden)/kt)) <= (double)rand()/RAND_MAX){
        array[element]  *= -1;
        *newen = *olden;
        *newmag = *oldmag;
    }
}