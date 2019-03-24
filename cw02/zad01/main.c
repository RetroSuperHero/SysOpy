#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>

double clockToTime(clock_t czas) {
    return (double) (czas) / CLOCKS_PER_SEC;
}

double clockToTimes(clock_t czas) {
    int ticks_per_second = sysconf(_SC_CLK_TCK); 
    return (double) (czas) / ticks_per_second;
}

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

void generate(char* fileName, size_t size, int recordNumber) {
    FILE* file = fopen(fileName, "w+");

    if(file) {
        char* content = calloc(size, sizeof(char));
        if(file) {
            for(int j=0; j<recordNumber; ++j) {
                for(int i=0; i<size; ++i) {
                    content[i] = (unsigned char) (rand() % 255 + 1);
                }

                fwrite(content, sizeof(char), size, file);
            }
        }
        fclose(file);
        free(content);
    } else {
        printf("Wystąpił błąd przy generowaniu pliku\n");
    }
}

int sortLib(char* fileName, int recordNumber, int recordSize) {
    FILE* file = fopen(fileName, "r+");

    if(file) {
        char* bufferOne = calloc(recordSize, sizeof(char));
        char* bufferTwo = calloc(recordSize, sizeof(char));
        char one, two;

        for(int i=0; i<recordNumber; ++i) {
            fseek(file, i*recordSize, SEEK_SET);
            fread(&one, sizeof(char), 1, file);
            int smallestPosition = i;
            for(int j=i+1; j<recordNumber; ++j) {
                fseek(file, j*recordSize, SEEK_SET);
                fread(&two, sizeof(char), 1, file);
                if(two < one) {
                    smallestPosition = j;
                    one = two;
                }
            }
            fseek(file, i*recordSize, SEEK_SET);
            fread(bufferOne, sizeof(char), recordSize, file);
            fseek(file, smallestPosition*recordSize, SEEK_SET);
            fread(bufferTwo, sizeof(char), recordSize, file);

            fseek(file, i*recordSize, SEEK_SET);
            fwrite(bufferTwo, sizeof(char), recordSize, file);
            fseek(file, smallestPosition*recordSize, SEEK_SET);
            fwrite(bufferOne, sizeof(char), recordSize, file);
        }

        free(bufferOne);
        free(bufferTwo);
        fclose(file);
        return 0;
    } else {
        return -1;
    }
}

int sortSys(char* fileName, int recordNumber, int recordSize) {
    int file = open(fileName, O_RDWR);

    if(file != -1) {
        char* bufferOne = calloc(recordSize, sizeof(char));
        char* bufferTwo = calloc(recordSize, sizeof(char));
        char one, two;

        for(int i=0; i<recordNumber; ++i) {
            lseek(file, i*recordSize, SEEK_SET);
            read(file, &one, 1);
            int smallestPosition = i;
            for(int j=i+1; j<recordNumber; ++j) {
                lseek(file, j*recordSize, SEEK_SET);
                read(file, &two, 1);
                if(two < one) {
                    smallestPosition = j;
                    one = two;
                }
            }
            lseek(file, i*recordSize, SEEK_SET);
            read(file, bufferOne, recordSize);
            lseek(file, smallestPosition*recordSize, SEEK_SET);
            read(file, bufferTwo, recordSize);

            lseek(file, i*recordSize, SEEK_SET);
            write(file, bufferTwo, recordSize);
            lseek(file, smallestPosition*recordSize, SEEK_SET);
            write(file, bufferOne, recordSize);
        }

        free(bufferOne);
        free(bufferTwo);
        close(file);
        return 0;
    } else {
        return -1;
    }
}

int copyLib(char* inputFileName, char* outputFileName, int recordNumber, int recordSize) {
    FILE* inputFile = fopen(inputFileName, "r+");
    FILE* outputFile = fopen(outputFileName, "w+");

    if(inputFile) {
        char* buffer = calloc(recordSize, sizeof(char));

        for(int i=0; i<recordNumber; ++i) {
            fread(buffer, sizeof(char), recordSize, inputFile);
            fwrite(buffer, sizeof(char), recordSize, outputFile);
        }

        free(buffer);
        fclose(inputFile);
        fclose(outputFile);
        return 0;
    } else {
        return -1;
    }
}

int copySys(char* inputFileName, char* outputFileName, int recordNumber, int recordSize) {
    int inputFile = open(inputFileName, O_RDONLY);
    int outputFile = open(outputFileName, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

    if(inputFile != -1 && outputFile != -1) {
        char* buffer = calloc(recordSize, sizeof(char));

        for(int i=0; i<recordNumber; ++i) {
            read(inputFile, buffer, recordSize);
            int a = write(outputFile, buffer, recordSize);
        }

        free(buffer);
        close(inputFile);
        close(outputFile);
        return 0;
    } else {
        return -1;
    }
}

void copyTime(int (*func)(char*, char*, int, int), char* inputFile, char* outputFile, int recordNumber, int recordSize, char* type) {
    struct tms timer;

    times(&timer);
    clock_t startUser = timer.tms_utime;
    clock_t startSystem = timer.tms_stime;
    clock_t start = clock();
    int copySuccess = func(inputFile, outputFile, recordNumber, recordSize);
    clock_t end = clock();
    times(&timer);
    clock_t endUser = timer.tms_utime;
    clock_t endSystem = timer.tms_stime;

    double realTime = clockToTime(end - start);
    double sysTime = clockToTimes(endSystem - startSystem);
    double userTime = clockToTimes(endUser - startUser);
    
    if(!copySuccess)
        printf("Czas kopiowanie %s dla %d rekordów o rozmiarze %d:\n Rzeczywisty: %fs\n Jądra: %fs\n Użytkownika: %fs\n\n", type, recordNumber, recordSize, realTime, sysTime, userTime);
    else 
        printf("Wystąpił błąd przy kopiowaniu plików\n");
}

void sortTime(int (*func)(char*, int, int), char* fileName, int recordNumber, int recordSize, char* type) {
    struct tms timer;

    times(&timer);
    clock_t startUser = timer.tms_utime;
    clock_t startSystem = timer.tms_stime;
    clock_t start = clock();
    int sortSuccess = func(fileName, recordNumber, recordSize);
    clock_t end = clock();
    times(&timer);
    clock_t endUser = timer.tms_utime;
    clock_t endSystem = timer.tms_stime;

    double realTime = clockToTime(end - start);
    double sysTime = clockToTimes(endSystem - startSystem);
    double userTime = clockToTimes(endUser - startUser);
       
    if(!sortSuccess) 
        printf("Czas sortowanie %s dla %d rekordów o rozmiarze %d:\n Rzeczywisty: %fs\n Jądra: %fs\n Użytkownika: %fs\n\n", type, recordNumber, recordSize, realTime, sysTime, userTime);
    else 
        printf("Wystąpił błąd przy sortowaniu pliku\n");
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int currentPosition = 1;

    while(currentPosition < argc - 1) {
        if(!strcmp(argv[currentPosition],"generate")) {
            char* fileName = argv[currentPosition + 1];
            int recordNumber = stringToInt(argv[currentPosition + 2]);
            int recordSize = stringToInt(argv[currentPosition + 3]);

            generate(fileName, recordSize, recordNumber);

            currentPosition += 4;
        } else if(!strcmp(argv[currentPosition],"sort")) {
            char* fileName = argv[currentPosition + 1];
            int recordNumber = stringToInt(argv[currentPosition + 2]);
            int recordSize = stringToInt(argv[currentPosition + 3]);
            char* type = argv[currentPosition + 4];

            if(!strcmp(type,"sys")) {
                sortTime(sortSys, fileName, recordNumber, recordSize, type);
            } else if (!strcmp(type,"lib")) {
                sortTime(sortLib, fileName, recordNumber, recordSize, type);
            } else {
                printf("Nieprawidłowy sposób sortowania");
            }

            currentPosition += 5;
        } else if(!strcmp(argv[currentPosition],"copy")) {
            char* inputFileName = argv[currentPosition + 1];
            char* outputFileName = argv[currentPosition + 2];
            int recordNumber = stringToInt(argv[currentPosition + 3]);
            int recordSize = stringToInt(argv[currentPosition + 4]);
            char* type = argv[currentPosition + 5];

            if(!strcmp(type,"sys")) {
                copyTime(copySys, inputFileName, outputFileName, recordNumber, recordSize, type);
            } else if (!strcmp(type,"lib")) {
                copyTime(copyLib, inputFileName, outputFileName, recordNumber, recordSize, type);
            } else {
                printf("Nieprawidłowy sposób kopiowania");
            }

            currentPosition += 6;
        } else {
            printf("Nieprawidłowy format, wpisano %s\n", argv[currentPosition]);
            currentPosition++;
        }
    }
    
    return 0;
}