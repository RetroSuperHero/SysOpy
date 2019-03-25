#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/resource.h>

typedef struct monitor {
    int noOfCycles;
    int noOfBackups;
    clock_t startTime;
    time_t modificationTime;
    char* content;
    int contentSize;
} monitor;

double clockToTime(clock_t czas) {
    return (double) (czas) / CLOCKS_PER_SEC;
}

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

char* getDateFromTimestamp(time_t timestamp, struct tm* ts) {
    char* date = calloc(80, sizeof(char));

    *ts = *localtime(&timestamp);
    strftime(date, 80*sizeof(char), "_%Y-%m-%d_%H-%M-%S", ts);
    return date;
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

void setMonitorLib(char* fileName, char* directory, int timeDelay, int maxTime, int maxTimeRes, int maxSizeRes) {
    pid_t child_pid = fork();

    if(child_pid == 0) {
        struct rlimit processLimitTime;
        struct rlimit processLimitSize;
        processLimitTime.rlim_max = maxTimeRes;
        processLimitTime.rlim_cur = maxTimeRes;
        processLimitSize.rlim_max = maxSizeRes * 1048576;
        processLimitSize.rlim_cur = maxSizeRes * 1048576;
        setrlimit(RLIMIT_CPU, &processLimitTime);
        setrlimit(RLIMIT_DATA, &processLimitSize);

        monitor thisMonitor;
        thisMonitor.noOfCycles = 1;
        thisMonitor.noOfBackups = 0;
        thisMonitor.startTime = clock();
        thisMonitor.modificationTime = -1;

        char* filePath = calloc(strlen(fileName) + strlen(directory), sizeof(char));
        stringCat(filePath, 2, directory, fileName);

        while(thisMonitor.noOfCycles*timeDelay + clockToTime(clock() - thisMonitor.startTime) < maxTime) {
            FILE* file = fopen(filePath, "r");

            if(file) {
                struct tm ts;
                struct stat st;
                lstat(fileName, &st);
                
                if(thisMonitor.modificationTime != -1 && thisMonitor.modificationTime != st.st_mtime) {
                    char* newFileName = calloc(strlen(fileName) + 30, sizeof(char));
                    stringCat(newFileName, 3, "./archive/", fileName, getDateFromTimestamp(time(0), &ts));

                    FILE* newFile = fopen(newFileName, "ab+");
                    fwrite(thisMonitor.content, sizeof(char), thisMonitor.contentSize, newFile);
                    fclose(newFile);
                    free(newFileName);

                    thisMonitor.noOfBackups++;
                }
                
                if(thisMonitor.modificationTime == -1 || thisMonitor.modificationTime != st.st_mtime) {
                    fseek(file, 0L, SEEK_END);
                    thisMonitor.contentSize = ftell(file);
                    fseek(file, 0L, SEEK_SET); 

                    thisMonitor.content = calloc(thisMonitor.contentSize, sizeof(char));
                    fread(thisMonitor.content, sizeof(char), thisMonitor.contentSize, file);
                    
                    thisMonitor.modificationTime = st.st_mtime;
                }
            } else {
                printf("Problem ze znalezieniem pliku %s\n", filePath);
                exit(0);
            }

            fclose(file);
            sleep(timeDelay);
            thisMonitor.noOfCycles++;
        }
        
        free(filePath);

        struct rusage usage;
        getrusage(RUSAGE_SELF , &usage); 
        printf("Czas użytkownika: %ldus\nCzas systemowy: %ldus\n", usage.ru_utime.tv_usec, usage.ru_stime.tv_usec);
        exit(thisMonitor.noOfBackups);
    }
}

void setMonitorSys(char* fileName, char* directory, int timeDelay, int maxTime, int maxTimeRes, int maxSizeRes) {
    pid_t child_pid = fork();

    if(child_pid == 0) {
        struct rlimit processLimitTime;
        struct rlimit processLimitSize;
        processLimitTime.rlim_max = maxTimeRes;
        processLimitTime.rlim_cur = maxTimeRes;
        processLimitSize.rlim_max = maxSizeRes * 1048576;
        processLimitSize.rlim_cur = maxSizeRes * 1048576;
        setrlimit(RLIMIT_CPU, &processLimitTime);
        setrlimit(RLIMIT_DATA, &processLimitSize);

        struct tm ts;
        struct stat st;
        monitor thisMonitor;
        thisMonitor.noOfCycles = 1;
        thisMonitor.noOfBackups = 0;
        thisMonitor.startTime = clock();
        thisMonitor.modificationTime = st.st_mtime;

        char* filePath = calloc(strlen(fileName) + strlen(directory), sizeof(char));
        stringCat(filePath, 2, directory, fileName);

        char* command = calloc(999, sizeof(char));
        stringCat(command, 5, "cp ", filePath, " ./archive/", fileName, getDateFromTimestamp(time(0), &ts));
        system(command);
        free(command);

        while(thisMonitor.noOfCycles*timeDelay + clockToTime(clock() - thisMonitor.startTime) < maxTime) {            
            FILE* file = fopen(filePath, "r");

            if(file) {
                lstat(fileName, &st);
                
                if(thisMonitor.modificationTime != st.st_mtime) {
                    char* command = calloc(999, sizeof(char));
                    stringCat(command, 5, "cp ", filePath, " ./archive/", fileName, getDateFromTimestamp(time(0), &ts));
                    system(command);

                    thisMonitor.noOfBackups++;
                    thisMonitor.modificationTime = st.st_mtime;
                }
            } else {
                printf("Problem ze znalezieniem pliku %s\n", filePath);
                exit(0);
            }

            fclose(file);
            sleep(timeDelay);
            thisMonitor.noOfCycles++;
        }
        
        free(filePath);

        struct rusage usage;
        getrusage(RUSAGE_CHILDREN , &usage); 
        printf("Czas użytkownika: %ldus\nCzas systemowy: %ldus\n", usage.ru_utime.tv_usec, usage.ru_stime.tv_usec);
        exit(thisMonitor.noOfBackups);
    }
}

int main(int argc, char* argv[]) {
    int numberOfProcesses = 0;

    if(argc < 6) {
        printf("Podano złą liczbę argumentów\n");
    } else {
        char* listaPath = argv[1];
        int maxTime = stringToInt(argv[2]);
        int mode = stringToInt(argv[3]);
        int maxTimeRes = stringToInt(argv[4]);
        int maxSizeRes = stringToInt(argv[5]);

        FILE* lista = fopen(listaPath, "r");

        if(lista) {
            fseek(lista, 0L, SEEK_END);
            int size = ftell(lista);
            fseek(lista, 0L, SEEK_SET);

            char* buff = calloc(size, sizeof(char));
            if(fread(buff, sizeof(char), size, lista)) {
                char *line = strtok(buff, " ");
                while(line) {
                    char* file = line;
                    line = strtok(NULL, " ");
                    char* directory = line;
                    line = strtok(NULL, "\n");
                    int timeDelay = stringToInt(line);
                    line = strtok(NULL, " ");

                    if(mode == 0) {
                        setMonitorLib(file, directory, timeDelay, maxTime, maxTimeRes, maxSizeRes);
                    } else {
                        setMonitorSys(file, directory, timeDelay, maxTime, maxTimeRes, maxSizeRes);
                    }
                    numberOfProcesses++;
                }
            }
            
            free(buff);
            fclose(lista);
        }
    }

    for(int i=0; i<numberOfProcesses; ++i) {
        int status;
        pid_t processId = wait(&status);
        printf("Proces %d utworzył %d kopii pliku.\n", processId, WEXITSTATUS(status));
    }
    return 0;
}
