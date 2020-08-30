#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/time.h>

unsigned long get_usec_elapsed(struct timeval start, struct timeval end){
	unsigned long usec;
	
	if(end.tv_usec < start.tv_usec){
		printf("end < start\n");
	}

	usec = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);

	return usec;
}

int main(int argc,char **argv)
{
	int i,num,fd,ret,pktlen;
	unsigned int addrlen;
	char buf[100];
	struct timeval end;
	struct sockaddr_in server,cli_addr;

	if(argc != 3){
		fprintf(stderr,"USAGE: %s num_of_trial pktlen",argv[0]);
	}

	fd = socket(AF_INET,SOCK_DGRAM,0);
	if(fd < 0){
		perror("socket");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(30000);

	ret = bind(fd,(struct sockaddr *)&server,sizeof(server));
	if(ret < 0){
		perror("bind");
		return -1;
	}

	num = atoi(argv[1]);
	pktlen = atoi(argv[2]);
	addrlen = sizeof(cli_addr);
	memset(buf,0,sizeof(buf));

	for(i=0;i<num;i++){
		ret = recvfrom(fd,buf,pktlen,0,(struct sockaddr *)&cli_addr,&addrlen);
		if(ret < 0){
			perror("recvfrom");
			return -1;
		}
		gettimeofday(&end,NULL);
		printf("%lu\n",end.tv_sec*1000000 + end.tv_usec);
	}

	close(fd);

	return 0;
}
