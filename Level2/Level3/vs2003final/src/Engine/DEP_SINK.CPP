#include "stdafx.h"
#include <string.h>
#include "engine\dep_sink.h"
#include "engine\dep_srce.h"
#include "engine\proclist.h"
#include "common\termfile.h"
#include "inputs\miscproc.h"
#include "inputs\in_term.h"
#include "common\states.h"
#include "engine\person.h"
#include "engine\procq.h"
#include "engine\terminal.h"
#include "Engine\hold.h"
#include "Engine\lineproc.h"



void DependentSink::initDependent (int p_count, const GroupIndex * p_list )
{
	if( m_bSetInSink )
		return;

	m_sourceList.clear();
	//GroupIndex *m_pList = new GroupIndex[p_count];
	//memcpy( m_pList, p_list,(p_count*sizeof(GroupIndex)));

	// now set source's priority.
	for( int i=0; i<p_count; i++ )
	{
		struct PriorityDSource priorityDSource;
		priorityDSource.m_sourceGroup = p_list[i];
		priorityDSource.m_nPriority = 0;

		m_sourceList.push_back( priorityDSource );		
	}
}


void DependentSink::initDependent( const MiscProcessorIDListWithLevel* _pIdList )
{
	m_sourceList.clear();

	int nIdCount = _pIdList->getCount();

	// now set source's priority.
	for( int i=0; i<nIdCount; i++ )
	{
		struct PriorityDSource priorityDSource;
		priorityDSource.m_sourceGroup = m_pTerm->procList->getGroupIndex(*_pIdList->getItem(i));
		priorityDSource.m_nPriority = ((MiscProcessorIDWithLevel* )_pIdList->getItem( i ))->GetLevel();

		m_sourceList.push_back( priorityDSource );		
	}
	m_bSetInSink = true;
}


//Obtains the processor group and calls initDependent() to initialize the dependent sink.
void DependentSink::initSpecificMisc (const MiscData *miscData)
{
    if( miscData == NULL )
	{
		m_sourceList.clear();
		return;
	}

	MiscProcessorIDListWithLevel* idList = (MiscProcessorIDListWithLevel*)((MiscDepSinkData*)miscData)->getDependentList();
	
	if(idList->getCount() > 0)
		initDependent( idList );
}

   
//Sets the receiver to "Available", and tests to see if there is someone at the head of the queue to be served.
void DependentSink::makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay)
{
	writeLogEvent (aPerson, availTime, CompleteService);

    available = TRUE;
	releaseNext (availTime );
	WakeupWaitingPaxInPrevProc( availTime );
	WakeupWaitingPaxInDependentProc(availTime);
}

//When a person is coming, appends him to mobile element list.
//If the processor "queue" is empty, reset "available" false.
void DependentSink::addPerson (Person *aPerson)
{
    occupants.addItem (aPerson);
    if (!m_pQueue)
        available = FALSE;
}




// given an priority, check if any higher priority processor has some one waiting.
bool DependentSink::AnyOneWaitInHigherPrioritySource( int _nMyPriority )
{				
	if( _nMyPriority == 0 )	// 0 is the highest priority
		return false;

	for( unsigned int k = 0; k < m_sourceList.size(); k++ )
	{
		for(int l = m_sourceList[k].m_sourceGroup.start;l<=m_sourceList[k].m_sourceGroup.end; l++ )
		{
			Processor* pProc = m_pTerm->procList->getProcessor(l);
						
			if( pProc->getProcessorType() != DepSourceProc )
				continue;
						
			if( _nMyPriority > GetPriorityofSource( l ) )
			{
				if( pProc->getQueueLength() > 0 )	// include the one at service.
					return true;
			}
						
     	}
	  }

	return false;				
}

bool DependentSink::IsWantSettingClosing(CMobileElemConstraint& _constrain)
{
	m_closingConstrain = _constrain;
	if((!available + occupants.getCount()) || !available) return true;

	//if(_constrain.isDefault()) return false;

	for( unsigned int k=0;k<m_sourceList.size();k++)
	{
		for( int l = m_sourceList[k].m_sourceGroup.start; l <= m_sourceList[k].m_sourceGroup.end; l++ ) //every depsource
		{
			Processor* pProc = m_pTerm->procList->getProcessor(l);
			
			if( pProc->getProcessorType() != DepSourceProc )
				continue;
			DependentSource* depSource = (DependentSource *)pProc;
			
			if( depSource->getQueueLength() > 0 || depSource->GetOccupants()->getCount()>0)	// include the one at service.
			{
				for(int i=0;i<depSource->GetSinkCount();i++)
				{
					GroupIndex *sinks = depSource->GetSink();
					for (int j = sinks[i].start; j <= sinks[i].end; j++)  //every depsink
					{
						Processor *sProc = m_pTerm->procList->getProcessor(j);
						if (sProc->getProcessorType() != DepSinkProc)
							continue;
						DependentSink * depSink = (DependentSink *)sProc;
						if(depSink==this) continue;
						else if(depSink->canServe(_constrain))
							return false;
						
					}
				}
				return true;
			}
		}
	}
	return false;
}

Processor* DependentSink::CreateNewProc()
{
	Processor* pProc = new DependentSink;
	return pProc;
}

//====================================================
//		Protected:
//====================================================
// Notifies a dependent source that the receiver is available for service for any waiting passengers
void DependentSink::releaseNext (ElapsedTime availTime)
{
    // cycle through all m_sources until a waiting passenger is found
	assert( m_pTerm );

	// to randomly draw out a source with proirity consideration [4/7/2005] Jason
	bool bHaveProirity = false;
	{
		bool bProirityFlag = false;
		
		//for(int nSourceGroup=0; nSourceGroup<m_sourceCount; nSourceGroup++)
		 for( unsigned int nSourceGroup=0;nSourceGroup<m_sourceList.size();nSourceGroup++)
		{
			//for(int nSource=m_sources[nSourceGroup].start; nSource<=m_sources[nSourceGroup].end; nSource++)
			for(int nSource=m_sourceList[nSourceGroup].m_sourceGroup.start;nSource<=m_sourceList[nSourceGroup].m_sourceGroup.end;nSource++)
			{
				const Processor* pProc = m_pTerm->procList->getProcessor(nSource);
				if(pProc->getProcessorType()==DepSourceProc)
				{

					//bHaveProirity =  static_cast<const DependentSource*>(pProc)->GetPriority()!=-1;
					int m_id = static_cast<const DependentSource*>(pProc)->getIndex();
                    bHaveProirity = GetPriorityofSource(m_id) != -1;
					bProirityFlag = true;  
					break;
				}
			}
			if(bProirityFlag)break;

		}
	}

	DependentSource *aProc;

	//std::vector<bool> vCheckedSourceGroup(m_sourceCount, false);
	std::vector<bool> vCheckedSourceGroup(m_sourceList.size(),false);
	int nCurGroup = -1;
	int nCurSourceProc = -1;

	////change by Mark start ----------------------------------------------------------------------
	//for( int iLoopCount =0; iLoopCount < m_sourceCount; iLoopCount++ )
	for( unsigned int iLoopCount = 0;iLoopCount < m_sourceList.size();iLoopCount++)
	{
		//select a group
		if(bHaveProirity)
		{
			nCurGroup = iLoopCount;
		}
		else
		{
			while(1)
			{
				nCurGroup = rand()%m_sourceList.size();
				if(vCheckedSourceGroup[nCurGroup]==false)
				{
					vCheckedSourceGroup[nCurGroup]=true;
					break;
				}
			}
		}
		//std::vector<bool> vCheckedSource(m_sources[nCurGroup].end-m_sources[nCurGroup].start+1, false);
        std::vector<bool> vCheckedSource(m_sourceList[nCurGroup].m_sourceGroup.end-m_sourceList[nCurGroup].m_sourceGroup.start+1, false);
		///for (int j = m_sources[nCurGroup].start; j <= m_sources[nCurGroup].end; j++)
        for (int j = m_sourceList[nCurGroup].m_sourceGroup.start; j <= m_sourceList[nCurGroup].m_sourceGroup.end; j++)
		{
			//select a processor in current group
			while(1)
			{
				nCurSourceProc = rand() % vCheckedSource.size();
				if(vCheckedSource[nCurSourceProc]==false)
				{
					vCheckedSource[nCurSourceProc]=true;
					break;
				}
			}
			//Processor* pTmp = m_pTerm->procList->getProcessor(m_sources[nCurGroup].start + nCurSourceProc);	
            Processor* pTmp = m_pTerm->procList->getProcessor(m_sourceList[nCurGroup].m_sourceGroup.start + nCurSourceProc);
			ASSERT(pTmp != NULL);
			if ( pTmp->getProcessorType() ==  DepSourceProc )
			{			
				aProc = (DependentSource *)pTmp;
				if (!aProc->isAvailable())
				{
					Person* pPerson = aProc->GetOccupant();
					if( pPerson && (canServe( pPerson->getType())||isClosing ) )
					{
						aProc->release (availTime);
						if(isClosing)
						{
							if(!IsWantSettingClosing(m_closingConstrain))
								isClosing =false;
						}
						return;
					}
				}
				if( aProc->getQueueLength() > 0 )
				{
					return;
				}
				if(isClosing)
				{
					if(!IsWantSettingClosing(m_closingConstrain))
						isClosing =false;
				}
			}
		}
	}	
}


int DependentSink::GetPriorityofSource ( int Id )
{
  for( unsigned int i=0;i< m_sourceList.size();i++ )
  {
	  if( m_sourceList[i].m_sourceGroup.start <= Id && Id <= m_sourceList[i].m_sourceGroup.end)
	  {
		  return m_sourceList[i].m_nPriority;
	  }      
  } 
  return 0;

}

void DependentSink::WakeupPaxWaitingOnDependSource( const ElapsedTime& releaseTime )
{
	for( unsigned int k=0;k<m_sourceList.size();k++)
	{
		for( int l = m_sourceList[k].m_sourceGroup.start; l <= m_sourceList[k].m_sourceGroup.end; l++ ) //every depsource
		{
			Processor* pProc = m_pTerm->procList->getProcessor(l);

			if( pProc->getProcessorType() != DepSourceProc )
				continue;

			std::vector<Processor*>vLinkedProc;
			m_pTerm->procList->GetLinkedWaitAreas( pProc,vLinkedProc );
			int iCount = vLinkedProc.size();
			for( int j=0; j<iCount; ++j )
			{
				Processor* pWaitArea = vLinkedProc[ j ];
				if( pWaitArea->getProcessorType() == HoldAreaProc )
				{						
					((HoldingArea*)pWaitArea)->releaseWaitInAreaPax(releaseTime, pProc );
				}
				else if( pWaitArea->getProcessorType() == LineProc )
				{
					((LineProcessor*)pWaitArea)->releaseWaitInAreaPax( releaseTime, pProc );
				}
			}

		}
	}
}