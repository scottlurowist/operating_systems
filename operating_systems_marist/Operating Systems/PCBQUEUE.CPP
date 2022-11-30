
//QUEUE.CPP - this file implements the methods of the queue class, along
//            with the default errorhandler.
//
//Dominic Bruen 10/6/96

#include <iostream.h>
#include <stdlib.h> /*exit*/
#include "pcbqueue.h"



PCBqueuenode::PCBqueuenode(PCB *v)
{
 data=v;
}
void PCBqueuenode::put_next(PCBnode* n)
{
 next = n;
}
PCBnode* PCBqueuenode::get_next()
{
 return(next);
}
PCB* PCBqueuenode::get_info()
{
 return(data);
}


void PCBqerr(queue_error e)
{
    switch(e) {
      case NONEXISTANCE:
        //cerr<<"\nNO QUEUE EXISTS!";
        break;
      case UNDERFLOW:
	//cout<<"\nUNDERFLOW!";
	//exit(1);
	break;
      case EMPTYQUEUE:
	//cout<<"\nQUEUE IS EMPTY!\n";
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

PCBqueue::PCBqueue()
{
    head = new PCBqueue_header;   //create the queue
    head->next = NULL;
    head->error = PCBqerr;         //use default errorhandler
}

PCBqueue::PCBqueue(errorhandler clienterr)
{
   head = new PCBqueue_header;   //create the queue
   head->next = NULL;
   head->error = clienterr;   //use the client's errorhandler
}

PCBqueue::~PCBqueue()
{
    delete head;              //delete the header node
}

boolean PCBqueue::empty()
{
    if(head->next==NULL)      //see if header node points to anything
     return TRUE;      //return TRUE if no queuenodes exist
    else
     return FALSE;
}


void PCBqueue::insert(PCB *info)
{
    PCBqueuenode* cursor = (PCBqueuenode*)head->next;   //point cursor to the
    PCBqueuenode* temp = new PCBqueuenode(info);        //first node in queue
                                              //and create a new node
    temp->next = NULL;

    if(!cursor)         //if this id the first node, link it to the head
        head->put_next(temp);
    else
    {       //find the end of the queue, and link the new node to the end
      while(cursor->next)
      cursor = (PCBqueuenode*)cursor->get_next();

      cursor->put_next(temp);
    }
}

PCB* PCBqueue::remove()
{
    PCBqueuenode* temp;
    PCB* info;

    if(!head->next)     //check for an empty queue
    {
       head->error(UNDERFLOW);  //raise exception if the queue is empty
       return(NULL);
    }
    temp = (PCBqueuenode*)head->next;  //get the fist node in the queue
    info = temp->get_info();        //get the data from the node
    head->next = (PCBqueue_header*)temp->next; //unlink the first node
    delete temp;                            //delete it from memory
    return(info);
}

long PCBqueue::length()
{
    long counter = 0;
    //point a cursor at the first node in the queue
    PCBqueuenode* cursor = (PCBqueuenode*)head->next;

   while(cursor) //iterate through the whole queue, counting the nodes
   {
      counter++;
      cursor = (PCBqueuenode*)cursor->next;
   }
   return counter;
}

PCB* PCBqueue::front()
{
    PCBqueuenode* temp;
    PCB* data;

    if(!head->next) //raise exception if the queue is empty
    {
       head->error(EMPTYQUEUE);
       return(NULL);
    }
    //find the first node in the queue and send its data back to caller
    temp = (PCBqueuenode*)head->next;
    data = temp->get_info();
    return(data);
}

long PCBqueue::dissolve()
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

char* PCBqueue::see()
{
   PCBqueuenode* cursor;
   PCB *data;
   long info;
   char q[30];
   char n[3];
   int i=0;

   PCBqueuenode* temp;

   cursor = (PCBqueuenode*)head->next;

   strcpy(q,"=[");
   while(cursor) {
       i++;
       temp = cursor;
       data = temp->get_info();
       info = data->getPID();
       ltoa(info,n,10);
       strcat(q,n);
       strcat(q," ");
       cursor = (PCBqueuenode*)cursor->next;
   }
   if(i==0)
      strcat(q,"EMPTY");
   strcat(q,"]");
   return(q);

}





