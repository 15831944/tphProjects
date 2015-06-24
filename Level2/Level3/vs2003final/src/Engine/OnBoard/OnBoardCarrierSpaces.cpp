#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "OnBoardCarrierSpaces.h"
#include "OnBoardCandidateFlights.h"
#include "CarrierSpaceRenderer.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "Services/SeatAssignmentService.h"

OnBoardCarrierSpaces::OnBoardCarrierSpaces(const OnBoardCandidateFlights* pCandidateFlts)
{
	Initialize(pCandidateFlts);
}

OnBoardCarrierSpaces::~OnBoardCarrierSpaces(void)
{
}

void OnBoardCarrierSpaces::Initialize(const OnBoardCandidateFlights* pCandidateFlts)
{
	CAircarftLayoutManager* pLayoutManager = CAircarftLayoutManager::GetInstance();

	int nCandidateFlight = pCandidateFlts->getCount();
	for (int i = 0; i < nCandidateFlight; i++)
	{
		CandidateFlight* pFlight =  pCandidateFlts->getItem( i );
		CAircaftLayOut* pCandidateFlightLayout = 
			pLayoutManager->GetAircraftLayOutByFlightID( pFlight->getID() );
        if(pCandidateFlightLayout == NULL)
			throw new OnBoardSimEngineConfigException( new OnBoardDiagnose( _T("Cannot find candidate flight's layout config")));

		//CAircraftPlacements* pAircraftPlacements = pCandidateFlightLayout->GetPlacements();
		//ASSERT(pAircraftPlacements != NULL);
		//pAircraftPlacements->ReadData();

		carrierSpace* pNewCarrierSpace = new carrierSpace( pFlight->getACType() );
		
		CarrierSpaceRenderer renderer(pCandidateFlightLayout, pNewCarrierSpace);

		if( !pNewCarrierSpace->isValid() )
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("Aircraft Mode:%s layout define invalid"), pCandidateFlightLayout->GetName() );

			throw new OnBoardSimEngineConfigException( new OnBoardDiagnose(strErrorMsg) );
		}

		addItem( pNewCarrierSpace);
	}
}

carrierSpace* OnBoardCarrierSpaces::getCarrierSpace(CString strACType)
{
	for (int i = 0; i < getCount(); i++)
	{
		carrierSpace* pCarrier = getItem( i);
		if(pCarrier->fits(strACType))
			return pCarrier;
	}
	return NULL;
}





















