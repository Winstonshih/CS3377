/**
 * Assignment 4 Calculate Program
 * Winston Shih
 * 4/8/2024
 * Calculates sum, mean, max, or min of a text file of random numbers.
 **/
 #include <string>
 #include <fstream>
 #include <iostream>
 #include <vector>
 #include <iterator>
 #include <numeric>
 #include <algorithm>
 #include "LineInfo.h"
 using namespace std;
 int main(int argc, char* argv[])
 {
     try{
         if(argc!=3)
         {
             stringstream s;
             s<<endl<<endl<<
                "Wrong arguments"<<endl<<
                "Usage:"<<endl<<
                "calculate < sum | average | greatest | least > filename.txt "<<endl<<
                "filename - a file that contains integers, 1 integer per line "<<
                "Example:" << endl<<
                "Calculate sum filerandom.txt"<<endl<<endl;
             throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
         }
         cout<<argv[0]<<" "<<argv[1]<<" "<<argv[2]<<endl;
         vector<int> vectorInts;
         ifstream randomNosStreamObj(argv[2]);
         if(randomNosStreamObj.fail())
         {
             throw domain_error(LineInfo("\n\nopen FAILURE File\n\n"+*argv[2], __FILE__, __LINE__));
         }
         istream_iterator<int> inputIt(randomNosStreamObj);
         copy(inputIt, istream_iterator<int>(), back_inserter(vectorInts));
         string command = argv[1];
         if(command=="sum")
         {
             cout<<"Sum      : "<<accumulate(vectorInts.begin(), vectorInts.end(), 0)<<endl;
         }
         else if(command=="average")
         {
             cout<<"Average  : "<<((accumulate(vectorInts.begin(), vectorInts.end(), 0))/((float)vectorInts.size()))<<endl;
         }
         else if(command=="least")
         {
             cout<<"Least    : "<<*(min_element(vectorInts.begin(), vectorInts.end()))<<endl;
         }
         else if(command=="greatest")
         {
             cout<<"Greatest : "<<*(max_element(vectorInts.begin(), vectorInts.end()))<<endl;
         }
         else
         {
             stringstream s;
             s<<endl<<endl<<
              "Invalid Command: command"<<endl<<
              "Usage:"<<endl<<
              "calculate < sum | average | greatest | least > filename.txt "<<endl<<
              "filename - a file that contains integers, 1 integer per line "<<
              "Example:" <<endl<<
              "calculate sum filerandom.txt"<<endl<<endl;
             throw domain_error(LineInfo(s.str(), __FILE__, __LINE__));
              
         }
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
