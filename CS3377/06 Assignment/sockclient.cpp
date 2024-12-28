/**
 * CS 3377 Assignment 6 Sock Client Program
 * Winston Shih
 * 4/22/2024
 * Sock Client Implementation for Sockets Guess Number Program.
 **/
#include <string>
#include <iostream>
#include <sstream>

#include <sys/socket.h>	
#include <arpa/inet.h>	
#include <unistd.h>

#include "LineInfo.h"

using namespace std;

const int    MESSAGE_MAX_SIZE = 1024;
const int    ERROR     = -1;
const int    SOCK_PORT = 8888;
const string LOCAL_HOST_ADDRESS = "127.0.0.1";

const int MAX_GUESS_LIMIT = 100;
const int MAX_NO_TRIES    =  12;

int main(int argc, char* argv[]) {

    int          socketServerId;
    sockaddr_in  server;
    string       messageStr;

  try {

    //Create socketServerId
    socketServerId=socket(AF_INET, SOCK_STREAM, 0);
    if(socketServerId==ERROR)
    { 
      throw domain_error(LineInfo("Socket creation failed", __FILE__, __LINE__));
    }
    server.sin_addr.s_addr = inet_addr(LOCAL_HOST_ADDRESS.c_str());
    server.sin_family      = AF_INET;
    server.sin_port        = htons(SOCK_PORT);

    //Connect to remote server socketServerId
    if(connect(socketServerId, (struct sockaddr*) &server, sizeof(server))==ERROR)
    {
      throw domain_error(LineInfo("Socket connection failure", __FILE__, __LINE__));
    }
    // Prompt for a guess number in staed range
    cout<<"Try to guess a number between 1 and 100"<<endl;
    int nooftries = 0;
    int guessno;

    // start game
    do {
        // prompt for guessno
        cout<<"Current Try Number : "<<nooftries+1<<endl;
        cout<<"Enter number guess : ";
        cin>>guessno;

        stringstream guessnoss; 
        guessnoss << guessno;
        string guessNoStr = guessnoss.str();

        // write to socketServerId guessNoStr
        ssize_t writeStatus = send(socketServerId, guessNoStr.c_str(), guessNoStr.size(), 0);
        if(writeStatus == ERROR) {
            throw domain_error(LineInfo("Socket write failure ", __FILE__, __LINE__));
        }
        
        // read from socketServerId the receiveMessage
        char receiveMessage[MESSAGE_MAX_SIZE]={0};
        ssize_t readStatus = recv(socketServerId, receiveMessage, MESSAGE_MAX_SIZE, 0);
        if(readStatus == ERROR) {
            throw domain_error(LineInfo("Socket read failure ", __FILE__, __LINE__));
        }

        string receiveMessageStr = receiveMessage;
        cout << endl << "Server reply :" << endl << endl << receiveMessageStr << endl << endl;

        // Check if a Win Message
        if (receiveMessageStr.find("Win") != string::npos)
            break;
        nooftries++;
        // Processs if equal to max numbere of tries
        if (nooftries == MAX_NO_TRIES){
            // display done for max no of tries
            // write socketServerId Sned Random Number
            cout<<"The maximum number of tries 12 is done"<<endl;
            cout<<"Send Random Number"<<endl;
            string reqRandNo = "Random Number";
            writeStatus = send(socketServerId, reqRandNo.c_str(), reqRandNo.size(), 0);
            if(writeStatus == ERROR) {
                throw domain_error(LineInfo("Socket write failure", __FILE__, __LINE__));
            }

            // read from socketServerId randomNoMessage
            char randomNoMessage[MESSAGE_MAX_SIZE] = {0};

            readStatus = recv(socketServerId, randomNoMessage, MESSAGE_MAX_SIZE, 0);
            if(readStatus == ERROR) {
                throw domain_error(LineInfo("Socket read failure", __FILE__, __LINE__));
            }

            int randomno = stoi(randomNoMessage);
            
            //display the random no and state the game is over 
            cout<<"The random number is : "<<randomno<<endl;
            cout<<"Game is over"<<endl;
            //leave the loop
            break;
        }

    } while (true);

    if (close(socketServerId) == ERROR)
        throw domain_error(LineInfo("Socket close failure ", __FILE__, __LINE__));
  }
  //try
  catch (exception& e) {
      cout << e.what() << endl;
      exit(EXIT_FAILURE);
  }//catch

  exit(EXIT_SUCCESS);
        
}