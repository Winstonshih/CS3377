/**
 * Assignment 4 Fork Execvp
 * Winston Shih
 * 4/8/2024
 * Designs forks that will execute child processes that will
 * execute calculate program and display results using cat commands.
 **/
#include <string>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdexcept>
#include <sstream>
#include "LineInfo.h"
using namespace std;
int const READ=0;
int const WRITE=1;
int const PIPE_ERROR=-1;
int const CHILD_PID=0;
int const MAX_PIPE_MESSAGE_SIZE=1000;
int const EXECVP_FAILED=-1;
void CreateArg1FileWithArg2RandomNumbersArg3RandomRange(string randomFileNameStr, unsigned noRandomNumbersUns, unsigned randomRange)
{
    ofstream outfileStream(randomFileNameStr);
    if(outfileStream.fail())
    {
        stringstream s;
        s<<"Error opening file Random Numbers File "<<randomFileNameStr<<endl;
        throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
    }
    time_t t;
    srand((unsigned)time(&t));
    unsigned i;
    for(i=0; i<noRandomNumbersUns;++i)
    {
        outfileStream<<( (rand() % randomRange)+1 )<<endl;
    }
    outfileStream.close();
}
int main(int argc, char* argv[])
 {
     try{
         if (argc != 4)
         {
             stringstream s;
             s<<endl<<endl
              <<"Wrong arguments"<<endl
              <<"Usage: "<<endl
              <<"forkexecvp <file to create> <amount of random numbers> <random number range>"<<endl
              <<"Example:"<<endl
              <<"forkexecvp randomnumber 1000 100 "<<endl<<endl;
              throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
         }
         string randomFileNameStr=argv[1];
         unsigned noRandomNumbersUns=atoi(argv[2]);
         unsigned randomRange=atoi(argv[3]);
         cout<<endl<<argv[0]<<" "<<randomFileNameStr<<" "<<noRandomNumbersUns<<" "<<randomRange<<endl;
         CreateArg1FileWithArg2RandomNumbersArg3RandomRange(randomFileNameStr, noRandomNumbersUns, randomRange);
         int pipeParentWriteChildReadfds[2];
         string messages[]={"sum", "average", "greatest", "least"};
         int noOfMessages=sizeof(messages)/sizeof(messages[0]);
         for(int childProcessNo=0; childProcessNo<noOfMessages;++childProcessNo)
         {
             if(pipe(pipeParentWriteChildReadfds)==PIPE_ERROR)
             {
                 stringstream s;
                 s<<"Unable to create pipe pipeParentWriteChildReadfds";
                 throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
             }
             pid_t forkpid=fork();
             if(forkpid<0)
             {
                 stringstream s;
                 s<<"fork failed\n";
                 throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
             }
             if(forkpid!=CHILD_PID)
             {
                 close(pipeParentWriteChildReadfds[READ]);
                 cout<<"Parent pid : "<<getpid()<<" to Child Process No : "
                     <<childProcessNo<<endl<<
                     "Send Message : "<<messages[childProcessNo]<<endl;
                 if(write(pipeParentWriteChildReadfds[WRITE], messages[childProcessNo].c_str(), sizeof(messages[childProcessNo].c_str()))==PIPE_ERROR)
                 {
                     stringstream s;
                     s<<"pipe write failed";
                     throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
                 }
            }
            else
            {
                 close(pipeParentWriteChildReadfds[WRITE]);
                 char pipeReadMessage[MAX_PIPE_MESSAGE_SIZE]={0};
                 if(read(pipeParentWriteChildReadfds[READ], pipeReadMessage, sizeof(pipeReadMessage))==PIPE_ERROR)
                 {
                     stringstream s;
                     s<<"pipe read failed";
                     throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
                 }
                 cout<<"Child pid : "<<getpid()<<" Child Process No : "<< childProcessNo<<endl<<
                 " Received Message : "<<pipeReadMessage<<endl;
                 char * arglist[] = { (char *)"./calculate", pipeReadMessage, (char*)randomFileNameStr.c_str(), NULL};
                 cout<<"Child pid : "<<getpid()<<" Child Process No : "<<childProcessNo<<endl<<
                 "execvp("<<arglist[0]<<", ./calculate, "<<randomFileNameStr.c_str()<<endl;
                 string answerFileName="answer"; 
                 answerFileName+=pipeReadMessage;
                 answerFileName+=".txt";
                 freopen(answerFileName.c_str(), "w", stdout);
                 close(pipeParentWriteChildReadfds[READ]);
                 if(execvp(arglist[0], arglist)==EXECVP_FAILED)
                 {
                     stringstream s;
                     s<<"execvp failed";
                     throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
                 }
            }
        }
        
        cout<< "Parent pid: "<<getpid()<<" Start - Wait for all calculate children to finish"<<endl;
        while (wait(NULL)>0);
        cout<< "Parent pid: "<<getpid()<<" Done - Wait for all calculate children to finish"<<endl;
        close(pipeParentWriteChildReadfds[WRITE]);
        cout<<"Parent pid : "<<getpid()<<" Use execvp() cat to display answer files: "<<endl;
        for(int childProcessNo=0;childProcessNo<noOfMessages;++childProcessNo)
        {
            pid_t forkpid=fork();
            if(forkpid<0) 
            {
                stringstream s;
                s<<"fork failed";
                throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
            }
            if(forkpid==CHILD_PID)
            {
                string answerFileName="answer";
                answerFileName+=messages[childProcessNo];
                answerFileName+=".txt";
                char* arglist[]={ (char*)"cat", (char*)answerFileName.c_str(), NULL};
                cout<<"Child pid : "<<getpid()<<endl<<
                    "cat "<<answerFileName<<endl;
                if(execvp("cat", arglist)==EXECVP_FAILED)
                {
                    stringstream s;
                    s<<"execvp failed";
                    throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
                }
            }
        }
        cout<< "Parent pid: "<<getpid()<<" Start - Wait for all calculate children to finish"<<endl;
        while (wait(NULL)>0);
        cout<< "Parent pid: "<<getpid()<<" Done - Wait for all calculate children to finish"<<endl; 
              
     }
     catch(exception& e)
     {
         cout<<e.what()<<endl<<endl;
         cout<<"Press the enter key once or twice to leave..."<<endl<<endl;
         cin.ignore();
         cin.get();
         exit(EXIT_FAILURE);
     }
     exit(EXIT_SUCCESS);
 }
