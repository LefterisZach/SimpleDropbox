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


int online;

void my_function(int sig) {
    online = 0;
}

void successTransmit(int sig) {
    printf("Received successfully!!\n");
}

void errorTransmit(int sig) {
    printf("Error during file transfer!!\n");
}


int main(int argc, char *argv[]) {

    int i, x, j, id, buffer_size;
    char *common_dir, *input_dir, *mirror_dir, *log_file;

    for (i = 1; i < argc; i++) {
        if (i + 1 != argc) {
            if (strcmp(argv[i], "-n") == 0) {
                id = atoi(argv[i + 1]);
                if (id < 0) {
                    printf("oops, id must be a positive integer\n");
                    exit(-1);
                }
                i++;    // Move to the next flag
            }
            if (strcmp(argv[i], "-c") == 0) {
                common_dir = strdup(argv[i + 1]);
                i++;    // Move to the next flag
            }
            if (strcmp(argv[i], "-i") == 0) {
                input_dir = strdup(argv[i + 1]);
                if (!dirExists(input_dir)) {
                    printf("%s doesn't exist!!", input_dir);
                    exit(-1);
                }
                i++;    // Move to the next flag
            }
            if (strcmp(argv[i], "-m") == 0) {
                mirror_dir = strdup(argv[i + 1]);
                i++;    // Move to the next flag
            }
            if (strcmp(argv[i], "-b") == 0) {
                buffer_size = atoi(argv[i + 1]);
                if (buffer_size <= 0) {
                    printf("oops, buffer_size must be a positive integer\n");
                    exit(-1);
                }
                i++;    // Move to the next flag
            }
            if (strcmp(argv[i], "-l") == 0) {
                log_file = strdup(argv[i + 1]);
                i++;    // Move to the next flag
            }
        }
    }

    create_directory(common_dir);
    create_directory(mirror_dir);
    logfp = fopen(log_file, "w");


    createFile(common_dir, id);

    int *id_array = NULL;
    int count = 0;

    online = 1;

    // Register signals
    signal(SIGINT, my_function);
    signal(SIGQUIT, my_function);
    signal(SIGUSR1, successTransmit);
    signal(SIGUSR2, errorTransmit);


    while (online == 1) {
        sleep(4);

        /* Returns id appeared last or -1 if nothing new detected */
        x = checkDir(common_dir, &id_array, &count, id);

        if (x != -1) {
            if(x!=id){
                createClients(x, id, common_dir, input_dir, buffer_size, mirror_dir);
            }
        }

        updateArray(common_dir, &id_array, &count);
        for (j = 0; j < count; ++j) {
            printf("\n Currently logged-in: %d", id_array[j]);
        }
        puts("\n");
    }


    puts("\nGoing offline..\n");

    free(id_array);
    fclose(logfp);

    free(common_dir);
    free(mirror_dir);   //COMMAND LINE ARGS
    free(input_dir);
    free(log_file);
    return 0;
}
