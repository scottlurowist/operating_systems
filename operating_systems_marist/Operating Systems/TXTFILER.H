//  File Txtfiler.h
//  Scott Lurowist
//
//
//  This file provide declarations for a file managing class.

#ifndef TXTFILERH
#define TXTFILERH


#include <fstream.h>
#include "globals.h"


const   long BUFFERSIZE   = 81;        // Number of characters in a buffer
const   long FILENAMESIZE = 13;        // 8.3 for a filename plus NULL char


class txtfiler {
   private:
      boolean  ENDOFFILE;               // True when eof is reached
      fstream  f;
      char     fname[FILENAMESIZE];
      char     buffer[BUFFERSIZE];
      char     ch;

   public:
      txtfiler() {};
      ~txtfiler();
      // Saves the name of the file in the object
      void     store(char* filename) {strcpy(fname, filename);};
      boolean  openfile_read();
      boolean  openfile_write();
      boolean  is_end() {return ENDOFFILE;};
      char*    getfilename() {return fname;};
      void     closefile() {f.close();};
      char     get_char();
      void     put_char(char ch);
      char*    get_line();
      void     put_line(char* line);
      char     peeknextchar()  {return f.peek();};
};


#endif
