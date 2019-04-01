#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int receivedNo = 0;

void ignore(int signum) {
    printf("Ignored\n");
}

void receive(int signum) {
    receivedNo++;
}

void send(int sig, siginfo_t *info, void *context) {
    int signalPid = info->si_pid;

    for(int i=0; i<receivedNo; ++i) {
        kill(signalPid, SIGUSR1);
    }

    kill(signalPid, SIGUSR2);
    exit(0);
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
    printf("%d\n", getpid());

    struct sigaction act; 
    act.sa_handler = ignore; 
    sigfillset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction(SIGINT, &act, NULL); 

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = send;
    sigaction(SIGUSR2, &sa, NULL);

    struct sigaction se;
    se.sa_flags = SA_SIGINFO;
    se.sa_sigaction = send;
    sigaction(SIGRTMAX, &se, NULL);

    signal(SIGUSR1, receive);
    signal(SIGRTMIN, receive);

    while(1) { }

    return 0;
}