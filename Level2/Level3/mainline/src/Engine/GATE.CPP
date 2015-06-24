#include "stdafx.h"
#include <string.h>
#include "inputs\in_term.h"
#include "engine\gate.h"
#include "engine\hold.h"
#include "engine\proclist.h"
#include "inputs\miscproc.h"
#include "common\termfile.h"
#include "engine\person.h"
#include "engine\terminal.h"

GateProcessor::GateProcessor()
{
	gateWaitArea = NULL;
	waitAreaCount = 0; 
	fCanBeACStandGate = FALSE;
	m_emGateType = NoGateType;
	m_eLastPaxServiceTime = ElapsedTime(0L);
	m_eLastPaxOutConsTime = ElapsedTime(0L);
}
//Attaches a set of gates to a waitarea.
void GateProcessor::initHoldAreas (int p_count, const GroupIndex *p_holds)
{
	if( gateWaitArea )
		delete [] gateWaitArea;
	
    gateWaitArea = new GroupIndex[p_count];
    memcpy (gateWaitArea, p_holds, (p_count * sizeof (GroupIndex)));
    waitAreaCount = p_count;
}

//reset visitorFlag.
void GateProcessor::initMiscData( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates )
{
	m_bDisallowGroup = _bDisallowGroup;
    visitorFlag = visitors;
}

//Initializes a group of waitarea that attached to a gate.
//It call initHoldAreas().
void GateProcessor::initSpecificMisc (const MiscData *p_miscData)
{
	if( p_miscData == NULL )
	{
		if (waitAreaCount) 
			delete []gateWaitArea;
		gateWaitArea = NULL;
		waitAreaCount = 0;
		return;
	}

    MiscGateData *data = (MiscGateData *)p_miscData;
    ProcessorIDList *idList = data->getHoldAreaList();
    GroupIndex groups[32];
	assert( m_pTerm );
	for (int i = 0; i < idList->getCount(); i++)
		groups[i] = m_pTerm->procList->getGroupIndex (*(idList->getID (i)));

    initHoldAreas (idList->getCount(), groups);
	//int linkage gate list
	
	m_vGateGroupIndex.clear();

	if(m_emGateType == DepGate)
	{
		ProcessorIDList *pGateIDList = data->getGateList();

		for (int i = 0; i < pGateIDList->getCount(); i++)
		{
			GroupIndex groups = m_pTerm->procList->getGroupIndex (*(pGateIDList->getID (i)));
			m_vGateGroupIndex.push_back(groups);
		}

	}
	MiscProcessorIDList* misIdList = data->getBridgeConnectorList();
	appendLinkedBridgeProcs(misIdList);
}

//Init Link Bridge connector
void GateProcessor::appendLinkedBridgeProcs(const MiscProcessorIDList* _pLinkedBridgeIdList)
{
	m_vBridgeGroupIndex.clear();
	if (_pLinkedBridgeIdList==NULL) return;

	const MiscProcessorIDList* pMiscLinkedBridgeIDList = _pLinkedBridgeIdList;

	GroupIndex pLinkedBridgeGroupIndex;

	assert( m_pTerm );		
	
	for (int i = 0; i < pMiscLinkedBridgeIDList->getCount(); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pIDWithFlag = (const MiscProcessorIDWithOne2OneFlag*) pMiscLinkedBridgeIDList->getID( i ) ;
		if( pIDWithFlag )
		{

			if( pIDWithFlag->getOne2OneFlag() )//is one to one 
			{
				ProcessorArray tempArray;
				GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( *pMiscLinkedBridgeIDList->getID( i ) );
	
				ASSERT( groupIdx.start >=0 );
				for( int j=groupIdx.start; j<=groupIdx.end; ++j )
				{
					tempArray.addItem( m_pTerm->procList->getProcessor( j ) );
				}
				//get 1:to:1
				Processor* pProcOneToOne = CFlowChannel::GetOutChannelProc( *this, tempArray, pMiscLinkedBridgeIDList->getID( i )->idLength());
				if( pProcOneToOne )
				{
					pLinkedBridgeGroupIndex = m_pTerm->procList->getGroupIndex ( *pProcOneToOne->getID());
					m_vBridgeGroupIndex.push_back(pLinkedBridgeGroupIndex);
				}
			}
			else
			{
				pLinkedBridgeGroupIndex = m_pTerm->procList->getGroupIndex (*(pMiscLinkedBridgeIDList->getID (i)));
				m_vBridgeGroupIndex.push_back(pLinkedBridgeGroupIndex);
			}
		}

	}	
}

//If gateList is empty, creates a new gate list.
//Assigns the input gate index to logEntry.
void GateProcessor::setIndex (int p_ndx)
{
    if (!gateList)
        gateList = new GroupIndex;

    gateCount = 1;
    gateList->start = gateList->end = index = p_ndx;
    logEntry.setIndex (p_ndx);
}

//Releases passengers from waitarea.
void GateProcessor::releaseWaitPax (float p_percent, const CMobileElemConstraint& p_type, ElapsedTime p_time, int _iStage)
{
    Processor *aProc;
    HoldingArea *waitArea;

	assert( m_pTerm );
    for (int i = 0; i < m_pTerm->procList->getProcessorCount(); i++)
    {
        aProc = m_pTerm->procList->getProcessor(i);
        if (aProc->getProcessorType() == HoldAreaProc)
        {
            waitArea = (HoldingArea *) aProc;

			if( waitArea->getStageID() == _iStage )// the intersting stage
			{
				waitArea->releasePax (p_type, p_percent, p_time);
			}            
        }
    }
}



// virtual function to read & write SpecialField( fCanBeACStandGate )
int GateProcessor::readSpecialField(ArctermFile& procFile)
{
	// for back compare if read field failed, set fCanBeACStandGate = TRUE
	char chFlag;
	procFile.getChar( chFlag );
	fCanBeACStandGate = ( chFlag == 'N' ? FALSE : TRUE );
	
	procFile.getChar( chFlag );
	m_bEmergentFlag = ( chFlag == 'Y' ? TRUE : FALSE );

	m_emGateType = NoGateType;

	int nGateType;
	if (procFile.getVersion() > float(2.3))
	{
		procFile.getInteger(nGateType);
		m_emGateType = (GateType)nGateType;
	}

	return TRUE;
}

int GateProcessor::writeSpecialField(ArctermFile& procFile) const
{
	char chFlag = fCanBeACStandGate ? 'Y' : 'N';
	procFile.writeChar( chFlag );
	
	procFile.writeChar( m_bEmergentFlag ? 'Y' : 'N' );

	procFile.writeInt((int)m_emGateType);
	
	return TRUE;
}

Processor* GateProcessor::CreateNewProc()
{
	Processor* pProc = new GateProcessor;
	return pProc;
}
bool GateProcessor::CopyOtherData( Processor* _pDestProc )
{
	ASSERT(_pDestProc->getProcessorType() == GateProc);
	if(_pDestProc->getProcessorType() == GateProc)
	{
		GateProcessor *pDestProc = (GateProcessor *)_pDestProc;
		if(pDestProc)
		{
			pDestProc->setGateType(m_emGateType);
		}
	}
	return true;
}
void GateProcessor::beginService( Person *aPerson, ElapsedTime curTime )
{
	if(m_emGateType == DepGate)
	{
		//get linkage processor
		if(m_vGateGroupIndex.size() > 0)
		{
			ElapsedTime nextAvailableTime = curTime;

			ElapsedTime eSafeTime = ElapsedTime(10L);

			//get linkage gate processor
			int nGateCount = static_cast<int>(m_vGateGroupIndex.size());
			for( int i = 0; i < nGateCount; ++ i)
			{
				GroupIndex gpIndex = m_vGateGroupIndex[i];
				for (int i = gpIndex.start; i <= gpIndex.end; i++)
				{
					Processor *pProc = m_pTerm->procList->getProcessor(i);
					if(pProc == NULL || pProc->getProcessorType() != GateProc)
						break;

					GateProcessor *gateProc = (GateProcessor *)m_pTerm->procList->getProcessor(i);
					//get the last person walk out of the service point, if have out constraint, compare two time, use lager one
					ElapsedTime eLastServiceTime =	gateProc->GetLastServicePaxTime();
					ElapsedTime eLastOurConsTime = gateProc->GetLastPaxOutConsTime();
					ElapsedTime eLastTime = eLastServiceTime;
					if(eLastOurConsTime >eLastServiceTime)
						eLastTime = eLastOurConsTime;

					if( nextAvailableTime >= eSafeTime + eLastTime)
					{
						//if the next available time is fit the safe time range, check the queue has person or not
						//if have ,wait 10 seconds and check it again
						if(gateProc->getTotalQueueLength() > 0)
						{
							nextAvailableTime += eSafeTime;
							break;
						}

					}
					else// if not fit, wait more 10 seconds and check it again
					{
						nextAvailableTime = eSafeTime + eLastTime;
					}
				}
			}

			//can go through 
			if(curTime >= nextAvailableTime)
			{
				aPerson->regetAttatchedNopax();
				// determine service time, based on the Person's type
				ElapsedTime serviceTime;

				serviceTime = getServiceTime(aPerson,curTime);
				aPerson->generateEvent (curTime+serviceTime,false);
				writeLogEvent (aPerson, curTime, BeginService);
				
				m_eLastPaxServiceTime = curTime + serviceTime;
			}
			else
			{
				aPerson->setState(ArriveAtServer);
				aPerson->generateEvent(nextAvailableTime,false);
			}
		}
		else
		{
			aPerson->regetAttatchedNopax();
			// determine service time, based on the Person's type
			ElapsedTime serviceTime;

			serviceTime = getServiceTime(aPerson,curTime);
			aPerson->generateEvent (curTime+serviceTime,false);
			writeLogEvent (aPerson, curTime, BeginService);
			
			m_eLastPaxServiceTime = curTime + serviceTime;
			
			//Processor::beginService(aPerson,curTime);
		}
	}
	else
	{		
		aPerson->regetAttatchedNopax();
		// determine service time, based on the Person's type
		ElapsedTime serviceTime;

		serviceTime = getServiceTime(aPerson,curTime);
		aPerson->generateEvent (curTime+serviceTime,false);
		writeLogEvent (aPerson, curTime, BeginService);
		//Processor::beginService(aPerson,curTime);

		m_eLastPaxServiceTime = curTime + serviceTime;

	}
}

const ElapsedTime& GateProcessor::GetLastServicePaxTime()
{
	return m_eLastPaxServiceTime;
}

const ElapsedTime& GateProcessor::GetLastPaxOutConsTime()
{
	return m_eLastPaxOutConsTime;
}

void GateProcessor::SetLastPaxOutConsTime( const ElapsedTime& eTime )
{
	m_eLastPaxOutConsTime = eTime;
}

BridgeConnector* GateProcessor::getBridgeConnector(int nFlightIndex)
{
	int nCount = (int)m_vBridgeGroupIndex.size();
	for (int i = 0; i < nCount; i++)
	{
		GroupIndex grpIndex = m_vBridgeGroupIndex.at(i);
		for (int j = grpIndex.start; j <= grpIndex.end; j++)
		{
			BridgeConnector* pBridge = (BridgeConnector*)m_pTerm->procList->getProcessor(j);
			if (pBridge->IsBridgeConnectToFlight(nFlightIndex))
			{
				return pBridge;
			}
		}
	}
	return NULL;
}


