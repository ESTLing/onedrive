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
#include "filedata.h"

extern char api_code[NAMESIZE];
extern char api_client_id[];
extern char api_client_secret[];
extern char api_redirect_url[];
extern char api_access_token[];
extern char api_refresh_token[];
extern char api_authentication_token[];
extern char api_user_id[];

extern char client_request[];
extern char service_respones[];
extern char client_request_date[];
extern char tem_buf[];

extern int client_fd;
extern struct sockaddr_in serves_addr;
extern SSL_CTX *ctx;
extern SSL *ssl;
extern int state;

extern struct FileData* FileMes;
extern int GetFileData();
extern int GetFileDataFromRespones();

int GetQuotaData()
{
    GetConnect();
    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "GET /v5.0/me/skydrive/quota?access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n\r\n");
 //   printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
  //  printf("sned success!\n");

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
  //  printf("recv success!\n");
    GetQuotaDataFromRespones();

    CloseConnect();
    return 0;
}

int GetQuotaDataFromRespones()
{
// example:    {"quota": 32212254720,\r"available": 31572243278}
    char *tmp1 = service_respones;
    char *tmp2;
    char c;
    long long num;
    for(; *tmp1 != '{'; tmp1++);
    for(; *tmp1 != '"'; tmp1++);
    tmp1++;
    for(tmp2 = tmp1 + 1; *tmp2 != '"'; tmp2++);
    *tmp2 = '\0';
    for(; *tmp2 != ','; tmp2++);
    *tmp2 = '\0';
    for(; *tmp2 != '"'; tmp2++);
    for(tmp2 += 1; *tmp2 != '"'; tmp2++);
    *tmp2 = '\0';
    for(; *tmp2 != '}'; tmp2++);
    *tmp2 = '\0';
    printf("%s:\t\t",tmp1);
    for(; *tmp1 != ':'; tmp1++);
    tmp1 += 2;
    num = readable(atoll(tmp1), &c);
    printf("%lld%c\n", num, c);
    for(; *tmp1 != '"'; tmp1++);
    tmp1++;
    printf("%s:\t", tmp1);
    for(; *tmp1 != ':'; tmp1++);
    tmp1 += 2;
    num = readable(atoll(tmp1), &c);
    printf("%lld%c\n", num, c);

    return 0;
}

int upload(char *client_name,char *service_name)
{
    int file_fd;
    char file_data[REQUESTSIZE - NAMESIZE];
    int buf_size;

    GetConnect();
    file_fd = open(client_name, O_RDONLY);
    buf_size = read(file_fd, file_data, REQUESTSIZE - NAMESIZE);
    sprintf(tem_buf, "%d", buf_size);

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "PUT /v5.0/me/skydrive/files/");
    strcat(client_request, service_name);
    strcat(client_request, "?access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n");
 //   strcat(client_request, "Content-Type:application/octet-stream\r\n");
    strcat(client_request, "Content-Length:");
    strcat(client_request, tem_buf);
    strcat(client_request, "\r\n\r\n");
    strcat(client_request, file_data);
  //  printf("\n\n%s\n\n\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
 //   printf("sned success!\n");
 //   printf("%s\n", client_request);

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
 //   printf("%s\n", service_respones);

    close(file_fd);
    CloseConnect();

    FreeFileData(FileMes);
    GetFileData();
    return 0;
}

int download(char *service_name)
{
    char *location,*location_tmp;

    GetConnect();
    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "GET /v5.0/");
    strcat(client_request, service_name);
    strcat(client_request, "/content?download=true&access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n\r\n");
  //  printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
//    printf("sned success!\n");

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
//    printf("recv success!\n");
//    printf("%s\n\n",service_respones);
    for(location = service_respones; *location != '\0'; location++)
    {
        if(*location == '\n')
        {
            location++;
            if(strncmp(location, "Location", 8) == 0)
                break;
        }
    }
    location+=10;
    for(location_tmp = location; *location_tmp != '\r'; location_tmp++);
    for(; *location_tmp !='?'; location_tmp--);
    *location_tmp = '\0';
    location-=strlen("wget --no-check-certificate ");
    strncpy(location, "wget --no-check-certificate ",strlen("wget --no-check-certificate "));
   // printf("%s",location);
    system(location);

    CloseConnect();
    FreeFileData(FileMes);
    GetFileData();
    return 0;
}

int move_copy(char *operation,char *path1, char *path2)
{
    GetConnect();

    memset(client_request_date, 0, REQUESTSIZE);
    strcat(client_request_date, "destination=");
    strcat(client_request_date, path2);

    sprintf(tem_buf ,"%d" ,strlen(client_request_date));

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, operation);
    strcat(client_request, " /v5.0/");
    strcat(client_request, path1);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n");
    strcat(client_request, "Authorization: Bearer ");
    strcat(client_request, api_access_token);
    strcat(client_request, "\r\n");
    strcat(client_request, "Content-type:application/x-www-form-urlencoded\r\n");
    strcat(client_request, "Content-Length:");
    strcat(client_request, tem_buf);
    strcat(client_request, "\r\n\r\n");
    strcat(client_request, client_request_date);
 //   printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
 //   printf("sned success!\n");

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
  //  printf("%s\n",service_respones);

    CloseConnect();
    FreeFileData(FileMes);
    GetFileData();
    return 0;
}

int deletefile(char *path)
{
    GetConnect();

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "DELETE /v5.0/");
    strcat(client_request, path);
    strcat(client_request, "?access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n\r\n");
 //   printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
 //   printf("sned success!\n");

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
 //   printf("%s\n",service_respones);

    CloseConnect();
    FreeFileData(FileMes);
    GetFileData();
    return 0;
}

int createfolder(const char *name)
{
    GetConnect();

    memset(client_request_date, 0, REQUESTSIZE);
  //  strcat(client_request_date, "type=folder");
    strcat(client_request_date, "name=");
    strcat(client_request_date, name);

    sprintf(tem_buf ,"%d" ,strlen(client_request_date));

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "POST /v5.0/me/skydrive");
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n");
    strcat(client_request, "Authorization: Bearer ");
    strcat(client_request, api_access_token);
    strcat(client_request, "\r\n");
    strcat(client_request, "Content-type:application/x-www-form-urlencoded\r\n");
    strcat(client_request, "Content-Length:");
    strcat(client_request, tem_buf);
    strcat(client_request, "\r\n\r\n");
    strcat(client_request, client_request_date);
//    printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
 //   printf("sned success!\n");

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
//    printf("recv success!\n");
//    printf("%s\n",service_respones);

    CloseConnect();
    FreeFileData(FileMes);
    GetFileData();
    return 0;
}

int GetFolderInfo(struct FileData* Folder)
{
    GetConnect();
    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "GET /v5.0/");
    strcat(client_request, Folder->id);
    strcat(client_request, "/files?access_token=");
    strcat(client_request, api_access_token);
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST: apis.live.net\r\n\r\n");
 //   printf("%s\n",client_request);

    if((SSL_write(ssl, client_request, strlen(client_request))) == -1)
    {
        printf("send fail.\n");
        return -1;
    }
 //   printf("sned success!\n");

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
 //   printf("%s\n",service_respones);
    GetFolderInfoFromRespones(Folder);

    CloseConnect();
    return 0;
}

int GetFolderInfoFromRespones(struct FileData* Folder)
{
    char *tmp = service_respones;
    Folder->child = (struct FileData*)malloc(sizeof(struct FileData));
    memset(Folder->child, 0, sizeof(struct FileData));
    struct FileData *Filetmp = Folder->child;
    while(1)
    {
        if((tmp = GetFrom(tmp,"id") ) == 0)
        {
            break;
        }
        strcat(Filetmp->id, tmp);
    //    printf("id:%s\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp ,"name");
        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "name");
        strcat(Filetmp->name, tmp);
   //     printf("name:%s\t\n",tmp);

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
   //     printf("size:");
        for(;*tmp != ',';tmp++)
        {
            strncat(Filetmp->size, tmp, 1);
   //         printf("%c",*tmp);
        }
   //     printf("\n");

        tmp = GetFrom(tmp, "link");
        tmp = tmp + strlen(tmp) + 1;

        tmp = GetFrom(tmp, "type");
        strcat(Filetmp->type, tmp);
     //   printf("type:%s\n", tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "created_time");
        strcat(Filetmp->created_time, tmp);
     //   printf("created_time:%s\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        tmp = GetFrom(tmp, "updated_time");
        strcat(Filetmp->updated_time, tmp);
     //   printf("updated_time:%s\n\n",tmp);

        tmp = tmp + strlen(tmp) + 1;
        Filetmp->next = (struct FileData*)malloc(sizeof(struct FileData));
        Filetmp = Filetmp->next;
        memset(Filetmp, 0, sizeof(struct FileData));
    }
    Filetmp = 0;
    return 0;
}

