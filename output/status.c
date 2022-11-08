#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void){
    int i,j;
    char a = '0';
    printf("\033[44m\n%c",a);
    for(i=0;i<5;i++){
        printf("\ri:%d\n",i+1);
        for(j=0;j<=i;j++){
            sleep(1);
            printf("\r\033[Kj:%d",j+1);
            fflush(stdout);
        }
        printf("\r\033[K\033[1A");
    }
    printf("\n\033[49m\n");
    return 0;
}