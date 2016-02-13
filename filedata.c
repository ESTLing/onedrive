#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "filedata.h"
extern struct FileData* FileMes;
extern int GetFolderInfo(struct FileData* Folder);

struct FileData* findfromfolder(struct FileData *path, char *name)
{
     for(; path->id != NULL; path = path->next)
     {
          if(strcmp(name, path->name) == 0)
               return path;
     }
     printf("No such file or folder.\n");
     return 0;
}

int print(struct FileData* Filetmp)
{
     char c;
     long long size;

     printf("%35s", "name");
     printf("%10s", "type");
     printf("%10s", "size");
     printf("%30s", "create");
     printf("%30s\n", "update");
     for(; Filetmp->next != NULL; Filetmp = Filetmp->next)
     {
          size = readable(atoll(Filetmp->size), &c);
          //     printf("id:%s\n",Filetmp->id);
          printf("%35s",Filetmp->name);
          printf("%10s",Filetmp->type);
          printf("%9lld%c", size, c);
          printf("%30s",Filetmp->created_time);
          printf("%30s\n",Filetmp->updated_time);
     }
     return 0;
}

int FreeFileData(struct FileData* File_root)
{
     struct FileData* File_current;
     struct FileData* File_next;
     for(File_current = File_root; File_current != 0; File_current = File_next)
     {
          File_next = File_current->next;
          if(strcmp(File_current->type, "folder") == 0)
               FreeFileData(File_current->child);
          free(File_current);
     }
     return 0;
}

struct FileData* ls(char* foldername)
{
     struct FileData* Filetmp = FileMes;
     struct FileData* last;
     char* tmp1, *tmp2;

     if(*foldername == '\0')  return FileMes;

     strcat(foldername,"/");
     for(tmp1 = foldername,tmp2 = foldername; *tmp1 != '\0'; tmp1++)
     {
          if(*tmp1 == '/')
          {
               *tmp1 = '\0';
               Filetmp = findfromfolder(Filetmp, tmp2);
               GetFolderInfo(Filetmp);
               last = Filetmp;
               Filetmp = Filetmp->child;
               tmp2 = tmp1 + 1;
          }
     }
     return last;
}

char* find(char *fullpath)
{
     char* ptr;
     ptr = ls(fullpath)->id;
     return ptr;
}


long long readable(long long int num,char* c) {
     int i = 0;
     while(num > 1024) {
          num /= 1024;
          ++i;
     }
     switch(i){
     case 0:
          *c = 'B';
          break;
     case 1:
          *c = 'K';
          break;
     case 2:
          *c = 'M';
          break;
     case 3:
          *c = 'G';
          break;
     default:
          break;
     }
     return num;
}
