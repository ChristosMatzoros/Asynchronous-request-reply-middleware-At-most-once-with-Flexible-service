int service = -1;
int port = 1900;
char* group = "239.255.255.251";
struct sockaddr_in src_addr;
struct sockaddr_in client_dst_addr;
struct sockaddr_in server_addr;
struct sockaddr_in broker_addr;
struct sockaddr_in serv_to_broker__addr;
int server_fd, client_fd, broker_fd;
int state_fd;
int clientid=-1;
int client_thread_exists = 0;
int server_thread_exists = 0;
int pthread_res = 0;
pthread_t client_thread;
pthread_t server_thread;
pthread_t state_server_thread,states_client_thread;
pthread_mutex_t mtx;
pthread_mutex_t mtx2;
pthread_mutex_t mtx3;
pthread_mutex_t mtx4;

//function to check the return value of
//pthread_mutex_lock() and pthread_mutex_lock()
void check_lock(int pthread_res){
	if(pthread_res){
		printf("ERROR\n");
		exit(0);
	}
}

//structure definition
typedef struct msgStruct{
	uint32_t reqid;
	uint32_t service_id;
	char msg[MSGBUFSIZE];
	struct sockaddr_in address;
}mystruct;

void *client_states(void *arg){
	struct node4* res;
		int nbytes;
		mystruct sendst;
		int reqid = *((int *) arg);
		printf("reqid %d\n",reqid);

		int client_states_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (client_states_fd < 0) {
			perror("socket");
			return NULL;
		}
		memset(&client_dst_addr, 0, sizeof(client_dst_addr));
		client_dst_addr.sin_family = AF_INET;
		client_dst_addr.sin_addr.s_addr = inet_addr(group);
		client_dst_addr.sin_port = htons(port);

		while(1){
			sleep(5);
			pthread_res = pthread_mutex_lock(&mtx4);
			check_lock(pthread_res);
			res = find4(reqid);
			pthread_res = pthread_mutex_unlock(&mtx4);
			check_lock(pthread_res);
			if(res == NULL){
				continue;
			}

			if(res->replyReceived==0){
				sendst.reqid = htonl(res->reqid);
				sendst.service_id = htonl(res->service_id);
				strcpy(sendst.msg,res->buf);
				printf("client_states reqid:%d, svcid: %d\n", ntohl(sendst.reqid), ntohl(sendst.service_id));
				nbytes = sendto(client_fd,(const void *)&sendst,sizeof(mystruct),0,(struct sockaddr*) &client_dst_addr,sizeof(client_dst_addr));
				if (nbytes < 0) {
					perror("sendto");
					return(NULL);
				}
			}
			else if(res->replyReceived==1){
				pthread_res = pthread_mutex_lock(&mtx4);
				check_lock(pthread_res);
				delete4(reqid);
				pthread_res = pthread_mutex_unlock(&mtx4);
				check_lock(pthread_res);
				break;
			}
		}
		return NULL;
}


void *client_side_listener(void *arg){
	int nbytes;
	struct sockaddr_in temp_addr;
	socklen_t temp_addr_len = sizeof(temp_addr);
	mystruct st;
	while(1){
		printf("BEFORE RECEIVE\n");

		while(1){
			fd_set select_fds;               // fd's used by select
			struct timeval timeout;	         // Time value for time out

			FD_ZERO(&select_fds);            // Clear out fd's
			FD_SET(client_fd, &select_fds);  //Set the interesting fd's

			timeout.tv_sec = 4;              //Timeout set for 5 sec + 0 micro sec
			timeout.tv_usec = 0;

			if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 ){
			}
			else{
				nbytes = recvfrom(client_fd,(void *)&st,sizeof(mystruct),0,(struct sockaddr *) &temp_addr,&temp_addr_len);
				if (nbytes < 0) {
					printf("Error at client_side_listener\n");
					perror("recvfrom");
					exit(1);
				}
				break;
			}
		}
		//used as ack for the reply of the server
		if(ntohl(st.reqid) == -3){
			st.reqid = htonl(-4);
			int nbytes = sendto(client_fd,(const void *)&st,sizeof(mystruct),0,(struct sockaddr*) &temp_addr,sizeof(temp_addr));
			if (nbytes < 0) {
				perror("sendto");
				return(NULL);
			}

			continue;
		}

		pthread_res = pthread_mutex_lock(&mtx);
		check_lock(pthread_res);
		insertFirst(ntohl(st.reqid), st.msg);
		pthread_res = pthread_mutex_unlock(&mtx);
		check_lock(pthread_res);
	}
	return NULL;
}

void *server_side_listener(void *arg){
	int nbytes;
	socklen_t src_addr_len = sizeof(src_addr);
	mystruct st;
	while(1){
		printf("Server listener waiting\n");
		while(1){
			fd_set select_fds;    
			struct timeval timeout;	    

			FD_ZERO(&select_fds);
			FD_SET(state_fd, &select_fds);

			timeout.tv_sec = 4;
			timeout.tv_usec = 0;

			if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 ){
			}
			else{
				nbytes = recvfrom(state_fd,(void *)&st,sizeof(mystruct),0,(struct sockaddr *) &src_addr,&src_addr_len);
				if (nbytes < 0) {
					printf("Error at server_side_listener()\n");
					perror("recvfrom");
					exit(1);
				}
				break;
			}
		}

		printf("service_id: %d, Request id: %d\n", ntohl(st.service_id), ntohl(st.reqid));
		if(ntohl(st.reqid) == -1 || ntohl(st.reqid) == -2 ||ntohl(st.service_id) != service){
			continue;
		}
		pthread_res = pthread_mutex_lock(&mtx2);
		check_lock(pthread_res);

		insertFirst2(ntohl(st.reqid), ntohl(st.service_id), st.msg, st.address);

		pthread_res = pthread_mutex_unlock(&mtx2);
		check_lock(pthread_res);

	}
	return NULL;
}

void *state_sender(void *arg){
	int list_length;
	mystruct st;

	while(1){
		pthread_res = pthread_mutex_lock(&mtx2);
		check_lock(pthread_res);
		list_length = length2();	//use to specify the load of this server
		pthread_res = pthread_mutex_unlock(&mtx2);
		check_lock(pthread_res);

		st.reqid=htonl(-2);
		st.service_id = htonl(service);
		sprintf(st.msg,"%d",list_length);
		printf("list length: %d\n", list_length);

		int nbytes = sendto(state_fd,(const void *)&st,sizeof(mystruct),0,(struct sockaddr*) &serv_to_broker__addr,sizeof(serv_to_broker__addr));
		if (nbytes < 0) {
			perror("sendto");
			return(NULL);
		}
		usleep(50000);
	}

	return(NULL);
}


//Client side
int sendRequest(int svcid, void *buf, int len){
	mystruct st;
	int reqid;
    struct node* res;
	int i=-1;
	if(client_thread_exists == 0){
		client_thread_exists = 1;
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

		pthread_res = pthread_create(&client_thread, NULL, client_side_listener, NULL);
		if(pthread_res){
			printf("Error with 1 thread creation\n");
			exit(1);
		}
	}

	st.service_id = htonl(svcid);
	strcpy(st.msg,buf);
	st.reqid = htonl(-1);

	int nbytes = sendto(client_fd,(const void *)&st,sizeof(mystruct),0,(struct sockaddr*) &client_dst_addr,sizeof(client_dst_addr));
	if (nbytes < 0) {
		perror("sendto");
		return(1);
	}

	clock_t start, end;
	double cpu_time_used;
	start = clock();

	
	do{
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		if(cpu_time_used > 2){
			int nbytes = sendto(client_fd,(const void *)&st,sizeof(mystruct),0,(struct sockaddr*) &client_dst_addr,sizeof(client_dst_addr));
			if (nbytes < 0) {
				perror("sendto");
				return(1);
			}
			start = clock();
		}
		pthread_res = pthread_mutex_lock(&mtx);
		check_lock(pthread_res);
		res = find(-1);
		pthread_res = pthread_mutex_unlock(&mtx);
		check_lock(pthread_res);

	}while(res == NULL&& i < 12);
	if(i == 12){
		return -1;	//No reply within a time interval
	}

	reqid = atoi(res->buf);

	pthread_res = pthread_mutex_lock(&mtx);
	check_lock(pthread_res);

	delete(-1);                            //delete the node with id =-1 (the ack that contains the new id for the client)

	pthread_res = pthread_mutex_unlock(&mtx);
	check_lock(pthread_res);


	pthread_res = pthread_mutex_lock(&mtx4);
	check_lock(pthread_res);
	insertFirst4(reqid, svcid,buf);
	printf("REQUEST LIST");
	pthread_res = pthread_mutex_unlock(&mtx4);
	check_lock(pthread_res);

	pthread_res = pthread_create(&states_client_thread, NULL, client_states, &reqid);
	if(pthread_res){
		printf("Error with 1 thread creation\n");
		exit(1);
	}

	return reqid;
}


int getReply(int reqid, void *buf,int *len,int block){
	struct node* res;
	struct node4 *reply_node;
	do{
		pthread_res = pthread_mutex_lock(&mtx);
		check_lock(pthread_res);

		res = find(reqid);                        //if res!= NULL it means that i have recieved the message with the the id == reqid

		pthread_res = pthread_mutex_unlock(&mtx);
		check_lock(pthread_res);

	}while(block == 1 && res == NULL);
	
	if(res == NULL){
		return -1;
	}
	
	strcpy((char *)buf, res->buf);

	pthread_res = pthread_mutex_lock(&mtx);
	check_lock(pthread_res);
	delete(reqid);                             //now we can delete the node from the list
	pthread_res = pthread_mutex_unlock(&mtx);
	check_lock(pthread_res);

	pthread_res = pthread_mutex_lock(&mtx4);
	check_lock(pthread_res);
	reply_node = find4(reqid);
	reply_node->replyReceived = 1;
	pthread_res = pthread_mutex_unlock(&mtx4);
	check_lock(pthread_res);

	*len = sizeof(buf);

	return(1);
}


//Server side
int register_service(int svcid){
	service = svcid;
	state_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (state_fd < 0) {
		perror("socket");
		return 1;
	}
	// set up destination address

	memset(&serv_to_broker__addr, 0, sizeof(serv_to_broker__addr));
	serv_to_broker__addr.sin_family = AF_INET;
	serv_to_broker__addr.sin_addr.s_addr = inet_addr(group);
	serv_to_broker__addr.sin_port = htons(port);

	// create what looks like an ordinary UDP socket
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return(1);
	}

	// allow multiple sockets to use the same PORT number
	u_int yes = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0){
		perror("Reusing ADDR failed");
		return(1);
	}

	// set up destination address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
	server_addr.sin_port = htons(port);

	// bind to receive address
	if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		return(1);
	}

	// use setsockopt() to request that the kernel join a multicast group
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(group);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(server_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0){
		perror("setsockopt");
		return (1);
	}

	pthread_res = pthread_create(&server_thread, NULL, server_side_listener, NULL);
	if(pthread_res){
		printf("Error with 1 thread creation\n");
		exit(1);
	}

	///
	pthread_res = pthread_create(&state_server_thread, NULL, state_sender, NULL);
	if(pthread_res){
		printf("Error with 2 thread creation\n");
		exit(1);
	}

	return(1);
}

int unregister_service(int svcid){
	service = -1;
	return(1);
}

int getRequest(int svcid, void *buf,int *len){
	struct node2* res;

	do{
		pthread_res = pthread_mutex_lock(&mtx2);
		check_lock(pthread_res);
		res = findsvc(svcid);
		if(res != NULL){
			res->service_id = -1;
		}
		pthread_res = pthread_mutex_unlock(&mtx2);
		check_lock(pthread_res);
	}while(res == NULL);

	strcpy((char *)buf, res->buf);
	return(res->reqid);
}

void sendReply(int reqid, void *buf,int len){
	int nbytes,i=3;
	mystruct st,st2;
	int pthread_res;

	struct node2* res;
	pthread_res = pthread_mutex_lock(&mtx2);
	check_lock(pthread_res);

	res = find2(reqid);

	pthread_res = pthread_mutex_unlock(&mtx2);
	check_lock(pthread_res);

	st.service_id = htonl(0);/////////////////zero for now, we don't send back the service id
	st.reqid = htonl(res->reqid);
	strcpy(st.msg,(char*)buf);
	nbytes = sendto(server_fd, &st, sizeof(mystruct),0, (const struct sockaddr *) &(res->addr),sizeof(res->addr));
	if (nbytes < 0) {
		perror("sendto");
		exit(1);
	}

	//used for the ack o the server's reply
	fd_set select_fds;		    /* fd's used by select */
	struct timeval timeout;

	FD_ZERO(&select_fds);		 /* Clear out fd's */
	FD_SET(state_fd, &select_fds);	     /* Set the interesting fd's */

	timeout.tv_sec = 4;		/* Timeout set for 5 sec + 0 micro sec*/
	timeout.tv_usec = 0;

	int ack_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ack_fd < 0) {
		perror("socket");
		return;
	}

	int serv_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (serv_fd < 0) {
		perror("socket");
		return;
	}

	socklen_t size;
	while(i<3){
		if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 ){
			i++;
		}
		else{
			size =sizeof(res->addr);
			nbytes = recvfrom(ack_fd,(void *)&st2,sizeof(mystruct),0,(struct sockaddr *)&(res->addr),&size);
			if (nbytes < 0) {
				printf("Error at server_side_listener()\n");
				perror("recvfrom");
				exit(1);
			}
			if(ntohl(st.reqid != -4)){
				continue;
			}
			break;
		}
		//send back to the server
		nbytes = sendto(serv_fd, &st, sizeof(mystruct),0, (const struct sockaddr *) &(res->addr),sizeof(res->addr));
		if (nbytes < 0) {
			perror("sendto");
			exit(1);
		}
	}

	pthread_res = pthread_mutex_lock(&mtx2);
	check_lock(pthread_res);
	delete2(reqid);
	pthread_res = pthread_mutex_unlock(&mtx2);
	check_lock(pthread_res);

	printf("SEND_REPLY\n");
}
