#include "stdafx.h"
#include <string.h>
#include "inputs\in_term.h"
#include "engine\dep_srce.h"
#include "engine\dep_sink.h"
#include "engine\proclist.h"
#include "engine\person.h"
#include "inputs\miscproc.h"
#include "common\termfile.h"
#include "engine\fixedq.h"
#include "engine\nfixedq.h"
#include "inputs\procdist.h"
#include "common\states.h"
#include "common\util.h"
#include "common\exeption.h"
#include "common\ARCException.h"
#include "engine\terminal.h"

#include "anServeProcList.h"

#include "inputs\assign.h"
#include <limits>
#include <boost/scoped_ptr.hpp>
 
#include "Inputs\Procdb.h"
#include "Inputs\Assigndb.h"
DependentSource::~DependentSource () 
{
	if (m_nSinkCount != NULL) 
		
	{
		delete[] m_pSinks; 
		m_pSinks = NULL; 
	}
	
	if (m_pOccupant != NULL ) 
	{
	//	delete m_pOccupant; because this person has been deleted from Processor::occupants after simulation
		m_pOccupant = NULL;
	}
	
} 


void DependentSource::initDependent (int listCount, const GroupIndex *sinkList)
{
    m_pSinks = new GroupIndex[listCount];
    memcpy (m_pSinks, sinkList, (listCount * sizeof (GroupIndex)));
    m_nSinkCount = listCount;

    DependentSink *depSink;
    Processor *aProc;
    GroupIndex source;
    source.start = index;
    source.end = index;

	assert( m_pTerm );
    for (int i = 0; i < m_nSinkCount; i++)
        for (int j = m_pSinks[i].start; j <= m_pSinks[i].end; j++)
        {
            aProc = m_pTerm->procList->getProcessor(j);
            if (aProc->getProcessorType() != DepSinkProc)
                continue;
            depSink = (DependentSink *)aProc;
            depSink->initDependent (1, &source);
        }
}

//It calls initDependent() and assigns destination to the flow source.
void DependentSource::initSpecificMisc (const MiscData *miscData)
{
	if( miscData == NULL )
	{
		if (m_nSinkCount) 
			delete []m_pSinks; 
		m_pSinks = NULL;
		m_nSinkCount = 0;
		return;
	}
	
	MiscDepSourceData *data = (MiscDepSourceData *)miscData;
    ProcessorIDList *idList = data->getDependentList();
    int listCount = MIN ((idList->getCount()),128);

    GroupIndex groups[128];
	assert( m_pTerm );

	const MiscProcessorIDWithOne2OneFlag* pProcIDWithOne2One=NULL;
	GroupIndex groupDependent;
	ProcessorArray arrayDependent;
    for (int i = 0; i < listCount; i++)
	{
		pProcIDWithOne2One=(const MiscProcessorIDWithOne2OneFlag*)idList->getID (i);
		CString str  ;
		str = pProcIDWithOne2One->GetIDString();
		if(pProcIDWithOne2One->getOne2OneFlag())
		{
			//handle 1:1
			groupDependent=m_pTerm->procList->getGroupIndex( *pProcIDWithOne2One);
			arrayDependent.clear() ;
			for(int j=groupDependent.start;j<=groupDependent.end;j++)
			{
				arrayDependent.addItem( m_pTerm->procList->getProcessor( j ) );
			}
			Processor* pProcOneToOne = CFlowChannel::GetOutChannelProc( *this, arrayDependent, pProcIDWithOne2One->idLength());
			CString  str ;
			if(pProcOneToOne)
				str = pProcOneToOne->getIDName() ;
			if(pProcOneToOne)
				groups[i] = m_pTerm->procList->getGroupIndex ( *pProcOneToOne->getID());
			else
				groups[i] = m_pTerm->procList->getGroupIndex (*pProcIDWithOne2One);
		}
		else
		{
			//handle no 1:1
			groups[i] = m_pTerm->procList->getGroupIndex (*pProcIDWithOne2One);
		}
	}
    initDependent (listCount, groups);

    unsigned char probs[128];
    for (int i = 0; i < listCount; i++)
        probs[i] = (unsigned char)(100.0 * ((1.0+(float)i) / (float)listCount));

    ProcessorID destList[128];
    for (int i = 0; i < listCount; i++)
        destList[i] = *(idList->getID (i));
//    flow.initDestinations (destList, probs, listCount);
}


void DependentSource::release (ElapsedTime availTime)
{
    if (m_pOccupant != NULL && !available)
    {
		if(nextAvailable(m_pOccupant))
		{
			m_pOccupant->generateEvent (availTime,false);
			m_pOccupant = NULL;
		}
    }
}

//The coming person is served and removed from dependent source list.
//The nextAvailable() is called to invoke next processor.
void DependentSource::beginService (Person *person, ElapsedTime curTime)
{
    if (m_pOccupant != NULL)
    {
        writeLogEvent (person, curTime, BeginService);

        person->flushLog (curTime);

		throw new ARCSystemError("dependent source (" + getID()->GetIDString() + ") must have a Queue","",curTime.printTime());



    }

	if (nextAvailable ( person ))
	{
		//ElapsedTime serviceTime = getServiceTime(person,curTime);
        //person->generateEvent (curTime+1l);
		person->generateEvent (curTime,false);
	}
    else
        m_pOccupant = person;

	writeLogEvent (person, curTime, BeginService);
}

void DependentSource::CleanData()
{
	Processor::CleanData();
	m_pOccupant = NULL; 
}
// copy dependent sink's roster to its source
void DependentSource::CopySinkAssignToSource()
{
	return;

	//if (assignmentSchedule == NULL)
	//	return;

	//ASSERT( m_pTerm );
	////TRACE("%s\n", getID()->GetIDString());
	//boost::scoped_ptr<ProcessorRosterSchedule> assignmentScheduleSink(new ProcessorRosterSchedule);
	//for (int i = 0; i < m_nSinkCount; i++)    //get all sink's assignmentSchedule
	//{
 //       for (int j = m_pSinks[i].start; j <= m_pSinks[i].end; j++)
 //       {
 //           Processor* aProc = m_pTerm->procList->getProcessor(j);

 //           if (aProc->getProcessorType() != DepSinkProc)
 //               continue;

	//		//TRACE("%s\n", aProc->getID()->GetIDString());
 //           DependentSink* depSink = (DependentSink *)aProc;
	//		ProcessorRosterSchedule * pSinkAssign = depSink->getSchedule();
	//		//it have no settings on the Roster, handle it as default
	//		if (pSinkAssign == NULL)
	//			return;

	//		int iCount = pSinkAssign->getCount();
	//		for( int s=0; s<iCount; ++s )
	//		{
	//			ProcessorRoster* pAssignment = pSinkAssign->getItem( s );
	//			if( pAssignment->isDefault() )// if one sink has no defined assignment, do nothing
	//				return;

	//			if( !assignmentScheduleSink->ifExist( *pAssignment ) )
	//			{
	//				assignmentScheduleSink->addItem( new ProcessorRoster(*pAssignment));
	//			}				
	//		}            
 //       }
	//}
	//
	////////////////////////////////////////////////////////////////////////////
	////compare Source's roster and Sink's roster
 //   int count = assignmentScheduleSink->getCount();
	//bool bSrcRosterContainSinkRoster = true;
 //   for (int i = 0; i < count; i++)
 //   {
 //       ProcessorRoster* origAssign = assignmentScheduleSink->getItem (i);
 //       const CMultiMobConstraint* multiConst = origAssign->getAssignment();
 //       int constCount = multiConst->getCount();
 //       for (int j = 0; j < constCount; j++)
 //       {             
	//		for(int k = 0 ;k < assignmentSchedule->getCount(); k++)
	//		{
	//			ProcessorRoster* SourceAssign = assignmentSchedule->getItem(k);
	//			const CMultiMobConstraint* SourceMultiMobConstraint = SourceAssign->getAssignment();
	//			bSrcRosterContainSinkRoster = FALSE != SourceMultiMobConstraint->isMatched(*(multiConst->getConstraint (j)));
	//			if (!bSrcRosterContainSinkRoster) break;
	//		}	
	//		if (!bSrcRosterContainSinkRoster) break;
 //       }
	//	if (!bSrcRosterContainSinkRoster) break;
 //   }
	////////////////////////////////////////////////////////////////////////////
	////if Source's roster >= Sink's roster ,  set Sink's roster to Source's roster
	//if (bSrcRosterContainSinkRoster)   
	//{
	//	if(m_pTerm->procAssignDB->FindEntry(*getID()) == NULL)
	//	{
	//		m_pTerm->procAssignDB->addEntry(*getID());
	//		ProcAssignEntry *pEntry = (ProcAssignEntry *)m_pTerm->procAssignDB->getEntryPoint (*getID());
	//		if(pEntry)
	//			assignmentSchedule = pEntry->getSchedule();
	//	}


	//	//clear source's assignment
	//	assignmentSchedule->ClearAllAssignment();

	//	int days = m_pTerm->flightSchedule->GetFlightScheduleEndTime().GetDay() ;
	//
	//	for (int i = 0; i < m_nSinkCount; i++)    //Put all sink's Roster into Source's Roster
	//	{
	//		for (int j = m_pSinks[i].start; j <= m_pSinks[i].end; j++)
	//		{
	//			Processor* aProc = m_pTerm->procList->getProcessor(j);
	//			if (aProc->getProcessorType() != DepSinkProc)
	//				continue;
	//			DependentSink* depSink = (DependentSink *)aProc;
	//			//// TRACE ("\n%s\n",aProc->getID()->GetIDString() );
	//			ProcessorRosterSchedule * pSinkAssign = depSink->getSchedule();
	//			//// TRACE ("\n%s\n",depSink->getID()->GetIDString() );
	//			
	//			int iCount = pSinkAssign->getCount();
	//			for( int s=0; s<iCount; ++s )
	//			{
	//				ProcessorRoster* pAssignment = pSinkAssign->getItem( s );
	//				if( !assignmentSchedule->ifExist( *pAssignment ) )
	//				{
	//					assignmentSchedule->addItem( new ProcessorRoster(*pAssignment));
	//					assignmentSchedule->AddDailyData(*pAssignment,days);

	//					//if(this->index == 77)
	//					{
	//						TRACE("\r\n ----Open: %d, %s   Close: %d, %s \r\n",
	//							pAssignment->getOpenTime().getPrecisely(),
	//							pAssignment->getOpenTime().printTime(),
	//							pAssignment->getCloseTime().getPrecisely(),
	//							pAssignment->getCloseTime().printTime());
	//					}



	//				}				
	//			}            
	//		}
	//	}	


	//}


}


Processor* DependentSource::CreateNewProc()
{
	Processor* pProc = new DependentSource;
	return pProc;
}

//====================================================
//		Protected:
//====================================================
int DependentSource::nextAvailable (Person *_pPerson)
{
	ASSERT( m_pTerm );

	std::vector< std::pair<DependentSink *, int> > vAvailableSinkProc;
    for (int i = 0; i < m_nSinkCount; i++)
    {
        for (int j = m_pSinks[i].start; j <= m_pSinks[i].end; j++)
        {
            Processor* aProc = m_pTerm->procList->getProcessor(j);
			//TRACE( "%s\n", aProc->getID()->GetIDString() );
            if (aProc->getProcessorType() != DepSinkProc)
                continue;
            DependentSink* depSink = (DependentSink *)aProc;
			int nMyPriority = depSink->GetPriorityofSource( index );
            if(		(depSink->isAvailable()||depSink->isClosingAvailable()) &&							// available
					(depSink->canServe( _pPerson->getType() )||depSink->closing()) &&			// type match
					!depSink->GetReserved() &&							// not reserved by other source
                    !depSink->AnyOneWaitInHigherPrioritySource( nMyPriority ) )
			{	
				vAvailableSinkProc.push_back(std::make_pair(depSink,j));
			}
        }
    }
	if(vAvailableSinkProc.size() <= 0)
		return FALSE;

	DependentSink *pSelSink = NULL;
	int nSelSinkIndex = -1;
	double dDistanceFromSourceToSink = (std::numeric_limits<double>::max)();
	//get the closest processor
	std::vector< std::pair<DependentSink *, int> >::iterator iter = vAvailableSinkProc.begin();
	for (;iter != vAvailableSinkProc.end();++iter)
	{
		DependentSink* dTempSink = (*iter).first;
		DistanceUnit dDistance = GetServiceLocation().getDistanceTo(dTempSink->GetServiceLocation());
		if(dDistance < dDistanceFromSourceToSink)
		{
			pSelSink = dTempSink;
			nSelSinkIndex = (*iter).second;
			dDistanceFromSourceToSink = dDistance;
		}
	}

	if(pSelSink != NULL)
	{
		//get processorID
		pSelSink->SetReserved( true );
		SetReservedSink( nSelSinkIndex);
		return TRUE;
	}

	if(vAvailableSinkProc.size() > 0)
	{
		pSelSink = vAvailableSinkProc[0].first;
		if(pSelSink)
		{
			pSelSink->SetReserved( true );
			SetReservedSink( vAvailableSinkProc[0].second);
			return TRUE;
		}
	}

    return FALSE;
}

int DependentSource::canServe( const CMobileElemConstraint& paxType, CanServeProcList* _canServeProcList /*= NULL*/ ) const
{
 	CanServeProcList canServeSinkList; // no need this even the rule of "Exclusive"
 	for (int i = 0; i < m_nSinkCount; i++)
 	{
 		for (int j = m_pSinks[i].start; j <= m_pSinks[i].end; j++)
 		{
 			Processor* aProc = m_pTerm->procList->getProcessor(j);
 			if (aProc->getProcessorType() == DepSinkProc
 				&& aProc->isOpened()
 				&& !aProc->closing()
 				&& aProc->canServe(paxType/*, &canServeSinkList*/)
 				)
 			{
				return Processor::canServe(paxType, _canServeProcList);
 			}
 		}
 	}
 	return false;
}
































