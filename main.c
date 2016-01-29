#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <resolv.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "operator.h"

char api_code[NAMESIZE];
char api_client_id[NAMESIZE] = "000000004C12EED0";
char api_client_secret[NAMESIZE] = "b77A2bGXms5rtF4hDS9TpoxGZSFuktMm";
char api_redirect_url[NAMESIZE] = "https%3A%2F%2Flogin.live.com%2Foauth20_desktop.srf";
char api_access_token[NAMESIZE];
char api_refresh_token[NAMESIZE];
char api_authentication_token[NAMESIZE];
char api_user_id[NAMESIZE];

char client_request[REQUESTSIZE] ;
char service_respones[REQUESTSIZE];
char client_request_date[REQUESTSIZE];
char tem_buf[NAMESIZE];

int client_fd;
struct sockaddr_in serves_addr;
SSL_CTX *ctx;
SSL *ssl;
int state;

struct FileData* FileMes;

int help();
int GetFileData();
int GetFileDataFromRespones();

int main(int argc,char* argv[])
{
    char args[3][NAMESIZE];
    char input[NAMESIZE];
    char *tmp1, *tmp2;
    int i;

    printf("Plesae enter this webpage and get your code.\n");
    printf("https://login.live.com/oauth20_authorize.srf?client_id=000000004C12EED0"
           "&scope=wl.signin%%20wl.skydrive%%20wl.skydrive_update%%20wl.basic%%20wl.offline_access%%20wl.photos"
           "&response_type=code&display=touch"
           "&redirect_uri=https%%3A%%2F%%2Flogin.live.com%%2Foauth20_desktop.srf");
    printf("\n\ncode:");
    scanf("%s",api_code);
    fgets(input, NAMESIZE, stdin);

    if(GetToken(api_code) == -1) exit(0);
    if(GetFileData() == -1) exit(0);
    if(GetQuotaData() == -1) exit(0);
    while(1)
    {
        printf("Onedrive:");
        fgets(input, NAMESIZE, stdin);
        memset(args[0], 0, NAMESIZE);
        memset(args[1], 0, NAMESIZE);
        memset(args[2], 0, NAMESIZE);
        for(tmp1 = input, tmp2 = input, i = 0; *tmp1 != '\n'; tmp1++)
        {
            if(*tmp1 ==' ')
            {
                *tmp1 = '\0';
                strcpy(args[i], tmp2);
                tmp2 = tmp1 + 1;
                i++;
            }
        }
        *tmp1 = '\0';
        strcpy(args[i], tmp2);

        if(strcmp(args[0], "help") == 0) {help(); continue;}
        if(strcmp(args[0], "exit") == 0) break;
        if(strcmp(args[0], "upload") == 0) {
            if(upload(args[1], args[2]) == -1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "download") == 0) {
            if(download(find(args[1])) == -1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "move") == 0) {
            if(move_copy("MOVE", find(args[1]), find(args[2])) == -1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "copy") == 0) {
            if(move_copy("COPY", find(args[1]), find(args[2])) == -1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "delete") == 0) {
            if(deletefile(find(args[1])) == -1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "create") == 0) {
            if(createfolder(args[1]) ==-1)
                exit(0);
            continue;
        }
        if(strcmp(args[0], "ls") == 0){
            if(*args[1] == '\0')
            {
                print(FileMes);
                continue;
            }
            else
            {
                print(ls(args[1])->child );
                continue;
            }
        }
        if(strcmp(args[0], "") == 0) continue;
        printf("command not found.\n");
    }
 //   if(upload("hello.txt","Hello.txt") == -1) exit(0);
 //   if(download("file.c8afda5ddbd9254d.C8AFDA5DDBD9254D!191") == -1) exit(0);
 //   if(move_copy("MOVE","file.c8afda5ddbd9254d.C8AFDA5DDBD9254D!191","folder.c8afda5ddbd9254d.C8AFDA5DDBD9254D!118") == -1) exit(0);
 //   if(deletefile("file.c8afda5ddbd9254d.C8AFDA5DDBD9254D!192") == -1) exit(0);
 //   if(createfolder("example") == -1) exit(0);
    return 0;
}

int GetFileData()
{
    GetConnect();
    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "GET /v5.0/me/skydrive/files?access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n\r\n");
 //   printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
   // printf("sned success!\n");

    memset(service_respones, 0, REQUESTSIZE);
    if((state = SSL_read(ssl, service_respones, REQUESTSIZE)) == -1)
    {
        printf("recv faile.\n");
        return -1;
    }
    else if(state == 0)
    {
        printf("No available data.\n");
  //      return -1;
    }
 //   printf("recv success!\n");
    GetFileDataFromRespones();

    CloseConnect();
    return 0;
}

int GetFileDataFromRespones()
{
    char *tmp = service_respones;
    FileMes = (struct FileData*)malloc(sizeof(struct FileData));
    memset(FileMes, 0, sizeof(struct FileData));
    struct FileData *Filetmp = FileMes;
    while(1)
    {
        if((tmp = GetFrom(tmp,"id") ) == 0)
        {
            break;
        }
        strcat(Filetmp->id, tmp);
     //   printf("id:%s\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp ,"name");
        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "name");
        strcat(Filetmp->name, tmp);
     //   printf("name:%s\t\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        for(;*tmp != '\0'; tmp++)
        {
            if(*tmp == '"')
            {
                tmp++;
                if(strncmp(tmp, "size",4) == 0)
                    break;
            }
        }
        tmp = tmp + 7;
     //   printf("size:");
        for(;*tmp != ',';tmp++)
        {
            strncat(Filetmp->size, tmp, 1);
    //        printf("%c",*tmp);
        }
   //     printf("\n");

        tmp = GetFrom(tmp, "link");
        tmp = tmp + strlen(tmp) + 1;

        tmp = GetFrom(tmp, "type");
        strcat(Filetmp->type, tmp);
    //    printf("type:%s\n", tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "created_time");
        strcat(Filetmp->created_time, tmp);
     //   printf("created_time:%s\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "updated_time");
        strcat(Filetmp->updated_time, tmp);
 //       printf("updated_time:%s\n\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        Filetmp->next = (struct FileData*)malloc(sizeof(struct FileData));
        Filetmp = Filetmp->next;
        memset(Filetmp, 0, sizeof(struct FileData));
    }
    Filetmp = 0;
    return 0;
}

int help()
{
    printf("\nWelcome to onedrive client!\n");
    printf("usage:\n");
    printf("\t exit:exit.\n");
    printf("\t upload filename1 filename2: upload filename1 to onedrive and rename filename2.\n");
    printf("\t download filename:   download this file to your computer.\n");
    printf("\t move filename pathname:  move filename to pathname.\n");
    printf("\t copy filename pathname:  copy filename to pathname.\n");
    printf("\t delete filename(foldername): delete this file or folder.\n");
    printf("\t create foldername:   create a folder.\n");
    printf("\t help:    show this message.\n");
    printf("\t ls foldername:  show this folder.\n");
    return 0;
}
