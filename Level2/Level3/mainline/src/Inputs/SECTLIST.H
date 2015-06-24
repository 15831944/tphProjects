/*****
*
*   Module:     Sectors List
*   Author:     Lincoln Fellingham
*   Date:       Jan 96
*   Purpose:    Manages list of sectors, user defined classification for
*               flights (Domestic, International, etc.)
*
*****/

#ifndef SECTOR_LIST_H
#define SECTOR_LIST_H

#include <string.h>
#include "common\containr.h"
#include "common\strdict.h"
#include "common\dataset.h"
#include "inputsdll.h"

class INPUTS_TRANSFER Sector : public StringList
{
protected:
    char code[SECTOR_CODE_LEN];

public:
    Sector () { code[0] = '\0'; }
    virtual ~Sector () {};

    int operator == (const Sector& aSect) const
        { return _strcmpi (code, aSect.code) == 0; }
    int operator < (const Sector& aSect) const 
        { return _strcmpi (code, aSect.code) < 0; }

    void getCode (char *p_str) const { strcpy (p_str, code); }
    void setCode (const char *p_str) { strcpy (code, p_str); }

    virtual int contains (const char *p_airport) const;
};

class INPUTS_TRANSFER SectorList : public SortedContainer<Sector>, public DataSet
{
public:
    SectorList() : SortedContainer<Sector> (4, 0, 4),
        DataSet (SectorFile) { ownsElements(1); }
    virtual ~SectorList() {};

    virtual void clear (void) { SortedContainer<Sector>::clear(); }

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const { return "Sector file"; }
    virtual const char *getHeaders (void) const { return "Sector,Airports"; }

    virtual void getSectors (StringList *p_list) const;
};

#endif
