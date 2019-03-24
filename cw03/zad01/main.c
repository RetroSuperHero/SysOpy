#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/types.h>
#include <unistd.h>

void printData(const char* path, int pid, char* command) {
    printf("Katalog: %s\nPID: %d\n", path, pid);
    system(command);
    printf("\n");
    exit(0);
}

int checkData(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if(tflag == FTW_D) {
        pid_t child_pid;
        child_pid = vfork();
        if(child_pid == 0) {
            char* command = calloc(999, sizeof(char));
            strcat(command, "ls -l ");
            strcat(command, fpath);
            printData(fpath, (int)getpid(), command);
        }
    } 

    return 0;
}

int main(int argc, char* argv[]) {
    char* startDir = argv[1];
    int flags = 0;
    flags |= FTW_PHYS;

    if(argc < 2) {
        printf("Podano złą liczbę argumentów\n");
    } else {
        nftw(startDir, checkData, 20, flags);
    }
    return 0;
}
