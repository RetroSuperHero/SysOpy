#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

int processPID;

void sigSTP(int sig, siginfo_t *siginfo, void *context) {
    if(!waitpid(processPID, NULL, WNOHANG)) {
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        kill(processPID, SIGKILL);
    } else {
        if (!(processPID = fork())) {
            execl("/bin/sh", "sh", "./script.sh", (char *) NULL);
        }
    }
}

void sigINT(int signum){
    if(!waitpid(processPID, NULL, WNOHANG))
        kill(processPID, SIGKILL);
    printf("\nOdebrano sygnał SIGINT\n");
    exit(1);
}

void printError(char *message) {
    printf("%s", message);
    exit(1);
}

int main(int argc, char **argv) {
    struct sigaction act;
    act.sa_sigaction = &sigSTP;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTSTP, &act, NULL);

    signal(SIGINT, sigINT);

    if (!(processPID = fork())) {
        execl("/bin/sh", "sh", "./script.sh", (char *) NULL);
        exit(0);
    }
    while(1) {}
}