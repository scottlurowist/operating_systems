#ifndef PROCLIST_H
#define PROCLIST_H

#include "procnode.h"

//This object maintains a list of processes that
//currently have data stored on the drum. It
//manages the drum based on the process id's of
//all active processes, and also the type of data
//that is being stored. All data is retrieved from
//the filesystem in FIFO order with the execption of
//swapped data, which is retrieved based on the
//page number of the data that was swapped.
class processlist {
     private:
         processnode *head;
     public:
         processlist();
         int  addinputdata(int pid, int tracknum);
         int  addoutputdata(int pid, int tracknum);
         int  addprogramdata(int pid, int tracknum);
         int  addstatdata(int pid, int tracknum);
         void addprocess(int pid);
         int  addswapdata(int pid, int tracknum, int pagenum);
         int  removeprocess(int pid);
         int  getinputdata(int pid);
         int  getoutputdata(int pid);
	 int  getoutputdata2(int pid);
	 int  getstatdata(int pid);
         int  getswapdata(int pid, int pagenum);
         int  getnextswapdata(int pid);
	 int  getprogdata(int pid, int pagenum);
	 int  getprogdata2(int pid, int pagenum);
	 int  getnextprogpage(int pid);
	 boolean queryprocess(int pid);
};
#endif

