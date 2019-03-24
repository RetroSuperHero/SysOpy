#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }
    return result;
}

char* getType(int fileType) {
    char* type;

    switch (fileType) {
        case DT_REG:
            type = "file";
            break;
        case DT_DIR:
            type = "dir";
            break;
        case DT_FIFO:
            type = "fifo";
            break;
        case DT_SOCK:
            type = "sock";
            break;
        case DT_CHR:
            type = "char dev";
            break;
        case DT_BLK:
            type = "block dev";
            break;
        case DT_LNK:
            type = "slink";
            break;
        default:
            type ="error";
            break;
    }

    return type;
}

char* getDateFromTimestamp(time_t timestamp, struct tm* ts) {
    char* date = calloc(80, sizeof(char));

    *ts = *localtime(&timestamp);
    strftime(date, 80*sizeof(char), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    return date;
}

void printData(struct dirent* thisDir, struct stat stats, char* newPath, struct tm* ts) {
    char* type = getType(thisDir->d_type);
    char* path = realpath(newPath, NULL);
    char* aDate = getDateFromTimestamp(stats.st_atime, ts);
    char* mDate = getDateFromTimestamp(stats.st_mtime, ts);
    printf("%s:\n Pełna ścieżka: %s\n Rodzaj pliku: %s\n Rozmiar pliku: %ld B\n Data ostatniego dostępu: %s\n Data ostatnie modyfikacji: %s\n", thisDir->d_name, path, type, stats.st_size, aDate, mDate);

    free(aDate);
    free(mDate);
}

void searchDir(char* dirName, char mode, int day, int month, int year) {
    DIR* currentDirectory = opendir(dirName);

    if(currentDirectory == NULL) {
        printf("Nie ma takiego katalogu\n");
    } else {
        struct dirent* thisDir;

        while((thisDir = readdir(currentDirectory)) != NULL) {
            if(strcmp(thisDir->d_name, ".") && strcmp(thisDir->d_name, "..")) {
                char* newPath = calloc(1024, sizeof(char));
                struct stat stats;

                strcat(newPath, dirName);
                strcat(newPath, "/");
                strcat(newPath, thisDir->d_name);
                lstat(newPath, &stats);

                struct tm ts = *localtime(&stats.st_mtime);

                if(mode == '=') {
                    if(ts.tm_mday == day && ts.tm_mon == month && ts.tm_year == year) {
                        printData(thisDir, stats, newPath, &ts);             
                    }
                } else if(mode == '<') {
                    if(ts.tm_year < year || (ts.tm_year == year && ts.tm_mon < month) || (ts.tm_year == year && ts.tm_mon == month && ts.tm_mday < day)) {
                        printData(thisDir, stats, newPath, &ts);             
                    }
                } else if(mode == '>') {
                    if(ts.tm_year > year || (ts.tm_year == year && ts.tm_mon > month) || (ts.tm_year == year && ts.tm_mon == month && ts.tm_mday > day)) {
                        printData(thisDir, stats, newPath, &ts);             
                    }
                }

                if(thisDir->d_type == DT_DIR && thisDir->d_type != DT_LNK) {
                    searchDir(newPath, mode, day, month, year);
                }

                free(newPath);
            }
        }
    }
}  

int main(int argc, char* argv[]) {
    char* startDir = argv[1];
    char* mode = argv[2];
    char* day = argv[3];
    char* month = argv[4];
    char* year = argv[5];

    if(argc < 6) {
        printf("Podano złą liczbę argumentów\n");
    } else if(strcmp(mode, "=") && strcmp(mode, ">") && strcmp(mode, "<")) {
        printf("Wprowadzono nieprawidłowy znak porówniania\n");
    } else {
        searchDir(startDir, mode[0], stringToInt(day), stringToInt(month) - 1, stringToInt(year) - 1900);
    }
    return 0;
}