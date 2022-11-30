//FILE FILESYS.H

/*This object will manages all data that needs to be spooled to the drum.
  All manipulation of the drum will be controlled by this class.
  Calls to storepage() will store program, data, and
  statistics pages on the drum and maintain a group of queues to
  manage them. Calls to getpage() will retrieve the data from the drum
  in FIFO order, grouped by the type of data. For example,
  subsequent calls to getpage() using INPUT as the data type
  will retrieve all of the input data pages in FIFO order.
  The drum tracks will be automaically deallocated as pages
  are retrieved. The exception to this are swapped pages, which are
  retrieved by random access using the page number as the key */

#include "drum.h"
#include "proclist.h"

#ifndef FILESYS_H
#define FILESYS_H

enum errortype {INVALIDTYPE,NOTRACKSALLOCATED,NOINPUTDATA,
                NOOUTPUTDATA,NOPROGRAMDATA,NOSWAPDATA,NOSTATDATA};

class filesystem {
   private:
     drum d;           // the drum  - 100 tracks, 40 bytes each
     processlist p;    // list of all processes that own tracks on the drum
   public:
      long storepage(byte *page, pagetype t, long pid);
      long getpage(byte *page, pagetype t, long pid);
      long storeswappage(byte *page, long pagenum, long pid );
      long getswappage(byte *page, long pagenum, long pid);
      long getprogpage(byte *page, long cardnum, long pid);
      long swap2output(long pagenum, long pid);
      long prog2output(long pagenum, long pid);
      long progexist(long pagenum, long pid);
      long outputexist(long pid);
      long killswappage(long pagenum, long pid);
      long cleardrum(long pid);
};
#endif

