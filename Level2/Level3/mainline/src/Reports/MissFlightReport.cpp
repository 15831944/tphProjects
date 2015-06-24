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
#include "servelem.h"
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

	ServiceElement telement;
    int paxCount = m_pTerm->paxLog->getCount();    

	CProgressBar bar( "Passenger Miss Flight Report", 100, paxCount, TRUE );

    for (int i = 0; i < paxCount; i++)
	{	
		m_pTerm->paxLog->getItem (telement, i);
		telement.clearLog();

		if( !telement.IsMissFlight() )
			continue;

		ReportPaxEntry bagEntry;

		bagEntry.SetOutputTerminal(m_pTerm);
		bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);
		m_pTerm->paxLog->getItem(bagEntry, i);
		bagEntry.clearLog();
		int iFlightIdx = bagEntry.getFlightIndex();
		Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
		ASSERT( aFlight );

		if (aFlight->isDeparting())
		{
			ElapsedTime deptime = telement.GetRealDepartureTime();
			//ElapsedTime deptime = GetDepartingTime( i );
			ElapsedTime misstime =  telement.GetMissFlightTime() - deptime;
		//	ElapsedTime misstime =  GetPaxExitTime(i) - deptime;
			//if( misstime.asSeconds() < 0 && !telement.IsMissFlight() )
			//	continue;
		//	if( /*misstime.asSeconds() > 0 ||*/ telement.IsMissFlight() )
			{
				p_file.writeInt( telement.getID() );		
				p_file.writeField((const char*)(char*)GetFlightID(i).GetBuffer());
				p_file.writeField( (char*)(const char*)GetPaxLastProcIndex(i) );	

				char timestr[50];
				misstime.printTime(timestr, 1);
				p_file.writeField( timestr );

				p_file.writeTime( deptime, TRUE);
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

CString CMissFlightReport::GetPaxLastProcIndex(int paxid)
{
	CString ProcessorStr = "";
	DistanceElement element;

	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );

	
	m_pTerm->paxLog->getItem( element, paxid );
	element.clearLog();
	
	if( element.alive( m_startTime, m_endTime ) )
	{
//		if( element.fits( m_multiConst ) )
		{
			element.calculateDistance( m_fromToProcsTools );
					
			std::vector<CFromToProcCaculation::FROM_TO_PROC>& values = m_fromToProcsTools.GetValues();
			for( unsigned i=0; i<values.size(); ++i )
			{
				if( values[i].m_enFlag == CFromToProcCaculation::END_COMPUTE || values[i].m_iToProc == END_PROCESSOR_INDEX )
				{
					Processor* pProc = m_pTerm->procList->getProcessor( values[i].m_iToProc );
					ProcessorStr = pProc->getID()->GetIDString();			

				}
			}			
			m_fromToProcsTools.ResetAll();
		}
	}

	return ProcessorStr;
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