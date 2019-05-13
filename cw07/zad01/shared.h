#ifndef SHARED
#define SHARED

#define SHARED_ADDRESS getenv("HOME")
#define SHARED_ID 0xDEAD

#define CARRIER_WEIGHT_SEMAPHORE 0
#define CARRIER_LENGTH_SEMAPHORE 1
#define MAX_QUEUE_SIZE 64

typedef struct Package {
    int packageWeight;
    pid_t workerID;
    time_t loadTime;
} Package;

typedef struct Truck{
    int truckCapacity;
    int currentWeight;
} Truck;

struct Carrier {
    int semaphores;
    int currentQueueIndex;
    int currentQueueLength;
    int carrierLength;
    int carrierCapacity;
    Package carrierQueue[MAX_QUEUE_SIZE];
} *Carrier;

void decrementSem(int semaphores, int semaphoreID, int value) {
    struct sembuf s;
    s.sem_flg = 0;
    s.sem_num = semaphoreID;
    s.sem_op = -1 * value;
    semop(semaphores, &s, 1);
}

void incrementSem(int semaphores, int semaphoreID, int value) {
    struct sembuf s;
    s.sem_flg = 0;
    s.sem_num = semaphoreID;
    s.sem_op = value;
    semop(semaphores, &s, 1);
}

#endif