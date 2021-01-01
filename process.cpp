/**
 @author Halil Baydar
 @no 2017400297 
 @date December 2020
 */
#include <bits/stdc++.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
string name = "";     /**< this variable is to keep name of process given by watcdog process*/
string filepath = ""; /**<this variable is for the output file path*/
fstream output;       /**<this variable is to write output into given file path*/
/**
 * @see sighandler
 * @param sentsignal this parameter is to keep sent signal number
 * this functin handles the given signal that means write outputs according to given signal
 * if the sent signal is SIGTERM, process is quit with exit(0)
*/
void sighandler(int sentsignal)
{
    output.open(filepath, ios::app);
    signal(sentsignal, sighandler);
    output << name + " received signal " << sentsignal;
    if (sentsignal == 15)
    {
        output << ", terminating gracefully" << endl;
        output.close();
        exit(0);
    }
    output << endl;
    output.close();
}
/**
 * @see main
 * @param argc this variable is for given parameters total size
 * @param argv this variable is used to take given parameters
 * main function continues until SOIGTERM signal is sent thanks to infinite for loop
*/
int main(int argc, char *argv[])
{
    name = "P";
    for(int i=0; i<strlen(argv[2]); i++)
        name +=argv[2][i];
    filepath = argv[1];
    output.open(argv[1], ios::app);
    output << name << " is waiting for a signal\n";
    output.close();
    signal(SIGHUP, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGTRAP, sighandler);
    signal(SIGFPE, sighandler);
    signal(SIGBUS, sighandler);
    signal(SIGSEGV, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGXCPU, sighandler);
    for (;;)
    {
    }
    return 0;
}