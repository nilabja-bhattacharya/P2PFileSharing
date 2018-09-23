#include <unistd.h>
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;
void child();
void parent();
int main(){
    int pid = fork();
    if(pid==0){
        child();
    }
    else
        parent();
    return 0;
}
void child(){
    for(int i=0;i<10;i++)
        cout<<i<<endl;
    exit(0);
}
void parent(){
    for(int i=20;i<100;i++)
    cout<<i<<endl;
}