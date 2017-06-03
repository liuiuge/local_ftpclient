#include "ftpcli.h"

int cli_reg(int sfd){
		char buf[16];
		int flag = 0, ret;
		memset(buf,0,16);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		memset(buf,0,16);
input_name:
		ret = read(0,buf,16);
		if(ret < 7){
			printf("name too short\n");
			goto input_name;
		}
		send(sfd,buf,strlen(buf)-1,0);
		while(1){
				recv(sfd,(char*)&flag,sizeof(int),0);
				if(flag > 0){
							break;
					}else{
						printf("Err input, try again\n");
						bzero(buf,16);
						scanf("%s",buf);
						send(sfd,buf,strlen(buf),0);
					}
		}
		memset(buf,0,16);
		recv(sfd, buf, 16, 0);
		printf("%s\n",buf);
		memset(buf,0,16);
input key:		
		ret = scanf("%s",buf);
		if(ret < 7){
			printf("key too short\n");
			goto input_key;
		}
		if(buf[0] == buf[1]){
			printf("key too simple\n");
			goto input_key;
		}
		char key_send[16] = {0};
		cli_crypt(buf,key_send);
		send(sfd,key_send,strlen(key_send),0);
		recv(sfd,(char*)&flag,sizeof(int),0);
		return flag;
}

int cli_log(int sfd){
		char buf[16];
		int flag = 0;
		memset(buf,0,16);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		memset(buf,0,16);
		read(0,buf,16);
		send(sfd,buf,strlen(buf)-1,0);
		memset(buf,0,16);
		recv(sfd, buf, 16, 0);
		printf("%s\n",buf);
		memset(buf,0,16);
		scanf("%s",buf);
		char key_send[16] = {0};
		cli_crypt(buf,key_send);
		send(sfd,key_send,strlen(key_send),0);
		recv(sfd,(char*)&flag,sizeof(int),0);
		return flag;
}

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
launch_page:		
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
				if(!strcmp(icmd,"exit")){
					break;
				}
				else if(!strcmp(icmd,"reg")){
					ret = cli_reg(ftp_fd);
					sleep(2);
					if(ret <= 0)
						printf("reg fail\n");
					else
						printf("reg success\n");
					goto launch_page;
				}
				else if(!strcmp(icmd,"log")){
					ret = cli_log(ftp_fd);
					if(ret == 0){
						printf("Err usr info, go back to first page in 2 secs..\n");
						sleep(2);
						goto launch_page;
					}else if(ret < 0){
						break;
					}else{
						printf("login success\n");
						while(1){
							memset(icmd,0,10);
							memset(buf,0,128);
							printf(">");
							scanf("%s",icmd);
							ret = send(ftp_fd,icmd,strlen(icmd),0);
							printf("ret = %d\n",ret);
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
									goto end_page;
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
							}//if(strcmp(icmd,all_above))
						}//while(1)
					}//else
				}else{
					recv(ftp_fd, buf, sizeof(buf), 0);
					printf("%s\n",buf);
				}//if(strcmp(icmd,all_above))
		}
end_page:
		close(ftp_fd);
		return ;
}
