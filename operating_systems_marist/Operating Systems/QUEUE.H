//QUEUE.H - declaration for a class which implents a queue along with the
//          methods that manage the queue.
//
#ifndef QUEUE_H
#define QUEUE_H

#include "globals.h"

//class declaration for a generic node
class node {
   friend class queue;
   protected:
      node* next;
};

//class declaration for a header node
//enum queue_error {UNDERFLOW,EMPTYQUEUE,NONEXISTANCE,NONEMPTYQUEUE};
typedef void (*errorhandler) (queue_error);
class queue_header : private node {
  friend class queue;
   private:
        errorhandler error;
    public:
        node* get_next() { return(next); };
        void  put_next(node* n) { next = n; }

};

//class declaration of a queue node
class queuenode : private node {
  friend class queue;
  private:
    long data;

  public:
    queuenode(long v);
    void put_next(node* n);
    node* get_next();
    long get_info();
};


class queue {
   private:
     queue_header *head;
   public:
     queue();                      //default constructor
     queue(errorhandler clienterr);//client supplied errorhandler
     ~queue();
     void insert(long data); //adds an item to the queue
     long remove();     //removes item from front of queue
     long length();           //returns the current length of queue
     boolean empty();        //returns TRUE if the queue is empty
     long front();      //returns the item at the front of the queue
     long dissolve();   //destoys the queue. queue MUST be empty
     void see();       //displays contents of queue on the CRT
};
#endif
