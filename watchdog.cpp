#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <map>
struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.5 sec.
struct timespec deltaforexec = {0 /*secs*/, 200000000 /*nanosecs*/}; //0.5 sec
using namespace std;
void sigterm(int sigterm);
ofstream outf;
map<int, string> childids;
int unnamedPipe;
int main(int argc, char *argv[])
{
    int numberofprocess;
    numberofprocess = atoi(argv[1]);
    string process_output = argv[2];
    string watchdog_output = argv[3];
    outf.open(watchdog_output);
    pid_t pid;
    int i;
    for (i = 0; i < numberofprocess; i++)
    {
        pid = fork();
        if (pid == 0) /* creating a child process */
        {
            sleep(1);
            execl("./process", "./process", process_output.c_str(), to_string(i + 1).c_str(), NULL);
            cout << "fail";
        }
        else if (pid > 0)
        {
            childids[pid] = "P" + to_string(i + 1);
            continue;
        }
        else if (pid < 0)
        {
            outf << "fail";
            return 1;
        }
    }

    char *myfifo = (char *)"/tmp/myfifo";
    unnamedPipe = open(myfifo, O_WRONLY);

    string s = "P0 ";
    s += to_string(getpid());
    char watchdogid[s.size()];
    strcpy(watchdogid, s.c_str());
    write(unnamedPipe, watchdogid, 30);

    for (map<int, string>::iterator it = childids.begin(); it != childids.end(); it++)
    {
        string tmp = it->second;
        tmp += " ";
        tmp += to_string(it->first);
        write(unnamedPipe, tmp.c_str(), 30);
    }
    while (true)
    {
        pid_t result;
        result = wait(NULL);
        if (childids[result] == "P1")
        {
            outf << "P1 is killed, all processes must be killed\n";
            nanosleep(&delta, &delta);
            outf << "Restarting all processes\n";
            map<int, string>::iterator it;
            it = childids.begin();
            for (it++; it != childids.end(); it++)
            {
                nanosleep(&delta, &delta);
                kill(it->first, 15);
                wait(NULL);
            }
            childids.clear();
            pid_t pid = 0;
            int i;
            for (i = 0; i < numberofprocess; i++)
            {
                if ((pid = fork()) == 0) /* creating a child process */
                {
                    nanosleep(&deltaforexec,&deltaforexec);
                    execl("./process", "./process", process_output.c_str(), to_string(i + 1).c_str(), NULL);
                }
                if (pid > 0)
                {
                    childids[pid] = "P" + to_string(i + 1);
                    outf << "P" + to_string(i + 1) << " is started and it has a pid of" << pid << "\n";
                }
                else if (pid < 0)
                {
                    outf << "fail";
                    return 1;
                }
            }
            for (map<int, string>::iterator it = childids.begin(); it != childids.end(); it++)
            {
                string tmp = it->second;
                tmp += " ";
                tmp += to_string(it->first);
                nanosleep(&delta, &delta);
                write(unnamedPipe, tmp.c_str(), 30);
            }
        }
        else
        {
            outf << childids.at(result) << " is killed" << endl;
            outf << "Restarting " + childids.at(result) << endl;
            string id = childids.at(result).substr(1);
            int newchild = 0;
            if ((newchild = fork()) == 0)
            {
                execl("./process", "./process", process_output.c_str(), id.c_str(), NULL);
            }
            childids[newchild] = childids.at(result);
            string tmp = childids.at(newchild) + " ";
            tmp += to_string(newchild);
            write(unnamedPipe, tmp.c_str(), 30);
            childids.erase(result);
            outf << childids.at(newchild) << " is started and it has a pid of " << newchild << endl;
        }
    }
    signal(SIGTERM, sigterm);
    return 0;
}
void sigterm(int segterm)
{
    close(unnamedPipe);
    outf << "Watchdog is terminating gracefully";
    outf.close();
    map<int, string>::iterator it;
    for (it = childids.begin(); it != childids.end(); it++)
    {
        nanosleep(&delta, &delta);
        kill(it->first, 15);
        //wait(NULL);
    }
    exit(0);
}