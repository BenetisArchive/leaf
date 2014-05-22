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
#include <stdlib.h>

void *getInfo(void *ptr);

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
  char *response;
  response = (char *)malloc(1>>5);
  response = "<html><body><h3>Waiting</h3></body></html>";
  int connfd = 0;
  printf("Waiting for clients to connect\n");
  while(1) {
    connfd = accept(socketfd,(struct sockaddr*)NULL, NULL);
    if(connfd == -1) perror("Could not accept socket");
    pid_t pid = fork();
    if (!pid) {
      close(connfd);
    } else {
      write(connfd, "HTTP/1.1 200 OK\n", 16);
      char contentLength[127];

      pthread_t tid;
      pthread_create(&tid, NULL, getInfo, NULL);

      void *return_value;
      if(pthread_join(tid, &return_value)) { // BLOCKING ;(
        fprintf(stderr, "Failed to create thread\n");
        return 2;
      }
      response = (char *)return_value;

      sprintf(contentLength, "Content-length: %d\n", (int)strlen(response));
      write(connfd, contentLength, strlen(contentLength));
      write(connfd, "Content-Type: text/html\n\n", 25);
      write(connfd, response ,strlen(response));
      close(connfd);
      free(return_value);
      exit(0);
    }
    // usleep(100);
  }
  free(response);
  return 0;
}

void *getInfo(void *ptr) {
  FILE* file = popen("/usr/bin/top -b -n1 2>&1", "r");
  char* buffer = malloc(100000000);
  size_t len = 0;
  ssize_t read;
  int written = 0;
   if (file == NULL)
       exit(EXIT_FAILURE);
    char * line = NULL;
   while ((read = getline(&line, &len, file)) != -1) {
       buffer[written] = line;
       written = read;
       buffer[++written] = '\0';
   }

  pthread_exit((void *)buffer);
}
