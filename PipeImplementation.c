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

#define PIPE_PERMISSIONS 0777

int createPipe(char *name) {

    int returnValue = mkfifo(name, PIPE_PERMISSIONS);
    if (returnValue < 0) {
        fprintf(logfp, "mkfifo() failed. %s\n", name);
        fprintf(logfp, "errno = %d\n", errno);
        if (errno == EEXIST) {
            fprintf(logfp, "That file already exists.\n");
            fprintf(logfp, "(or we passed in a symbolic link, which we did not.)\n");
        }
    }
    return returnValue;
}

// This function checks if a pipe exists
int pipeExists(const char *filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else
        return 0;
}

int destroyPipe(char *name) {
    int returnValue = unlink(name);
    if (returnValue == 0) {
        fprintf(logfp, "FIFO deleted.\n");
    }
    return returnValue;
}