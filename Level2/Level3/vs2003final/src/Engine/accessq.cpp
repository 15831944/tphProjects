#include "stdafx.h"
#include "engine\accessq.h"

//It gets an item from the mobile element list which 
//is an unsorted container. 
MobileElement *AccessibleQueue::removeHead (void)
{
    MobileElement *obj = getItem (HEAD);
    removeItem (HEAD);
    return obj;
}

//The first item in queue is obtained but not removed if the queue is not empty;
//otherwise NULL is returned.
MobileElement *AccessibleQueue::getHead (void) const
{
    return (getCount())? getItem (HEAD): NULL;
}

//The last item in queue is obtained but not removed if the queue is not empty;
//otherwise NULL is returned.
MobileElement *AccessibleQueue::getTail (void) const
{
    return (getCount())? getItem (getCount()-1): NULL;
}

