#include "stdafx.h"
#include <string.h>
#include "engine\floorchg.h"
#include "engine\proclist.h"
#include "inputs\in_term.h"
#include "inputs\miscproc.h"
#include "common\termfile.h"
#include "engine\procq.h"
#include "engine\terminal.h"

//Initializes connection group.
void FloorChangeProcessor::initSpecificMisc (const MiscData *miscData)
{
	if( miscData == NULL )
	{	
		if (connectionCount)
            delete []connections; 
		connections = NULL;
		connectionCount = 0;
		return;
	}
	
    MiscFloorChangeData *data = (MiscFloorChangeData *)miscData;
    ProcessorIDList *idList = data->getConnections();

    connectionCount = idList->getCount();
    connections = new GroupIndex[connectionCount];
	assert( m_pTerm );

    for (int i = 0; i < connectionCount; i++)
        connections[i] = m_pTerm->procList->getGroupIndex (*(idList->getID (i)));
}

//Returns true if some connections belong to the connection group otherwise false.
int FloorChangeProcessor::isConnected (const Processor *aProc) const
{
    for (int i = 0; i < connectionCount; i++)
        if (aProc->getIndex() >= connections[i].start &&
            aProc->getIndex() <= connections[i].end)
                return TRUE;

    return FALSE;
}

Point FloorChangeProcessor::GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime)
{

	//AfxMessageBox("the Same Floor Problem !");
	if( In_Constr.getCount() )
		return In_Constr.getPoint( 0 );
	
	if( m_pQueue != NULL )
		return	m_pQueue->corner( m_pQueue->cornerCount() - 1 );
	
	return getServicePoint( 0 );
}

Processor* FloorChangeProcessor::CreateNewProc()
{
	Processor* pProc = new FloorChangeProcessor;
	return pProc;
}

