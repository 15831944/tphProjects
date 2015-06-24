// BagWaitElement.cpp: implementation of the CBagWaitElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "BagWaitElement.h"
#include "..\common\point.h"
#include "..\common\states.h"
#include "engine\terminal.h"

#include <CommonData/procid.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBagWaitElement::CBagWaitElement()
{

}

CBagWaitElement::~CBagWaitElement()
{

}


bool CBagWaitElement::calculateBagWaitTime (ElapsedTime p_start,
    ElapsedTime p_end, Terminal* _pTerm, ProcessorIDList* _pIDList )
{
    totalTime = 0l;
    procCount = 0;

	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( _pTerm );
	logEntry.SetEventLog(_pTerm->m_pProcEventLog);
	int procLogCount = _pTerm->procLog->getCount();

	ElapsedTime paxEntryBaggageProc;
	ElapsedTime paxExitBaggageProc;
	
	for (int i = 0; i < procLogCount && procCount < 2 ; ++i)
	{
		_pTerm->procLog->getItem (logEntry, i);
		if (logEntry.getProcType() == BaggageProc )
		{
			char tempID[256];
			logEntry.getID( tempID );
			ProcessorID procID;
			procID.SetStrDict( _pTerm->inStrDict );
			procID.setID( tempID );

			int iIDCount = _pIDList->getCount();
			
			for( int j=0; j<iIDCount; ++j )
			{
				if( _pIDList->getItem( j )->idFits( procID ))
				{
					break;
				}
			}

			if( j < iIDCount )// fits
			{
				ProcEventStruct event;
				logEntry.setToFirst();
				long eventCount = logEntry.getCount();	
				for (long k = 0; k < eventCount && procCount <2; ++k )
				{
					event = logEntry.getNextEvent();
					if( event.element == this->getID() )// interest pax 
					{
						if( event.type == PaxArriveAtBaggageProc )
						{
							procCount++;
							paxEntryBaggageProc.setPrecisely(event.time);
						}
						else if( event.type == PaxLeaveBaggageProc )
						{
							procCount++;
							paxExitBaggageProc.setPrecisely(event.time);
						}
					}

				}
			}		
		}
	}

	if (procCount == 0)
		return false;
	totalTime = paxExitBaggageProc - paxEntryBaggageProc;	
	return true;
}

void CBagWaitElement::writeEntry (ArctermFile& p_file) const
{
    if (!procCount)
        return;

    p_file.writeInt (item.id);
    p_file.writeInt (item.bagCount);
    p_file.writeTime (totalTime, TRUE);

    char str[MAX_PAX_TYPE_LEN];
    printFullType (str);
    p_file.writeField (str);
    p_file.writeLine();
}

