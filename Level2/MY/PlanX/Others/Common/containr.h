/*****
*
*   Module:     UnsortedContainer
*               SortedContainer
*   Author:     Lincoln Fellingham
*   Date:       Feb 95
*   Purpose:    Creates two standardized container classes based on
*               Borlands template based BIDS library, which provides
*               growable, dynamic arrays with common interface
*               functions.
*
*               These classes were originally written to overcome the
*               changes in naming convention between the Borland 3.x
*               BIDS library and the Borland 4.x version. Code can be
*               recompiled without changes with either version of the
*               compiler. An added advantage of this is that additional
*               wrappers can be provided for any template based
*               container set, allowing portability of ARC code between
*               various compilers and libraries, the STL in particular.
*
*****/

#ifndef CONTAINERS_H
#define CONTAINERS_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "arrays.h"

template <class T> 
class  UnsortedContainer : public TIArrayAsVector<T>
{
public:

    // default array size 16, subscript 0, delta (inc) 16
    UnsortedContainer (int init = 16, int p_ndx = 0, int delta = 16)
        : TIArrayAsVector<T> (init, p_ndx, delta) { ownsElements(0); }
    virtual ~UnsortedContainer () {};

    // returns number of items in the Container
    int getCount (void) const { return (int)GetItemsInContainer(); }

    // adds new p_item to the Container if not yet added
    void addItem (T *p_item) { Add (p_item); }

    // sets container ownership
    void ownsElements (int x) { OwnsElements(x); }

    // returns p_ndx of passed p_item in Container, INT_MAX if not found
    int findItem (const T *p_item) const { return Find (p_item); }

    // returns the p_item found at the passed p_ndx
    T *getItem (int p_ndx) const { return (*this)[p_ndx]; }

    // removes p_item at the passed p_ndx
    void removeItem (int p_ndx) { Detach (p_ndx, TShouldDelete::NoDelete); }

    // removes and deallocates p_item at the passed p_ndx
    void deleteItem (int p_ndx) { Detach (p_ndx, TShouldDelete::Delete); }

    // removes all items from the container
    void clear (void) { Flush(); }
};

template <class T> class  SortedContainer : public TISArrayAsVector<T>
{
public:

    // default array size 16, subscript 0, delta (inc) 16
    SortedContainer (int init = 16, int p_ndx = 0, int delta = 16)
        : TISArrayAsVector<T> (init, p_ndx, delta) { ownsElements(0); }
    virtual ~SortedContainer () {};

    // returns number of items in the Container
    int getCount (void) const { return GetItemsInContainer(); }

    // adds new p_item to the Container if not yet added
    void addItem (T *p_item) { Add (p_item); }

    // sets container ownership
    void ownsElements (int x) { OwnsElements(x); }

    // returns p_ndx of passed p_item in Container, INT_MAX if not found
    int findItem (const T *p_item) const { return Find (p_item); }
#if 0
    // wrapper for the container function FirstThat
    T *firstThat (CondFunc cf, void *args) const { return FirstThat (cf, args); }

    // wrapper for the container function LastThat
    T *lastThat (CondFunc cf, void *args) const { return LastThat (cf, args); }
#endif
    // returns the p_item found at the passed p_ndx
    T *getItem (int p_ndx) const { return (*this)[p_ndx]; }

    // removes p_item at the passed p_ndx
    void removeItem (int p_ndx) { Detach (p_ndx, TShouldDelete::NoDelete); }
    void removeItem (T* p_ndx) { Detach (p_ndx, TShouldDelete::NoDelete); }

    // removes and deallocates p_item at the passed p_ndx
    void deleteItem (int p_ndx) { Detach (p_ndx, TShouldDelete::Delete); }

    // removes all items from the container
    void clear (void) { Flush(); }
};

#endif // ifdef CONTAINERS_H
