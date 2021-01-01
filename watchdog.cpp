/**
 @author Halil Baydar
 @no 2017400297
 @date December 2020
 */

/*
   The watchdog.cpp keeps track af all child processes and checks whether a child is died 
   or not. If it is, watchdog creates the child again
*/
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
#include <functional>
struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; /**<This time is taken from executor file to keep order 
of child process outputs*/
using namespace std;
/**
 * @see sigterm
*/
void sigterm(int segterm);           /**<This function is SIGTERM signal handling*/
ofstream outf;                       /**<this variable is to give the output of watchdog process to given file as parameter*/
map<int, string> childids;           /**<this map keep child ids and names in that manner to keep track of the childeren*/
vector<pair<string, int>> childids2; /**<this is an also keep names and ids of childeren but in this map the order 
of child is not changed after any child is gone. Thanks to this map I kill all process in ascending order*/
int unnamedPipe;                     /**< this variable is for unnamedpipe commutication with executor process*/
int numberofprocess;                 /**<This varibale keeps the number of child processes*/
string process_output;               /**<This variable keeps the process output file path in string type*/
string watchdog_output;              /**<this variable keeps the watchdog output file path in string type*/

pid_t result; /**<this variable keeps the dead child id*/
char *myfifo; /*<This is for common file to used between watchdog and executor*/
/**
 * @see main
 * The main function gets the parameters and according to given the number of children 
 * create child process and gives them their parameter which is output path and the child id like "P1"
 * Child process is converted into prcess.cpp binary code by taking parameters process output file path 
 * and name
 * while loop continues until all instructions are executed
 * this while loop waits until any child is gone
 * if gone process is "P1" all process is terminated and again created, if not, only related child is 
 * again created.
 * @param argc gives the size of the given parametes
 * @param argv gives the parameter to me: the number of process, process output file, wathcdog output file
 * */
int main(int argc, char *argv[])
{
    numberofprocess = atoi(argv[1]);
    process_output = argv[2];
    watchdog_output = argv[3];
    pid_t pid;
    int i;
    for (i = 0; i < numberofprocess; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            execl("./process", "./process", process_output.c_str(), to_string(i + 1).c_str(), NULL);
            cout << "fail";
        }
        else if (pid > 0)
        {
            sleep(1);
            childids[pid] = "P" + to_string(i + 1);
            childids2.push_back({"P" + to_string(i + 1),pid});
            continue;
        }
        else if (pid < 0)
        {
            i--;
            continue;
        }
    }

    char *myfifo = (char *)"/tmp/myfifo";
    unnamedPipe = open(myfifo, O_WRONLY);
    outf.open(watchdog_output);
    string s = "P0 ";
    s += to_string(getpid());
    char watchdogid[s.size()];
    strcpy(watchdogid, s.c_str());
    write(unnamedPipe, watchdogid, 30); /**/

    for (map<int, string>::iterator it = childids.begin(); it != childids.end(); it++)
    {
        string tmp = it->second;
        tmp += " ";
        tmp += to_string(it->first);
        outf << it->second << " "
             << "is started and it has a pid of " << it->first << '\n';
        write(unnamedPipe, tmp.c_str(), 30);
    }
    signal(SIGTERM, sigterm);
    while (true)
    {
        result = wait(NULL);
        if(result==-1)
            continue;
        if (childids[result] == "P1")
        {
            outf << "P1 is killed, all processes must be killed\n";
            nanosleep(&delta, &delta);
            outf << "Restarting all processes\n";
            vector<pair<string, int>>::iterator it;
            sleep(1);
            it = childids2.begin();
            for (it++; it != childids2.end(); it++)
            {
                kill(it->second, 15);
                nanosleep(&delta, &delta);
                wait(NULL);
            }
            childids.clear();
            childids2.clear();
            pid_t pid = 0;
            int i;
            for (i = 0; i < numberofprocess; i++)
            {
                if ((pid = fork()) == 0) /* creating a child process */
                {
                    execl("./process", "./process", process_output.c_str(), to_string(i + 1).c_str(), NULL);
                }
                if (pid > 0)
                {
                    sleep(1);
                    childids[pid] = "P" + to_string(i + 1);
                    childids2.push_back({"P" + to_string(i), pid});
                    outf << "P" + to_string(i + 1) << " is started and it has a pid of " << pid << "\n";
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
            string tmp = childids.at(result) + " ";
            tmp += to_string(newchild);
            write(unnamedPipe, tmp.c_str(), 30);
            childids[newchild] = childids.at(result);
            childids.erase(result);
            for (int i = 0; i < numberofprocess; i++)
            {
                if (childids2[i].second == result)
                {
                    childids2[i].second = newchild;
                    break;
                }
            }
            outf << childids.at(newchild) << " is started and it has a pid of " << newchild << endl;
        }
    }
    return 0;
}
/**
 * the sigterm function is called if the watchdog gets a SIGTERM signal from executor
 * @param segterm says us the given signal is 15
 * */
void sigterm(int segterm)
{
    close(unnamedPipe);
    outf << "Watchdog is terminating gracefully";
    vector<pair<string, int>>::iterator it;
    sleep(1);
    for (it = childids2.begin(); it != childids2.end(); it++)
    {
        kill(it->second, 15);
        nanosleep(&delta, &delta);
        wait(NULL);
    }
    if(segterm==15){
        outf.close();
        exit(0);
    }
}