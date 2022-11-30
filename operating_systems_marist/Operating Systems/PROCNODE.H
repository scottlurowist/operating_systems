#ifndef PROCESSNODE_H
#define PROCESSNODE_H

#include "queue.h"
#include "swaplist.h"
#include "proglist.h"

class processnode {
  friend class processlist;
    private:
        int pid;             // the ID number of the process being maintained
        processnode *next;   // pointer to the next process node in the list
        queue *inputdata;    // queue for input data card track numbers
        queue *outputdata;   // queue for output data card track numbers
	proglist *programdata;  // list for program card track numbers
	queue *statdata;     // queue for program card track numbers
	swaplist *swapdata;  // list for swapped page's track numbers
			     //    and page numbers
    public:
	processnode(int pid);
        ~processnode();
        void put_next(processnode *n) { next = n; }   // link another node to this one
        processnode* get_next()  { return next; }

};

#endif
