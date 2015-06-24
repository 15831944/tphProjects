#include "StdAfx.h"
#include ".\intrailseparationinsim.h"
#include "AircraftSectorManager.h"
#include "../../InputAirside/InTrailSeperationEx.h"
#include "AircraftClassificationManager.h"
#include "CommonInSim.h"
#include "AirsideFlightInSim.h"
#include "common\elaptime.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/EngineDiagnoseEx.h"
#include "../../Common/ARCExceptionEx.h"

CInTrailSeparationInSim::CInTrailSeparationInSim(void)
: m_pAircraftSectorManager(NULL)
{
	m_pInTrailSeparationListEx = new CInTrailSeparationListEx;
}

CInTrailSeparationInSim::~CInTrailSeparationInSim(void)
{
	if (NULL != m_pInTrailSeparationListEx)
	{
		delete m_pInTrailSeparationListEx;
		m_pInTrailSeparationListEx = NULL;
	}

	if (NULL != m_pAircraftSectorManager)
	{
		delete m_pAircraftSectorManager;
		m_pAircraftSectorManager = NULL;
	}
}

void CInTrailSeparationInSim::Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID)
{
	ASSERT(NULL != pAircraftClassificationManager);

	m_pAircraftClassificationManager = pAircraftClassificationManager;

	if (NULL == m_pAircraftSectorManager)
	{
		m_pAircraftSectorManager = new CAircraftSectorManager(nProjID);
	}
	
	m_pInTrailSeparationListEx->ReadData(nProjID);
}

void CInTrailSeparationInSim::GetInTrailSeparationRadiusOfConcern(double& dRadiusofConcern)
{
	ASSERT(NULL != m_pInTrailSeparationListEx);

	dRadiusofConcern = m_pInTrailSeparationListEx->getRadiusofConcer();
	dRadiusofConcern = ARCUnit::ConvertLength(dRadiusofConcern,ARCUnit::NM,ARCUnit::CM);
}

//Called the GetAircraftClass() function but not catch the exception
void CInTrailSeparationInSim::GetInTrailSeparationDistanceAndTime(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, 
																  double dTrailFlightAltitude, double& dMinDistance, ElapsedTime& tMinTime)
{
	ASSERT(NULL != pTrailFlight);
	ASSERT(NULL != pLeadFlight);
	ASSERT(NULL != m_pAircraftSectorManager);

	int nSectorID = m_pAircraftSectorManager->GetSectorID(pTrailFlight, dTrailFlightAltitude);

	AirsideMobileElementMode flightMode = pTrailFlight->GetMode();

	PhaseType curFlightPhaseType = AllPhase;

	//set phase type
	if (flightMode <= OnCruiseArr
		|| flightMode==OnCruiseDep)
	{
		curFlightPhaseType = CruisePhase;
	}
	else if (flightMode == OnTerminal|| flightMode == OnWaitInHold )
	{
		curFlightPhaseType = TerminalPhase;
	}
	else if (flightMode > OnTerminal
		&& flightMode < OnLanding)
	{
		curFlightPhaseType = ApproachPhase;
	}
	else if (pLeadFlight->GetMode() >= OnLanding && pLeadFlight->GetMode() <= OnTakeoff)
	{
		dMinDistance = 0;
		tMinTime = 0L;
		return;
	}

	CInTrailSeparationEx* pInTrailSeparation = m_pInTrailSeparationListEx->GetItemByType(nSectorID, curFlightPhaseType);
	if (NULL == pInTrailSeparation)
	{
		pInTrailSeparation = m_pInTrailSeparationListEx->GetSectorAllItemByPhaseType(curFlightPhaseType);
	}
	AircraftClassificationItem* trailFlightItem = NULL;
	AircraftClassificationItem* leadFlightItem = NULL;
	try
	{

		CString strTrailACType;
		pTrailFlight->getACType(strTrailACType.GetBuffer(1024));
		strTrailACType.ReleaseBuffer();

		//trail flight item
		trailFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strTrailACType, pInTrailSeparation->getCategoryType());
		ASSERT(NULL != trailFlightItem);

		CString strLeadACType;
		pLeadFlight->getACType(strLeadACType.GetBuffer(1024));
		strLeadACType.ReleaseBuffer();

		//lead flight item
		leadFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strLeadACType, pInTrailSeparation->getCategoryType());
		ASSERT(NULL != leadFlightItem);
		//no sector value define, set default value
		ASSERT(NULL != pInTrailSeparation);


	}
	catch(AirsideACTypeMatchError* pError)
	{

		AirsideDiagnose *pDiagnose = (AirsideDiagnose *)pError->GetDiagnose();
		CString strErrorMsg = pDiagnose->GetDetails();
		CString strDetails;
		CString strFlightID;
		pTrailFlight->getFullID(strFlightID.GetBuffer(1024));

		CString strFrontID;
		pLeadFlight->getFullID(strFrontID.GetBuffer(1024));

		strDetails.Format(_T("Can't claculate in trail speration between flight %s and %s, Detail:%s"),strFlightID,strFrontID,strErrorMsg);
		pDiagnose->SetDetails(strDetails);

		throw pError;
	}

	CInTrailSeparationDataEx* pInTrailSeparationDataEx = pInTrailSeparation->GetItemByACClass(trailFlightItem, leadFlightItem);
	ASSERT(NULL != pInTrailSeparationDataEx);

	dMinDistance = pInTrailSeparationDataEx->getMinDistance();
	//dMinDistance += (int)(pInTrailSeparationDataEx->GetTrailProbDistribution()->getRandomValue());

	tMinTime = ElapsedTime((long)pInTrailSeparationDataEx->getMinTime()*60);
	//tMinTime += ElapsedTime(pInTrailSeparationDataEx->GetTimeProbDistribution()->getRandomValue());

	dMinDistance = ARCUnit::ConvertLength(dMinDistance,ARCUnit::NM,ARCUnit::CM);

}

//throw AirsideACTypeMatchError exception
void CInTrailSeparationInSim::GetInTrailSeparationSpatialConvergingAndDiverging(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, 
																				double dTrailFlightAltitude, double& dSpatialConverging, double& dSpatialDiverging)
{
	ASSERT(NULL != pTrailFlight);
	ASSERT(NULL != pLeadFlight);
	ASSERT(NULL != m_pAircraftSectorManager);

	int nSectorID = m_pAircraftSectorManager->GetSectorID(pTrailFlight, dTrailFlightAltitude);
	AirsideMobileElementMode flightMode = pTrailFlight->GetMode();

	PhaseType curFlightPhaseType = AllPhase;

	//set phase type
	if (flightMode <= OnCruiseArr
		|| flightMode==OnCruiseDep)
	{
		curFlightPhaseType = CruisePhase;
	}
	else if (flightMode == OnTerminal|| flightMode == OnWaitInHold)
	{
		curFlightPhaseType = TerminalPhase;
	}
	else if (flightMode > OnTerminal
		&& flightMode < OnLanding)
	{
		curFlightPhaseType = ApproachPhase;
	}
	else if(pLeadFlight->GetMode() >= OnLanding && pLeadFlight->GetMode() <= OnTakeoff)
	{
		dSpatialConverging = 0;
		dSpatialDiverging = 0;
		return;
	}

	CInTrailSeparationEx* pInTrailSeparation = m_pInTrailSeparationListEx->GetItemByType(nSectorID, curFlightPhaseType);
	if (NULL == pInTrailSeparation)
	{
		pInTrailSeparation = m_pInTrailSeparationListEx->GetSectorAllItemByPhaseType(curFlightPhaseType);
	}

	ASSERT(pInTrailSeparation != NULL);

	AircraftClassificationItem* trailFlightItem = NULL;
	AircraftClassificationItem* leadFlightItem =  NULL;
	try
	{
		CString strTrailACType;
		pTrailFlight->getACType(strTrailACType.GetBuffer(1024));
		strTrailACType.ReleaseBuffer();

		//trail flight item
		trailFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strTrailACType, pInTrailSeparation->getCategoryType());
		ASSERT(NULL != trailFlightItem);

		CString strLeadACType;
		pLeadFlight->getACType(strLeadACType.GetBuffer(1024));
		strLeadACType.ReleaseBuffer();

		//lead flight item
		leadFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strLeadACType, pInTrailSeparation->getCategoryType());
		ASSERT(NULL != leadFlightItem);

	}
	catch(AirsideACTypeMatchError* pError)
	{

		AirsideDiagnose *pDiagnose = (AirsideDiagnose *)pError->GetDiagnose();
		CString strErrorMsg = pDiagnose->GetDetails();
		CString strDetails;
		CString strFlightID;
		pTrailFlight->getFullID(strFlightID.GetBuffer(1024));

		CString strFrontID;
		pLeadFlight->getFullID(strFrontID.GetBuffer(1024));

		strDetails.Format(_T("Can't claculate in trail speration between flight %s and %s, Detail:%s"),strFlightID,strFrontID,strErrorMsg);
		pDiagnose->SetDetails(strDetails);

		throw pError;
	}


	CInTrailSeparationDataEx* pInTrailSeparationDataEx = pInTrailSeparation->GetItemByACClass(trailFlightItem, leadFlightItem);
	ASSERT(NULL != pInTrailSeparationDataEx);

	dSpatialConverging = pInTrailSeparationDataEx->getSpatialConverging();
	//dSpatialConverging += (pInTrailSeparationDataEx->GetSpatialDisProbDistribution()->getRandomValue());

	dSpatialDiverging = pInTrailSeparationDataEx->getSpatialDiverging();
	//dSpatialDiverging += (pInTrailSeparationDataEx->GetSpatialDisProbDistribution()->getRandomValue());

	dSpatialConverging = ARCUnit::ConvertLength(dSpatialConverging,ARCUnit::NM,ARCUnit::CM);
	dSpatialDiverging = ARCUnit::ConvertLength(dSpatialDiverging,ARCUnit::NM,ARCUnit::CM);

}
