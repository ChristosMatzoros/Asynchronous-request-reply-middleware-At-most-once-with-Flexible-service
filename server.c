#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define MSGBUFSIZE 256
#include "list.h"
#include "header.h"

//Uncomment this to introduce packet loss in the system
//#define DROP_PACKETS

//Prime function implementation (if the number is prime return 1 else return 0)
int isPrime(int number){
	int i;

	for (i = 2; i <= number / 2; i++) {
		if (number % i == 0) {
			return(0);
		}
	}
	return (1);
}

int main(int argc, char *argv[]){
	char msgbuf[MSGBUFSIZE];   //used for the messege that is recieved from getRequest
	char buffer[sizeof(int)];  //used for the messege that is send with sendRequest
	int len,reqid;             
	int res;

	if (argc != 2) {
		printf("Command line argument should be service id\n");
		exit(EXIT_FAILURE);
	}
	int svcid =atoi(argv[1]);
	
    register_service(svcid);        //register the service from the network
    
    #ifdef DROP_PACKETS
    int i=0;
    #endif
    
	while(1){
		msgbuf[0]='\0';
		reqid = getRequest(svcid, msgbuf, &len);      //recieve request from the client
		if(reqid==-1){
			printf("Problem\n");
			return(1);
		}
		//msgbuf[nbytes]='\0';
		printf("reqid is %d\n", reqid);
		res = isPrime(atoi(msgbuf));
		printf("RES:%d\n",res);
		sprintf(buffer,"%d",res);
#ifdef DROP_PACKETS
        int i=0;
        //introduce loss every 5 replys
		if(i%5 == 0){
			printf("reqid server: %d\n", reqid);
			sendReply(reqid, buffer, sizeof(buffer));    //send reply to the client
		}
        i++;
#else
		printf("reqid server: %d\n", reqid);
		sendReply(reqid, buffer, sizeof(buffer));     //send reply to the client
#endif
	}
	unregister_service(svcid);     //unregister the service from the network

	return 0;
}
