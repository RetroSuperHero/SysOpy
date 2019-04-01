#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int catcherPID;
int noOfSignals;
int noSend = 0;
int noOfReceived = 0;

void sendSignal() {
    if(noSend < noOfSignals) {
        kill(catcherPID, SIGUSR1); 
        noSend++; 
    } else {
        kill(catcherPID, SIGUSR2);
        printf("Wysłano %d, a otrzymano %d\n", noOfSignals, noOfReceived);
        exit(0);
    }
}

void receive(int signum) {
    noOfReceived++;
    sendSignal();
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

    if(argc < 3) {
        printf("Podano złą liczbę argumentów\n");
    } else {
        catcherPID = stringToInt(argv[1]);
        noOfSignals = stringToInt(argv[2]);

        sendSignal();
    }

    while(1) { }

    return 0;
}