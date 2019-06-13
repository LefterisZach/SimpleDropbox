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

#define DIR_PERMISSIONS 0777
#define NAME_LENGTH 256


// This function is creating a directory
void create_directory(char *dir_name) {

    if (mkdir(dir_name, DIR_PERMISSIONS) == -1)     // Creating a directory
        printf("Error :  %s\n ", strerror(errno));    // already exists
    else
        printf("Directory %s created\n",dir_name);      // Creation success
}


int dirExists(const char *dirname) {
    struct stat buffer;
    int exist = stat(dirname, &buffer);
    if (exist == 0)
        return 1;
    else
        return 0;
}


int remove_directory(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;

        while (!r && (p = readdir(d))) {
            int r2 = -1;
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
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = remove_directory(buf);
                    } else {
                        r2 = unlink(buf);
                    }
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }
    if (!r) {
        r = rmdir(path);
    }
    printf("directory deleted successfully!!\n");
    return r;
}


void createFile(char *path, int id) {
    char filename[NAME_LENGTH];
    sprintf(filename, "%s/%u.id", path, id);

    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%u", getpid());
        fclose(fp);
    }
}


int fileExists(const char *filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else
        return 0;
}


void deleteFile(char *path) {
    if (remove(path) == 0)
        printf("File:%s deleted successfully\n", path);
    else
        printf("Unable to delete the file: %s\n", path);
}