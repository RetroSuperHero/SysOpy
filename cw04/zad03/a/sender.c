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

void killMode(int catcherPID, int noOfSignals) {
    for(int i=0; i<noOfSignals; ++i) {
        kill(catcherPID, SIGUSR1);  
    }
    kill(catcherPID, SIGUSR2);
}

void sigqueueMode(int catcherPID, int noOfSignals) {
    for(int i=0; i<noOfSignals; ++i) {
        const union sigval value;
        sigqueue(catcherPID, SIGUSR1, value);
    }
    const union sigval value;
    sigqueue(catcherPID, SIGUSR2, value);
}

void sigrtMode(int catcherPID, int noOfSignals) {
    for(int i=0; i<noOfSignals; ++i) {
        kill(catcherPID, SIGRTMIN);
    }
    kill(catcherPID, SIGRTMAX);
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
            killMode(catcherPID, noOfSignals);
        } else if(mode == 1) {
            sigqueueMode(catcherPID, noOfSignals);
        } else if(mode == 2) {
            sigrtMode(catcherPID, noOfSignals);
        } else {
            printf("Podano nieprawidłowy tryb\n");
            exit(0);
        }
    }

    while(1) {

    }

    return 0;
}