#include <sys/types.h> 
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

char* stringCat(char* buffer, int args, ...) {
    va_list lst;
    
    va_start(lst, args);
        for(int i=0; i<args; ++i) {
            strcat(buffer, va_arg(lst, char*));
        }  
    va_end(lst);
     
    return buffer;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if(argc < 3) {
        printf("Podano za małą liczbę argumentów.\n");
    } else {
        const char* fifoPath = argv[1];
        const int N = stringToInt(argv[2]);

        int fifoFile = open(fifoPath, O_RDWR);

        printf("%d\n", getpid());

        for(int i=0; i<N; ++i) {
            FILE* date = popen("date", "r");
            char buff[80];
            fread(buff, sizeof(char), 80, date);
            pclose(date);

            char pid[20];
            sprintf(pid, "%d", getpid());

            char* buffer = calloc(120, sizeof(char));
            stringCat(buffer, 3, pid, " ", buff);

            write(fifoFile, buffer, strlen(buff));
            free(buffer);
            sleep(rand()%3 + 2);
        }

        close(fifoFile);
    }
}