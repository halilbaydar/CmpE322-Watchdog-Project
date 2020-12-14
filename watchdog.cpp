#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string.h> 

using namespace std;
ofstream outputf;
int createChildProcesses(map<int,string> & childids,string process_output,int numberofprocess){
    fstream outputstream;
    pid_t pid=0;
    int i;
    for(i=0; i<numberofprocess; i++)
        {   
	        if ((pid = fork()) == 0) /* creating a child process */
            {
                execl("./process.out","./process.out",process_output,0,NULL);
            }           
            childids[pid]="P"+i;
            string uu = "P"+to_string(i+1);
            string ss=uu +" started and it has a pid of "; ss+=pid+"\n";
            outputstream<<ss;
        }
        return 0;
    outputstream.close();    
    return 0;
}

int main(int argc, char  * argv [])
{

    map<int,string>  childids;
    int numberofprocess;
    numberofprocess=atoi(argv[1]);
    string process_output = argv[2];
    string watchdog_output = argv[3];
    outputf.open(watchdog_output);

    createChildProcesses(childids,process_output,numberofprocess);
    
    int unnamedPipe;
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0644);
    char temp[30];
    unnamedPipe = open(myfifo,O_WRONLY);

    string s= "P0 " +getpid();
    
    char watchdogid[s.size() + 1];
    strcpy(watchdogid, s.c_str()); 

    write(unnamedPipe,watchdogid,s.size()+1);    

    for(int i=0; i<childids.size(); i++){
        string tmp="P"+i;
        tmp+=" "+childids[0];
        char process_name_and_id[tmp.size()+1];
        strcpy(process_name_and_id,tmp.c_str());
        write(unnamedPipe,process_name_and_id,strlen(process_name_and_id)+1);
    }
    int status=-1;
    while(1){
        wait(&status);
        if(status>0){

        outputf<<"Restarting " + childids.at(status);
        int childidtemp=0;
        if((childidtemp=fork())==0){
            execl("./process.out","./process.out",process_output,0,NULL);
        }
        childids[childidtemp]=childids.at(status);
        string tmp=childids.at(status)+" ";
        childids.at(status)+=status;
        childids.erase(status);

        char process_name_and_id[tmp.size()+1];
        strcpy(process_name_and_id,tmp.c_str());
        write(unnamedPipe,process_name_and_id,strlen(process_name_and_id)+1);
        }
    }



    outputf << "Watchdog is terminating gracefully";
     
    outputf.close();
    return 0;
}