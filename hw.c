#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_BUFFER_SIZE 10




int main(int argc, char *argv[]){
    //grabs the 5 numbers that represent [p c i b d]
    int p = atoi(argv[1]);
    int c = atoi(argv[2]);
    int i = atoi(argv[3]);
    int b = atoi(argv[4]);
    int d = atoi(argv[5]);

    if (c < (p * i)){
        perror("c should be less than (p * i)");
        exit(1);
    }
    else if (b > MAX_BUFFER_SIZE){
        perror("buffer has to be less than 10");
        exit(1);
    }

    if(d == 0){//0.5s delay for consumer
        

    }
    else if (d==1){ //0.5s delay for producer

    }
    else{ //no delay

    }



    return 0;
}