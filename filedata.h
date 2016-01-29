#ifndef FILEDATA_H
#define FILEDATA_H
#include "http.h"
struct FileData
{
    char id[NAMESIZE];
    char name[NAMESIZE];
    char size[NAMESIZE];
    char type[NAMESIZE];
    char created_time[NAMESIZE];
    char updated_time[NAMESIZE];
    struct FileData *child;
    struct FileData *next;
};

struct FileData* findfromfolder(struct FileData *path, char *name);
int print(struct FileData*);
struct FileData* ls(char* foldername);
char* find(char *fullpath);
int FreeFileData();
#endif // FILEDATA_H
