#ifndef ARRAY_BASED_QUEUE_H
#define ARRAY_BASED_QUEUE_H

#include "engine\mobile.h"

#define HEAD 0

class AccessibleQueue : public MobileElementList
{
public:
    AccessibleQueue () {};
    virtual ~AccessibleQueue () {};

    // This function removes and returns head of queue.
    MobileElement *removeHead (void);

    // This function adds Object to the tail of Queue, a mobile element list.
    void addToTail (MobileElement *tail) { addItem (tail); };

    MobileElement *getHead (void) const;
    MobileElement *getTail (void) const;
};

#endif
