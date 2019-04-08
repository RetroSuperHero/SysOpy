#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

int getNumberOfTokens(char* string, char* tokens) {
    int noOfTokens = 1;
    char* stringCpy = calloc(strlen(string), sizeof(char));
    strcpy(stringCpy, string);

    strtok(stringCpy, tokens);
    while(strtok(NULL, tokens)) {
        noOfTokens++;
    }

    free(stringCpy);

    return noOfTokens;
}

char** getCommandElements(char *line){
    char tokens[3] = {' ','\n','\t'};
    int noOfArguments = getNumberOfTokens(line, tokens);

    char** arguments = calloc(noOfArguments+1, sizeof(char*));
    arguments[0] = strtok(line, tokens);

    for(int i=1; i<noOfArguments; ++i) {
        arguments[i] = strtok(NULL, tokens);
    }

    arguments[noOfArguments] = NULL;

    return arguments;
}

void execLine(char* line) {
    char tokens[1] = {'|'};
    int pipes[2][2];
    int commandsNumber = getNumberOfTokens(line, tokens);
    char **commands = calloc(commandsNumber, sizeof(char*));

    commands[0] = strtok(line, tokens);
    for(int i=1; i<commandsNumber; ++i) {
        commands[i] = strtok(NULL, tokens);
    }

    for (int i=0; i<commandsNumber; ++i) {
        if (i>0) {
            close(pipes[i % 2][0]);
            close(pipes[i % 2][1]);
        }

        if(pipe(pipes[i % 2]) == -1) {
            printf("Błąd potoku\n");
            exit(-1);
        }

        if (fork() == 0) {
            char** commandElements = getCommandElements(commands[i]);

            if (i != commandsNumber-1) {
                close(pipes[i % 2][0]);
                if (dup2(pipes[i % 2][1], STDOUT_FILENO) < 0) {
                    exit(-1);
                };
            }
            if (i != 0) {
                close(pipes[(i + 1) % 2][1]);
                if (dup2(pipes[(i + 1) % 2][0], STDIN_FILENO) < 0) {
                    close(-1);
                }
            }
            execvp(commandElements[0], commandElements);
            free(commandElements);

            exit(0);
        }
    }
    free(commands);
    close(pipes[commandsNumber % 2][0]);
    close(pipes[commandsNumber % 2][1]);
    close(pipes[(commandsNumber + 1) % 2][0]);
    close(pipes[(commandsNumber + 1) % 2][1]);
    for(int i=0; i<commandsNumber; ++i) {
        wait(NULL);
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Podano za małą liczbę argumentów.\n");
    } else {
        char* listaPath = argv[1];
        FILE* lista = fopen(listaPath, "r");

        if(lista) {
            fseek(lista, 0L, SEEK_END);
            int size = ftell(lista);
            fseek(lista, 0L, SEEK_SET);

            char* buff = calloc(size, sizeof(char));

            if(fread(buff, sizeof(char), size, lista)) {
                char tokens[1] = {'\n'};
                int noOfLines = getNumberOfTokens(buff, tokens);

                char* lines[noOfLines];
                char* first = strtok(buff, "\n");
                printf("\n");

                for(int i=0; i<noOfLines; ++i) {
                    lines[i] = first;
                    first = strtok(NULL, "\n");
                }
                
                for(int i=0; i<noOfLines; ++i) {
                    execLine(lines[i]);
                }
            }
            free(buff);
        }
    }
}