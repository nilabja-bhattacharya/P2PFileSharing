#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <openssl/sha.h>
using namespace std;
#define BUFFERLENGTH 512*1024
string str = "";
bool generatesha(char* input){
    unsigned char obuf[BUFFERLENGTH];

    SHA1((unsigned char*)input, strlen(input), obuf);

    int i;
    char newstr[40];
    //fprintf(mt,"%s\n",input);
    for (i = 0; i < 20; i++) {
        sprintf(newstr,"%02x",obuf[i]);
        //cout<<newstr<<endl;
        str.append(newstr);
    }
    return true;
}

void createmtorrent(const char *filename,const  char *mfilename){
    //cout<<"hi"<<endl;
    //cout<<mname<<endl;
    FILE *fl = fopen(filename,"rb");
	ofstream tmp;
	tmp.open(mfilename,ios::out);
    char buffer[BUFFERLENGTH];
    size_t res;
    int totalsize=0;
    //cout<<cnt<<endl;
    //fread(buffer,sizeof(char),BUFFERLENGTH,fl);
    //cout<<buffer<<endl;
    while((res = fread(buffer,sizeof(char),BUFFERLENGTH,fl))>0){
        //cout<<buffer<<endl;
        generatesha(buffer);
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
