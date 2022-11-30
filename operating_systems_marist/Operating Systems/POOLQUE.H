//QUEUE.H - declaration for a class which implents a queue along with the
//          methods that manage the queue.
//
//Dominic Bruen 10/6/97

#ifndef POOLQUE_H
#define POOLQUE_H

#include "globals.h"
#include "buffer.h"

//class declaration for a generic node
class poolnode {
   friend class poolqueue;
   protected:
      poolnode* next;
};

//class declaration for a header node
typedef void (*errorhandler) (queue_error);
class poolqueue_header : private poolnode {
  friend class poolqueue;
    private:
	errorhandler error;
	int count;   //number of nodes in queue
    public:
	poolnode* get_next() { return(next); };
	void  put_next(poolnode* n) { next = n; }
	void inc_count() { count++; };
	void dec_count() { count--; };
	int  get_count() { return count; };

};

//class declaration of a queue node
class poolqueuenode : private poolnode {
  friend class poolqueue;
  private:
    ABuffer *data;

  public:
    poolqueuenode(ABuffer *v)       { data=v; }
    void put_next(poolnode* n) { next = n; }
    poolnode* get_next()       { return(next); }
    ABuffer* get_info()         { return(data); }
};


class poolqueue {
   private:
     poolqueue_header *head;
   public:
     poolqueue();                      //default constructor
     poolqueue(errorhandler clienterr);//client supplied errorhandler
     ~poolqueue();
     void insert(ABuffer *b); //adds an item to the queue
     ABuffer* remove();     //removes item from front of queue
     long length();           //returns the current length of queue
     boolean empty();        //returns TRUE if the queue is empty
     ABuffer* front();      //returns the item at the front of the queue
     long dissolve();   //destoys the queue. queue MUST be empty
     void see();       //displays contents of queue on the CRT
};
#endif
