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

#define NAME_LENGTH 256


int sendFile(int fdout, char *path, unsigned int file_size, int buf_size) {

    int fd;
    unsigned int ret_in;
    unsigned int ret_out;

    char *buffer = malloc(sizeof(char) * buf_size);
    fd = open(path, O_RDONLY);
    path = strchr(path, '/') + 1;

    unsigned short length = strlen(path);
    unsigned short zero = 0;

    write(fdout, &length, 2); printf("SENDFILE: Name length %d \n", length);
    write(fdout, path, length); printf("SENDFILE: Path is %s \n", path);
    write(fdout, &file_size, 4);

    while ((ret_in = read(fd, buffer, buf_size)) > 0) {
        ret_out = write(fdout, buffer, (ssize_t) ret_in);
        if (ret_out != ret_in) {
            /* Write error */
            perror("Write error");
            close(fd);
            free(buffer);
            return 4;
        }
    }
    write(fdout, &zero, 2);
    free(buffer);
    close(fd);
    return 0;
}

void sendProcess(int fdout, char *path, int buf_size) {
    DIR *d;
    size_t path_len = strlen(path);

    d = opendir(path);
    if (d) {
        struct dirent *p;

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
                    if (!S_ISDIR(statbuf.st_mode))      // If it is a file
                    {
                        printf("SEND_PROCESS: File size: %lld bytes\n", (long long) statbuf.st_size);
                        sendFile(fdout, buf, statbuf.st_size, buf_size);
                    } else    // If it is a directory
                    {
                        sendProcess(fdout, buf, buf_size);
                    }
                }
                free(buf);
            }
        }
        closedir(d);
    }
}

int receiveProcess(int fdin, char *path, int buf_size) {

    unsigned short length = 0;
    unsigned short zero = 0;
    unsigned int file_size;

    unsigned int ret_in, sum = 0;
    unsigned int ret_out;

    char pathfname[NAME_LENGTH];
    char dir_mirror[15];

    FILE *mirror_ptr;
    int id_sender, count = 3;


    for (;;) {
        int mirrorfd;
        sum = 0;
        while ((ret_in = read(fdin, &length, 2)) < 0 && count > 0) {
            sleep(30);
            count--;
        }
        if (count == 0 || ret_in <= 0) {
            puts("RECEIVE_PROCESS: Nothing to read\n");
            kill(getppid(), SIGUSR2);
            close(fdin);
            exit(0);
        }
        printf("RECEIVE_PROCESS: File name length  %d \n", length);
        char *file_name = malloc(sizeof(char) * length);


        read(fdin, file_name, length); printf("RECEIVE_PROCESS: file name  %s \n", file_name); //file_name->1_input/lkj.txt
        sscanf(file_name, "%d%s", &id_sender, file_name);
        fprintf(logfp, "File name: %d%s ", id_sender, file_name);

        read(fdin, &file_size, 4); printf("RECEIVE_PROCESS: File size %d \n", file_size);
        fprintf(logfp, "%d\n", file_size);


        char *buffer = malloc(sizeof(char) * buf_size);
        file_name = strchr(file_name, '/') + 1;

        sprintf(dir_mirror, "./%s/%d", path, id_sender);
        sprintf(pathfname, "%s/%s", dir_mirror, file_name);  // path->2_mirror    file_name->lkj.txt


        create_directory(dir_mirror);


        if (fileExists(pathfname)) {
            printf("File %s already exists!!\n",pathfname);
            close(fdin);
            exit(0);
        }

        mirror_ptr = fopen(pathfname, "w");
        if (mirror_ptr == NULL) {
            printf("File %s failed to open.\n",pathfname);
            exit(0);
        }
        mirrorfd = open(pathfname, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        
        while ((ret_in = read(fdin, buffer, buf_size)) > 0 && sum < length) {
            
            if (sum + ret_in >= length) {
                ret_in -= 2;
            }

            ret_out = write(mirrorfd, buffer, ret_in);
            sum = sum + ret_in;
            if (ret_out != ret_in) {
                /* Write error */
                perror("Write error");
                close(mirrorfd);
                free(buffer);
                return 4;
            }
        }
        if (ret_in < 0) {
            kill(getppid(), SIGUSR2);
            return 0;
        }

        kill(getppid(), SIGUSR1);
        fclose(mirror_ptr);
        close(mirrorfd);
        return 0;
    }
    close(fdin);
}

int createClients(int id_client, int my_id, char *common, char *input, int buf_size, char *path) {
    pid_t childpid;
    char namedPipe[NAME_LENGTH];

    sprintf(namedPipe, "%s/%u_to_%u.fifo", common, my_id, id_client);
    if (!pipeExists(namedPipe)) {
        createPipe(namedPipe);
    }

    childpid = fork();
    if (childpid == -1) {
        fprintf(logfp, "Failed  to fork\n");
        return -2;
    }
    if (childpid == 0) {
        // Open FIFO for write only
        int fd_out = open(namedPipe, O_WRONLY);
        sendProcess(fd_out, input, buf_size);
        exit(0);
    }
    sprintf(namedPipe, "%s/%u_to_%u.fifo", common, id_client, my_id);
    if (!pipeExists(namedPipe)) {
        createPipe(namedPipe);
    }


    childpid = fork();
    if (childpid == -1) {
        fprintf(logfp, "Failed  to fork\n");
        return -2;
    }
    if (childpid == 0) {
        // Open FIFO for read only
        int fd_in = open(namedPipe, O_RDONLY);
        receiveProcess(fd_in, path, buf_size);
        exit(0);
    }

    return 0;
}