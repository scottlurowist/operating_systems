#include <stdlib.h>
#include "proclist.h"


processlist::processlist()
{
     head= NULL;
}

/******************************************************************************
FUNCTION: addprocess()
PURPOSE:  Adds a process to the linked list of process nodes.
          Each node contains four queues which are used to keep track
          of which drum tracks have been allocated to this process,
          what order they were allocated in, and what type of data is
          stored on the track.
******************************************************************************/
void processlist::addprocess(int pid)
{
    processnode* cursor = head;                //point cursor to the
    processnode* temp = new processnode(pid);  //first node in the list
                                               //and create a new node
    temp->next = NULL;

    if(!cursor)         //if this id the first node, make it the head of the list
        head = temp;
    else
    {       //find the end of the list, and attach the new node
      while(cursor->next)
         cursor = cursor->get_next();

      cursor->put_next(temp);
    }
}
/******************************************************************************
FUNCTION: removeprocess()
PURPOSE:  Removes a process node from the linked list of process nodes.
	  It is up to the caller to deallocate the drum tracks that were
          allocated to the removed process prior to its removal.
RETURNS:  0 if successful
          1 if process is not found on the list
******************************************************************************/
int processlist::removeprocess(int pid)
{
    processnode *temp, *cursor, *prev;

    if(!head)                //check for an empty list
           return(1);
    if(head->pid == pid) {   //if the process is at the front of the
       temp = head->next;    //list, delete it and reset the head
       delete head;          //to be the next node in the list
       head = temp;
       return(0);
    }
    cursor = prev = head;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            prev->next = cursor->next;
            delete cursor;
            return(0);
       }
       prev = cursor;
       cursor = cursor->next;
    }//end while

    return(1);   //process not found
}

/******************************************************************************
FUNCTION: addinputdata()
PURPOSE:  Adds the track number where input data has been allocated for
          this process on the drum to the input data queue of the process's
          entry in the linked list.
RETURNS:  0 if successful
          1 if process id is not found on the list
******************************************************************************/
int processlist::addinputdata(int pid, int tracknum)
{
    processnode *cursor;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       head->inputdata->insert(tracknum);
       return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            cursor->inputdata->insert(tracknum);
            return(0);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: addoutputdata()
PURPOSE:  Adds the track number where output data has been allocated for
          this process on the drum to the output data queue of the process's
          entry in the linked list.
RETURNS:  0 if successful
          1 if process id is not found on the list
******************************************************************************/
int processlist::addoutputdata(int pid, int tracknum)
{
    processnode *cursor;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       head->outputdata->insert(tracknum);
       return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            cursor->outputdata->insert(tracknum);
            return(0);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: addstatdata()
PURPOSE:  Adds the track number where stat data has been allocated for
          this process on the drum to the stat data queue of the process's
          entry in the linked list.
RETURNS:  0 if successful
          1 if process id is not found on the list
******************************************************************************/
int processlist::addstatdata(int pid, int tracknum)
{
    processnode *cursor;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       head->statdata->insert(tracknum);
       return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    cursor->statdata->insert(tracknum);
	    return(0);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: addprogramdata()
PURPOSE:  Adds the track number where program data has been allocated for
	  this process on the drum to the program data queue of the process's
	  entry in the linked list.
RETURNS:  0 if successful
	  1 if process id is not found on the list
******************************************************************************/
int processlist::addprogramdata(int pid, int tracknum)
{

    processnode *cursor;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       head->programdata->storeprogpage(tracknum);
       return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    cursor->programdata->storeprogpage(tracknum);
	    return(0);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: addswapdata()
PURPOSE:  Adds the track number where swap data has been allocated for
	  this process on the drum to the swap data queue of the process's
	  entry in the linked list.
RETURNS:  0 if successful
	  1 if process id is not found on the list
******************************************************************************/
int processlist::addswapdata(int pid, int tracknum, int pagenum)
{
    processnode *cursor;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       head->swapdata->storeswappage(tracknum,pagenum);
       return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    cursor->swapdata->storeswappage(tracknum,pagenum);
	    return(0);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getinputdata()
PURPOSE:  Gets the track number where input data is stored on the drum
	  from the front of the input data queue in the list entry for
          this process. The track number will be removed from the queue
          and the track number returned to the caller.
RETURNS:  Tracknumber if successful
          -1 if process id is not found on the list
             or if there are no tracks in the queue
******************************************************************************/
int processlist::getinputdata(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       tracknumber = head->inputdata->remove();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            tracknumber = cursor->inputdata->remove();
            return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getoutputdata()
PURPOSE:  Gets the track number where output data is stored on the drum
          from the front of the output data queue in the list entry for
          this process. The track number will be removed from the queue
          and the track number returned to the caller.
RETURNS:  0 if successful
          -1 if process id is not found on the list
             or if there are no tracks in the queue
******************************************************************************/
int processlist::getoutputdata(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       tracknumber = head->outputdata->remove();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    tracknumber = cursor->outputdata->remove();
	    return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getoutputdata2()
PURPOSE:  Gets the track number where output data is stored on the drum
	  from the front of the output data queue in the list entry for
	  this process. The track number WON'T be removed from the queue
	  and the track number returned to the caller.
RETURNS:  0 if successful
	  -1 if process id is not found on the list
	     or if there are no tracks in the queue
******************************************************************************/
int processlist::getoutputdata2(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       tracknumber = head->outputdata->front();
       if(tracknumber>=0)
	  return(1);
       else
	  return(0);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    tracknumber = cursor->outputdata->front();
	       if(tracknumber>=0)
		  return(1);
	       else
		  return(0);
	}
	cursor = cursor->next;
    }//end while

    return(0);   //process not found

}
/******************************************************************************
FUNCTION: getstatdata()
PURPOSE:  Gets the track number where stat data is stored on the drum
	  from the front of the stat data queue in the list entry for
          this process. The track number will be removed from the queue
          and the track number returned to the caller.
RETURNS:  0 if successful
          -1 if process id is not found on the list
             or if there are no tracks in the queue
******************************************************************************/
int processlist::getstatdata(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       tracknumber = head->statdata->remove();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            tracknumber = cursor->statdata->remove();
            return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getprogdata()
PURPOSE:  Gets the track number where program data is stored on the drum
          from the front of the program data queue in the list entry for
	  this process. The track number will be removed from the queue
          and the track number returned to the caller.
RETURNS:  0 if successful
	  -1 if process id is not found on the list
	     or if there are no tracks in the queue
******************************************************************************/
int processlist::getprogdata(int pid, int cardnum)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       tracknumber = head->programdata->getprogpage(cardnum);
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    tracknumber = cursor->programdata->getprogpage(cardnum);
	    return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

   return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getprogdata2()
PURPOSE:  Gets the track number where program data is stored on the drum
          from the front of the program data queue in the list entry for
	  this process. The track number won't be removed from the queue
          and the track number will be returned to the caller.
RETURNS:  0 if successful
	  -1 if process id is not found on the list
	     or if there are no tracks in the queue
******************************************************************************/
int processlist::getprogdata2(int pid, int cardnum)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       tracknumber = head->programdata->getprogpage2(cardnum);
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    tracknumber = cursor->programdata->getprogpage2(cardnum);
	    return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

   return(-1);   //process not found
}


int processlist::getnextprogpage(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       tracknumber = head->programdata->getnextprogpage();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            tracknumber = cursor->programdata->getnextprogpage();
            return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}

/******************************************************************************
FUNCTION: getswapdata()
PURPOSE:  Gets the track number where swap data is stored on the drum
          from the front of the swap data queue in the list entry for
          this process. The track number will be removed from the queue
          and the track number returned to the caller.
RETURNS:  0 if successful
          -1 if process id is not found on the list
	     or if there are no tracks in the queue
******************************************************************************/
int processlist::getswapdata(int pid, int pagenum)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
	   return(-1);
    if(head->pid == pid) {
       tracknumber = head->swapdata->getswappage(pagenum);
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
	    tracknumber = cursor->swapdata->getswappage(pagenum);
	    return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}
/******************************************************************************
FUNCTION: getnextswapdata()
PURPOSE:  Gets the track number where swap data is stored on the drum
          from the front of the swap data list in the list entry for
          this process. The track number will be removed from the list
          and the track number returned to the caller.
RETURNS:  0 if successful
          -1 if process id is not found on the list
             or if there are no tracks in the list
******************************************************************************/

int processlist::getnextswapdata(int pid)
{
    processnode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
           return(-1);
    if(head->pid == pid) {
       tracknumber = head->swapdata->getnextswappage();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->pid==pid) {
            tracknumber = cursor->swapdata->getnextswappage();
            return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //process not found
}

/******************************************************************************
FUNCTION: queryprocess()
PURPOSE:  Checks the process list to help the caller determine if the
          process id passed in as a parameter has an existing node
          on te list.
RETURNS:  TRUE if node exists on the list fr tis process
          FALSE if no node exists on the lst for the process
******************************************************************************/
boolean processlist::queryprocess(int pid)
{
    processnode *cursor;

    if(!head)                //check for an empty list
       return(FALSE);
    if(head->pid == pid)
       return(TRUE);

    cursor = head->next; //traverse the list
    while(cursor) {
       if(cursor->pid==pid)
            return(TRUE);
       cursor = cursor->next;
    }//end while

    return(FALSE);   //process not found
}
