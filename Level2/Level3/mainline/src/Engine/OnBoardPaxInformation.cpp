#include "StdAfx.h"
#include ".\onboardpaxinformation.h"
#include "../InputOnboard/KineticsAndSpace.h"
#include "../InputOnboard/UnimpededStateTransTime.h"
#include "../InputOnboard/TransitionTimeModifiers.h"
#include "../InputOnboard/DocDisplayTimeDB.h"
#include "../InputOnboard/TargetLocations.h"
#include "../InputOnboard/UnexpectedBehaviorVariables.h"
#include "../InputOnboard/SeatingPreference.h"
#include "../InputOnBoard/CarryonOwnership.h"
#include "../InputOnboard/WayFindingIntervention.h"

COnBoardPaxInformation::COnBoardPaxInformation(void)
{
	//m_pKineticsAndSpaceData = NULL;

	//document display time
	m_pDocDisplayTime = NULL;

	//way finding intervention
	m_pWayFindingIntervention = NULL;

	//unimpeded state transition time
	m_pUnimpededStateTransTime = NULL;

	//transition time modifiers
	m_pTansitionTimeModifiers = NULL;

	//target location variables
	m_pTargetLocations = NULL;


	//unexpected behaviors
	m_pUnexpectedBehaviorVariables = NULL;

	//seating performance
	m_pSeatingPerformance = NULL;

	//carry on priorities
}

COnBoardPaxInformation::~COnBoardPaxInformation(void)
{
	std::map<int, KineticsAndSpaceDataList*>::iterator iter = m_mapkineticsAndSapceData.begin();
	for (; iter != m_mapkineticsAndSapceData.end(); ++iter)
	{
		delete (*iter).second;
	}
	
	m_mapkineticsAndSapceData.clear();

	//document display time
	delete m_pDocDisplayTime;
	m_pDocDisplayTime = NULL;

	//way finding intervention
	delete m_pWayFindingIntervention;
	m_pWayFindingIntervention = NULL;

	//unimpeded state transition time
	delete m_pUnimpededStateTransTime;
	m_pUnimpededStateTransTime = NULL;

	//transition time modifiers
	delete m_pTansitionTimeModifiers;
	m_pTansitionTimeModifiers = NULL;

	//target location variables
	delete m_pTargetLocations;
	m_pTargetLocations = NULL;


	//unexpected behaviors
	delete m_pUnexpectedBehaviorVariables;
	m_pUnexpectedBehaviorVariables = NULL;

	//seating performance
	delete m_pSeatingPerformance;
	m_pSeatingPerformance = NULL;



}

void COnBoardPaxInformation::Initialize(int nProjID, StringDictionary * pStrDict)
{
	/*
	enum KineticsAndSpaceDataType
	{
		Speed =0,
		FreeInStep,
		CongestionInStep,
		QueueInStep,
		FreeSideStep,
		CongestionSideStep,
		QueueSideStep,
		InteractionDistance,
		AffinityFreeDistance,
		AffinityCongestionDistance,
		Squeezability
	};*/
	//m_pKineticsAndSpaceData = new KineticsAndSpaceData(Speed);

	for (int i = Speed; i < KineticsAndSpaceDataType_count; ++i)
	{
		KineticsAndSpaceDataList *pKineticSpaceData = new KineticsAndSpaceDataList;
		pKineticSpaceData->ReadData(nProjID,(KineticsAndSpaceDataType)i,pStrDict);
		m_mapkineticsAndSapceData[i] = pKineticSpaceData;

	}


	//document display time
	m_pDocDisplayTime = new CDocDisplayTimeDB;
	m_pDocDisplayTime->ReadDataFromDB(pStrDict);

	//way finding intervention
	m_pWayFindingIntervention = new CWayFindingInterventionList;
	m_pWayFindingIntervention->ReadData(nProjID,pStrDict);


	//unimpeded state transition time
	m_pUnimpededStateTransTime = new UnimpededStateTransTime;
	m_pUnimpededStateTransTime->ReadData(pStrDict);

	//transition time modifiers
	m_pTansitionTimeModifiers = new CTransitionTimeModifiers;
	m_pTansitionTimeModifiers->ReadData(pStrDict);

	//target location variables
	m_pTargetLocations = new CTargetLocationsList;
	m_pTargetLocations->ReadData(nProjID,pStrDict);


	//unexpected behaviors
	m_pUnexpectedBehaviorVariables = new UnexpectedBehaviorVariablesList;
	m_pUnexpectedBehaviorVariables->ReadData(pStrDict);

	//seating performance
	m_pSeatingPerformance = new CSeatingPreferenceList(pStrDict);
	m_pSeatingPerformance->ReadData(nProjID,pStrDict);

}

double COnBoardPaxInformation::GetPaxKinetics( CPassengerType& paxTypePassed ,KineticsAndSpaceDataType kineticsType) const
{
	std::map<int, KineticsAndSpaceDataList*>::const_iterator iter = m_mapkineticsAndSapceData.find(kineticsType);
	if(iter != m_mapkineticsAndSapceData.end())
	{
		KineticsAndSpaceData *pData = (*iter).second->getData(paxTypePassed);
		if(pData)
		{
			return pData->GetProbDistValue();
		}
	}
	return 1.0;
}

double COnBoardPaxInformation::GetDocumentDisplayTime( CPassengerType& paxTypePassed ) const
{
	CDocDisplayTimeEntry *pData = m_pDocDisplayTime->GetData(paxTypePassed);
	if(pData)
	{
		return pData->GetProbDistValue();
	}

	return 1.0;
}

double COnBoardPaxInformation::GetWayFindingIntervention( CPassengerType& paxTypePassed ) const
{
	CWayFindingInterventionItem *pData = m_pWayFindingIntervention->getData(paxTypePassed);
	if(pData)
	{
		return pData->GetProbDistValue();
	}

	return 1.0;
}

double COnBoardPaxInformation::GetUnimpededStateTransTime( CPassengerType& paxTypePassed, OnBoard_BodyState stateOrignial, OnBoard_BodyState stateDest ) const
{
	UnimpededStateTransTimePaxType *pData = m_pUnimpededStateTransTime->GetData(paxTypePassed);
	if(pData)
	{
		return pData->GetProbDistValue(stateOrignial,stateDest);
	}

	return 1.0;
}

int COnBoardPaxInformation::GetTransitionTimeModifier_Inbibitor( CPassengerType& paxTypePassed ) const
{
	CTransitionTimeModifiersData *pData = m_pTansitionTimeModifiers->GetData(paxTypePassed);
	if(pData)
	{
		return pData->getInbibitor();
	}

	return 1;
}

int COnBoardPaxInformation::GetTransitionTimeModifier_MTrigger( CPassengerType& paxTypePassed ) const
{
	CTransitionTimeModifiersData *pData = m_pTansitionTimeModifiers->GetData(paxTypePassed);
	if(pData)
	{
		return pData->getMTrigger();
	}

	return 1;
}


double COnBoardPaxInformation::GetTransitionTimeModifier_Factor( CPassengerType& paxTypePassed ) const
{
	CTransitionTimeModifiersData *pData = m_pTansitionTimeModifiers->GetData(paxTypePassed);
	if(pData)
	{
		return pData->getFactor();
	}

	return 1.0;
}







double COnBoardPaxInformation::GetTargetLocation( CPassengerType& paxTypePassed ) const
{
	return 1.0;
}

double COnBoardPaxInformation::GetUnexpectedBehaviorVariable( CPassengerType& paxTypePassed ) const
{
	return 1.0;
}

double COnBoardPaxInformation::GetSeatingPerformance( CPassengerType& paxTypePassed ) const
{
	return 1.0;
}



















