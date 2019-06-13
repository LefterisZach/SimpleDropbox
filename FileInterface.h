
#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H


void create_directory(char *dir_name);
int dirExists(const char *dirname);
int remove_directory(const char *path);
void createFile(char *path, int id);
int fileExists(const char *filename);
void deleteFile(char *path);

#endif
