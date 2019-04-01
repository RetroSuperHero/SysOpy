#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void ignore(int signum) {
    printf("Ignored\n");
}

void terminate(int signum) {
    exit(0);
}

void receiveAndSend(int sig, siginfo_t *info, void *context) {
    int signalPid = info->si_pid;

    kill(signalPid, SIGUSR1);
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
    sa.sa_sigaction = receiveAndSend;
    sigaction(SIGUSR1, &sa, NULL);

    signal(SIGUSR2, terminate);

    while(1) { }

    return 0;
}