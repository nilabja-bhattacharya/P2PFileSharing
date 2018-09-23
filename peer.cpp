#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <pthread.h>
#include <bits/stdc++.h>
#include <iostream>
using namespace std;
#define endl "\n"

#define PORT 21000 
#define BACKLOG 5
#define LENGTH 512

void connection(string fr_name){
	int sockfd; 
	int nsockfd;
	char revbuf[LENGTH]; 
	struct sockaddr_in remote_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		pthread_exit(NULL);
		//exit(1);
	}
	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(PORT); 
	inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		pthread_exit(NULL);
		//exit(1);
	}
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);

	printf("[Client] Receiveing file from Server and saving it as final.txt...\n");
    char buf[LENGTH] = "4caece539b039b16e16206ea2478f8c5ffb2ca05daeb7165c91b30319a117220ff900272d43fc6988dbdfb9d81bd6fec7f3227ce79326535c9b5e993be60b084642976ec7580b5bdb830d715d089c4f67d769274bbd649407c89d90694a284ec64897e387fc64b7a3b4a941377ede04c674eaa67140c7e8197c72fee4091f610384354de77a9530bf915a9bd448128ea9cbc9e417f9b2b46052695104bfb846379cf6903b89bb8418c846fb7475f9ca43405934f130c1a27cffe9cd37435a2838a8920555e3508bc";
    send(sockfd,buf,LENGTH,0);
	FILE *fr = fopen(fr_name.c_str(), "w");
	if(fr == NULL)
		printf("File %s Cannot be opened.\n", fr_name);
	else
	{
		bzero(revbuf, LENGTH); 
		int fr_block_sz = 0;
	    while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
	    {
			//generatesha(revbuf);
			int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
		}    
	    printf("Ok received from server!\n");
	    fclose(fr);
	}
	close (sockfd);
	printf("[Client] Connection lost.\n");
	pthread_exit(NULL);
}

int main(){
    connection("f10.png");
    return 0;
}