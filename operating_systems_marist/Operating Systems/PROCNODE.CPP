#include <stdlib.h>
#include "procnode.h"


processnode::processnode(int id)
{
   pid = id;            // set the process id;
   next = NULL;
   inputdata = new queue;
   outputdata = new queue;
   programdata = new proglist;
   statdata = new queue;
   swapdata = new swaplist;
}

processnode::~processnode()
{
   delete inputdata;
   delete outputdata;
   delete programdata;
   delete swapdata;
}

