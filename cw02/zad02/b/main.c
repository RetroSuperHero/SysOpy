#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>
#include <stdint.h>

char* mode;
int day;
int month;
int year;

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
        case FTW_F:
            type = "file";
            break;
        case FTW_D:
            type = "dir";
            break;
        case FTW_SL:
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

void printData(int tflag, const struct stat *stats, const char* newPath, struct tm* ts) {
    char* type = getType(tflag);
    char* path = realpath(newPath, NULL);
    char* aDate = getDateFromTimestamp(stats->st_atime, ts);
    char* mDate = getDateFromTimestamp(stats->st_mtime, ts);
    printf("%s:\n Pełna ścieżka: %s\n Rodzaj pliku: %s\n Rozmiar pliku: %ld B\n Data ostatniego dostępu: %s\n Data ostatnie modyfikacji: %s\n", newPath, path, type, stats->st_size, aDate, mDate);
}

int checkData(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    struct tm ts = *localtime(&sb->st_mtime);

    if(mode[0] == '=') {
        if(ts.tm_mday == day && ts.tm_mon == month && ts.tm_year == year) {
            printData(tflag, sb, fpath, &ts);             
        }
    } else if(mode[0] == '<') {
        if(ts.tm_year < year || (ts.tm_year == year && ts.tm_mon < month) || (ts.tm_year == year && ts.tm_mon == month && ts.tm_mday < day)) {
            printData(tflag, sb, fpath, &ts);             
        }
    } else if(mode[0] == '>') {
        if(ts.tm_year > year || (ts.tm_year == year && ts.tm_mon > month) || (ts.tm_year == year && ts.tm_mon == month && ts.tm_mday > day)) {
            printData(tflag, sb, fpath, &ts);             
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    char* startDir = argv[1];
    mode = argv[2];
    day = stringToInt(argv[3]);
    month = stringToInt(argv[4]) -1;
    year = stringToInt(argv[5])- 1900;
    int flags = 0;
    flags |= FTW_PHYS;

    if(argc < 6) {
        printf("Podano złą liczbę argumentów\n");
    } else if(strcmp(mode, "=") && strcmp(mode, ">") && strcmp(mode, "<")) {
        printf("Wprowadzono nieprawidłowy znak porówniania\n");
    } else {
        nftw(startDir, checkData, 20, flags);
    }
    return 0;
}
