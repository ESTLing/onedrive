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
#include "http.h"

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

char* GetFrom(char *respones,const char *data)
{
    char *tmp1,*tmp2;
    for(tmp1 = respones; *tmp1 != '\0'; tmp1++)
    {
        if(*tmp1 == '"')
        {
            tmp1++;
            if(strncmp(tmp1, data, strlen(data)) == 0)
                break;
        }
    }
    if(*tmp1 == '\0') return NULL;
    tmp1 = tmp1 + strlen(data) + 4;
    for(tmp2 = tmp1; *tmp2 != '"';tmp2++);
    *tmp2 = '\0';
    return tmp1;
}

int getCode()
{
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serves_addr, sizeof(struct sockaddr_in));
    serves_addr.sin_family = AF_INET;
    serves_addr.sin_addr.s_addr = inet_addr("131.253.61.80");
    serves_addr.sin_port = htons(443);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if(ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        exit(0);
    }

    if(connect(client_fd, (struct sockaddr*)(&serves_addr), sizeof(struct sockaddr)) == -1)
    {
        printf("connect failed.\n");
        exit(0);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);
    if(SSL_connect(ssl) == -1)
    {
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    else
    {
 //       printf("Connected with %s encrytion.\n",SSL_get_cipher(ssl));
//        ShowCerts(ssl);
    }

    memset(client_request_date, 0, REQUESTSIZE);
  //  strcat(client_request_date, "client_id=");
  //  strcat(client_request_date, api_client_id);
  //  strcat(client_request_date, "&redirect_uri=");
  //  strcat(client_request_date, api_redirect_url);
  //  strcat(client_request_date, "&client_secret=");
  //  strcat(client_request_date, api_client_secret);
  //  strcat(client_request_date, "&code=");
  //  strcat(client_request_date, code);
    strcat(client_request_date, "client_id=000000004C12EED0");
    strcat(client_request_date, "&scope=wl.signin%%20wl.skydrive%%20wl.skydrive_update%%20wl.basic%%20wl.offline_access%%20wl.photos");
    strcat(client_request_date, "&response_type=code&display=touch");
    strcat(client_request_date, "&redirect_uri=https%%3A%%2F%%2Flogin.live.com%%2Foauth20_desktop.srf");
 //   strcat(client_request_date, "&grant_type=authorization_code");

    sprintf(tem_buf ,"%d" ,strlen(client_request_date));

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "GET /oauth20_authorize.srf?");
    strcat(client_request, "client_id=000000004C12EED0");
    strcat(client_request, "&scope=wl.signin%20wl.skydrive%20wl.skydrive_update%20wl.basic%20wl.offline_access%20wl.photos");
    strcat(client_request, "&response_type=code&display=touch");
    strcat(client_request, "&redirect_uri=https%3A%2F%2Flogin.live.com%2Foauth20_desktop.srf");
    strcat(client_request, " HTTP/1.1\r\n");
    strcat(client_request, "HOST:login.live.com\r\n");
 //   strcat(client_request, "Content-type:application/x-www-form-urlencoded\r\n");
 //   strcat(client_request, "Content-Length:");
//    strcat(client_request, tem_buf);
    strcat(client_request, "\r\n");
  //  strcat(client_request, "Connection:Keep-Alive\r\n");
 //   strcat(client_request, "Cache-Control:no-cache\r\n\r\n");
 //   strcat(client_request, client_request_date);
 //   printf("%s\n\n",client_request);

    if(SSL_write(ssl, client_request, strlen(client_request)) == -1)
    {
        printf("send fail.\n");
        return -1;
    }

    if((state = SSL_read(ssl, service_respones, REQUESTSIZE)) == -1)
    {
        printf("recv faile.\n");
        return -1;
    }
    else if(state == 0)
    {
        printf("No available data.\n");
        return -1;
    }

    printf("%s\n\n",service_respones);
    if(GetTokenFromResponse() == -1) return -1;

    printf("Get access token success.\n");

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    SSL_CTX_free(ctx);
    return 0;
}

int GetToken(char *code)
{
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serves_addr, sizeof(struct sockaddr_in));
    serves_addr.sin_family = AF_INET;
    serves_addr.sin_addr.s_addr = inet_addr("131.253.61.80");
    serves_addr.sin_port = htons(443);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if(ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        exit(0);
    }

    if(connect(client_fd, (struct sockaddr*)(&serves_addr), sizeof(struct sockaddr)) == -1)
    {
        printf("connect failed.\n");
        exit(0);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);
    if(SSL_connect(ssl) == -1)
    {
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    else
    {
 //       printf("Connected with %s encrytion.\n",SSL_get_cipher(ssl));
//        ShowCerts(ssl);
    }

    memset(client_request_date, 0, REQUESTSIZE);
    strcat(client_request_date, "client_id=");
    strcat(client_request_date, api_client_id);
    strcat(client_request_date, "&redirect_uri=");
    strcat(client_request_date, api_redirect_url);
    strcat(client_request_date, "&client_secret=");
    strcat(client_request_date, api_client_secret);
    strcat(client_request_date, "&code=");
    strcat(client_request_date, code);
    strcat(client_request_date, "&grant_type=authorization_code");

    sprintf(tem_buf ,"%d" ,strlen(client_request_date));

    memset(client_request, 0, REQUESTSIZE);
    strcat(client_request, "POST /oauth20_token.srf HTTP/1.1\r\n");
    strcat(client_request, "HOST:login.live.com\r\n");
    strcat(client_request, "Content-type:application/x-www-form-urlencoded\r\n");
    strcat(client_request, "Content-Length:");
    strcat(client_request, tem_buf);
    strcat(client_request, "\r\n");
    strcat(client_request, "Connection:Keep-Alive\r\n");
    strcat(client_request, "Cache-Control:no-cache\r\n\r\n");
    strcat(client_request, client_request_date);
 //   printf("%s\n\n",client_request);

    if(SSL_write(ssl, client_request, strlen(client_request)) == -1)
    {
        printf("send fail.\n");
        return -1;
    }

    if((state = SSL_read(ssl, service_respones, REQUESTSIZE)) == -1)
    {
        printf("recv faile.\n");
        return -1;
    }
    else if(state == 0)
    {
        printf("No available data.\n");
        return -1;
    }

 //   printf("%s\n\n",service_respones);
    if(GetTokenFromResponse() == -1) return -1;

    printf("Get access token success.\n");

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    SSL_CTX_free(ctx);
    return 0;
}

int GetTokenFromResponse()
{
    char *temp1, *temp2;
    for(temp1 = service_respones; *temp1 != '\0'; temp1++)
    {
        if(*temp1 == '"' && *(temp1+1) == 'a')
            break;
    }
    if(*temp1 == '\0')
    {
        printf("Can't get token.\n");
        return -1;
    }
    temp1 = temp1 + 16;
    for(temp2 = temp1; *temp2 != '"'; temp2++);
    *temp2 = '\0';
    strcat(api_access_token, temp1);
    *temp2 = '"';
 //   printf("%s\n", api_access_token);

    temp1 = temp2 + 3;
    if(*temp1 != 'r')
    {
        printf("Can't get token.\n");
        return 0;
    }
    temp1 = temp1 + 16;
    for(temp2 = temp1; *temp2 != '"'; temp2++);
    *temp2 = '\0';
    strcat(api_refresh_token, temp1);
    *temp2 = '"';
 //   printf("%s\n", api_refresh_token);

    temp1 = temp2 + 3;
    if(*temp1 != 'a')
    {
        printf("Can't get token.\n");
        return 0;
    }
    temp1 = temp1 + 23;
    for(temp2 = temp1; *temp2 != '"'; temp2++);
    *temp2 = '\0';
    strcat(api_authentication_token, temp1);
    *temp2 = '"';
 //   printf("%s\n", api_authentication_token);

    temp1 = temp2 + 3;
    if(*temp1 != 'u')
    {
        printf("Can't get token.\n");
        return 0;
    }
    temp1 = temp1 +10;
    for(temp2 = temp1; *temp2 != '"'; temp2++);
    *temp2 = '\0';
    strcat(api_user_id, temp1);
    *temp2 = '"';
   // printf("%s\n",api_user_id);
    return 0;
}

int GetConnect()
{
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serves_addr, sizeof(struct sockaddr_in));
    serves_addr.sin_family = AF_INET;
    serves_addr.sin_addr.s_addr = inet_addr("134.170.108.72");
    serves_addr.sin_port = htons(443);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if(ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        exit(0);
    }

    if(connect(client_fd, (struct sockaddr*)(&serves_addr), sizeof(struct sockaddr)) == -1)
    {
        printf("connect failed.\n");
        exit(0);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);
    if(SSL_connect(ssl) == -1)
    {
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    return 0;
}

int CloseConnect()
{
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    SSL_CTX_free(ctx);
    return 0;
}

