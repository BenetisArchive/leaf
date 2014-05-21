#include "stdio.h"
#include <string.h>
#include <unistd.h>
//sockets
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
//other files
#include "testF.h"
#include <pthread.h>

void *serverSocket(void *ptr);

int main (int argc, char const *argv[])
{
  struct sockaddr_in server_address;
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&server_address, '0', sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(1337);
  //when socket is create, it does not have address assigned
  //bind assigns address to socket
  bind(socketfd, (struct sockaddr*) &server_address, sizeof(server_address));

  // 15 => max connections for backlog(clients who can use this socket)
  // if it exceeds this limit, client will get ECONNREFUSED
  listen(socketfd, 15);

  int connfd = 0;
  printf("Waiting for clients to connect\n");
  while(1) {
    connfd = accept(socketfd,(struct sockaddr*)NULL, NULL);
    if(connfd == -1) perror("Could not accept socket");
  	pthread_t tid;
    pthread_create(&tid, NULL, serverSocket, &connfd);
  }

  return 0;
}

void *serverSocket(void *ptr) {
  char response[] = "<html><body><h3>Static content</h3></body></html>";
  int connfd = *((int *)ptr);
  write(connfd, "HTTP/1.1 200 OK\n", 16);
  char contentLength[127];
  sprintf(contentLength, "Content-length: %d\n", (int)strlen(response));
  write(connfd, contentLength, strlen(contentLength));
  write(connfd, "Content-Type: text/html\n\n", 25);
  write(connfd, response ,strlen(response));
  close(connfd);
}
