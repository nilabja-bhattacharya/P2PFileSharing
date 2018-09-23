#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <openssl/sha.h>
using namespace std;
#define BUFFERLENGTH 512*1024
typedef std::basic_string<unsigned char> ustring;
void createmtorrent(vector<string> trackers,const char *filename,const  char *mfilename){
    string str;
    FILE *fl = fopen(filename,"r");
	ofstream tmp;
    unsigned char obuf[BUFFERLENGTH];
	tmp.open(mfilename,ios::out);
    unsigned char buffer[BUFFERLENGTH];
    size_t res;
    int totalsize=0;
    while((res = fread(buffer,sizeof(char),BUFFERLENGTH,fl))>0){
        //cout<<buffer<<endl;
        SHA256(buffer, strlen((char *)buffer), obuf);
        char newstr[2];
        for(int i=0;i<20;i++){
            sprintf(newstr,"%02x",obuf[i]);
            str = str + newstr;
        }
        bzero(buffer,BUFFERLENGTH);
        totalsize+=res;
        //cout<<res<<endl;
    }
    tmp<<filename<<endl;
    tmp<<totalsize<<endl;
    tmp<<str.c_str()<<endl;
    for(int i=0;i<trackers.size();i++)
        tmp<<trackers[i]<<endl;
    tmp.close();
    fclose(fl);
    //cout<<str<<endl;
}
