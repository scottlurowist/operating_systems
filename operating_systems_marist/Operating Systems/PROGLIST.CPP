// FILE PROGLIST.CPP
#include "proglist.h"

//stores a page of program data onto the drum
int proglist::storeprogpage(int tracknum)
{
    int cardnum = 0;

    prognode* cursor = head;                          //point cursor to the
    prognode* temp = new prognode(tracknum,cardnum);  //first node in the list
                                             //and create a new node
    temp->putnext(0);

    if(!cursor)    //if this id the first node, make it the head of the list
        head = temp;
    else
    {    //find the end of the list, and attach the new node
      cardnum++;
      while(cursor->next) {
          cursor = cursor->getnext();
          cardnum++;
      }
      temp->SetCardNum(cardnum);
      cursor->putnext(temp);
    }
    return(0);
}

//gets the location on the drum of a page
//of program data for a process.
//The return value is the track number for the page
//specified as the input parameter. The card# is removed from
//the list
int proglist::getprogpage(int cardnum)
{
    prognode *cursor, *prev;
    int tracknumber;

    prev = head;

    if(!head)                //check for an empty list
           return(-1);
    if(head->GetCardNum() == cardnum) {
       tracknumber = head->GetTrack();
       //delete the page from the list
       head = head->next;
       delete prev;
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->GetCardNum()==cardnum) {
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

//gets the location on the drum of a page
//of program data for a process.
//The return value is the track number for the page
//specified as the input parameter. The card# is left
//in the list
int proglist::getprogpage2(int cardnum)
{
    prognode *cursor;
    int tracknumber;

    if(!head)                //check for an empty list
           return(-1);
    if(head->GetCardNum() == cardnum) {
       tracknumber = head->GetTrack();
       return(tracknumber);
    }
    cursor = head->next;//traverse the list until the process is found
    while(cursor) {
       if(cursor->GetCardNum()==cardnum) {
            tracknumber = cursor->GetTrack();
            return(tracknumber);
       }
       cursor = cursor->next;
    }//end while

    return(-1);   //page not found
}

//returns the track number of the next program card
int proglist::getnextprogpage(void)
{
    prognode *cursor, *prev;
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
