#include "ftpcli.h"

void main(int argc, char **argv){
		if(argc != 3){
				printf("Err args!\n");
				return ;
		}
		struct sockaddr_in ftp_local;
		memset(&ftp_local,0,sizeof(ftp_local));
		ftp_local.sin_family = AF_INET;
		ftp_local.sin_addr.s_addr = inet_addr(argv[1]);
		ftp_local.sin_port = htons(atoi(argv[2]));
		int ftp_fd = socket(AF_INET, SOCK_STREAM, 0),cli_fd;
		int ret_conn,ret;
		ret_conn = connect(ftp_fd,(struct sockaddr*) &ftp_local,sizeof(ftp_local));
		if (-1 == ret_conn){
				perror("connect");
				return ;
		}
		system("clear");
		printf(">connecting success\n");
		char icmd[10];
		int fd;
		char buf[128];
		while(1){
				memset(icmd,0,10);
				memset(buf,0,128);
				printf(">");
				scanf("%s",icmd);
				ret = send(ftp_fd,icmd,strlen(icmd),0);
				if(-1 == ret){
					perror("send");
					return ;
				}
				if(!strcmp(icmd,"cd")){
						cli_cd(ftp_fd);
				}else if(!strcmp(icmd,"ls")){
						cli_ls(ftp_fd);
				}else if(!strcmp(icmd,"exit")){
						sleep(2);
						break;
				}else if(! strcmp(icmd, "pwd")){
						recv(ftp_fd,buf,sizeof(buf),0);
						printf("%s\n",buf);
				}else if(! strcmp(icmd,"get")){
						cli_get(ftp_fd);
				}else if(! strcmp(icmd,"put")){
						cli_put(ftp_fd);
				}else if(!strcmp(icmd,"remove")){
						cli_remove(ftp_fd);
				}else{
						recv(ftp_fd, buf, sizeof(buf), 0);
						printf("%s\n",buf);
				}
		}
		close(ftp_fd);
		return ;
}
