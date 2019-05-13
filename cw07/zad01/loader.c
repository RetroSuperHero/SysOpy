#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "shared.h"

int sharedMemoryId;
struct Carrier* carrier;

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result*10 + (string[i] - '0');
    }
    return result;
}

void putPackage(Package package) {
    decrementSem(carrier->semaphores, CARRIER_LENGTH_SEMAPHORE, 1);
    decrementSem(carrier->semaphores, CARRIER_WEIGHT_SEMAPHORE, package.packageWeight);
    package.loadTime = time(NULL);
    carrier->carrierQueue[carrier->currentQueueIndex + carrier->currentQueueLength] = package;
    carrier->currentQueueLength++;

    printf("Załadowano paczkę:\n  PID pracownika: %d\n  Waga paczki: %d\n  Czas wysłania: %ld\n", package.workerID, package.packageWeight, package.loadTime);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    if(argc != 3) {}

    int workers = stringToInt(argv[1]);
    int maxPackageWeight = stringToInt(argv[2]);

    key_t sharedMemoryKey = ftok(SHARED_ADDRESS, SHARED_ID);
    if(sharedMemoryKey == -1) {
        fprintf(stderr, "Pobieranie klucza nie powiodło się");
        exit(1);
    }

    sharedMemoryId = shmget(sharedMemoryKey, 0, 0);
    if (sharedMemoryId == -1) {
        fprintf(stderr, "Otwieranie dzielonej przestrzeni nie powiodło się");
        exit(1);
    }

    carrier = shmat(sharedMemoryId, 0, 0);
    if (carrier == (void*) -1) {
        fprintf(stderr, "Nie można było dostać się do pamięci współdzielonej");
        exit(1);
    }

    for(int i=0; i<workers; ++i){
        srand(time(NULL));
        if(fork() == 0){
            int packageWeight = rand() % maxPackageWeight + 1;

            Package package;
            package.workerID = getpid();
            package.packageWeight = packageWeight;

            putPackage(package);
            exit(0);
        }
        sleep(1);
    }

    for (int i = 0; i<workers; ++i) {
        wait(NULL);
    }
}