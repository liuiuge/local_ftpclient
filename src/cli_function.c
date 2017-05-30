#include "ftpcli.h"

int cli_cd(int sfd){
		char buf[128];
		bzero(buf,128);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		bzero(buf,128);
		read(0,buf,sizeof(buf));
		send(sfd,buf,strlen(buf)-1,0);
		int flag = 0;
		while(1){
				recv(sfd,(char*)&flag,sizeof(int),0);
				if(flag > 0){
							break;
					}else{
						printf("Err input, try again\n");
						return -1;
					}
		}
		printf("Success.\n");
}
int cli_ls(int sfd){
		char buf[128];
		int i,len;
		//printf("ls start\n");
		while(1){
				recv_n(sfd,(char *)&len,sizeof(int));
				if(len > 0){
						memset(buf,0,128);
						recv_n(sfd,buf,len);
						printf("\t%s\n",buf);
				}else{
						break;
				}
		}
		//printf("ls finish\n");
}
int cli_get(int sfd){
		char buf[1000];
		int len = 0;
		int flag = 0;
		bzero(buf,1000);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		bzero(buf,1000);
		read(0,buf,sizeof(buf));
		send(sfd,buf,strlen(buf)-1,0);
		while(1){
				recv(sfd,(char*)&flag,sizeof(int),0);
				if(flag > 0){
						break;
				}else{
						printf("Err input, try again\n");
						return -1;
				}
		}
		memset(buf,0,1000);
		recv(sfd, &len,sizeof(int ),0);
		recv(sfd,buf,len,0);
		int fd = open(buf,O_CREAT|O_RDWR,0777);
		if(-1 == fd){
				return -1;
		}
		while(1){
				recv_n(sfd,(char *)&len,sizeof(int));
				if(len>0){
						memset(buf,0,1000);
						recv_n(sfd,buf,len);
						write(fd,buf,len);
				}else{
						break;
				}
		}
		printf("download finish\n");
		return 0;
}
int cli_put(int sfd){
	train t;
	char buf1[32];
	bzero(buf1,32);
	int flag = 0;
	recv(sfd,(char*)&flag,sizeof(int),0);	
	printf("input the name\n");
	scanf("%s",buf1);
	int fd;
	while((fd = open(buf1,O_RDONLY)) == -1){
		printf("No such file,input again");
		bzero(buf1,32);
		scanf("%s",buf1);
	}
	memset(&t,0,sizeof(t));
	t.len = strlen(buf1);
	strcpy(t.buf,buf1);
	send(sfd,&t,4+t.len,0);
	while(memset(&t,0,sizeof(t)), (t.len = read(fd, t.buf,sizeof(t.buf)))>0){
		if(-1 == send_n(sfd, (char*)&t,4+t.len)){
			close(sfd);
			return -1;
		}
	}
	flag = 0;
	send_n(sfd,(char*)&flag,sizeof(int));
	printf("upload success\n");
	close(fd);
	return 0;
}
int cli_remove(int sfd){
		char buf[128];
		bzero(buf,128);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		bzero(buf,128);
		read(0,buf,sizeof(buf));
		send(sfd,buf,strlen(buf)-1,0);
		int flag = 0;
		while(1){
				recv(sfd,(char*)&flag,sizeof(int),0);
				if(flag > 0){
							break;
					}else{
						printf("Err input, try again\n");
						return -1;
					}
		}
		printf("Success.\n");
}

























