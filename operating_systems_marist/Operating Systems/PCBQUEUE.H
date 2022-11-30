//PCBqueue.H - declaration for a class which implents a queue along with the
//          methods that manage the queue.
//
//Dominic Bruen 10/6/97

#ifndef PCBQUEUE_H
#define PCBQUEUE_H

#include "globals.h"
#include "pcb.h"

//class declaration for a generic node
class PCBnode {
   friend class PCBqueue;
   protected:
      PCBnode* next;
};

//class declaration for a header node
//enum queue_error {UNDERFLOW,EMPTYQUEUE,NONEXISTANCE,NONEMPTYQUEUE};
typedef void (*errorhandler) (queue_error);
class PCBqueue_header : private PCBnode {
  friend class PCBqueue;
    private:
        errorhandler error;
    public:
        PCBnode* get_next() { return(next); };
        void  put_next(PCBnode* n) { next = n; }

};

//class declaration of a queue node
class PCBqueuenode : private PCBnode {
  friend class PCBqueue;
  private:
    PCB *data;

  public:
    PCBqueuenode(PCB *v);
    void put_next(PCBnode* n);
    PCBnode* get_next();
    PCB* get_info();
};


class PCBqueue {
   private:
     PCBqueue_header *head;
   public:
     PCBqueue();                      //default constructor
     PCBqueue(errorhandler clienterr);//client supplied errorhandler
     ~PCBqueue();
     void insert(PCB* data); //adds an item to the queue
     PCB* remove();     //removes item from front of queue
     long length();           //returns the current length of queue
     boolean empty();        //returns TRUE if the queue is empty
     PCB* front();      //returns the item at the front of the queue
     long dissolve();   //destoys the queue. queue MUST be empty
     char* see();       //displays contents of queue on the CRT
};
#endif
