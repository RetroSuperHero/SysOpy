#include <stdio.h>

#ifndef CHAT_H
#define CHAT_H

enum messageTypes {
    ECHO = 1,
    LIST = 2,
    FRIENDS = 3,
    ADD = 4,
    DEL = 5,
    TO_ALL = 6,
    TO_FRIENDS = 7,
    TO_ONE = 8,
    STOP = 9,
    INIT = 10
};

#define MESSAGE_SIZE sizeof(Message)
#define MAX_MESSAGE_LENGTH 100
#define MAX_CLIENTS 10
#define MAX_FILE_SIZE 1000

typedef struct Message {
    long type;
    long clientID;
    size_t textLength;
    char text[MAX_MESSAGE_LENGTH];
} Message;

char* getTypeName(int typeNumber) {
    switch (typeNumber) {
        case 1:
            return "ECHO";
        case 2:
            return "LIST";
        case 3:
            return "FRIENDS";
        case 4:
            return "ADD";
        case 5:
            return "DEL";
        case 6:
            return "2ALL";
        case 7:
            return "2FRIENDS";
        case 8:
            return "2ONE";
        case 9:
            return "STOP";
        case 10:
            return "INIT";
        default:
            return "ERROR";
    }
}

#endif //CHAT_H