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

void createmtorrent(char *filename){
    //cout<<"hi"<<endl;
    char mname[100];
    strcpy(mname,filename);
    strcat(mname,".mtorrent");
    //cout<<mname<<endl;
    FILE *mt = fopen(mname,"w+");
    FILE *fl = fopen(filename,"rb");
    fprintf(mt,"%s\n",filename);
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
    fprintf(mt,"%d\n", totalsize);
    fprintf(mt,"%s",str.c_str());
    //cout<<str<<endl;
}

int main(){
    createmtorrent("f5.png");
    return 0;
}
