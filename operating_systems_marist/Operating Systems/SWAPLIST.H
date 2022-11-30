//FILE SWAPLIST.H

//This object is a node that contains information
//about a program page that has been swapped to the
//drum during paging.
class swapnode {
friend class swaplist;
   private:
     int tracknum;
     int pagenum;
     swapnode *next;
   public:
     swapnode(int t, int p) { tracknum=t; pagenum=p; };
     ~swapnode() {};
     void SetTrackNum(int t) { tracknum = t; };
     int GetTrack(void) { return(tracknum); };
     void SetPageNum(int p) { pagenum = p; };
     int GetPageNum(void) { return(pagenum); };
     void putnext(swapnode *n) { next = n; };
     swapnode* getnext(void) { return(next); };
};

//This object implements a linked list to keep
//track of program pages that are swapped to the drum
//as part of the paging algorithm.
class swaplist {
   private:
     swapnode *head;
   public:
     swaplist() { head=0; };
     ~swaplist() {};
     int storeswappage(int tracknum, int pagenum);
     int getswappage(int pagenum);
     int getnextswappage(void);
};

