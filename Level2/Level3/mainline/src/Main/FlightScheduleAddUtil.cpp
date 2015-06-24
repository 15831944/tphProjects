// FlightScheduleAddUtil.cpp: implementation of the CFlightScheduleAddUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlightScheduleAddUtil.h"
#include "../Common/ProbabilityDistribution.h"
#include "../inputs/probab.h"
#include "../Common/AirlineManager.h"
#include "../inputs/schedule.h"
#include <Inputs/IN_TERM.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlightScheduleAddUtil::CFlightScheduleAddUtil( InputTerminal* _pTerm )
:m_pTerm( _pTerm ),m_endTime( 3600*23l + 60*59l + 59l )
{
	
}

CFlightScheduleAddUtil::~CFlightScheduleAddUtil()
{
	if( m_pChangeDistribution )
	{
		delete m_pChangeDistribution;
		m_pChangeDistribution = NULL;
	}

	if( m_pTimeOffsetDistribution )
	{
		delete m_pTimeOffsetDistribution;
		m_pTimeOffsetDistribution = NULL;
	}
}
//create new flight according distribution
void CFlightScheduleAddUtil::BuildNewFlight( std::vector<Flight*>& _vNewFlights )
{
	ASSERT( m_pChangeDistribution );
	ASSERT( m_pTimeOffsetDistribution );
	ASSERT( m_pTerm );
	
	double dNewAddCount = m_pChangeDistribution->getRandomValue();
	if( dNewAddCount > 0 )
	{
		if( dNewAddCount >100.0 )
		{
			dNewAddCount =100.0 ;
		}

		std::vector<Flight*>vAllExistFlights;
		FindAllExistFlight( vAllExistFlights );
		int iSize = vAllExistFlights.size();
		if( iSize > 0 )
		{
			int iAddCount = (int)(dNewAddCount / 100.0 * iSize +0.5);
			for( int i=0; i<iAddCount; ++i )
			{
				int iFlightIdx = random( iSize );
				Flight* pFlight = vAllExistFlights[iFlightIdx];
				double dTimeOffset = m_pTimeOffsetDistribution->getRandomValue()*60;// mins
				Flight*  pResultFlight = new Flight( (Terminal*)m_pTerm );
				*pResultFlight = *pFlight;
				if( pFlight->isArriving() )
					pResultFlight->setArrTime( pResultFlight->getArrTime() + (long)dTimeOffset );
				if( pFlight->isDeparting() )
					pResultFlight->setDepTime( pResultFlight->getDepTime() + (long)dTimeOffset );
				BuildFlightAirlineName( pFlight, pResultFlight );
				_vNewFlights.push_back( pResultFlight );

				vAllExistFlights.erase( vAllExistFlights.begin() + iFlightIdx );
				iSize = vAllExistFlights.size();
			}
		}
		

	}
	
}
// find all flight whose flight constraint is fit with m_flightConstraint
void CFlightScheduleAddUtil::FindAllExistFlight( std::vector<Flight*>& _vExistFlights  )
{
	int iCount = m_pTerm->flightSchedule->getCount();
	for( int i=0; i<iCount; ++i )
	{
		Flight* pFlight = m_pTerm->flightSchedule->getItem( i );
	//	_vExistFlights.push_back( pFlight );
		
		if( m_flightConstraint.fits( pFlight->getLogEntry() ) )
		{
			if( pFlight->isArriving() )
			{
				if( pFlight->getArrTime() >= m_startTime && pFlight->getArrTime() <= m_endTime )
				{
					_vExistFlights.push_back( pFlight );
				}
				else if (pFlight->isDeparting())
				{
					if( pFlight->getDepTime() >= m_startTime && pFlight->getDepTime() <= m_endTime )
					{
						_vExistFlights.push_back( pFlight );
					}
				}
			}
			else
			{
				if( pFlight->getDepTime() >= m_startTime && pFlight->getDepTime() <= m_endTime )
				{
					_vExistFlights.push_back( pFlight );
				}
			}

		}		
	}
}

void CFlightScheduleAddUtil::WriteToFile( ArctermFile& p_file)const
{
	m_flightConstraint.writeConstraint( p_file );
	p_file.writeLine();

	m_pChangeDistribution->writeDistribution( p_file );
	p_file.writeLine();

	m_pTimeOffsetDistribution->writeDistribution( p_file );
	p_file.writeLine();

	p_file.writeTime( m_startTime, TRUE );
	p_file.writeTime( m_endTime, TRUE );
	p_file.writeLine();
}
void CFlightScheduleAddUtil::ReadFromFile( ArctermFile& p_file)
{
	p_file.getLine();
	//m_flightConstraint.SetInputTerminal( m_pTerm );
	m_flightConstraint.SetAirportDB(m_pTerm->m_pAirportDB);
	m_flightConstraint.readConstraint( p_file );

	p_file.getLine();	
	m_pChangeDistribution = GetTerminalRelateProbDistribution( p_file,m_pTerm, true );

	p_file.getLine();
	m_pTimeOffsetDistribution= GetTerminalRelateProbDistribution( p_file,m_pTerm, true );

	p_file.getLine();
	p_file.getTime( m_startTime, TRUE );
	p_file.getTime( m_endTime, TRUE );
}
// replace old changedistribution with the new one, and delete it
void CFlightScheduleAddUtil::ReplaceChangeDistribution( ProbabilityDistribution* _pChangeDist )
{
	if( m_pChangeDistribution )
	{
		delete m_pChangeDistribution;
		m_pChangeDistribution = _pChangeDist;
	}
}
// replace old timeoffsetdistribution with the new one, and delete it
void CFlightScheduleAddUtil::ReplaceTimeoffsetDistribution( ProbabilityDistribution* _pTimeOffsetDis )
{
	if( m_pTimeOffsetDistribution )
	{
		delete m_pTimeOffsetDistribution;
		m_pTimeOffsetDistribution = _pTimeOffsetDis;
	}
}

bool CFlightScheduleAddUtil::IfFlightExistInSchedule( CString sAirLine, CString sFlightID )
{
	int iCount = m_pTerm->flightSchedule->getCount();
	for( int i=0; i<iCount; ++i )
	{
		Flight* pFlight = m_pTerm->flightSchedule->getItem( i );
		char chAirline[AIRLINE_LEN];
		pFlight->getAirline(chAirline );
		if( sAirLine == chAirline )
		{
			char chFlightId[FULL_ID_LEN];
			pFlight->getFullID(chFlightId );
			if( sFlightID == chFlightId )
			{
				return true;
			}
		}				
	}

	return false;
}
//modify airline name ( that is , if prototype flight is A23, then new flight is A24 etc.
void CFlightScheduleAddUtil::BuildFlightAirlineName( Flight* _pPrototype, Flight* _pNewFlight)
{
	char sAirline[AIRLINE_LEN];
	_pPrototype->getAirline(sAirline);
	CString strAirline( sAirline );
	int iSize = strAirline.GetLength();
	
	if( iSize > 0 )
	{
		CString sTemp( strAirline );
		sTemp += "*";
		_pNewFlight->setAirline( sTemp );
		if( !_g_GetActiveAirlineMan( m_pTerm->m_pAirportDB )->IfExist( sTemp ) )
		{			
			_g_GetActiveAirlineMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( sTemp );			
		}
	}
}





















