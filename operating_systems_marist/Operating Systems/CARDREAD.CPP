// File cardread.cpp
// Scott Lurowist
//
//
//  this program implements a card reader simulator class


#include <string.h>
#include "cardread.h"


cardreader::cardreader() {

   // No sets are loaded yet
   SETLOADED   = FALSE;
   READEREMPTY = TRUE;
}


cardreader::~cardreader() {

   unload_cardset();
}


// This routine simulates a deck of cards being loaded into the reader.
// It takes the name of the cardset file, and opens it, so that it is
// ready to read.
boolean   cardreader::load_cardset(char* setname) {
   boolean   success = FALSE;


   // Check whether a card set is already loaded. If it is, unload it.
   if (SETLOADED) {
      unload_cardset();
   }

   strcpy(cardsetname, setname);
   tf.store(cardsetname);
   success = tf.openfile_read();

   // If the cardset was successfully loaded, return TRUE
   if (success) {
      SETLOADED   = TRUE;
      READEREMPTY = FALSE;
      return TRUE;
   }
   else
      return FALSE;
}



void  cardreader::unload_cardset() {

   if (SETLOADED)
      tf.closefile();

}


// This routine simulates reading a crad from the card reader
char* cardreader::get_card() {
   long  length;


   // Get a card. If get_card is called, and a card set is not presently
   // loaded, or the reader is empty, return an empty string
   if ((!SETLOADED) || (READEREMPTY)){
      buffer[0] = '\0';
      return buffer;
   }
   else
      strcpy(buffer, tf.get_line());
      buffer[40] = '\0';

   // Check if the text file is empty. If it is, then no "cards" remain in
   // the card reader
   if (tf.is_end())
      READEREMPTY = TRUE;

   return buffer;
}
