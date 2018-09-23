#include <iostream>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
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
#include "generatehash.h"

#define PORT 22000 
#define BACKLOG 5
#define LENGTH 512*1024

struct fileinfo{
    string filename;
    string filesize;
    string hashstring;
    string clientsock;
};
void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,x,y);
    fflush(stdout);
}
vector<string> trackers;
map<string, pair<int,string>> files;
map<string,string> hashoffile;
string logfile;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
vector<string> split(string phrase, string delimiter){
    vector<string> list;
    string s = string(phrase);
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}
void senddata(string fs_name,int nsockfd){
	freopen(logfile.c_str(),"a+",stderr);
	//pthread_mutex_lock(&lock);
	char sdbuf[LENGTH];
	fprintf(stderr,"[Server] Sending %s to the Client...\n", fs_name.c_str());
	FILE *fs = fopen(fs_name.c_str(), "r");
	if(fs == NULL){
	    fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", fs_name, errno);
		//pthread_exit(NULL);
		//exit(1);
	}

	bzero(sdbuf, LENGTH); 
	int fs_block_sz; 
	while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0){
		if(send(nsockfd, sdbuf, fs_block_sz, 0) < 0)
		{
		    fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
			//pthread_exit(NULL);
		    //exit(1);
		}
		bzero(sdbuf, LENGTH);
	}
	fprintf(stderr,"Ok sent to client!\n");
	close(nsockfd);
	fprintf(stderr,"[Server] Connection with Client closed. Server will wait now...\n");
	//pthread_mutex_unlock(&lock);
	//pthread_exit(NULL);
}

void server(string ip){
	freopen(logfile.c_str(),"a+",stderr);
	int sockfd; 
	int nsockfd; 
	int num;
	int sin_size; 
	struct sockaddr_in addr_local; 
	struct sockaddr_in addr_remote, remote_addr; 
	char revbuf[LENGTH]; 
	vector<string> mysock = split(ip,":");
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	else 
		fprintf(stderr,"[Server] Obtaining socket descriptor successfully.\n");
	addr_local.sin_family = AF_INET; 
	addr_local.sin_port = htons(stoi(mysock[1])); 
	addr_local.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(addr_local.sin_zero), 8); 

	if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 ){
		fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	else 
		fprintf(stderr,"[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",stoi(mysock[1]));

	if(listen(sockfd,BACKLOG) == -1){
		fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	int success = 0;
	std::vector<std::thread> threads;
	while(success == 0){
		sin_size = sizeof(struct sockaddr_in);
		if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, (socklen_t *)&sin_size)) == -1){
		    fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
			//pthread_exit(NULL);
			//exit(1);
		}
		else 
			fprintf(stderr,"[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
		/*string sock = "";
		sock.append(inet_ntoa(addr_remote.sin_addr));
        sock.append(":");
		sock.append(to_string(ntohs(addr_remote.sin_port)));
		printf("%s",sock.c_str());*/
		bzero(revbuf,LENGTH);
		if(recv(nsockfd, revbuf, LENGTH, 0)>0){
			//cout<<revbuf<<endl;
			vector<string> hash = split(revbuf,"$");
			//cout<<hash[1]<<endl;
			string fs_name = hashoffile[hash[1]];
			//cout<<fs_name<<endl;
			threads.push_back(std::thread(senddata, fs_name,nsockfd));
		}
		success = 0;
	}
	for (auto& th : threads) th.join();  
}


void getdatafromclient(string fr_name, string ip, string val){
	freopen(logfile.c_str(),"a+",stderr);
	int sockfd; 
	int nsockfd;
	char revbuf[LENGTH]; 
	struct sockaddr_in remote_addr;
	vector<string> sock = split(ip,":");
	//cout<<ip<<endl;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(stoi(sock[1])); 
	inet_pton(AF_INET, sock[0].c_str(), &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	else 
		fprintf(stderr,"[Client] Connected to server at port %d...ok!\n", stoi(sock[1]));

	fprintf(stderr,"[Client] Receiveing file from Server and saving it as final.txt...\n");
	//cout<<val<<endl;
	if(send(sockfd, val.c_str(), LENGTH, 0) < 0)
	{
		fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n",errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	vector<string> op=split(val,"$");
	FILE *fr = fopen(fr_name.c_str(), "w");
	if(fr == NULL)
		fprintf(stderr,"File %s Cannot be opened.\n", fr_name);
	else
	{
		bzero(revbuf, LENGTH); 
		int fr_block_sz = 0;
		
	    while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
	    {
			//generatesha(revbuf);
			int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
		}   
		files[op[1]] = {1,fr_name}; 
	    fprintf(stderr,"Ok received from server!\n");
	    fclose(fr);
	}
	close (sockfd);
	fprintf(stderr,"[Client] Connection lost.\n");
	//pthread_exit(NULL);
 }
void client(string fr_name, string ip, string val){
	freopen(logfile.c_str(),"a+",stderr);
	int sockfd; 
	int nsockfd;
	char revbuf[LENGTH]; 
	struct sockaddr_in remote_addr;
	vector<string> sock = split(ip,":");
	//cout<<ip<<endl;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(stoi(sock[1])); 
	inet_pton(AF_INET, sock[0].c_str(), &remote_addr.sin_addr); 
	//inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		//cout<<sock[0]<<endl;
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		//pthread_exit(NULL);
		//exit(1);
	}
	else {
		fprintf(stderr,"[Client] Connected to server at port %d...ok!\n", stoi(sock[1]));

		fprintf(stderr,"[Client] Receiveing file from Server and saving it as final.txt...\n");
		if(send(sockfd, val.c_str(), LENGTH, 0) < 0)
		{
			fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n",errno);
			//pthread_exit(NULL);
			//exit(1);
		}
		vector<string> act = split(val,"$");
		if(act[0]=="get"){
				vector<thread> threads;
				bzero(revbuf, LENGTH); 
				int fr_block_sz = 0;
				int fl=0;
				for(int i=0;i<trackers.size();i++){
					if(trackers[i]==ip){
						fl=1;
						if((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
						{
							vector<string> serverip = split(revbuf,"#");
							for(int i=0;i<serverip.size();i++){
								//cout<<serverip[i]<<endl;
								if(serverip[i].size()>0)
									threads.push_back(thread(getdatafromclient,fr_name,serverip[i],val));
							}
						}    
					}
				}
				for (auto& th : threads) th.join();
			}
		close (sockfd);
	}
	fprintf(stderr,"[Client] Connection lost.\n");
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	logfile = argv[argc-1];
   	freopen(logfile.c_str(),"a+",stderr);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int num_of_row = w.ws_row;
    int num_of_col = w.ws_col;
    printf("\033[H\033[J");
    printf("\033[3J");
    gotoxy(num_of_row,0);
    char root[500];
    getcwd(root,100);
    strcat(root,"/");
	string clientip = argv[1];
	std::vector<std::thread> threads;
	thread t1(server,clientip);
	t1.detach();
		while(1){
			printf("\033[H\033[J");
    		printf("\033[3J");
    		gotoxy(num_of_row,0);
			string str;
			getline(cin,str);
			for(int i=2;i<argc-1;i++)
				trackers.push_back(argv[i]);
			vector<string> command = split(str," ");
			/*for(int i=0;i<command.size();i++){
				cout<<command[i]<<endl;
			}*/
			
			if(command[0]=="share"){
				if(command[1][0]=='/'){
					createmtorrent(command[1].c_str(), command[2].c_str());
				}
				else if(command[1][0]=='~'){
					command[1] = root + command[1].substr(2);
					createmtorrent(command[1].c_str(), command[2].c_str());
				}
				else{
					command[1] = root + command[1];
					createmtorrent(command[1].c_str(), command[2].c_str());;
				}
				ifstream fp (command[2],ios::out);
				struct fileinfo f;
				//cout<<"x"<<endl;
				if(fp.is_open()){
					getline(fp,f.filename);
					getline(fp,f.filesize);
					getline(fp,f.hashstring);
					f.clientsock=clientip;
					//cout<<f.filename<<endl;
					//cout<<f.filesize<<endl;
					//cout<<f.hashstring<<endl;
				}
				fp.close();
				string str="";
				str = str + "share";
				str = str+ "$";
				str = str + f.filename;
				str = str + "$";
				str = str + f.filesize;
				str = str + "$";
				str = str + f.hashstring;
				str = str + "$";
				str = str + f.clientsock;
				//cout<<str<<endl;
				hashoffile[f.hashstring]=command[1];
				files[f.hashstring] = make_pair(1,f.filename);
				/*for(int i=0;i<trackers.size();i++)
					client("share",command[2],trackers[i], str);*/
				for(int i=0;i<trackers.size();i++)
					threads.push_back(std::thread(client,"none",trackers[i], str));
			}
			else if(command[0]=="get"){
				if(command[1][0]=='/'){
			
				}
				else if(command[1][0]=='~'){
					command[1] = root + command[1].substr(2);
				}
				else{
					command[1] = root + command[1];
				}
				ifstream fp (command[1],ios::in);
				struct fileinfo f;
				//cout<<"x"<<endl;
				if(fp.is_open()){
					getline(fp,f.filename);
					getline(fp,f.filesize);
					getline(fp,f.hashstring);
					f.clientsock=clientip;
					//cout<<f.filename<<endl;
					//cout<<f.filesize<<endl;
					//cout<<f.hashstring<<endl;
				}
				string str = "";
				str = str + "get";
				str = str + "$";
				str = str + f.hashstring;
				files[f.hashstring] = make_pair(0,f.filename);
				for(int i=0;i<trackers.size();i++){
					//client("get", command[2], trackers[i],f.hashstring);
					threads.push_back(std::thread(client, command[2], trackers[i],str));	
				}
			}

			else if(command[0]=="remove"){
				if(command[1][0]=='/'){
			
				}
				else if(command[1][0]=='~'){
					command[1] = root + command[1].substr(2);
				}
				else{
					command[1] = root + command[1];
				}
				ifstream fp (command[1],ios::in);
				struct fileinfo f;
				//cout<<"x"<<endl;
				if(fp.is_open()){
					getline(fp,f.filename);
					getline(fp,f.filesize);
					getline(fp,f.hashstring);
					f.clientsock=clientip;
					//cout<<f.filename<<endl;
					//cout<<f.filesize<<endl;
					//cout<<f.hashstring<<endl;
				}

				string str = "";
				str = str + "remove";
				str = str + "$";
				str = str + f.hashstring;
				str = str + "$";
				str = str + clientip;
				remove(command[1].c_str());
				hashoffile.erase(f.hashstring.c_str());
				files.erase(f.hashstring.c_str());
				for(int i=0;i<trackers.size();i++){
					//client("get", command[2], trackers[i],f.hashstring);
					threads.push_back(std::thread(client, root, trackers[i],str));	
				}
			}
			else if(command[0]=="close"){
				string str = "";
				str = str + "remove";
				str = str + "$";
				str = str + clientip;
				//hashoffile.erase(f.hashstring.c_str());
				for(int i=0;i<trackers.size();i++){
					//client("get", command[2], trackers[i],f.hashstring);
					threads.push_back(std::thread(client, root, trackers[i],str));	
				}
				break;
			}
			else if(command[0]=="show" && command[1]=="download"){
				for(auto it = files.begin(); it != files.end(); ++it){
					//printf("\033[H\033[J");
    				//printf("\033[3J");
					if(it->second.first==1)
						cout<<"D	";
					else
						cout<<"S	";
					cout<<it->second.second<<endl;
					//sleep(5);
				}
				gotoxy(num_of_row,0);
			}
		}
		for (auto& th : threads) th.join();
		fclose(stderr);
    return 0;
}