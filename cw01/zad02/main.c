#include "../zad01/library.h"
#include <sys/times.h>
#include <time.h>  
#include <unistd.h> 
#include <limits.h> 
#include <string.h>

typedef struct operationTimes {
    double wholeTime;
    double userTime;
    double systemTime;
} operationTimes;

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

double clockToTime(clock_t czas) {
    return (double) (czas) / CLOCKS_PER_SEC;
}

double clockToTimes(clock_t czas) {
    int ticks_per_second = sysconf(_SC_CLK_TCK); 
    return (double) (czas) / ticks_per_second;
}

int main(int argc, char* argv[]) {
    int size = stringToInt(argv[2]);
    int currentPosition = 3;
    struct tms timer;
    operationTimes operationTime;

    blocksContainer* container = createBlocksContainer(size);

    while(currentPosition < argc - 1) {
        if(!strcmp(argv[currentPosition],"search_directory")) {
            char* directory = argv[currentPosition + 1];
            char* fileName = argv[currentPosition + 2];
            char* tmpFileName = argv[currentPosition + 3];

            times(&timer);
            clock_t startUser = timer.tms_utime;
            clock_t startSystem = timer.tms_stime;
            clock_t startClock = clock();
            initSearch(container, directory, fileName, tmpFileName);
            clock_t endClock = clock();
            times(&timer);
            clock_t endUser = timer.tms_utime;
            clock_t endSystem = timer.tms_stime;

            operationTime.wholeTime = clockToTime(endClock - startClock);
            operationTime.userTime = clockToTimes(endUser - startUser);
            operationTime.systemTime = clockToTimes(endSystem - startSystem);

            printf("add %s/%s\n  ĹÄ…czny czas: %fs\n  Czas uĹĽytkownika: %fs\n  Czas jÄ…dra: %fs\n", directory, fileName, operationTime.wholeTime, operationTime.userTime, operationTime.systemTime);

            currentPosition += 4;
        } else if(!strcmp(argv[currentPosition],"remove_block")) {
            int index = stringToInt(argv[currentPosition + 1]);
            char* file = calloc(container->singleBlockArray[index]->singleBlockSize, sizeof(char));
            strcpy(file, container->singleBlockArray[index]->singleBlockContent);

            times(&timer);
            clock_t startUser = timer.tms_utime;
            clock_t startSystem = timer.tms_stime;
            clock_t startClock = clock();
            deleteSingleBlock(container, index);
            clock_t endClock = clock();
            times(&timer);
            clock_t endUser = timer.tms_utime;
            clock_t endSystem = timer.tms_stime;

            operationTime.wholeTime = clockToTime(endClock - startClock);
            operationTime.userTime = clockToTimes(endUser - startUser);
            operationTime.systemTime = clockToTimes(endSystem - startSystem);

            printf("remove /%s\n  ĹÄ…czny czas: %fs\n  Czas uĹĽytkownika: %fs\n  Czas jÄ…dra: %fs\n", file, operationTime.wholeTime, operationTime.userTime, operationTime.systemTime);

            free(file);

            currentPosition += 2;
        } else {
            printf("NieprawidĹ‚owy format, wpisano %s\n", argv[currentPosition]);
            currentPosition++;
        }
    }

//  for(int i=0; i<=container->blocksContainerIndex; ++i) {
//      printf("%s\n", container->singleBlockArray[i]->singleBlockContent);
//  }
}