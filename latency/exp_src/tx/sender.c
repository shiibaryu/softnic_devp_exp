#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>

unsigned long get_usec_elapsed(struct timeval start, struct timeval end){
	unsigned long usec;
	
	if(end.tv_usec < start.tv_usec){
		printf("end < start\n");
	}

	usec = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);

	return usec;
}

int main(int argc,char *argv[])
{
	int i,num,s_fd,f_fd,ret;
	int pktlen;
	unsigned int addrlen;
	unsigned long latency;
	struct sockaddr_in addr;
	struct timeval start,end;
	char buf[100];
	char rbuf[100];

	if(argc != 5){
		fprintf(stderr,"USAGE: %s file addr num_of_trial pktlen\n",argv[0]);
		return 1;
	}

	f_fd = open(argv[1],O_RDONLY,0600);
	if(f_fd < 0){
		perror("open");
	}

	s_fd = socket(AF_INET,SOCK_DGRAM,0);

	memset(&addr,0,sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[2]);
	addr.sin_port = htons(30000);

	ret = read(f_fd,buf,sizeof(buf));
	if(ret < 0){
		perror("read");
	}

	num = atoi(argv[3]);
	pktlen = atoi(argv[4]);
	addrlen = sizeof(addr);


	for(i=0;i<num;i++){
		gettimeofday(&start,NULL);
		ret = sendto(s_fd,buf,pktlen,0,(struct sockaddr *)&addr,sizeof(addr));
		if(ret<0){
			perror("sendto");
			return -1;
		}
		printf("%lu \n",start.tv_sec * 1000000 + start.tv_usec);
		sleep(1);
	}
	
	close(s_fd);
	close(f_fd);

	return 0;
}
