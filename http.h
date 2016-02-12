#ifndef HTTP_H
#define HTTP_H

#define NAMESIZE 1024
#define REQUESTSIZE 32768
char* GetFrom(char* respones,const char *data);

int getCode();
int GetToken(char *code);
int RefreshToken();
int GetTokenFromResponse();

int GetConnect();
int CloseConnect();

#endif // HTTP_H
