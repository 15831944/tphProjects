#include "stdafx.h"
#include "common\termfile.h"
#include "inputs\sectlist.h"
#include "common\exeption.h"

int Sector::contains (const char *p_airport) const
{
    char str[AIRPORT_LEN];
    for (int i = 0; i < getCount(); i++)
    {
        strcpy (str, getString (i));
        // replace wildcards
        for (int j = 0; str[j] && p_airport[j]; j++)
        {
            if (str[j] == '?')
                str[j] = p_airport[j];
            else if (str[j] == '*')
                strcpy (str + j, p_airport + j);
        }
        if (!strcmpi (p_airport, str))
            return 1;
    }
    return 0;
}

void SectorList::readData (ArctermFile& p_file)
{
    char str[64];
    Sector *newSector;
    while (p_file.getLine())
    {
        p_file.getField (str, SECTOR_CODE_LEN);
        newSector = new Sector;
        newSector->setCode (str);
        addItem (newSector);

        while (!p_file.eol())
        {
            p_file.getField (str, AIRPORT_LEN);
            newSector->addString (str);
        }                            
    }
}

void SectorList::writeData (ArctermFile& p_file) const
{
    char str[64];
    Sector *aSector;
    int sectorCount, count = getCount();
    for (int i = 0; i < count; i++)
    {
        aSector = getItem (i);
        aSector->getCode (str);
        p_file.writeField (str);

        sectorCount = aSector->getCount();
        for (int j = 0; j < sectorCount; j++)
            p_file.writeField (aSector->getString (j));
        p_file.writeLine();
    }
}

void SectorList::getSectors (StringList *p_list) const
{
    char str[SECTOR_CODE_LEN];
    int count = getCount();
    Sector *aSector;

    for (int i = 0; i < count; i++)
    {
        aSector = getItem (i);
        aSector->getCode (str);
        p_list->addString (str);
    }
}

