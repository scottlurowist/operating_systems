// File txtfiler.cpp
// Scott Lurowist
//
//
//  this program implements a file manager class

#include <string.h>
#include <stdlib.h>
#include "txtfiler.h"


txtfiler::~txtfiler() {

   // If the file is open, then close it
   if (f)
      closefile();


}


// Opens the file specified for reading, and performs exception handling if
// the file doesn't exist.
boolean txtfiler::openfile_read() {
   f.open(fname, ios::in);

   if (!f) {
      return FALSE;
   }
   else {
      ENDOFFILE = FALSE;
      return TRUE;
   }
}


// Opens the file specified for writing, and performs exception handling if
// the file doesn't exist.
boolean txtfiler::openfile_write() {
   f.open(fname, ios::in | ios::out);

   if (!f) {
      return FALSE;
   }
   else {
      ENDOFFILE = FALSE;
      return TRUE;
   }
}


// Gets the next character in the file. If the next character is the
// end of the file, then ENDOFPROGRAM is set to true.
char txtfiler::get_char() {
   char ch;   // character read from the stream

   f >> ch;

   if (f.eof())
      ENDOFFILE = TRUE;

   return ch;
}


void txtfiler::put_char(char ch) {

   f << ch;

}


// Gets the next line in the file. If the end of the file, then ENDOFPROGRAM
// is set to true.
char* txtfiler::get_line() {

   if (f)
      f.getline(buffer, BUFFERSIZE);
   else {
      ENDOFFILE = TRUE;
      buffer[0] = '\0';
   }

   return buffer;
}


void txtfiler::put_line(char* line) {

   f << line << endl;

   ENDOFFILE = FALSE;
}
