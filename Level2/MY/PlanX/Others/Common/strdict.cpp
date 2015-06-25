#include "stdafx.h"
#include <string>
#include "fsstream.h"
#include "strdict.h"
#include "exeption.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "ARCTracker.h"

/*****
*
*   StringClass class
*
*****/

void StringClass::init (const char *p_str)
{
    clear();
    if (p_str)
    {
        string = new char[strlen (p_str) + 1];
        strcpy (string, p_str);
    }
}

/*****
*
*   StringClass Dictionary class
*
*****/

// adds newString to the StringDictionary if not yet added
// returns existing Index if already in dictionary
int StringDictionary::addString (const char *p_str)
{
    StringClass tempStr (p_str);

    // test if newString is already in dictionary
    int index = findItem (&tempStr);
    if (index != INT_MAX)
        return index;

    StringClass *newString = new StringClass (p_str);
    addItem (newString);
    return getCount() - 1;
}

// returns the char * stored in string object at index
const char *StringDictionary::getString (int p_ndx) const
{
    if (empty() || p_ndx >= getCount())
        throw StringError ("Out of bounds in StringDictionary");

    StringClass *aString = getItem (p_ndx);
    return aString->getString();
}

// replaces the string at p_ndx with p_str
void StringDictionary::replaceString (int p_ndx, const char *p_str)
{
    StringClass *aString = getItem (p_ndx);
    if (_stricmp (aString->getString(), p_str))
        aString->init (p_str);
}

// returns index of string in dictionary, 0 if not found
int StringDictionary::findString (const char *p_str)
{
    StringClass findString (p_str);
    int index = findItem (&findString);
    return (index == INT_MAX)? 0: index;
}

// save all strings in the dictionary to the passed file name
void StringDictionary::storeDictionary (const char *p_filename) const
{
	PLACE_METHOD_TRACK_STRING();
    ofsstream dictionaryFile (p_filename);
    int stringCount = getCount();
    for (int i = 0; i < stringCount; i++)
        dictionaryFile << getString(i) << '\n';
    dictionaryFile.close();
}

// read all strings from the passed file name into the dictionary
void StringDictionary::retrieveDictionary (const char *p_filename)
{
    clear();
    ifsstream dictionaryFile (p_filename);
	if (!dictionaryFile)
		return;

    char nextString[256];
    while (!dictionaryFile.eof())
    {
        dictionaryFile.getline (nextString, 256);
        addString (nextString);
    }
    dictionaryFile.close();
}


/*****
*
*   StringClass List class
*
*****/
// adds newString to the StringDictionary if not yet added
// returns existing Index if already in dictionary
void StringList::addString (const char *p_str)
{
//    StringClass *newString = new StringClass (p_str);
//    addItem (newString);
    StringClass str (p_str);

    // test if newString is already in dictionary
    int index = findItem (&str);
    if (index != INT_MAX)
        return;

    StringClass *newString = new StringClass (str);
    addItem (newString);
}

// returns the char * stored in string object at index
const char *StringList::getString (int p_ndx) const
{
    StringClass *aString = getItem (p_ndx);
    return aString->getString();
}

// returns index of string in dictionary, INT_MAX if not found
int StringList::findString (const char *p_str)
{
    StringClass findString (p_str);
    return findItem (&findString);
}

StringDictionary::~StringDictionary (void)
{
	
}

StringList::~StringList (void)
{
    ;
}

