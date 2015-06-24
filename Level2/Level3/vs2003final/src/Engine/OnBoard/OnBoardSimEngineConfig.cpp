#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "OnBoardSimEngineConfig.h"
#include "Common/AirportDatabase.h"
#include "../terminal.h"
#include "OnBoardCandidateFlights.h"
#include "OnBoardCarrierSpaces.h"
#include "../OnBoardPaxInformation.h"

OnBoardSimEngineConfig::OnBoardSimEngineConfig(void)
:m_pCandidateFlightsInSim(NULL)
,m_pCarrierSpaces(NULL)
,m_pAgentInfo(NULL)
{

}

OnBoardSimEngineConfig::~OnBoardSimEngineConfig(void)
{
	delete m_pCandidateFlightsInSim;
	delete m_pCarrierSpaces;
	delete m_pAgentInfo;
}

void OnBoardSimEngineConfig::loadConfig(int nPrjID, Terminal* _pInputTerm, const CString& strProjPath)
{
	// candidate flight
	m_pCandidateFlightsInSim = new OnBoardCandidateFlights(nPrjID, _pInputTerm);
	// init candidate flight's space in simEngine
	m_pCarrierSpaces = new OnBoardCarrierSpaces( m_pCandidateFlightsInSim );


	
	
	// avoid ambiguous error, disabled data load in current version.
	return;
	// Agents properties and behavior config data
	m_pAgentInfo = new COnBoardPaxInformation();
	try{
		m_pAgentInfo->Initialize( nPrjID, _pInputTerm->inStrDict );
	}
	catch (...){// catch all memory exception.
		CString strErrorMsg;
		strErrorMsg.Format(_T("error occured when load Passenger's kinetics and space config"));
		throw new OnBoardSimEngineConfigException(
		new OnBoardDiagnose(strErrorMsg) );
	}


}







