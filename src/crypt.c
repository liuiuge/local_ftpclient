#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>

void cli_crypt(char *key,char *key_send){
	char salt[16];
	sprintf(salt,"$6$%s$",key);
	strcpy(key_send,crypt(salt,key));
	//printf("%s\n",key_send);
	return ;
}
