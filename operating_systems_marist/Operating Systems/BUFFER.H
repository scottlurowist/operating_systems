//FILE BUFFER.H


// This is the buffer object that is used by the channels for
// input and output spooling

#ifndef BUFFER_H
#define BUFFER_H

#include <string.h>
#include "globals.h"


class ABuffer{
  private:
    long pid;
    pagetype pgtype;
    char buf[41];
  public:
     ABuffer() { };
     char * getBufStr(){ return buf; };            //get the data
     pagetype getPageType(){ return pgtype; };     //get the type of data
     long getProcessId(){ return pid; };            //get the process id
     void setpid(long p) { pid = p; };              //set the process id
     void setdata(char* d) { strcpy(buf,d); }; //store the data
     void setpagetype(pagetype t) { pgtype = t; }; //set the type of data
     ~ABuffer(){};
};

#endif
