#include "stdafx.h"
#include "results\procntry.h"
#include "results\out_term.h"
#include "common\strdict.h"
#include "..\common\projectmanager.h"


void ProcLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
    char *procTypes[] =
    {
        "PointProc",
        "DepSource",
        "DepSink",
        "Line",
        "Baggage",
        "HoldArea",
        "Gate",
        "FloorChange",
        "Barrier",
		"IntegratedStation",
		"MovingSideWalk"
    };

    char filename[_MAX_PATH];
    PROJMANAGER->getFileName (_csProjPath, StringDictBinFile, filename);
    StringDictionary strDict;
    strDict.retrieveDictionary (filename);

    p_stream << strDict.getString (item.name[0]);
    for (int i = 1; item.name[i] && i < MAX_PROC_IDS; i++)
        p_stream << '-' << strDict.getString (item.name[i]);
    p_stream << ',' << item.index << ',' << procTypes[item.procType]
        << ',' << ((item.hasQueue)? "Queue": "No Queue") << ',' <<
        getCount() << '\n';
}


void ProcLogEntry::getIDs (int *p_ids) const
{
    for (int i = 0; i < MAX_PROC_IDS; i++)
        p_ids[i] = item.name[i];
}

void ProcLogEntry::getIDs (short *p_ids) const
{
    for (int i = 0; i < MAX_PROC_IDS; i++)
        p_ids[i] = item.name[i];
}

void ProcLogEntry::getID (char *p_str) const
{
	assert( m_pOutTerm );
    strcpy (p_str, m_pOutTerm->outStrDict->getString (item.name[0]));
    for (int i = 1; i < MAX_PROC_IDS && item.name[i] != EMPTY_ID; i++)
    {
        strcat (p_str, "-");
        strcat (p_str, m_pOutTerm->outStrDict->getString (item.name[i]));
    }
}

void ProcLogEntry::setIDs (const int *p_ids)
{
    for (int i = 0; i < MAX_PROC_IDS; i++)
        item.name[i] = (short)p_ids[i];
}

void ProcLogEntry::sortProcEventLog( void )
{
	std::sort( m_vEventList.begin(), m_vEventList.end(), sortByEventTime );	
}

bool sortByEventTime( const ProcEventStruct& e1, const ProcEventStruct& e2 )
{
	return e1.time < e2.time;
}
