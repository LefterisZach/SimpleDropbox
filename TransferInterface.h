
#ifndef TRANSFERINTERFACE_H
#define TRANSFERINTERFACE_H

int sendFile(int fdout, char *path, unsigned int file_size, int buf_size);
void sendProcess(int fdout, char *path, int buf_size);
int receiveProcess(int fdin, char *path, int buf_size);
int createClients(int id_client, int my_id, char *common, char *input, int buf_size, char *path);

#endif
