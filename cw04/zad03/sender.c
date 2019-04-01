#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int noOfReceived = 0;

void terminate(int signum) {
    printf("%d\n", noOfReceived);
    exit(0);
}

void receive(int signum) {
    noOfReceived++;
}

void sigqueueMode(int catcherPID, int noOfSignals) {
    for(int i=0; i<noOfSignals; ++i) {
        kill(catcherPID, SIGUSR1);  
    }
    kill(catcherPID, SIGUSR2);
}

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

int main(int argc, char* argv[]) {
    signal(SIGUSR1, receive);
    signal(SIGUSR2, terminate);

    if(argc < 4) {
        printf("Podano złą liczbę argumentów\n");
    } else {
        int catcherPID = stringToInt(argv[1]);
        int noOfSignals = stringToInt(argv[2]);
        int mode = stringToInt(argv[3]);

        if(mode == 0) {
            sigqueueMode(catcherPID, noOfSignals);
        }
    }

    while(1) {

    }

    return 0;
}