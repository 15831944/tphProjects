#pragma once



int  LoadFromMemory(const wchar_t * filename);
void UnloadMemory();
int  KeyEncode( char *key, int keylen, char *outBuf,int * outLen);
int  PassEncode( char *pass, int passlen,  int flag, int *plen,char *key,int keylen);

 int  sub_14F40(char *a1, int a2, int a3, int * a4, char * a5, int a6);

 int   PassEncode3(char *a1, int  a2, int a3, int  a4, char * a5, int a6);