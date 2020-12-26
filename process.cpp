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

void sighandler(int sentsignal){
    signal(sentsignal,sighandler);
    output<< name+" received signal "<<sentsignal;
    if(sentsignal==15){
        output<<",terminating gracefully"<<endl;
        exit(0);
    }
    output<<endl;
}
int main(int argc , char * argv []){
    name="P";
    name+=(char)*argv[2];
    output.open(argv[1],ios::app);
    
    output <<name<<" is waiting for a signal\n";
        
    signal(SIGHUP,sighandler);
    signal(SIGINT,sighandler);    
    signal(SIGILL,sighandler);
    signal(SIGTRAP,sighandler);
    signal(SIGFPE,sighandler);
    signal(SIGBUS,sighandler);
    signal(SIGSEGV,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGXCPU,sighandler);
    for(;;){}
    return 0;
}