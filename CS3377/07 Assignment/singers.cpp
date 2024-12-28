/**
 * CS 3377 Assignment 7 Singer Threads
 * Winston Shih
 * 4/23/2024
 * This assignment will use posix threads (pthreads) to have pthread functions display song lyric lines and update a sung line counter for each lyric line displayed. 
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <pthread.h>

#include "LineInfo.h"

using namespace std;

pthread_mutex_t lock;
static unsigned totalLineCount;

void GetLyricLinesFromFileToVector(string          lyricsFilenameStr,
                                   vector<string>& lyricLinesVector,
                                   unsigned&       noOfLyricLines) {
    ifstream FileStringStream(lyricsFilenameStr);
    if(FileStringStream.fail())
    {
        throw domain_error(LineInfo("Error opening file PopLife File ", __FILE__, __LINE__));
    }
    string line;
    while(getline(FileStringStream, line))
    {
        if(!(line.empty()))
        {
            lyricLinesVector.push_back(line);
            noOfLyricLines++;
        }
    }
}

struct SingLinesThreadInfoStruct {
    unsigned*      singerNoIdPtr;
    vector<string> lyricLinesVector;
    unsigned       noOfLyricLines;
};

// cout is not thread safe, so all couts need to be locked and then unlocked
void* SingLinesThread(void* singLinesThreadInfoStructPtr) {

    SingLinesThreadInfoStruct* threadInfoPtr = (SingLinesThreadInfoStruct*)singLinesThreadInfoStructPtr;
    static unsigned previousNoId;

    const unsigned maxSingLineNo    = 10;
    unsigned*      singersNoIdPtr   = threadInfoPtr->singerNoIdPtr;
    vector<string> lyricLinesVector = threadInfoPtr->lyricLinesVector;
    unsigned       noOfLyricLines   = threadInfoPtr->noOfLyricLines;

    // $$ lock the mutex
    pthread_mutex_lock(&lock);
    //$$ display start thread message
    cout<<"start thread "<<*singersNoIdPtr<<endl;
    // $$ unlock the mutex
    pthread_mutex_unlock(&lock);
    //sing up to 10 lines at a time
    unsigned singLimit    = 0;

    for (unsigned lyricLineNo = 0; lyricLineNo < noOfLyricLines; lyricLineNo+= maxSingLineNo){

        unsigned currentLineSungNo = lyricLineNo;
        singLimit += maxSingLineNo;

        for ( ; (currentLineSungNo < singLimit) && (currentLineSungNo < noOfLyricLines); currentLineSungNo++) {

            // $$ lock the mutex
            pthread_mutex_lock(&lock);

            if (previousNoId != *singersNoIdPtr) // for dipslay readabilty
                cout << endl;

            //$$ display singer no and lyric line
            cout<<*singersNoIdPtr<<" :  "<<currentLineSungNo<<" > "<<lyricLinesVector[currentLineSungNo] << endl;   
            // $$ unlock the mutex
            pthread_mutex_unlock(&lock);
            //$$ update previousNoId and increment totalLineCount;
            previousNoId=*singersNoIdPtr;
           totalLineCount++;
        }

        //$$ get up time slice for 1 second
        sleep(1);
    }

    // $$ lock the mutex
    pthread_mutex_lock(&lock);
    //$$ display done   thread with singersNoIdPtr 
    cout<<"done thread "<<*singersNoIdPtr;
    cout << endl;
    pthread_mutex_unlock(&lock);
    
    //$$ exit the pthread
    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {

  try{

    if (argc !=3) {
        //$$string errorStr = "Usage : ";    
        throw domain_error(LineInfo("Usage: ./singers ", __FILE__, __LINE__));   
    }

    string         lyricsFilenameStr(argv[1]);
    unsigned       noOfSingers    = stoi(argv[2]);
    vector<string> lyricLinesVector;
    unsigned       noOfLyricLines = 0;

    GetLyricLinesFromFileToVector(lyricsFilenameStr, lyricLinesVector, noOfLyricLines);

    cout << endl << endl;
    cout<<"Song Lyrics File Name is : "<<lyricsFilenameStr<<endl;
    cout<<"Number of lyric lines is : "<<noOfLyricLines<<endl;
    cout << endl << endl;

    pthread_t*                 singersThreadIdPtr;
    unsigned*                  singersNoIdPtr;
    SingLinesThreadInfoStruct* singLinesThreadInfoStructPtr;

    singersThreadIdPtr           = new pthread_t[noOfSingers];
    singersNoIdPtr               = new unsigned [noOfSingers];
    singLinesThreadInfoStructPtr = new SingLinesThreadInfoStruct[noOfSingers];

    //$$ initialize the pthread mutex lock 
    pthread_mutex_init(&lock, NULL);
    // Create independent threads each of which will execute the pthread function
    for (unsigned singerNo = 0; singerNo < noOfSingers; ++singerNo) {

        unsigned *singerNoIdPtr = new unsigned;
        *singerNoIdPtr = singerNo+1;

        //$$ Fill the singLinesThreadInfoStructPtr[singerNo] with the corresponding variable values
        singLinesThreadInfoStructPtr[singerNo].singerNoIdPtr = singerNoIdPtr;
        singLinesThreadInfoStructPtr[singerNo].lyricLinesVector = lyricLinesVector;
        singLinesThreadInfoStructPtr[singerNo].noOfLyricLines = noOfLyricLines;

        // $$ lock the mutex
        pthread_mutex_lock(&lock);
        // display create thread with *singerNoIdPtr
        cout<<"create thread "<<*singerNoIdPtr<<endl;
        // $$ unlock the mutex
        pthread_mutex_unlock(&lock);
        //$$ create the thread using function SingLinesThread
        pthread_create(&singersThreadIdPtr[singerNo], NULL, SingLinesThread, (void*)&singLinesThreadInfoStructPtr[singerNo]);
    }//for

    // Wait until all threads have completed before main continues.
    // If main doesn't wait, main may continue to an exit which will terminate
    // the main process. When main terminates, all associated threads will also
    // terminate before the threads have completed. 

    //$$ join to all the running threads
    for (unsigned singerNo = 0; singerNo < noOfSingers; ++singerNo) 
    {
        pthread_join(singersThreadIdPtr[singerNo], NULL);
        delete singLinesThreadInfoStructPtr[singerNo].singerNoIdPtr;
    }
    cout << "\nProgram done " << endl;
    cout << "Total Line count sung : " << totalLineCount << endl;
    cout << endl;
    //$$ pthread_mutex_destroy
    pthread_mutex_destroy(&lock);
  }
  catch (exception& e) {
      cout << e.what() << endl;
      exit(EXIT_FAILURE);
  }//catch

  exit(EXIT_SUCCESS);
}