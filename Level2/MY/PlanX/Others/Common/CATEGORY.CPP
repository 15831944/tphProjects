#include "stdafx.h"
#include "category.h"
#include "common\termfile.h"
#include "common\exeption.h"

int ACCategory::contains (const char *p_acType) const
{
    char str[AC_TYPE_LEN];
    for (int i = 0; i < getCount(); i++)
    {
        strcpy (str, getString (i));
        // replace wildcards
        for (int j = 0; str[j] && p_acType[j]; j++)
        {
            if (str[j] == '?')
                str[j] = p_acType[j];
            else if (str[j] == '*')
                strcpy (str + j, p_acType + j);
        }
        if (!_strcmpi (p_acType, str))
            return 1;
    }
    return 0;
}

void CategoryList::readData (ArctermFile& p_file)
{
    char str[64];
    ACCategory *newCategory;
    while (p_file.getLine())
    {
        p_file.getField (str, CATEGORY_LEN);
        newCategory = new ACCategory;
        newCategory->setCatName (str);
        addItem (newCategory);

        while (!p_file.eol())
        {
            p_file.getField (str, AC_TYPE_LEN);
            newCategory->addString (str);
        }                            
    }
}

void CategoryList::writeData (ArctermFile& p_file) const
{
    char str[64];
    ACCategory *aCategory;
    int categoryCount, count = getCount();
    for (int i = 0; i < count; i++)
    {
        aCategory = getItem (i);
        aCategory->getCatName (str);
        p_file.writeField (str);

        categoryCount = aCategory->getCount();
        for (int j = 0; j < categoryCount; j++)
            p_file.writeField (aCategory->getString (j));
        p_file.writeLine();
    }
}

void CategoryList::getCategories (StringList *p_list) const
{
    char str[CATEGORY_LEN];
    int count = getCount();
    ACCategory *aCat;

    for (int i = 0; i < count; i++)
    {
        aCat = getItem (i);
        aCat->getCatName (str);
        p_list->addString (str);
    }
}

