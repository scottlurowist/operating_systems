//  File cardread.h
//  Scott Lurowist
//
//
//  This file provide declarations for a card reader simulator class.

#ifndef CARDREADH
#define CARDREADH


#include "globals.h"
#include "txtfiler.h"


class cardreader {
   private:
      txtfiler   tf;

      // True when no cards are left to read
      boolean    READEREMPTY;

      // True if a card set is loaded
      boolean    SETLOADED;

      // Name of the file simulating a set of cards
      char       cardsetname[FILENAMESIZE];

      char       buffer[NUMOFCOLUMNS];

      // Closes the file simulating a card set
      void       unload_cardset();

   public:
      cardreader();
      ~cardreader();
      boolean   load_cardset(char* setname);
      char*     get_card();

      // True if the card reader is empty
      boolean   reader_empty() {return READEREMPTY;};
};


#endif
