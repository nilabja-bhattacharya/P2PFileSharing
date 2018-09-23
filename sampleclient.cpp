#include <stdlib.h>
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
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#define PORT 21000
#define LENGTH 512

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

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

int client(){
	string fr_name="f1";
	std::vector<std::thread> threads;
    for(int i=0;i<5;i++) {
            threads.push_back(std::thread(connection, fr_name+to_string(i)+".png"));
    }
	for (auto& th : threads) th.join();
	return 1;
}