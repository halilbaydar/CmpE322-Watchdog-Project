#include <bits/stdc++.h>
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <unistd.h>

using namespace std;
string name="";
string filepath="";
fstream output;

void sighup(int sighup_){
    signal(SIGHUP,sighup);
    output<< name+"received signal 1\n";
}
void sigint(int sigint_){
    signal(SIGINT,sigint);
    output<< name+"received signal 2\n";
}
void sigill(int k){
    signal(SIGILL,sigill);
    output<< name+"received signal 4\n";
}
void sigtrap(int k){
    signal(SIGTRAP,sigtrap);
    output<< name+"received signal 5\n";
}
void sigfpe(int k){
    signal(SIGFPE,sigfpe);
    output<< name+"received signal 8\n";
}
void sigbus(int k){
    signal(SIGBUS,sigbus);
    output<< name+"received signal 10\n";
}
void sigsegv(int k){
    signal(SIGSEGV,sigsegv);
    output<< name+"received signal 11\n";
}
void sigterm(int k){
    signal(SIGTERM,sigterm);
    output<< name+"received signal 15\n";
    exit(0);
}
void sigxcpu(int k){
    signal(SIGXCPU,sigxcpu);
    output<< name+"received signal 24\n";
}
int main(int argc , char * argv []){
    string name="P"+*argv[2];
    output.open(argv[1],ios::out | ios::in);
    output << name+" is waiting for a signal\n";
    while(1>0){
    signal(SIGHUP,sighup);
    signal(SIGINT,sigint);    
    signal(SIGILL,sigill);
    signal(SIGTRAP,sigtrap);
    signal(SIGFPE,sigfpe);
    signal(SIGBUS,sigbus);
    signal(SIGSEGV,sigsegv);
    signal(SIGTERM,sigterm);
    signal(SIGXCPU,sigxcpu);
    }
    return 0;
}