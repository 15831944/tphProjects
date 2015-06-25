/*****
*
*   Module:     AircraftCategory
*   Author:     Lincoln Fellingham
*   Date:       Jan 96
*   Purpose:    Defines a user selectable set of categories to classify
*               all aircraft types, allowing definition of flight data
*               by groups of aircraft types rather than indiviual ones.
*
*****/

#ifndef AIRCRAFT_CATEGORIES_H
#define AIRCRAFT_CATEGORIES_H

#include <string.h>
#include "common\template.h"
#include "common\dataset.h"
#include "common\containr.h"
#include "common\strdict.h"
//#include "inputsdll.h"

class ACCategory : public StringList
{
protected:
    char catName[CATEGORY_LEN+1];

public:
    ACCategory () { catName[0] = '\0'; }
    virtual ~ACCategory () {};

    int operator == (const ACCategory& p_cat) const
        { return !strcmp (catName, p_cat.catName); }
    int operator < (const ACCategory& p_cat) const
        { return strcmp (catName, p_cat.catName) < 0; }

    void setCatName (const char *p_str) { strcpy (catName, p_str); }
    void getCatName (char *p_str) const { strcpy (p_str, catName); }

    virtual int contains (const char *p_acType) const;
};

class COMMON_TRANSFER CategoryList : public SortedContainer<ACCategory>, public DataSet
{
public:
    CategoryList () : SortedContainer<ACCategory>(4, 0, 4),
        DataSet (AircraftCategoryFile) { ownsElements(1); }
    virtual ~CategoryList() {};

    virtual void clear (void) { SortedContainer<ACCategory>::clear(); }

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const { return "Category file"; }
    virtual const char *getHeaders (void) const { return "Category,AC Types"; }

    virtual void getCategories (StringList *p_list) const;
};

#endif
