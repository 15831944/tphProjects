// MissFlightReport.cpp: implementation of the CMissFlightReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MissFlightReport.h"
#include "..\results\outpax.h"
#include "..\results\fltentry.h"
#include "..\results\fltlog.h"
#include "engine\proclist.h"
#include "distelem.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMissFlightReport::CMissFlightReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CMissFlightReport::~CMissFlightReport()
{

}
void CMissFlightReport::GenerateDetailed( ArctermFile& p_file )
{
	InitTools();

	p_file.writeField ("Passenger ID,Flight ID,Last Processor,Time,Dep Time");
    p_file.writeLine();

	ServiceElement element;
    int paxCount = m_pTerm->paxLog->getCount();    

	CProgressBar bar( "Passenger Miss Flight Report", 100, paxCount, TRUE );

    for (int i = 0; i < paxCount; i++)
	{	
		m_pTerm->paxLog->getItem (element, i);
		element.clearLog();

		if( !element.IsMissFlight() )
			continue;

		element.SetOutputTerminal(m_pTerm);
		element.SetEventLog(m_pTerm->m_pMobEventLog);
		m_pTerm->paxLog->getItem(element, i);
		element.clearLog();
		int iFlightIdx = element.getFlightIndex();
		Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
		ASSERT( aFlight );

		if (aFlight->isDeparting())
		{
			ElapsedTime deptime = element.GetRealDepartureTime();
			ElapsedTime misstime =  element.GetMissFlightTime() - deptime;

			/* T343: MissFlight Report doesn't filter the PassengerType based on the conditions set on GUI. */
			if( element.fits( m_multiConst ) )
			/* T343 end. */
			{
				p_file.writeInt( element.getID() );		
				p_file.writeField((const char*)(char*)GetFlightID(i).GetBuffer());
				p_file.writeField(GetLastProcIDName(element).GetBuffer());	

				char tempStr[256];
				misstime.printTime(tempStr, 1);
				p_file.writeField( tempStr );

				p_file.writeTime( deptime, TRUE);
				element.printFullType(tempStr);
				p_file.writeField(tempStr);
				p_file.writeLine();
			}
		}
		bar.StepIt();
    }

}
CString CMissFlightReport::GetFlightID(int paxid)
{
	ReportPaxEntry bagEntry;
	char szBuffer[128];

	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	m_pTerm->paxLog->getItem(bagEntry, paxid);
	bagEntry.clearLog();
	int iFlightIdx = bagEntry.getFlightIndex();
	Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
	ASSERT( aFlight );
	aFlight->getFullID( szBuffer, 'D' );
	szBuffer[7] = 0;

	return szBuffer;
//	MobLogEntry paxEntry;
//	char szBuffer[128];
//
//	m_pTerm->paxLog->getItem(paxEntry, paxid);
//	int flightIndex = paxEntry.getFlightIndex();//paxEntry.getDepFlight();
//	if (flightIndex == -1)
//		return "";
//	Flight *flight = m_pTerm->m_simFlightSchedule.getItem (flightIndex);
//	flight->getFullID( szBuffer, 'D' );
//	szBuffer[7] = NULL;

//	sprintf(szBuffer, "%d", flightIndex);
//	return szBuffer;
}

ElapsedTime CMissFlightReport::GetDepartingTime(int paxid)
{

//	FlightLogEntry fltEntry;
//	MobLogEntry paxEntry;
//
//	m_pTerm->paxLog->getItem(paxEntry, paxid);
//	int flightIndex = paxEntry.getFlightIndex();
//	m_pTerm->flightLog->getItem(fltEntry, flightIndex);
//	return fltEntry.getDepTime();
// ----------------
	ReportPaxEntry bagEntry;
	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	m_pTerm->paxLog->getItem(bagEntry, paxid);
	bagEntry.clearLog();
	int iFlightIdx = bagEntry.getFlightIndex();
	Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
	ASSERT( aFlight );
	return aFlight->getDepTime();



//-------------------
//	MobLogEntry paxEntry;
//	ElapsedTime ret((long)0);
//	m_pTerm->m_simFlightSchedule.getlogentry
//	m_pTerm->paxLog->getItem(paxEntry, paxid);
//	int flightIndex = paxEntry.getDepFlight();
//	if (flightIndex == -1)
//		return ret;
//	Flight *flight = m_pTerm->m_simFlightSchedule.getItem (flightIndex);
//
//	return flight->getDepTime();
}

ElapsedTime CMissFlightReport::GetPaxExitTime(int paxid)
{
	ElapsedTime ret((long)0);
	ReportPaxEntry element;


	ReportPaxEntry bagEntry;
	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	m_pTerm->paxLog->getItem(bagEntry, paxid);
	bagEntry.clearLog();
	ret = bagEntry.getExitTime();



//	element.SetEventLog( m_pTerm->m_pMobEventLog );
//
//	m_pTerm->paxLog->getItem (element, paxid);
//	element.SetOutputTerminal( m_pTerm );
//
//	if( element.alive (m_startTime, m_endTime))
//	{
//		if( element.fits (m_multiConst))
//		{
//			ret = element.getExitTime();			
//			
//		}
//	}
	return ret;

//	OutputPaxEntry entry;
//	m_pTerm->paxLog->getItem( entry, paxid );
//	return entry.getExitTime();
}

void CMissFlightReport::InitTools()
{
	m_fromToProcsTools.ClearAll();
	int iFromSize = m_fromToProcs.m_vFromProcs.size();
	int iToSize = m_fromToProcs.m_vToProcs.size();
	if( iFromSize >  0 )
	{
		for( int j=0; j<iFromSize; ++j )
		{
			GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vFromProcs[j] );
			ASSERT( groupIdx.start >=0 );	
			if( iToSize > 0 )
			{
				for( int s=0; s<iToSize ; ++s )
				{
					GroupIndex toProcGroupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vToProcs[s] );
					ASSERT( toProcGroupIdx.start>=0 );
					for( int jj=groupIdx.start; jj<=groupIdx.end; ++jj )
					{
						for( int ss=toProcGroupIdx.start; ss<=toProcGroupIdx.end; ++ss )
						{
							m_fromToProcsTools.InitData( jj,ss );
						}				
					}
				}
			}	
			else
			{
				for( int jj=groupIdx.start; jj<=groupIdx.end; ++jj )
				{									
					m_fromToProcsTools.InitData( jj,END_PROCESSOR_INDEX );					
				}
			}
		}
	}	
	else
	{
		if( iToSize > 0 )
		{
			for( int s=0; s<iToSize ; ++s )
			{
				GroupIndex toProcGroupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vToProcs[s] );
				ASSERT( toProcGroupIdx.start>=0 );
				for( int ss=toProcGroupIdx.start; ss<=toProcGroupIdx.end; ++ss )
				{
					m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,ss );
				}				
			}
		}
		else
		{
			m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,END_PROCESSOR_INDEX );
		}
	}
	
}

CString CMissFlightReport::GetLastProcIDName(ServiceElement& element)
{
	element.loadEvents();
	int trackCount = element.getCount();

	PaxEvent track;
	for (int i=trackCount-1; i>=0; i--)
	{
		track.init(element.getEvent(i));
		if(track.getProc() != END_PROCESSOR_INDEX)
			break;
	}
	Processor* pProc = m_pTerm->GetProcessorList()->getProcessor(track.getProc());	 
	return pProc->getID()->GetIDString();
}
