// FILE SWAPLIST.CPP
#include "swaplist.h"

//stores a page of swap data onto the drum
int swaplist::storeswappage(int tracknum, int pagenum)
{
    swapnode* cursor = head;                          //point cursor to the
    swapnode* temp = new swapnode(tracknum,pagenum);  //first node in the list
                                             //and create a new node
    temp->putnext(0);

    if(!cursor)    //if this id the first node, make it the head of the list
        head = temp;
    else
    {    //find the end of the list, and attach the new node
      while(cursor->next)
         cursor = cursor->getnext();

      cursor->putnext(temp);
    }
    return(0);
}

//gets the location on the drum of a page for a process
//that has previously been swapped to the drum.
//The return value is the track number for the page
//specified as the input parameter
int swaplist::getswappage(int pagenum)
{
    swapnode *cursor, *prev;
    int tracknumber;

    prev = head;

    if(!head)                //check for an empty list
           return(-1);
    if(head->GetPageNum() == pagenum) {
       tracknumber = head->GetTrack();
       //delete the page from the list
       head = head->next;
       delete prev;
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->GetPageNum()==pagenum) {
            tracknumber = cursor->GetTrack();
            //now take this page off the list
            prev->next = cursor->next;
            delete cursor;
            return(tracknumber);
       }
       prev = cursor;
       cursor = cursor->next;
    }//end while

    return(-1);   //page not found
}

//returns the next swap page track number on the list
int swaplist::getnextswappage(void)
{
    swapnode *cursor, *prev;
    int tracknumber;

    prev = head;

    if(!head)                //check for an empty list
           return(-1);
    tracknumber = head->GetTrack();
    //delete the page from the list
    head = head->next;
    delete prev;
    return(tracknumber);
}
