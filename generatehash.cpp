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
void createmtorrent(const char *filename,const  char *mfilename){
    ustring str;
    FILE *fl = fopen(filename,"r");
	ofstream tmp;
    unsigned char obuf[BUFFERLENGTH];
	tmp.open(mfilename,ios::out);
    unsigned char buffer[BUFFERLENGTH];
    size_t res;
    int totalsize=0;
    while((res = fread(buffer,sizeof(char),BUFFERLENGTH,fl))>0){
        //cout<<buffer<<endl;
        SHA1(buffer, strlen((char *)buffer), obuf);
        str = str + obuf;
        bzero(buffer,BUFFERLENGTH);
        totalsize+=res;
        //cout<<res<<endl;
    }
    tmp<<filename<<endl;
    tmp<<totalsize<<endl;
    tmp<<str.c_str()<<endl;
    tmp.close();
    fclose(fl);
    //cout<<str<<endl;
}
