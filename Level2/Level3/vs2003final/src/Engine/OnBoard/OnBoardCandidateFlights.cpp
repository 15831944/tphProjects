#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "OnBoardCandidateFlights.h"
#include "Common/AirportDatabase.h"
#include "Inputs/schedule.h"

OnBoardCandidateFlights::OnBoardCandidateFlights(int nPorjID, InputTerminal* pInTerminal)
:m_spAnalysisConditates(new COnBoardAnalysisCandidates(pInTerminal))
{
	m_spAnalysisConditates->ReadData(nPorjID);
	Initialize();
}

OnBoardCandidateFlights::~OnBoardCandidateFlights(void)
{
}

void OnBoardCandidateFlights::Initialize()
{
	int nOnBoardFltTypeCount = m_spAnalysisConditates->getCount();
	for (int i = 0 ; i < nOnBoardFltTypeCount; i++)
	{
		CandidateFlightTypeList* pOnBoardFltType = m_spAnalysisConditates->getItem( i );
		int nCanditateCount = pOnBoardFltType->getCount();
		for (int j = 0; j < nCanditateCount; j++)
		{
			CandidateFlight* pFlight =  pOnBoardFltType->getItem( j );
			addItem( pFlight );
		}

	}

	if( getCount() < 1)
		throw new OnBoardSimEngineConfigException( 
		new OnBoardDiagnose(_T("no candidate flight defined")) );

}


bool OnBoardCandidateFlights::getCandidateFlightIndexInSchedule(FlightSchedule* pFlightSchedule,
																std::vector< int >& onBoardFlightIDs)
{
	int numFlights = pFlightSchedule->getCount();
	for (int i = 0; i < numFlights; i++)
	{
		Flight* pCurFlight = pFlightSchedule->getItem( i );

		int nOnBoardFltTypeCount = m_spAnalysisConditates->getCount();
		for (int j = 0 ; j < nOnBoardFltTypeCount; j++)
		{
			CandidateFlightTypeList* pOnBoardFltTypeList = m_spAnalysisConditates->getItem( j );
			if(pOnBoardFltTypeList->GetFltType().fits(pCurFlight->getLogEntry()))
			{
				onBoardFlightIDs.push_back( i );
				// well, by now, OnBoard simEngine just support simulate one flight.
				// update this functiona's logic when simEngine has capability to simulate multi-flights.
				return true;
			}
		}
	}

	return onBoardFlightIDs.size() > 0;

}












