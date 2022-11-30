// File printer.cpp
// Scott Lurowist
//
//
//  this program implements a printer simulator class


#include <string.h>
#include "printer.h"


printer::printer() {

   // No "paper" (file) is loaded yet
   PAPERLOADED   = FALSE;

}


printer::~printer() {

   change_paper();
}


// This routine simulates loading paper into a printer.
// It takes the name of the file, and opens it, so that it is
// ready to write to.
boolean   printer::load_paper(char* name) {
   boolean   success = FALSE;

   // Check whether a roll of "paper" is already loaded. If it is, unload it.
   if (PAPERLOADED) {
      change_paper();
   }

   strcpy(filename, name);
   tf.store(filename);
   success = tf.openfile_write();

   // If the cardset was successfully loaded, return TRUE
   if (success) {
      PAPERLOADED = TRUE;
      return TRUE;
   }
   else
      return FALSE;
}



void  printer::change_paper() {

   if (PAPERLOADED)
      tf.closefile();

}


// This routine simulates a line being printed
boolean printer::print(char* line) {
   long   length;
   char  buffer[BUFFERSIZE];

   // Check that "paper" (the output file) is loaded
   if (!PAPERLOADED)
      return FALSE;
   else {
      // Need char array, or buffer[40] ='\0' will generate run-time error
      strcpy(buffer, line);
      buffer[40] = '\0';

      tf.put_line(buffer);
      return TRUE;
   }
}
