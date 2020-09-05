#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/socket.h>

#define END_IDX			3

#define TIME_SEEK		6
#define TIME_READ		9
#define TYPE_SEEK		87
#define TYPE_READ		4
#define MWR_SEEK		48
#define MRD_SEEK		44
#define CPLD_SEEK		49
#define LEN_READ		3

#define TYPE_MWR		1
#define TYPE_MRD		2
#define TYPE_CPLD		3

int pkt_type = 0;

void print_idx(int idx)
{
	printf("%s: error idx %d \n",__func__,idx);
}

void seek_next_line(int fd)
{
	int ret;
	char p[1];

	do{
		ret = read(fd,&p[0],1);
		if(ret < 0){
			perror("read");
		}
	}while(strncmp(p,"\n",1) != 0);

	/*
	ret = lseek(fd,1,SEEK_CUR);
	if(ret < 0){
		printf("lseek failed");
	}*/
	
	return;
}

int update_idx(int idx)
{
	if(idx == END_IDX){
		idx = 1;
	}
	else if(idx < END_IDX){
		idx++;
	}
	else{
		printf("idx errorrr\n");
	}

	return idx;
}

void set_param(unsigned long long *r_seek,int *r_len,int idx)
{
	if(idx == 1){
		*r_len  = TIME_READ;
		*r_seek = TIME_SEEK;
	}
	else if(idx == 2){
		*r_len  = TYPE_READ;
		*r_seek = TYPE_SEEK;
	}
	else if(idx == 3){
		*r_len  = LEN_READ;
		*r_seek = 0;
	}
	else{
		printf("idx errorrr\n");
	}
}

int init_seek(int fd)
{
	int ret;

	ret = lseek(fd,0,SEEK_SET);
	if(ret < 0){
		printf("lseek failed \n");
		return -1;
	}

	return 0;
}

int dt_write(int r_fd,int w_fd,unsigned long long r_seek,unsigned long long w_seek,int read_size,int idx)
{
	int ret;
	char sp[1];
	char buf[1000];

	ret = lseek(r_fd,r_seek,SEEK_CUR);
	if(ret < 0){
		print_idx(idx);
		printf("lseek failed \n");
		return -1;
	}

	ret = read(r_fd,buf,read_size);
	if(ret < 0){
		print_idx(idx);
		perror("read");
		return -1;
	}

	ret = lseek(w_fd,0,SEEK_END);
	if(ret < 0){
		print_idx(idx);
		printf("lseek failed \n");
		return -1;
	}

	ret = write(w_fd,buf,read_size);
	if(ret < 0){
		print_idx(idx);
		perror("write");
		return -1;
	}

	if(idx != END_IDX){
		ret = lseek(w_fd,-1,SEEK_END);
		if(ret < 0){
			print_idx(idx);
			printf("lseek failed \n");
			return -1;
		}

		ret = read(w_fd,sp,1);
		if(ret < 0){
			print_idx(idx);
			perror("read");
			return -1;
		}
		
		if(strncmp(sp,",",1) != 0){
			memcpy(&sp[0],",",1);

			ret = write(w_fd,sp,sizeof(sp));
			if(ret < 0){
				print_idx(idx);
				perror("write");
				return -1;
			}
		}
	}
	else if(idx == END_IDX){
		/*
		ret = lseek(w_fd,-1,SEEK_END);
		if(ret < 0){
			print_idx(idx);
			printf("lseek failed \n");
			return -1;
		}

		ret = read(w_fd,sp,1);
		if(ret < 0){
			print_idx(idx);
			perror("read");
			return -1;
		}*/
		
		/*
		if(strncmp(sp,",",1) == 0){
			memcpy(&sp[0],'\0',1);

			ret = write(w_fd,sp,sizeof(sp));
			if(ret < 0){
				print_idx(idx);
				perror("write");
				return -1;
			}
		}*/

		ret = lseek(w_fd,0,SEEK_END);
		if(ret < 0){
			printf("lseek failed \n");
			return -1;
		}

		memcpy(&sp[0],"\n",1);

		ret = write(w_fd,sp,sizeof(sp));
		if(ret < 0){
			print_idx(idx);
			perror("write");
			return -1;
		}
	}
	else{
		print_idx(idx);
		printf("idx error");
		return -1;
	}

	memset(buf,0,sizeof(buf));

	return 0;
}

int srw(int r_fd,int w_fd,int read_size)
{
	int ret;
	char buf[1000];

	ret = read(r_fd,buf,read_size);
	if(ret < 0){
		perror("read");
		return -1;
	}

	ret = lseek(w_fd,0,SEEK_END);
	if(ret < 0){
		printf("lseek failed \n");
		return -1;
	}

	ret = write(w_fd,buf,read_size);
	if(ret < 0){
		perror("write");
		return -1;
	}

	return 0;
}

int dtl_write(int r_fd,int w_fd,int read_size,int idx,int type)
{
	int ret,cnt;
	char sp[1];
	char buf[1000];
	
	cnt = 0;

	if(idx == END_IDX){
		do{
			ret = read(r_fd,&sp[0],1);
			if(ret < 0){
				perror("read");
			}

			ret = strncmp(sp,",",1);
			if(ret == 0){
				cnt++;
			}
		}while(cnt != 2);

		lseek(r_fd,1,SEEK_CUR);
		
		ret = read(r_fd,buf,read_size);
		if(ret < 0){
			print_idx(idx);
			perror("read");
			return -1;
		}

		//printf("%s\n",buf);

		ret = lseek(w_fd,0,SEEK_END);

		if(ret < 0){
			print_idx(idx);
			printf("lseek failed \n");
			return -1;
		}

		ret = write(w_fd,buf,read_size);
		if(ret < 0){
			print_idx(idx);
			perror("write");
			return -1;
		}

		ret = lseek(w_fd,-1,SEEK_END);
		if(ret < 0){
			print_idx(idx);
			printf("lseek failed \n");
			return -1;
		}

		ret = read(w_fd,sp,1);
		if(ret < 0){
			print_idx(idx);
			perror("read");
			return -1;
		}
			
		if(strncmp(sp,",",1) != 0){
			memcpy(&sp[0],",",1);

			ret = write(w_fd,sp,sizeof(sp));
			if(ret < 0){
				print_idx(idx);
				perror("write");
				return -1;
			}
		}
			
		memcpy(&sp[0],"\n",1);

		ret = write(w_fd,sp,sizeof(sp));
		if(ret < 0){
			print_idx(idx);
			perror("write");
			return -1;
		}
	}
	else if(idx != END_IDX){
		do{
			ret = read(r_fd,&sp[0],1);
			if(ret < 0){
				perror("read");
			}

			ret = strncmp(sp,",",1);
			if(ret == 0){
				cnt++;
			}
		}while(cnt != 6);

		lseek(r_fd,8,SEEK_CUR);

		srw(r_fd,w_fd,LEN_READ);

		ret = lseek(w_fd,-1,SEEK_END);
		if(ret < 0){
			printf("lseek failed \n");
			return -1;
		}

		ret = read(w_fd,&sp[0],1);

		if(strncmp(sp,")",1) == 0){

			lseek(w_fd,-1,SEEK_CUR);

			memcpy(&sp[0],",",1);

			ret = write(w_fd,sp,sizeof(sp));
			if(ret < 0){
				print_idx(idx);
				perror("write");
				return -1;
			}
		}
		else if(strncmp(sp,",",1) != 0){
			//lseek(w_fd,0,SEEK_CUR);

			memcpy(&sp[0],",",1);

			ret = write(w_fd,sp,sizeof(sp));
			if(ret < 0){
				print_idx(idx);
				perror("write");
				return -1;
			}

		}
	}
	else{
		printf("error\n");
	}

	return 0;
}

int main(int argc,char **argv)
{
	int i,ret,idx,r_fd,w_fd;
	int r_len,total_len;
	void *data,*data2;
	unsigned long long r_seek,w_seek;

	if(argc != 4){
		printf("Usage: %s read_file write_file total_line\n",argv[0]);
		return 0;
	}

	r_fd = open(argv[1],O_RDWR,0600);
	if(r_fd < 0){
		perror("open");
		return -1;
	}

	w_fd = open(argv[2],O_RDWR,0600);
	if(w_fd < 0){
		perror("open");
		return -1;
	}
	
	idx = 1;
	r_seek = 0;
	w_seek = 0;
	r_len = 0;
	total_len = atoi(argv[3]) * END_IDX;

	for(i=0;i<total_len;i++){
		set_param(&r_seek,&r_len,idx);

		ret = init_seek(w_fd);
		if(ret < 0){
			return -1;
		}

		if(idx == 1){
			ret = dt_write(r_fd,w_fd,r_seek,w_seek,r_len,idx);
			if(ret < 0){
				printf("dump write failed \n");
				return -1;
			}
		}
		else{
			ret = dtl_write(r_fd,w_fd,r_len,idx,pkt_type);
		}

		if(idx == END_IDX){
			seek_next_line(r_fd);
		}

		idx = update_idx(idx);
	}
	
	close(r_fd);
	close(w_fd);

	return 0;
}
