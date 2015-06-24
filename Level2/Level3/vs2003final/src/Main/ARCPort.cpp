#include "StdAfx.h"
#include ".\arcport.h"
#include <inputs\AirsideInput.h>
#include <InputAirside/InputAirside.h>
#include <InputOnBoard\CInputOnboard.h>
#include <InputOnBoard\ComponentEditContext.h>
#include <Results/OutputAirside.h>
#include "OnBoradInput.h"
#include "TermPlanDoc.h"
#include "Landside\LandSideOperationTypeData.h"
#include <Landside/InputLandside.h>
#include "Results/OutputLandside.h"
#include "../CommonData/ProjectCommon.h"
#include "../LandsideReport\LandsideReportManager.h"

CARCPort::CARCPort(CTermPlanDoc *pDoc)
{

	m_pAirsideOutput = new OutputAirside;
	m_pAirSideInput = new CAirsideInput;

	m_pInputOnboard = new InputOnboard(&m_terminal);
	m_pInputOnboard->GetComponentEditContext()->SetDocument(pDoc);

	//m_onborad_input = new OnBoradInput(pDoc);

	m_engine.m_pARCport = this;
	m_inputairside.SetProjID(pDoc->GetProjectID());

	//landSideOperationList=new CLandSideOperationTypeData();
	m_pInputLandside = new InputLandside(&m_terminal, &pDoc->GetFloorByMode(EnvMode_LandSide));
	m_OutputlandsiedInst = new OutputLandside();
	m_OutputlandsiedInst->m_SimLogs.SetInputTerminal(&getTerminal());
	
//	m_arcReportManager.GetLandsideReportManager()->se

	m_pCommonData = NULL;

}

CARCPort::~CARCPort(void)
{

	delete m_pAirsideOutput;
	delete m_pAirSideInput;

	//chloe
	//delete m_onborad_input; 
	//m_onborad_input = NULL;
	delete m_pInputOnboard;


	//delete landSideOperationList;
	delete m_pInputLandside;
	delete m_OutputlandsiedInst;

	delete m_pCommonData;
	m_pCommonData = NULL;
}

ProjectCommon * CARCPort::getProjectCommon()
{
	if(m_pCommonData == NULL)
		m_pCommonData = new ProjectCommon;

	//retrieve the data from the other object 

	m_pCommonData->setAirportDB(m_terminal.m_pAirportDB);
	m_pCommonData->setSchedule(m_terminal.flightSchedule);
	m_pCommonData->setStringDictionary(m_terminal.inStrDict);


	return m_pCommonData;
}