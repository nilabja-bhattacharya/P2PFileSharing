#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void * cientThread(void *arg)
{
  printf("In thread\n");
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  //Configure settings of the server address
 // Address family is Internet 
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function 
  serverAddr.sin_port = htons(21000);
 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("localhost");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Connect the socket to the server using the address
    addr_size = sizeof (serverAddr);
    //printf("HI");
    if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)==-1){
        fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n");
		   pthread_exit(NULL);
    }
    strcpy(message,"Hello");
    if( send(clientSocket , message , strlen(message) , 0) < 0)
    {
            printf("Send failed\n");
    }
    //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
       printf("Receive failed\n");
    }
    //Print the received message
    printf("Data received: %s\n",buffer);
    close(clientSocket);
    //printf("HI");
    pthread_exit(NULL);
}
int main(){
  int i = 0;
  pthread_t tid[51];
  while(i< 50){
    if( pthread_create(&tid[i], NULL, cientThread, NULL) != 0 )
           printf("Failed to create thread\n");
    i++;
  }
  sleep(20);
  i = 0;
  while(i< 50){
     pthread_join(tid[i++],NULL);
     printf("%d:\n",i);
  }
  return 0;
}