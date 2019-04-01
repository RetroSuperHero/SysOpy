#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int tstpNo = 0;

void sigINT(int signum) {
    printf("\nOdebrano sygnał SIGINT\n");
    exit(0);
}

void sigSTP(int signum) {
    tstpNo++;
    if(tstpNo%2 == 1)
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
}

int main() {
    signal(SIGINT, sigINT);

    struct sigaction act; 
    act.sa_handler = sigSTP; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction(SIGTSTP, &act, NULL);

    while(1) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        if(tstpNo%2 == 0) {
            printf("%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            sleep(1);
        }
    }

    return 0;
}