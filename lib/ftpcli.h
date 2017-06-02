#ifndef __FTPCLI_H__
#define __FTPCLI_H__
#include "head.h"

//å®¢æˆ·ç«¯åŠŸèƒ?
int cli_cd(int );
int cli_ls(int );
int cli_remove(int);
int cli_get(int);
int cli_put(int);

char *GetFileSHA1b(char *, char *, char *);

int send_n(int, char *, int);
int send_file(int);
void recv_n(int,char *,int);
void cli_crypt(char *,char *);
typedef struct data_send{
	int len;
	char buf[1000];
}train,*ptrain;

#endif
