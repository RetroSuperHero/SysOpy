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
Truck truck;
struct Carrier* carrier;

void handleSignal(int signal) {
    printf("Zamykanie procesu ciężarówki\n");
    exit(0);
}

void atExit() {
    if(carrier->semaphores != 0) {
        semctl(carrier->semaphores, 0, IPC_RMID);
    }
    if(sharedMemoryId != 0) {
        shmctl(sharedMemoryId, IPC_RMID, NULL);
    }
}

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result*10 + (string[i] - '0');
    }
    return result;
}

void initSems(int truckCapacity, int carrierLength, int carrierCapacity) {
    key_t sharedMemoryKey = ftok(SHARED_ADDRESS, SHARED_ID);
    if(sharedMemoryKey == -1) {
        fprintf(stderr, "Pobieranie klucza nie powiodło się");
        exit(1);
    }

    sharedMemoryId = shmget(sharedMemoryKey, sizeof(struct Carrier), S_IRWXU | IPC_CREAT);
    if (sharedMemoryId == -1) {
        fprintf(stderr, "Tworzenie dzielonej przestrzeni nie powiodło się");
        exit(1);
    }

    carrier = shmat(sharedMemoryId, 0, 0);
    if (carrier == (void*) -1) {
        fprintf(stderr, "Nie można było dostać się do pamięci współdzielonej");
        exit(1);
    }

    carrier->semaphores = semget(sharedMemoryKey, 1, IPC_CREAT | S_IRWXU);

    if (carrier->semaphores == -1) {
        fprintf(stderr, "Nie można było utworzyć semafora");
        exit(1);
    }

    carrier->carrierLength = carrierLength;
    carrier->carrierCapacity = carrierCapacity;
    carrier->currentQueueIndex = 0;
    carrier->currentQueueLength = 0;
    truck.truckCapacity = truckCapacity;
    
    semctl(carrier->semaphores, CARRIER_WEIGHT_SEMAPHORE, SETVAL, carrierCapacity);
    semctl(carrier->semaphores, CARRIER_LENGTH_SEMAPHORE, SETVAL, carrierLength);
}

void getPackage() {
    while (1) {
        if(carrier->currentQueueLength > 0) {
            if(carrier->carrierQueue[carrier->currentQueueIndex].packageWeight <= truck.truckCapacity - truck.currentWeight) {
                Package newPackage = carrier->carrierQueue[carrier->currentQueueIndex];
                carrier->currentQueueIndex = (carrier->currentQueueIndex + 1)%MAX_QUEUE_SIZE;
                carrier->currentQueueLength--;
                incrementSem(carrier->semaphores, CARRIER_WEIGHT_SEMAPHORE, newPackage.packageWeight);
                incrementSem(carrier->semaphores, CARRIER_LENGTH_SEMAPHORE, 1);
                truck.currentWeight += newPackage.packageWeight;

                printf("Załadowano paczkę:\n  PID pracownika: %d\n  Waga paczki: %d\n  Czas oczekiwania paczki: %ld\n  Obecny ładunek ciężarówki: %d\n  Pozostała pojemność ciężarówki: %d\n",
                    newPackage.workerID, newPackage.packageWeight, time(NULL) - newPackage.loadTime, truck.currentWeight,
                    truck.truckCapacity - truck.currentWeight);
                sleep(1);
            } else {
                printf("Ciężarówka pełna, odjeżdża\n");
                truck.currentWeight = 0;
                sleep(3);
                printf("Podjeżdża nowa ciężarówka\n");
            }
        } else {
            printf("Brak ładunków do zapakowania\n");
            sleep(1);
        }
    }
}

int main(int argc, char** argv) {
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
    atexit(atExit);

    if (argc != 4) {
        fprintf(stderr, "Podano nieprawidłową ilość argumentów\n");
        exit(1);
    }

    int truckCapacity = stringToInt(argv[1]);
    int carrierLength = stringToInt(argv[2]);
    int carrierCapacity = stringToInt(argv[3]);

    initSems(truckCapacity, carrierLength, carrierCapacity);

    printf("Podjeżdża nowa ciężarówka\n");

    getPackage();
}