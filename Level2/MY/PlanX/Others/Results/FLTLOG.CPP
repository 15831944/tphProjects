#include "stdafx.h"
#include "results\fltlog.h"
#include "results\fltentry.h"
#include "results\out_term.h"
#include "common\flt_cnst.h"
#include "common\category.h"
#include "common\strdict.h"

void FlightLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    FlightLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem( logEntry, i );
        logEntry.echo (echoFile, _csProjPath);
    }
    echoFile.close();
}

void FlightLog::getFlightID (char *p_str, int p_ndx, char p_mode)
{
    FlightLogEntry entry;
    getItem (entry, p_ndx);
    entry.getFlightID (p_str, p_mode);
}

/*****
*
*   Methods used for filling out flight selection tool
*
*****/

void FlightLog::getCarrierList (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();

    // remove airline & ID, as carriers should not block each other
    // filter is based on airport, sector, etc.
    FlightConstraint type (p_type);
    type.setAirline ("");
	CFlightID emptyID;
	type.setFlightID( emptyID );

    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        getItem (anEntry, i);
        anEntry.initStruct (flightStruct);
        arrType.initFlightType (flightStruct, 'A');
        depType.initFlightType (flightStruct, 'D');

        if ((anEntry.isArriving() && type.fits (arrType))
                || (anEntry.isDeparting() && type.fits (depType)))
            p_strDict.addString (flightStruct.airline);
    }
}
    
void FlightLog::getFlightList (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();

    // if no carrier is selected, no flights are listed
    char str[FULL_ID_LEN];
    p_type.getAirline (str);
    if (!str[0])
        return;

    // clear flightID, as flights should not block each other
    // filter is based on carrier, airport, sector, etc.
    FlightConstraint type (p_type);
	CFlightID emptyID;
    type.setFlightID ( emptyID );

    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        getItem (anEntry, i);
        anEntry.initStruct (flightStruct);
        arrType.initFlightType (flightStruct, 'A');
        depType.initFlightType (flightStruct, 'D');

        if (anEntry.isArriving() && type.fits (arrType))
        {
            //itoa (flightStruct.arrID, str, 10);
			strcpy(str, flightStruct.arrID);
            p_strDict.addString (str);
        }
        if (anEntry.isDeparting() && type.fits (depType))
        {
            //itoa (flightStruct.arrID, str, 10);
			strcpy(str, flightStruct.arrID);
            p_strDict.addString (str);
        }
    }
}

void FlightLog::getAirportList (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();

    // remove airport, as they should not block each other
    // filter is based on carrier, flight, sector, etc.
    FlightConstraint type (p_type);

    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        getItem (anEntry, i);
        anEntry.initStruct (flightStruct);
        arrType.initFlightType (flightStruct, 'A');
        depType.initFlightType (flightStruct, 'D');

        if (anEntry.isArriving() && type.fits (arrType) && flightStruct.origin[0])
			p_strDict.addString (flightStruct.origin);
        if (anEntry.isDeparting() && type.fits (depType) && flightStruct.arrStopover[0])
            p_strDict.addString (flightStruct.arrStopover);
        if (anEntry.isDeparting() && type.fits (depType) && flightStruct.depStopover[0])
            p_strDict.addString (flightStruct.depStopover);
        if (anEntry.isArriving() && type.fits (arrType) && flightStruct.destination[0])
			p_strDict.addString (flightStruct.destination);
    }
}

void FlightLog::getACTypeList (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();

    // remove acType, as aircraft should not block each other
    // filter is based on carrier, airport, sector, etc.
    FlightConstraint type (p_type);
    type.setACType ("");

    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        getItem (anEntry, i);
        anEntry.initStruct (flightStruct);
        arrType.initFlightType (flightStruct, 'A');
        depType.initFlightType (flightStruct, 'D');

        if ((anEntry.isArriving() && type.fits (arrType))
                || (anEntry.isDeparting() && type.fits (depType)))
            if (flightStruct.acType[0])
                p_strDict.addString (flightStruct.acType);
    }
}
/*
// for each sector, look for a flight that falls into it
// if there is, add the sector to the list
// result is that if DH8 is selected for acType and there are none
// flying international routes, international will not be selected
// same logic applies for categories, individual flights, carriers, etc.
void FlightLog::getSectors (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();
    FlightConstraint type (p_type);
    type.setSector ("");

    char str[SECTOR_CODE_LEN];
    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int flightCount = getCount(), allSectors = type.isDefault();
    int sectorCount = outputTerminal->outSectorList->getCount();
    Sector *aSector;

    for (int i = 0; i < sectorCount; i++)
    {
        aSector = outputTerminal->outSectorList->getItem (i);
        aSector->getCode (str);
        if (allSectors)
            p_strDict.addString (str);
        else
        {
            type.setSector (str);

            setToFirst();
            for (int j = 0; j < flightCount; j++)
            {
                getNextItem (anEntry);
                anEntry.initStruct (flightStruct);
                arrType.initFlightType (flightStruct, 'A');
                depType.initFlightType (flightStruct, 'D');

                if ((anEntry.isArriving() && type.fits (arrType))
                    || (anEntry.isDeparting() && type.fits (depType)))
                {
                    p_strDict.addString (str);
                    break;
                }
            }
        }
    }
}


// for each category, look for a flight that falls into it
// if there is, add the category to the list
// result is that if CYVR is selected for airport and there are no
// widebody aircraft flying that route, widebody will not be added
// same logic applies for sectors, individual flights, carriers, etc.
void FlightLog::getCategories (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();
    FlightConstraint type (p_type);
    type.setCategory ("");

    char str[CATEGORY_LEN];
    FlightDescStruct flightStruct;
    FlightLogEntry anEntry;
    FlightConstraint arrType, depType;

    int flightCount = getCount(), allCats = type.isDefault();
    int categoryCount = outputTerminal->outCatList->getCount();
    Category *aCat;

    for (int i = 0; i < categoryCount; i++)
    {
        aCat = outputTerminal->outCatList->getItem (i);
        aCat->getCatName (str);
        if (allCats)
            p_strDict.addString (str);
        else
        {
            type.setCategory (str);

            setToFirst();
            for (int j = 0; j < flightCount; j++)
            {
                getNextItem (anEntry);
                anEntry.initStruct (flightStruct);
                arrType.initFlightType (flightStruct, 'A');
                depType.initFlightType (flightStruct, 'D');

                if ((anEntry.isArriving() && type.fits (arrType))
                    || (anEntry.isDeparting() && type.fits (depType)))
                {
                    p_strDict.addString (str);
                    break;
                }
            }
        }
    }
}

*/