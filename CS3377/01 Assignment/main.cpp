#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <time.h>
#include <limits>
#include <locale>
#include "LineInfo.h"
using namespace std;
#define BILLION 1000000000L
const string INPUT_FILE="bisonsearchin.txt";
const string OUTPUT_FILE="bisonfoundin.txt";
const uint8_t MAX_FILE_NO=10;
int main(){
    uint64_t diff;
    struct timespec start, end;
    cout.imbue(locale(""));
    string searchBisonInGrassStr;
    string inputFile;
    string outputFile;
    uint16_t fileCount=0;
    try{
       do{
          fileCount=fileCount+1;
	  inputFile=outputFile=to_string(fileCount);
	  if(inputFile.length()==1)
	  {
	   inputFile="0"+inputFile;
	   outputFile="0"+outputFile;
	  }
	  inputFile+=INPUT_FILE;
	  outputFile+=OUTPUT_FILE;
	  ifstream inputParensStreamObj(inputFile);
	  if(!inputParensStreamObj.is_open())
	  {
	     throw domain_error(LineInfo("open FAILURE File "+inputFile, __FILE__, __LINE__));
	  }
	  ofstream outputParensStreamObj(outputFile);
	  if(!outputParensStreamObj.is_open())
	  {
	     throw domain_error(LineInfo("open FAILURE File "+outputFile, __FILE__, __LINE__));
	  }
	  outputParensStreamObj.imbue(locale(""));
	  inputParensStreamObj>>searchBisonInGrassStr;
	  unsigned answerFoundBisonPatternCount;
	  unsigned backParenCount=0;
	  clock_gettime(CLOCK_MONOTONIC, &start);
	  size_t size=searchBisonInGrassStr.size();
	  for(unsigned i=1;i<size;i++)
	  {
	      if(searchBisonInGrassStr[i-1]==')' && searchBisonInGrassStr[i]==')')
	      {
	         answerFoundBisonPatternCount+=backParenCount;
	      }
	      else if(searchBisonInGrassStr[i-1]=='('&& searchBisonInGrassStr[i]=='(')
	      {
	         backParenCount=backParenCount+1;
	      }
	  }
          clock_gettime(CLOCK_MONOTONIC, &end);
	  diff=BILLION*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;  
	  cout<<"elapsed time = "<<diff<< " nanoseconds"<<endl;
	  outputParensStreamObj<<"Time Elapsed (name) : "<<setw(15)<<diff<<endl<<endl;
	  outputParensStreamObj<<"Found Pattern Count : "<<answerFoundBisonPatternCount<<endl<<endl;
	  outputParensStreamObj<<"Searched Pattern    : "<<endl<<endl;
	  outputParensStreamObj<<searchBisonInGrassStr;
	  outputParensStreamObj.close();
       }while(!(fileCount==MAX_FILE_NO));
   }//try
   catch (exception& e)
   {
       cout<<e.what()<<endl;
       cout<<endl<<"Press the enter key once or twice to leave..."<<endl;
       cin.ignore(); cin.get();
       exit(EXIT_FAILURE);
   }//catch
   return EXIT_SUCCESS;
}//main()
