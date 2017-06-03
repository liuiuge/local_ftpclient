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
		char buf[1000],sha1[41] = {0}, buf_sha1[16] = {0};
		int flag_check = 0, len = 0, flag = 0, mmap_flag = 0,fds;
		long file_size = 0;
		memset(buf,0,1000);
		recv(sfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		memset(buf,0,1000);
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
		buf[strlen(buf)-1] = '\0';
		struct stat dbuf;
		memset(&dbuf,0,sizeof(dbuf));
		stat(buf,&dbuf);
		strcpy(buf_sha1,buf);
		strcat(buf_sha1,"sha1");
		if((fds = open(buf_sha1,O_RDONLY)) == -1){
			strcpy(sha1,"hello world");
		}else{
			read(fds,sha1,41);
			close(fds);
		}
//断点续传部分
		int fd = open(buf,O_RDWR);
		if(fd > 0){
				printf("file exist\n");
				flag_check = 1;
				send(sfd, &flag_check, 4, 0);
				
				printf("buf:%s\n",buf);
				send(sfd,&dbuf.st_size, 8, 0);
				send(sfd,sha1, 41, 0);
				close(fd);

		}else{
				send(sfd,&flag_check, 4, 0);
				recv(sfd,&sha1,41,0);
				fds = open(buf_sha1,O_CREAT|O_RDWR,0600);
				write(fds,sha1,41);
				close(fds);
		}
		printf("Start!\n");
		memset(buf,0,1000);
		recv(sfd, &len,sizeof(int ),0);
		recv(sfd,buf,len,0);
		fd = open(buf,O_CREAT|O_RDWR,0777);
		if(-1 == fd){
				return -1;
		}
		lseek(fd,dbuf.st_size,SEEK_SET);
		send(sfd, &len, 4, 0);
		recv(sfd, &mmap_flag, 4, 0);
		if(mmap_flag == 0){
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
		}else if(mmap_flag == 1){
			//mmap写入
			recv(sfd, &file_size, 8, 0);
			ftruncate(fd, file_size);
			printf("%d\n",fd);
			lseek(fd,0,SEEK_CUR);
			printf("%ld\n",file_size);
			char *mmap_s = (char *)mmap(NULL, file_size - dbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
			char *pcur = mmap_s, *mmap_e = mmap_s + file_size - dbuf.st_size;
			while(1){
					recv_n(sfd,(char *)&len,sizeof(int));
					if(len>0){
						memset(buf,0,1000);
						recv_n(sfd,buf,len);
						memcpy(pcur,buf,len);
						pcur += len;
					}else{
						break;
					}
			}
			munmap(mmap_s, file_size - dbuf.st_size);
			send(sfd, &mmap_flag, 4, 0);
		}else{
			printf("mmap fail\n");
			return -1;
		}
		printf("download finish\n");
		return 0;
}
int cli_put(int sfd){
	train t;
	char buf1[32],sha1[41] = {0},err[256] = {0},r_sha1[41];
	bzero(buf1,32);
	int flag = 0,flag_check = 0,flag_mmap = 0;
	long file_size = 0;
	recv(sfd,(char*)&flag,sizeof(int),0);	
	printf("input the name\n");
	scanf("%s",buf1);
	int fd;
	while((fd = open(buf1,O_RDONLY)) == -1){
		printf("No such file,input again\n");
		bzero(buf1,32);
		scanf("%s",buf1);
	}
	memset(&t,0,sizeof(t));
	t.len = strlen(buf1);
	strcpy(t.buf,buf1);
	send(sfd,&t,4+t.len,0);
	int ret = recv(sfd, &flag_check, 4, 0);
	if(flag_check == 1) {
			printf("find same files\n");
			recv(sfd,&file_size,8,0);
			recv(sfd,&r_sha1,41,0);
			printf("%ld\n",file_size);
			//if(!strcmp(sha1,r_sha1)){
			printf("upload from last time\n");
			close(fd);
			//}
	}
	fd = open(buf1,O_RDONLY);
	lseek(fd,file_size/2,SEEK_SET);
	lseek(fd,0,SEEK_CUR);
	struct stat dbuf;
	memset(&dbuf,0,sizeof(dbuf));
	stat(buf1,&dbuf);
	recv(sfd,&t.len,4,0);
	if(dbuf.st_size - file_size/2 < (1<<18)){
		send(sfd, &flag_mmap, 4, 0);
		while(memset(&t,0,sizeof(t)), (t.len = read(fd, t.buf,sizeof(t.buf)))>0){
				//printf("t.len:%d",t.len);
				if(-1 == send_n(sfd, (char*)&t,4+t.len)){
					close(sfd);
					return -1;
				}
		}
		flag = 0;
		send_n(sfd,(char*)&flag,sizeof(int));
	}else{
		char *mmap_s = (char *)mmap(NULL,dbuf.st_size - file_size,PROT_READ,MAP_SHARED,fd,0);
			if(mmap_s == (char *)-1){
				flag_mmap = -1;
				perror("mmap");
				send(sfd,&flag_mmap, 4, 0);
				return -1;
			}
			flag_mmap = 1;
			send(sfd, &flag_mmap, 4, 0);
			send(sfd, &dbuf.st_size, 8, 0);
			char *mmap_e = mmap_s + dbuf.st_size - file_size;
			//mmap读取
			char *pcur= mmap_s;
			printf("start");
			while((mmap_e>pcur) && (mmap_e - pcur )>512){
				memset(&t,0,sizeof(t));
				t.len = 512;
				memcpy(t.buf,pcur,512);
				if(-1 == send_n(sfd, (char*)&t,4+t.len)){
						close(sfd);
						return -1;
				}
				pcur += 512;
			}
			if(mmap_e> pcur){
				memset(&t,0,sizeof(t));
				t.len = mmap_e-pcur;
				memcpy(t.buf,pcur,1000);
				if(-1 == send_n(sfd, (char*)&t,4+t.len)){
						close(sfd);
						return -1;
				}
			}
			int flag = 0;
			send_n(sfd,(char*)&flag,sizeof(int));
			recv(sfd,&flag_check,4,0);
			munmap(mmap_s,dbuf.st_size - file_size);
	}
	printf("upload success\n");
	//close(fd);
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

























