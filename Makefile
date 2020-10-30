all: broker server client

broker: broker.c
	gcc -Wall -g broker.c -o broker -lpthread

server: server.c
	gcc -Wall -g server.c -o server -lpthread

client: client.c
	gcc -Wall -g client.c -o client -lpthread

clean:
	rm -rf broker server client
