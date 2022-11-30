//  File printer.h
//  Scott Lurowist
//
//
//  This file provide declarations for a printer simulator class.

#ifndef PRINTERH
#define PRINTERH


#include "globals.h"
#include "txtfiler.h"


class printer {
   private:
      txtfiler   tf;

      // True if the "paper" is loaded (an output textfile is open)
      boolean    PAPERLOADED;

      // Name of the file simulating a set of cards
      char       filename[FILENAMESIZE];

      char       buffer[NUMOFCOLUMNS];

      // Closes the file simulating a roll of paper
      void       change_paper();

   public:
      printer();
      ~printer();
      boolean   load_paper(char* name);
      boolean   print(char* line);
      boolean   printer_ready() {return PAPERLOADED;};
};


#endif
