#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

typedef struct imageMatrix {
    int width;
    int height;
    unsigned char** matrix;
} imageMatrix;

imageMatrix inFileMatrix;
imageMatrix filterMatrix;
imageMatrix outFileMatrix;
int threadsNo;

int max(int first, int second) {
    return first > second ? first : second;
}

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result*10 + (string[i] - '0');
    }
    return result;
}

long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

void missLines(FILE* file, int linesNo) {
    char* line = NULL;
    size_t len;
    for(int i=0; i<linesNo; ++i) {
        getline(&line, &len, file);
    }
}

void getMatrix(imageMatrix* matrix, FILE* inFile) {
    missLines(inFile, 2);

    size_t len = 0;
    char* line = NULL;
    if(getline(&line, &len, inFile) != -1) {
        strtok(line, " ");
        matrix->width = stringToInt(line);
        line = strtok(NULL, " \n\r");
        matrix->height = stringToInt(line);
    }

    matrix->matrix = calloc(matrix->height, sizeof(char*));
    for(int i=0; i<matrix->height; ++i) {
        unsigned char* row = calloc(matrix->width, sizeof(char)); 
        matrix->matrix[i] = row; 
    }

    missLines(inFile, 1);
    getline(&line, &len, inFile);
    line = strtok(NULL, " \n\r");


    for(int i=0; i<matrix->height; ++i) {
        for(int j=0; j<matrix->width; ++j) {
            if(!line) {
                getline(&line, &len, inFile);
                line = strtok(line, " \n\r");
                if(!line) break;
            }
            matrix->matrix[i][j] = stringToInt(line);
            line = strtok(NULL, " \n\r");
        }
    }
}

void getOutMatrix(imageMatrix* matrix) {
    matrix->height = inFileMatrix.height;
    matrix->width = inFileMatrix.width;
    matrix->matrix = calloc(matrix->height, sizeof(char*));
    for(int i=0; i<matrix->height; ++i) {
        unsigned char* row = calloc(matrix->width, sizeof(char)); 
        matrix->matrix[i] = row; 
        for(int j=0; j<matrix->width; ++j) {
            matrix->matrix[i][j] = 0;
        }
    }
}

void filter(int i, int j) {
    for(int k=0; k<filterMatrix.height; ++k) {
        for(int l=0; l<filterMatrix.height; ++l) {
            int half = (int) ceil(0.5*filterMatrix.width);
            int processedPixel = inFileMatrix.matrix[max(1, i - half + k)][max(1, j - half + l)];
            outFileMatrix.matrix[i][j] += processedPixel * filterMatrix.matrix[k][l]/100;
        }
    }
}

void* blockMode(void* id) {
    int thisId = *((int *) id);
    int height = outFileMatrix.height;
    int sector = (int) ceil(1.0*height/threadsNo);
    
    for(int i=(thisId-1)*sector; i<thisId*sector && i<outFileMatrix.height - filterMatrix.height/2; ++i) {
        for(int j=0; j<inFileMatrix.width; ++j) {
            filter(i, j);
        }
    }
    pthread_exit(0);
}

void createOutImage(FILE* outFile) {
    char* text = calloc(20, sizeof(char));
    sprintf(text, "P2\n%d %d\n255\n", outFileMatrix.width, outFileMatrix.height);
    fwrite(text, sizeof(char), strlen(text), outFile);

    for(int i=0; i<outFileMatrix.height; ++i) {
        char* buffer = calloc(outFileMatrix.width*4, sizeof(char));
        for(int j=0; j<outFileMatrix.width; ++j) {
            char one[4];
            sprintf(one, "%d ", outFileMatrix.matrix[i][j]);
            strcat(buffer, one);
        }
        fwrite(buffer, sizeof(char), strlen(buffer), outFile);
        free(buffer);
    }
}

int main(int argc, char** argv) {
    long int start = getMicrotime();

    if(argc != 6) {
        fprintf(stderr, "Podano złą liczbę argumentów\n");
        exit(0);
    }

    FILE* inFile = fopen(argv[3], "r");
    FILE* filterFile = fopen(argv[4], "r");
    FILE* outFile = fopen(argv[5], "w");

    threadsNo = stringToInt(argv[1]);
    pthread_t threads[threadsNo];
    char* mode = argv[2];

    getMatrix(&inFileMatrix, inFile);
    getMatrix(&filterMatrix, filterFile);
    getOutMatrix(&outFileMatrix);

    for(int i=0; i<threadsNo; ++i) {
        pthread_t thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int *arg = malloc(sizeof(*arg));
        *arg = i+1;
        if(!strcmp(mode, "block")) {
            pthread_create(&thread, &attr, &blockMode, arg);
        } else if(!strcmp(mode, "interleaved")) {
            pthread_create(&thread, &attr, &blockMode, arg);
        } else {
            fprintf(stderr, "Podano zły tryb\n");
            exit(0);
        }
        threads[i] = thread;
    }
    
    for(int i=0; i<threadsNo; ++i) {
        pthread_join(threads[i], NULL);
    }

    createOutImage(outFile);

    printf("%ld\n", getMicrotime() - start);

    return 0;
}