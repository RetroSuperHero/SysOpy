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
#include <signal.h>

typedef struct monitor {
    pid_t PID;
    int stopped;
    int noOfBackups;
    clock_t startTime;
    time_t modificationTime;
    char* content;
    int contentSize;
    int isTerminated;
} monitor;

typedef struct pidArray {
    pid_t* pids;
    int count;
} pidArray;

pidArray pidArr;
monitor thisMonitor;

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

void sigUSR1(int signum) {
    thisMonitor.stopped = 1;
}

void sigUSR2(int signum) {
    thisMonitor.stopped = 0;
}

void sigRTMIN(int signum) {
    printf("Proces o PID %d wykonał %d kopii\n", thisMonitor.PID, thisMonitor.noOfBackups);
    thisMonitor.isTerminated = 1;
}

void sigINT(int signum) {
    for(int i=0; i<pidArr.count; ++i) {
        kill(pidArr.pids[i], SIGRTMIN);
    }
    sleep(1);
    exit(0);
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

void setMonitorLib(char* fileName, char* directory, int timeDelay) {
    pid_t child_pid = fork();

    if(child_pid == 0) { 
        thisMonitor.stopped = 0;
        thisMonitor.PID = getpid();
        thisMonitor.noOfBackups = 0;
        thisMonitor.startTime = clock();
        thisMonitor.modificationTime = -1;

        char* filePath = calloc(strlen(fileName) + strlen(directory), sizeof(char));
        stringCat(filePath, 2, directory, fileName);

        while(!thisMonitor.isTerminated) {
            FILE* file = fopen(filePath, "r");

            if(!thisMonitor.stopped && file) {
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
        }
        
        free(filePath);
        exit(0);
    } else {
        pidArr.pids[pidArr.count++] = child_pid;
    }
}

int main(int argc, char* argv[]) {
    int numberOfProcesses = 0;
    signal(SIGUSR1, sigUSR1);
    signal(SIGUSR2, sigUSR2);
    signal(SIGRTMIN, sigRTMIN);
    signal(SIGINT, sigINT);
    
    pidArr.count = 0;
    pidArr.pids = calloc(10, sizeof(pid_t));

    if(argc < 2) {
        printf("Podano złą liczbę argumentów\n");
    } else {
        char* listaPath = argv[1];
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

                    setMonitorLib(file, directory, timeDelay);
                    numberOfProcesses++;
                }
            }
            
            free(buff);
            fclose(lista);
        }
    }
    
    while(1) {
        char cmd[20];
        char param[20];
        scanf("%s", cmd);
        if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<pidArr.count; ++i) {
                printf("PID: %d\n", pidArr.pids[i]);
            }
        } else if(!strcmp(cmd, "STOP")) {
            scanf("%s", param);
            if(!strcmp(param, "ALL")) {
                for(int i=0; i<pidArr.count; ++i) {
                    kill(pidArr.pids[i], SIGUSR1);
                }
            } else {
                int sPID = stringToInt(param);
                int found = 0;
                for(int i=0; i<pidArr.count && !found; ++i) {
                    if(sPID == pidArr.pids[i]) {
                        found = 1;
                    }
                }
                if(found) {
                    kill(sPID, SIGUSR1);
                } else {
                    printf("Nie ma procesu o podanym PID\n");
                }
            }
        } else if(!strcmp(cmd, "START")) {
            if(!strcmp(param, "ALL")) {
                for(int i=0; i<pidArr.count; ++i) {
                    kill(pidArr.pids[i], SIGUSR2);
                }
            } else {
                int sPID = stringToInt(param);
                int found = 0;
                for(int i=0; i<pidArr.count && !found; ++i) {
                    if(sPID == pidArr.pids[i]) {
                        found = 1;
                    }
                }
                if(found) {
                    kill(sPID, SIGUSR2);
                } else {
                    printf("Nie ma procesu o podanym PID\n");
                }
            }
        } else if(!strcmp(cmd, "END")) {
            for(int i=0; i<pidArr.count; ++i) {
                kill(pidArr.pids[i], SIGRTMIN);
            }
            sleep(1);
            return 0;
        }
    }
    return 0;
}