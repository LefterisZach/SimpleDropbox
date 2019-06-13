#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "log.h"
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include "FileInterface.h"
#include "CheckInterface.h"
#include "PipeInterface.h"
#include "TransferInterface.h"


int checkDir(char *path, int **array, int *array_size, int my_id) {
    int len, x;
    DIR *d;
    struct dirent *p;
    char buf[FILENAME_MAX];

    struct stat statbuf;
    d = opendir(path);
    if (d) {
        while ((p = readdir(d)) != NULL) {
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }

            len = strlen(path) + strlen(p->d_name) + 2;
            snprintf(buf, len, "%s/%s", path, p->d_name);

            if (!stat(buf, &statbuf) && !S_ISDIR(statbuf.st_mode) &&
                strncmp(p->d_name + (strlen(p->d_name) - 3), ".id", 3) == 0) {
                int flag = 0;
                sscanf(p->d_name, "%d%s", &x, p->d_name);
             //   printf("\ncheckdir: %d", x);
                if (my_id == x)
                    continue;
                for (int j = 0; j < *array_size; ++j) {
                    if ((*array)[j] == x) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) {
                    x = -1;
                    continue;
                }
                *array_size += 1;
                *array = realloc(*array, (*array_size) * sizeof(int));
                (*array)[*array_size - 1] = x;
                break;
            }
        }
        closedir(d);
    }
    return x;
}



int updateArray(char *path, int **array, int *array_size) {
    DIR *d;
    size_t path_len = strlen(path);
    int r = -1, x, flag;

    for (int i = 0; i < *array_size; ++i) {
        flag = 0;
        d = opendir(path);
        if (d) {
            struct dirent *p;
            r = 0;
            while (p = readdir(d)) {
                char *buf;
                size_t len;
                // Skip the names "." and ".." as we don't want to recurse on them.
                if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                    continue;
                }
                len = path_len + strlen(p->d_name) + 2;
                buf = malloc(len);
                if (buf) {

                    struct stat statbuf;
                    snprintf(buf, len, "%s/%s", path, p->d_name);
                    if (!stat(buf, &statbuf)) {
                        if (!S_ISDIR(statbuf.st_mode) && strncmp(p->d_name + (strlen(p->d_name) - 3), ".id", 3) == 0) {
                            sscanf(p->d_name, "%d%s", &x, p->d_name);
                            if (x == (*array)[i]) {
                                flag = 1;
                                free(buf);
                                break;
                            }
                        }
                    }
                    free(buf);
                }
            }
            closedir(d);
        }
        // Erase from array and shift all array elements
        if (flag == 0) {
            if (*array_size == 0) {
                free(*array);
                *array = NULL;
            } else {
                for (int j = i + 1; j < *array_size; ++j) {
                    (*array)[j - 1] = (*array)[j];
                }
                *array_size -= 1;
                *array = realloc(*array, (*array_size) * sizeof(int));
            }
            break;
        }
    }
    return x;
}