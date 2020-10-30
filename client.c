#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for sleep()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MSGBUFSIZE 256
#include "list.h"
#include "header.h"
#define USE_DEFAULT_INPUT
/*
COMMANDS
for sendRequest: 1 <space> service <space> buff <space> sleep time
for getReply: 2 <space> reqid <space> block <space> sleep time
to exit the client: 3
*/
int main(int argc, char *argv[]){
	int len, reqid;
	char msgbuf[MSGBUFSIZE];
	int reply;
	int res;
#ifdef USE_DEFAULT_INPUT
	int i=0, svcid;
	if(argc != 3){
		printf("Command line arguments should be service id and number for primality test\n");
		exit(EXIT_FAILURE);
	}
	svcid = atoi(argv[1]);
	msgbuf[0]='\0';
#else
	int choice, num, sleep_time, service, req_id, block;
	char buf[MSGBUFSIZE];
#endif

#ifdef USE_DEFAULT_INPUT
	while (i<10) {
		i++;
		reqid = sendRequest(svcid,(void*)argv[2],MSGBUFSIZE);
		printf("reqid client: %d\n", reqid);
		len = MSGBUFSIZE;
		res = getReply(reqid,msgbuf,&len,1);
		if(res == -1){
			printf("No reply found\n");
			continue;
		}

		reply = atoi(msgbuf);
		if(reply == 0){
			printf("Number %s is not prime.\n",argv[2]);
		}
		else{
			printf("Number %s is prime.\n",argv[2]);
		}
	}
#else
	while(1){
		scanf("%d", &choice);
		if(choice == 1){				//sendRequest
			scanf("%d", &service);
			scanf("%d", &num);
			scanf("%d", &sleep_time);
			sprintf(buf,"%d",num);
			reqid = sendRequest(service, (void*)buf, MSGBUFSIZE);
			printf("Sent request with reqid: %d\n", reqid);
			sleep(sleep_time);
		}
		if(choice == 2){				//getReply
			scanf("%d", &req_id);
			scanf("%d", &block);
			scanf("%d", &sleep_time);
			res = getReply(req_id,msgbuf,&len,block);
			if(res == -1){
				printf("No reply found\n");
				continue;
			}
			reply = atoi(msgbuf);
			if(reply == 0){
				printf("Number is not prime.\n");
			}
			else{
				printf("Number is prime.\n");
			}
			sleep(sleep_time);
		}
		if(choice == 3){				//quit
			break;
		}
    }
#endif
    return 0;
}
