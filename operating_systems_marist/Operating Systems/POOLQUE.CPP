//poolqueue.CPP - this file implements the methods of the poolqueue class, along
//            with the default errorhandler.
//
//Dominic Bruen 10/6/96

#include <iostream.h>
#include <stdlib.h> /*exit*/
#include "poolque.h"

void err(queue_error e)
{
    switch(e) {
      case NONEXISTANCE:
        //cerr<<"\nNO QUEUE EXISTS!";
        break;
      case UNDERFLOW:
        //cerr<<"\nUNDERFLOW!";
        break;
      case EMPTYQUEUE:
        //cerr<<"\nQUEUE IS EMPTY!\n";
        break;
      case NONEMPTYQUEUE:
        //cerr<<"\nQUEUE IS NOT EMPTY!\n";
        break;
      default:
        //cerr<<"\nUNKOWN ERROR!\n";
      break;
      }
//exit(1);
}

poolqueue::poolqueue()
{
    head = new poolqueue_header;   //create the queue
    head->next = NULL;
    head->error = err;         //use default errorhandler
    head->count = 0;           // number of nodes in the queue
}

poolqueue::poolqueue(errorhandler clienterr)
{
   head = new poolqueue_header;   //create the queue
   head->next = NULL;
   head->error = clienterr;   //use the client's errorhandler
   head->count = 0;           // number of nodes in the queue

}

poolqueue::~poolqueue()
{
    delete head;              //delete the header node
}

boolean poolqueue::empty()
{
    if(head->next==NULL)      //see if header node points to anything
     return TRUE;      //return TRUE if no queuenodes exist
    else
     return FALSE;
}


void poolqueue::insert(ABuffer *b)
{
    poolqueuenode* cursor = (poolqueuenode*)head->next;   //point cursor to the
    poolqueuenode* temp = new poolqueuenode(b);        //first node in queue
                                              //and create a new node
    temp->next = NULL;

    if(!cursor)         //if this id the first node, link it to the head
        head->put_next(temp);
    else
    {       //find the end of the queue, and link the new node to the end
      while(cursor->next)
      cursor = (poolqueuenode*)cursor->get_next();

      cursor->put_next(temp);
    }
    head->inc_count(); //keep count of the number of nodes in the queue
}

ABuffer* poolqueue::remove()
{
    poolqueuenode* temp;
    ABuffer *b;

    if(!head->next)  {    //check for an empty queue
       head->error(UNDERFLOW);  //raise exception if the queue is empty
       return(NULL);
    }
    temp = (poolqueuenode*)head->next;  //get the fist node in the queue
    b = temp->get_info();        //get the data from the node
    head->next = (poolqueue_header*)temp->next; //unlink the first node
    //delete temp;                            //delete it from memory
    head->dec_count(); //keep count of the number of nodes in the queue
    return(b);
}

long poolqueue::length()
{
    long counter = 0;
    //point a cursor at the first node in the queue
    poolqueuenode* cursor = (poolqueuenode*)head->next;

   while(cursor) //iterate through the whole queue, counting the nodes
   {
      counter++;
      cursor = (poolqueuenode*)cursor->next;
   }
   return counter;
}

ABuffer* poolqueue::front()
{
    poolqueuenode *temp;
    ABuffer *b;

    if(!head->next) //raise exception if the queue is empty
    {
       head->error(EMPTYQUEUE);
       return(NULL);
    }
    //find the first node in the queue and send its data back to caller
    temp = (poolqueuenode*)head->next;
    b = temp->get_info();
    return(b);
}

long poolqueue::dissolve()
{
   if(head->next)    //raise exception if the queue isn't empty
   {
      head->error(NONEMPTYQUEUE);
      return(-1);
   }
   else
      delete this;  //delete this instance of a queue object
   return 0;
}

void poolqueue::see()
{
//   queuenode* temp;
//   queuenode* cursor;
//   int info;

//   cursor = (queuenode*)head->next;

//   while(cursor) {
//       temp = cursor;
//       info = temp->get_info();
//       cout << info << " ";
//       cursor = (queuenode*)cursor->next;
//   }
}





