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

#endif //CHAT_H