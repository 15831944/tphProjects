#include "stdafx.h"
#include "results\baglog.h"
#include "common\containr.h"
#include "../Common/ARCTracker.h"


void BagLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    BagLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int count = getCount();
    for (long i = 0; i < count; i++)
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile,_csProjPath);
    }
    echoFile.close();
}

void BagLog::sortBags (void)
{
	PLACE_METHOD_TRACK_STRING();
	int count = getCount();
    if( !count )
        return;

    SortedContainer<BagLogEntry> elementArray (count, 0, 512);
    elementArray.ownsElements(1);

    BagLogEntry *entry;
    for (long i = 0; i < count; i++)
    {
        entry = new BagLogEntry;
        getItem( *entry, i );
        elementArray.addItem (entry);
    }

    BagDescStruct tStruct;
    for (long i = 0; i < count; i++)
    {
        entry = elementArray.getItem(i);
        entry->setID (i);
        entry->initStruct (tStruct);
		updateItem( tStruct, i );
    }
    elementArray.clear();
}

