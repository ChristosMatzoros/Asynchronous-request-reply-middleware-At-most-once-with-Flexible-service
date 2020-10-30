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
pthread_t broker_thread;


void *broker_side_listener(void *arg){
	struct sockaddr_in client_addr, dest_address;  //use client_addr as source address and  dest_address as destination address for the sendto/recvfrom functions
	int nbytes;
	socklen_t client_addr_len = sizeof(client_addr);
	mystruct st, st2,st3;
	struct node3* res = NULL;
	struct node3* result;

	clock_t start, end;
	double cpu_time_used;
	start = clock();

	while(1){
		while(1){
			fd_set select_fds;                   //fd's used by select 
			struct timeval timeout;              // Time value for time out 

			FD_ZERO(&select_fds);                // Clear out fd's 
			FD_SET(broker_fd, &select_fds);      // Set the interesting fd's 

			timeout.tv_sec = 4;                  // Timeout set for 5 sec + 0 micro sec
			timeout.tv_usec = 0;

			end = clock();
			cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
			if(cpu_time_used > 5){
				while(!isEmpty3()){
					deleteFirst3();
				}
				start = clock();
			}

			if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 ){       //use select to protect the system from packet losses
			}
			else{
				nbytes = recvfrom(broker_fd,(void *)&st,sizeof(mystruct),0,(struct sockaddr *) &client_addr,&client_addr_len);
				if (nbytes < 0) {
					printf("Error at server_side_listener()\n");
					perror("recvfrom");
					exit(1);
				}
				break;
			}
		}
        printf("BROKER input\n");
        
		if(ntohl(st.reqid) == -2){    //if reqid == -2 the message is come from a server
			res = find3_with_addr(client_addr);
			if(res == NULL){
				insertFirst3(atoi(st.msg), ntohl(st.service_id), client_addr);
			}
			else{
				res->load = atoi(st.msg);
			}
			continue;
		}
		else if(ntohl(st.reqid) == -1){   //if reqid == -2 the message is come from a client to produce a new global id for a new request
			pthread_res = pthread_mutex_lock(&mtx3);
			clientid++;
			pthread_res = pthread_mutex_unlock(&mtx3);
			check_lock(pthread_res);

			st2.reqid = htonl(-1);   //send the new id for the new request
			sprintf(st2.msg,"%d",clientid);

			nbytes = sendto(broker_fd, &st2, sizeof(mystruct),0, (const struct sockaddr *) &client_addr,sizeof(client_addr));
			if (nbytes < 0) {
				perror("sendto");
				exit(1);
			}
			st.address = client_addr;
			st.reqid = htonl(clientid);
			result = find3_optimal_server(ntohl(st.service_id));
			if(result == NULL){
				continue;
			}
			
			dest_address = (result)->addr;
			printf("Address: %u, Port: %d, Service ID: %d asks new id\n", dest_address.sin_addr.s_addr, dest_address.sin_port, ntohl(st.service_id));
			int nbytes = sendto(client_fd,(const void *)&st,sizeof(mystruct),0,(struct sockaddr*) &dest_address,sizeof(dest_address));
			if (nbytes < 0) {
				perror("sendto");
				return(NULL);
			}
			sleep(1);
		}
		else{ //if the ids are different from -1 or -2 it means that the packets have been resend
			st3.address = client_addr;
			st3.reqid = st.reqid;
			st3.service_id = st.service_id;
			strcpy(st3.msg,st.msg);
			result = find3_optimal_server(ntohl(st3.service_id));
			if(result == NULL){
				continue;
			}
			dest_address = result->addr;
			int nbytes = sendto(client_fd,(const void *)&st3,sizeof(mystruct),0,(struct sockaddr*) &dest_address,sizeof(dest_address));
			if (nbytes < 0) {
				perror("sendto");
				return(NULL);
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[]){
	int pthread_res;

	broker_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (broker_fd < 0) {
		perror("socket");
		return 1;
	}

	// allow multiple sockets to use the same PORT number
	u_int yes = 1;
	if (setsockopt(broker_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0){
		perror("Reusing ADDR failed");
		return(1);
	}

	// set up destination address
	memset(&broker_addr, 0, sizeof(broker_addr));
	broker_addr.sin_family = AF_INET;
	broker_addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
	broker_addr.sin_port = htons(port);

	// bind to receive address
	if (bind(broker_fd, (struct sockaddr*) &broker_addr, sizeof(broker_addr)) < 0) {
		perror("bind");
		return(1);
	}

	// use setsockopt() to request that the kernel join a multicast group
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(group);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(broker_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0){
		perror("setsockopt");
		return (1);
	}

	client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_fd < 0) {
		perror("socket");
		return 1;
	}
	
	// set up destination address
	memset(&client_dst_addr, 0, sizeof(client_dst_addr));
	client_dst_addr.sin_family = AF_INET;
	client_dst_addr.sin_addr.s_addr = inet_addr(group);
	client_dst_addr.sin_port = htons(port);

	pthread_res = pthread_create(&broker_thread, NULL, broker_side_listener, NULL);
	if(pthread_res){
		printf("Error with 1 thread creation\n");
		exit(1);
	}

	while(1);

	return 0;
}
