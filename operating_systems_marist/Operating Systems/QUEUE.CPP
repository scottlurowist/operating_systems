//QUEUE.CPP - this file implements the methods of the queue class, along
//            with the default errorhandler.
//
//Dominic Bruen 10/6/96

#include <iostream.h>
#include <stdlib.h> /*exit*/
#include "queue.h"

queuenode::queuenode(long v)
{
 data=v;
}
void queuenode::put_next(node* n)
{
 next = n;
}
node* queuenode::get_next()
{
 return(next);
}
long queuenode::get_info()
{
 return(data);
}

void fserr(queue_error e)
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

queue::queue()
{
    head = new queue_header;   //create the queue
    head->next = NULL;
    head->error = fserr;         //use default errorhandler
}

queue::queue(errorhandler clienterr)
{
   head = new queue_header;   //create the queue
   head->next = NULL;
   head->error = clienterr;   //use the client's errorhandler
}

queue::~queue()
{
    delete head;              //delete the header node
}

boolean queue::empty()
{
    if(head->next==NULL)      //see if header node points to anything
     return TRUE;      //return TRUE if no queuenodes exist
    else
     return FALSE;
}


void queue::insert(long info)
{
    queuenode *temp, *cursor;

    cursor = (queuenode*)head->next;   //point cursor to the
    temp = new queuenode(info);        //first node in queue
					      //and create a new node
    temp->next = NULL;

    if(!cursor)         //if this id the first node, link it to the head
	head->put_next(temp);
    else
    {       //find the end of the queue, and link the new node to the end
      while(cursor->next)
      cursor = (queuenode*)cursor->get_next();

      cursor->put_next(temp);
    }
}

long queue::remove()
{
    queuenode* temp;
    long info;

    if(!head->next)     //check for an empty queue
    {
       head->error(UNDERFLOW);  //raise exception if the queue is empty
       return(-1);
    }
    temp = (queuenode*)head->next;  //get the fist node in the queue
    info = temp->get_info();        //get the data from the node
    head->next = (queue_header*)temp->next; //unlink the first node
    delete temp;                            //delete it from memory
    return(info);
}

long queue::length()
{
    long counter = 0;
    //point a cursor at the first node in the queue
    queuenode* cursor = (queuenode*)head->next;

   while(cursor) //iterate through the whole queue, counting the nodes
   {
      counter++;
      cursor = (queuenode*)cursor->next;
   }
   return counter;
}

long queue::front()
{
    queuenode* temp;
    long data;

    if(!head->next) //raise exception if the queue is empty
    {
       head->error(EMPTYQUEUE);
       return(-1);
    }
    //find the first node in the queue and send its data back to caller
    temp = (queuenode*)head->next;
    data = temp->get_info();
    return(data);
}

long queue::dissolve()
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

void queue::see()
{
   queuenode* temp;
   queuenode* cursor;
   long info;

   cursor = (queuenode*)head->next;

   while(cursor) {
       temp = cursor;
       info = temp->get_info();
       cout << info << " ";
       cursor = (queuenode*)cursor->next;
   }
   cout << "\n";
}





