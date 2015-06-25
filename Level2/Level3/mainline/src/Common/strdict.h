/*****
*
*   Module:     StringDictionary
*   Author:     Lincoln Fellingham
*   Date:       November 1992
*   Purpose:    Stores a list of read-only strings
*               Limited to 256 Strings
*               Allows storage of strings as a single character index
*               - permanent container: owns elements
*
*   Revision:   November 2, 1993 - Lincoln Fellingham
*               Add delete functions, destructor
*               Format by ANSI standard code rules
*               Increase max string count to 65534 strings
*
*               October 8, 1994 - Lincoln Fellingham
*               Complete revision to utilize Borland BIDS libray
*               Removed all methods except file I/O, addString
*
*               Jan 96 - LF - Added sortable StringList class, and
*               modified StringClass class to do proper allocation and
*               deallocation
*
*****/
#ifndef STRDICT_H
#define STRDICT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
#include "containr.h"
#include "commondll.h"

//*tx #error the file is obsolete

class COMMON_TRANSFER StringClass
{
protected:
    char *string;

public:
    StringClass () { string = NULL; }
    StringClass (const char *p_str) { string = NULL; init (p_str); }
    StringClass (const StringClass& p_str) { string = NULL; init (p_str.string); }
	void operator=(const StringClass& p_str) {string = NULL; init (p_str.string); }
    virtual ~StringClass () { if (string) delete [] string; string = NULL; }

    void init (const char *p_str);
    void clear (void) { if (string) delete [] string; string = NULL; }

    int operator == (const StringClass& p_str) const
        { return !strcmp (getString(), p_str.getString()); }
    int operator < (const StringClass& p_str) const
        { return (strcmp (getString(), p_str.getString()) < 0); }

    virtual const char *getString (void) const { return string; }
};

typedef UnsortedContainer<StringClass> TStringClassIArray;
class COMMON_TRANSFER StringDictionary :public TStringClassIArray
{
public:
    // default array size 16, subscript 1, delta (inc) 16
    StringDictionary () : UnsortedContainer<StringClass>(16, 0, 16)
        { ownsElements(1); }
    virtual ~StringDictionary ();

    // adds newString to the StringDictionary if not yet added
    // returns existing Index if already in dictionary
    int addString (const char *p_str);

    // returns the char * stored in string object at index
    const char *getString (int p_ndx) const;

    // replaces the string at p_ndx with p_str
    void replaceString (int p_ndx, const char *p_str);

    // returns index of string in dictionary, 0 if not found
    int findString (const char *p_str);

    // save all strings in the dictionary to the passed file name
    void storeDictionary (const char *p_filename) const;

    // read all strings from the passed file name into the dictionary
    void retrieveDictionary (const char *p_filename);
};

typedef COMMON_TRANSFER  SortedContainer<StringClass> TStringClassISArray;

class COMMON_TRANSFER StringList :public TStringClassISArray
{
public:
    // default array size 16, subscript 0, delta (inc) 16
    StringList () : SortedContainer<StringClass>(16, 0, 16)
        { ownsElements(1); }
    virtual ~StringList () ;

    // adds newString to the StringList
    void addString (const char *p_str);

    // returns the char * stored in string object at index
    const char *getString (int p_ndx) const;

    // returns index of string in dictionary, INT_MAX if not found
    int findString (const char *p_str);
};

#endif
