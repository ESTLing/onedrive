#ifndef OPERATOR_H
#define OPERATOR_H
#include "filedata.h"

int GetQuotaData();
int GetQuotaDataFromRespones();
int upload(char *client_name,char *service_name);
int download(char *service_name);
int move_copy(char *operation,char *path1, char *path2);
int deletefile(char *path);
int createfolder(const  char*);
int GetFolderInfo(struct FileData*);
int GetFolderInfoFromRespones(struct FileData* folder);

#endif // OPERATOR_H
