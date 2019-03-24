#ifndef INC_1_LIBRARY_H
#define INC_1_LIBRARY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STR_DEF_SIZE 9999

typedef struct singleBlock {
    char* singleBlockContent;
    int singleBlockSize;
} singleBlock;

typedef struct blocksContainer {
    singleBlock** singleBlockArray;
    int blocksContainerSize;
    int blocksContainerIndex;
    char* directory;
    char* fileName;
} blocksContainer;

blocksContainer* createBlocksContainer(int blocksContainerSize);
int createSingleBlock(blocksContainer* blocksContainer, int blocksContainerIndex, char* content, int singleBlockSize);
void deleteSingleBlock(blocksContainer* blocksContainer, int blockContainerIndex);
void initSearch(blocksContainer* blocksContainer, char* directory, char* fileName, char* tmpFileName);

#endif