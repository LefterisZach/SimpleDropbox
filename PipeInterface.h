
#ifndef PIPEINTERFACE_H
#define PIPEINTERFACE_H

int createPipe(char *name);
int pipeExists(const char *filename);
int destroyPipe(char *name);

#endif
