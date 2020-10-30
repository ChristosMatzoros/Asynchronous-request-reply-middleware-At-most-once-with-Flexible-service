struct node {
	int reqid;
	char buf[MSGBUFSIZE];
	struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

//display the list
void printList() {
	struct node *ptr = head;
	printf("\n[ ");

	//start from the beginning
	while(ptr != NULL) {
		printf("(%d,%s) ",ptr->reqid,ptr->buf);
		ptr = ptr->next;
	}

	printf(" ]\n");
}

//insert link at the first location
void insertFirst(int reqid, char buf[]) {
	//create a link
	struct node *link = (struct node*) malloc(sizeof(struct node));

	link->reqid = reqid;
	strcpy(link->buf, buf);

//	printf("lBuf:%s,,%d",link->buf,link->reqid);
	//point it to old first node
	link->next = head;

	//point first to new first node
	head = link;
}

//delete first item
struct node* deleteFirst() {

	//save reference to first link
	struct node *tempLink = head;

	//mark next to first link as first
	head = head->next;

	//return the deleted link
	return tempLink;
}

//is list empty
int isEmpty() {
	return head == NULL;
}

int length() {
	int length = 0;
	struct node *current;

	for(current = head; current != NULL; current = current->next) {
		length++;
	}

	return length;
}

//find a link with given reqid
struct node* find(int reqid) {

	//start from the first link
	struct node* current = head;

	//if list is empty
	if(head == NULL) {
		return NULL;
	}

	//navigate through list
	while(current->reqid != reqid) {

		//if it is last node
		if(current->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current = current->next;
		}
	}

	//if buf found, return the current Link
	return current;
}

//delete a link with given reqid
struct node* delete(int reqid) {

	//start from the first link
	struct node* current = head;
	struct node* previous = NULL;

	//if list is empty
	if(head == NULL) {
		return NULL;
	}

	//navigate through list
	while(current->reqid != reqid) {

		//if it is last node
		if(current->next == NULL) {
			return NULL;
		} else {
			//store reference to current link
			previous = current;
			//move to next link
			current = current->next;
		}
	}

	//found a match, update the link
	if(current == head) {
		//change first to point to next link
		head = head->next;
	} else {
		//bypass the current link
		previous->next = current->next;
	}

	return current;
}

struct node2 {
	int reqid;
	int service_id;
	char buf[MSGBUFSIZE];
	struct sockaddr_in addr;
	struct node2 *next;
};

struct node2 *head2 = NULL;
struct node2 *current2 = NULL;

//display the list
void printList2() {
	struct node2 *ptr = head2;
	printf("\n[ ");

	//start from the beginning
	while(ptr != NULL) {
		printf("(%d,%s) ",ptr->reqid,ptr->buf);
		ptr = ptr->next;
	}

	printf(" ]\n");
}

//insert link at the first location
void insertFirst2(int reqid, int service_id, char buf[], struct sockaddr_in addr) {
	//create a link
	struct node2 *link = (struct node2*) malloc(sizeof(struct node2));

	link->reqid = reqid;
	link->service_id = service_id;
	strcpy(link->buf, buf);
	link->addr = addr;

//	printf("lBuf:%s,,%d",link->buf,link->reqid);
	//point it to old first node2
	link->next = head2;

	//point first to new first node2
	head2 = link;
}

//delete first item
struct node2* deleteFirst2() {

	//save reference to first link
	struct node2 *tempLink = head2;

	//mark next to first link as first
	head2 = head2->next;

	//return the deleted link
	return tempLink;
}

//is list empty
int isEmpty2() {
	return head2 == NULL;
}

int length2() {
	int length = 0;
	struct node2 *current2;

	for(current2 = head2; current2 != NULL; current2 = current2->next) {
		length++;
	}

	return length;
}

//find a link with given reqid
struct node2* find2(int reqid) {

	//start from the first link
	struct node2* current2 = head2;

	//if list is empty
	if(head2 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current2->reqid != reqid) {

		//if it is last node2
		if(current2->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current2 = current2->next;
		}
	}

	//if buf found, return the current2 Link
	return current2;
}

//find a link with given reqid
struct node2* findsvc(int service_id) {

	//start from the first link
	struct node2* current2 = head2;

	//if list is empty
	if(head2 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current2->service_id != service_id) {

		//if it is last node2
		if(current2->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current2 = current2->next;
		}
	}

	//if buf found, return the current2 Link
	return current2;
}

//delete a link with given reqid
struct node2* delete2(int reqid) {

	//start from the first link
	struct node2* current2 = head2;
	struct node2* previous = NULL;

	//if list is empty
	if(head2 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current2->reqid != reqid) {

		//if it is last node2
		if(current2->next == NULL) {
			return NULL;
		} else {
			//store reference to current2 link
			previous = current2;
			//move to next link
			current2 = current2->next;
		}
	}

	//found a match, update the link
	if(current2 == head2) {
		//change first to point to next link
		head2 = head2->next;
	} else {
		//bypass the current2 link
		previous->next = current2->next;
	}

	return current2;
}

struct node3 {
	int load;
	int service_id;
	struct sockaddr_in addr;
	struct node3 *next;
};

struct node3 *head3 = NULL;
struct node3 *current3 = NULL;

//display the list
void printList3() {
	struct node3 *ptr = head3;
	printf("\n[ ");

	//start from the beginning
	while(ptr != NULL) {
		printf("(%d,%d) ",ptr->load,ptr->service_id);
		ptr = ptr->next;
	}

	printf(" ]\n");
}

//insert link at the first location
void insertFirst3(int load, int service_id, struct sockaddr_in addr) {
	//create a link
	struct node3 *link = (struct node3*) malloc(sizeof(struct node3));

	link->load = load;
	link->service_id = service_id;
	link->addr = addr;

//	printf("lBuf:%s,,%d",link->buf,link->reqid);
	//point it to old first node3
	link->next = head3;

	//point first to new first node3
	head3 = link;
}

//delete first item
struct node3* deleteFirst3() {

	//save reference to first link
	struct node3 *tempLink = head3;

	//mark next to first link as first
	head3 = head3->next;

	//return the deleted link
	return tempLink;
}

//is list empty
int isEmpty3() {
	return head3 == NULL;
}

int length3() {
	int length = 0;
	struct node3 *current3;

	for(current3 = head3; current3 != NULL; current3 = current3->next) {
		length++;
	}

	return length;
}

//find a link with given reqid
struct node3* find3_with_addr(struct sockaddr_in addr) {

	//start from the first link
	struct node3* current3 = head3;

	//if list is empty
	if(head3 == NULL) {
		return NULL;
	}

	//navigate through list
	while( (current3->addr.sin_addr.s_addr != addr.sin_addr.s_addr) || (current3->addr.sin_port != addr.sin_port) ) {
		//if it is last node3
		if(current3->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current3 = current3->next;
		}
	}

	//if buf found, return the current3 Link
	return current3;
}

//find a link with given reqid
struct node3* find3_optimal_server(int service_id) {

	//start from the first link
	struct node3* current3 = head3;
	struct node3* res = NULL;
	int min_load = 2147483647;
	//if list is empty
	if(head3 == NULL) {
		return NULL;
	}

	if(isEmpty3()){
		return NULL;
	}

	//navigate through list
	while(1) {
		if(current3->service_id == service_id && current3->load < min_load){
			min_load = current3->load;
			res = current3;
		}
		//if it is last node3
		if(current3->next == NULL) {
			break;
		} else {
			//go to next link
			current3 = current3->next;
		}
	}

	//if buf found, return the current3 Link
	return res;
}

//delete a link with given reqid
struct node3* delete3(struct sockaddr_in addr) {

	//start from the first link
	struct node3* current3 = head3;
	struct node3* previous = NULL;

	//if list is empty
	if(head3 == NULL) {
		return NULL;
	}

	//navigate through list
	while( (current3->addr.sin_addr.s_addr != addr.sin_addr.s_addr) || (current3->addr.sin_port != addr.sin_port) ) {

		//if it is last node3
		if(current3->next == NULL) {
			return NULL;
		} else {
			//store reference to current3 link
			previous = current3;
			//move to next link
			current3 = current3->next;
		}
	}

	//found a match, update the link
	if(current3 == head3) {
		//change first to point to next link
		head3 = head3->next;
	} else {
		//bypass the current3 link
		previous->next = current3->next;
	}

	return current3;
}

struct node4 {
	int reqid;
	int service_id;
	char buf[MSGBUFSIZE];
	int replyReceived;
	struct node4 *next;
};

struct node4 *head4 = NULL;
struct node4 *current4 = NULL;

//display the list
void printList4() {
	struct node4 *ptr = head4;
	printf("\n[ ");

	//start from the beginning
	while(ptr != NULL) {
		printf("(%d,%s) ",ptr->reqid,ptr->buf);
		ptr = ptr->next;
	}

	printf(" ]\n");
}

//insert link at the first location
void insertFirst4(int reqid, int service_id, char buf[]) {
	//create a link
	struct node4 *link = (struct node4*) malloc(sizeof(struct node4));

	link->reqid = reqid;
	link->service_id = service_id;
	link->replyReceived = 0;
	strcpy(link->buf, buf);

//	printf("lBuf:%s,,%d",link->buf,link->reqid);
	//point it to old first node4
	link->next = head4;

	//point first to new first node4
	head4 = link;
}

//delete first item
struct node4* deleteFirst4() {

	//save reference to first link
	struct node4 *tempLink = head4;

	//mark next to first link as first
	head4 = head4->next;

	//return the deleted link
	return tempLink;
}

//is list empty
int isEmpty4() {
	return head4 == NULL;
}

int length4() {
	int length = 0;
	struct node4 *current4;

	for(current4 = head4; current4 != NULL; current4 = current4->next) {
		length++;
	}

	return length;
}

//find a link with given reqid
struct node4* find4(int reqid) {

	//start from the first link
	struct node4* current4 = head4;

	//if list is empty
	if(head4 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current4->reqid != reqid) {

		//if it is last node4
		if(current4->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current4 = current4->next;
		}
	}

	//if buf found, return the current4 Link
	return current4;
}

//find a link with given reqid
struct node4* findsvc4(int service_id) {

	//start from the first link
	struct node4* current4 = head4;

	//if list is empty
	if(head4 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current4->service_id != service_id) {

		//if it is last node4
		if(current4->next == NULL) {
			return NULL;
		} else {
			//go to next link
			current4 = current4->next;
		}
	}

	//if buf found, return the current4 Link
	return current4;
}

//delete a link with given reqid
struct node4* delete4(int reqid) {

	//start from the first link
	struct node4* current4 = head4;
	struct node4* previous = NULL;

	//if list is empty
	if(head4 == NULL) {
		return NULL;
	}

	//navigate through list
	while(current4->reqid != reqid) {

		//if it is last node4
		if(current4->next == NULL) {
			return NULL;
		} else {
			//store reference to current4 link
			previous = current4;
			//move to next link
			current4 = current4->next;
		}
	}

	//found a match, update the link
	if(current4 == head4) {
		//change first to point to next link
		head4 = head4->next;
	} else {
		//bypass the current4 link
		previous->next = current4->next;
	}

	return current4;
}
