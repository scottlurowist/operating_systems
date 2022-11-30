//FILE PROGLIST.H

//This object is a node that contains information
//about a program cards that were read from
//the card reader and stored on the drum.
class prognode {
friend class proglist;
   private:
     int tracknum;
     int cardnum;
     prognode *next;
   public:
     prognode(int t, int p) { tracknum=t; cardnum=p; };
     ~prognode() {};
     void SetTrackNum(int t) { tracknum = t; };
     int GetTrack(void) { return(tracknum); };
     void SetCardNum(int p) { cardnum = p; };
     int GetCardNum(void) { return(cardnum); };
     void putnext(prognode *n) { next = n; };
     prognode* getnext(void) { return(next); };
};

//This object implements a linked list to keep
//track of program pages that are stored on the drum
//after input spooling
class proglist {
   private:
     prognode *head;
   public:
     proglist() { head=0; };
     ~proglist() {};
     int storeprogpage(int tracknum);
     int getprogpage(int cardnum);
     int getprogpage2(int cardnum);
     int getnextprogpage(void);
};

