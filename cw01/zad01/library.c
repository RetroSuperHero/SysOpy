#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

blocksContainer* createBlocksContainer(int blocksContainerSize) {
    blocksContainer* tmpContainer = calloc(1, sizeof(blocksContainer));
    tmpContainer->blocksContainerSize = blocksContainerSize;
    tmpContainer->blocksContainerIndex = -1;
    tmpContainer->singleBlockArray = calloc(blocksContainerSize, sizeof(singleBlock));
    return tmpContainer;
}

int createSingleBlock(blocksContainer* blocksContainer, int blocksContainerIndex, char* content, int singleBlockSize) {
    content = realloc(content, singleBlockSize);
    singleBlock* singleBlock = calloc(1, sizeof(singleBlock));
    singleBlock->singleBlockSize = singleBlockSize;
    singleBlock->singleBlockContent = content;
    blocksContainer->singleBlockArray[blocksContainerIndex] = singleBlock;
    return blocksContainerIndex;
}

void deleteSingleBlock(blocksContainer* blocksContainer, int blockContainerIndex) {
    if(blockContainerIndex <= blocksContainer->blocksContainerIndex) {
        free(blocksContainer->singleBlockArray[blockContainerIndex]->singleBlockContent);
        blocksContainer->singleBlockArray[blockContainerIndex]->singleBlockContent = "";
        blocksContainer->singleBlockArray[blockContainerIndex]->singleBlockSize = 0;
    }
}

void initSearch(blocksContainer* blocksContainer, char* directory, char* fileName, char* tmpFileName) {
    blocksContainer->directory = directory;
    blocksContainer->fileName = fileName;
    
    char* command = calloc(STR_DEF_SIZE, sizeof(char));
    strcat(command, "find ");
    strcat(command, directory);
    strcat(command, " -name ");
    strcat(command, fileName);
    strcat(command, " > ");
    strcat(command, tmpFileName);

    int systemSuccess = system(command);

    if(systemSuccess != -1) {
        FILE* outputFile = fopen(tmpFileName, "r");
        char* str = calloc(STR_DEF_SIZE, sizeof(char));
        char* content = calloc(STR_DEF_SIZE, sizeof(char));

        if(outputFile) {
            while(fscanf(outputFile,"%s", str)!=EOF) {
                strcat(content, str);
                strcat(content, " ");
            }

            if(!strcmp(content,"")) {
                strcat(content, "Nie odnalezniono pliku.");
            }
        }

        if(blocksContainer->blocksContainerSize-1 >= blocksContainer->blocksContainerIndex + 1) {        
            createSingleBlock(blocksContainer, ++blocksContainer->blocksContainerIndex, content, strlen(content));
    //      printf("Wyszukiwanie %s w %s powiodło się!\n", fileName, directory);
        } else {
            printf("Brak miejsca w tablicy na wynik wyszukiwania pliku %s w folderze %s!\n", fileName, directory);
        }
        
        fclose(outputFile);
    } else {
        printf("Wywolanie komendy nie powiodło się!\n");
    }
}