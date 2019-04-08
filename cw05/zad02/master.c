#include <sys/types.h> 
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Podano za małą liczbę argumentów.\n");
    } else {
        const char* fifoPath = argv[1];
        int fifo = mkfifo(fifoPath, 0777);

        if(!fifo) {
            int fifoFile = open(fifoPath, O_RDWR);

            while(1) {
                char buff[100];
                read(fifoFile, buff, 100);
                printf("--%s\n", buff);
            }

            close(fifoFile);
        }
    }
}