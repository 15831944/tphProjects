// MathSimulation.cpp: implementation of the CMathSimulation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MathSimulation.h"
#include "TermPlanDoc.h"
#include "../Common/ACTypesManager.h"
#include "../Inputs/schedule.h"
#include "../Inputs/FlightConWithProcIDDatabase.h"
#include "../Inputs/PAXDATA.H"
#include "MathProcessEngine.h"
#include "MathProcessEngineFactory.h"
#include "MathFlow.h"
#include "MathProcess.h"
#include "CompRepLogBar.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include <fstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathSimulation::CMathSimulation()
{
	m_pTermPlanDoc	= NULL;
	m_pMathFlow		= NULL;
	m_pLogWnd		= NULL;
	m_nInterval		= 1800;
}

CMathSimulation::CMathSimulation(CMathFlow* pMathFlow)
	: m_pMathFlow(pMathFlow)
	, m_pTermPlanDoc(NULL)
{
}

CMathSimulation::~CMathSimulation()
{
	// Test codes
/*	std::ofstream outfile;
	outfile.open("report.txt");

	for (int j = 0; j < m_vpEngines.size(); j++)
	{
		outfile << "----------------------";
		outfile	<< m_vpEngines[j]->get_Name();
		outfile << "----------------------" << std::endl;
		outfile << ">>>>>>>>>>>>>>> ArrTime---LeaTime <<<<<<<<<<<<<<<<<" << std::endl;
		for (int k = 0; k < m_vpEngines[j]->get_ArrTime().size(); k++)
		{
			outfile << m_vpEngines[j]->get_ArrTime().at(k);
			outfile << "---";
			outfile << m_vpEngines[j]->get_LeaTime().at(k);
			outfile << std::endl;
		}
		outfile << std::endl << std::endl << std::endl;
	}
*/

	for (int i = 0; i < static_cast<int>(m_vpEngines.size()); i++)
	{
		delete m_vpEngines[i];
		m_vpEngines[i] = NULL;
	}
}


bool CMathSimulation::Run()
{
	bool bRet = false;
	if (m_pTermPlanDoc == NULL)
		return bRet;

	if (m_pTermPlanDoc->m_vProcess.empty())
	{
		AfxMessageBox(_T("There's no process!"));
		return bRet;
	}

	m_pTermPlanDoc->m_MathResultManager.ClearResult();

	if (!CreateEngines())
		return bRet;
	
	if (!RunRootProcess())
		return bRet;

	CMathProcess* pRootProcess = GetRootProcess();
	if (pRootProcess == NULL)
		return bRet;

	if (!RunChildProcess(pRootProcess->GetName()))
		return bRet;

	ShowLogInfo(_T("End of simulation run"));

	CString strPath = m_pTermPlanDoc->m_ProjInfo.path + "\\MathResult";
	m_pTermPlanDoc->m_MathResultManager.SetResultFileDir(strPath);
	m_pTermPlanDoc->m_MathResultManager.SaveResult();

	AfxMessageBox(_T("Simulation Done!"));

	bRet = true;
	return bRet;
}

bool CMathSimulation::RunRootProcess()
{
	ShowLogInfo(_T("Running..."));

	// first, find the root process.
	CMathProcess* pRootProcess = GetRootProcess();
	if (pRootProcess == NULL)
		return false;
	
	CMathProcessEngine* pEngineRoot = FindEngine(pRootProcess->GetName());
	if (pEngineRoot == NULL)
	{
		ShowLogInfo(_T("Can not run the process with current parameters!"));
		return false;
	}

	int nFlightCount = m_pTermPlanDoc->GetTerminal().flightSchedule->getCount();
    for (int i = 0; i < nFlightCount; i++)
	{
		if (!GenerateArrTimeOneFlight(i, pEngineRoot))
			return false;
	}

	pEngineRoot->SortArrTimes();
	pEngineRoot->set_MathResManager(&(m_pTermPlanDoc->m_MathResultManager));
	pEngineRoot->set_ServerCount(pRootProcess->GetProcessCount());
	AddProcessName(pRootProcess);
	pEngineRoot->set_Interval(m_nInterval);
	CString strRunInfo;
	if (!pEngineRoot->Run())
	{
		strRunInfo.Format(_T("Process of %s: Failed!"), pRootProcess->GetName());
		ShowLogInfo(strRunInfo);
		return false;
	}

	strRunInfo.Format(_T("Process of %s: Success!"), pRootProcess->GetName());
	ShowLogInfo(strRunInfo);
	AddProgressPos();

	return true;
}

bool CMathSimulation::RunChildProcess(LPCTSTR lpszParentProcessName)
{
	ASSERT(m_pTermPlanDoc != NULL && m_pMathFlow != NULL);
	std::string strParentProcessName(lpszParentProcessName);
	std::vector<MathLink> vOutLink;
	m_pMathFlow->OutLink(strParentProcessName, vOutLink);

	for (std::vector<MathLink>::const_iterator iter = vOutLink.begin();
		 iter != vOutLink.end();
		 iter++)
	{
		std::string strCurProcess(iter->name.second);
		if (CanRun(strCurProcess.c_str()))
		{
			if (!RunOneProcess(strCurProcess.c_str()))
				return false;

			RunChildProcess(strCurProcess.c_str());
		}
	}

	return true;
}

bool CMathSimulation::RunOneProcess(LPCTSTR lpszProcessName)
{
	ASSERT(m_pTermPlanDoc != NULL && m_pMathFlow != NULL);
	CMathProcessEngine* pEngine = FindEngine(lpszProcessName);
	CMathProcess* pMathProcess = FindProcess(lpszProcessName);
	if (pMathProcess == NULL || pEngine == NULL)
		return false;


	std::vector<MathLink> vInLink;
	m_pMathFlow->InLink(std::string(lpszProcessName), vInLink);
	for (std::vector<MathLink>::const_iterator iter = vInLink.begin();
		 iter != vInLink.end();
		 iter++)
	{
		GenerateArrTime(iter->name.first.c_str(), lpszProcessName, iter->percent, iter->distance);
	}

	pEngine->SortArrTimes();
	pEngine->set_MathResManager(&(m_pTermPlanDoc->m_MathResultManager));
	pEngine->set_ServerCount(pMathProcess->GetProcessCount());
	pEngine->set_Interval(m_nInterval);
	AddProcessName(pMathProcess);
	CString strRunInfo;
	if (!pEngine->Run())
	{
		strRunInfo.Format(_T("Process of %s: Failed!"), lpszProcessName);
		ShowLogInfo(strRunInfo);
		return false;
	}

	strRunInfo.Format(_T("Process of %s: Success!"), lpszProcessName);
	ShowLogInfo(strRunInfo);
	AddProgressPos();

	return true;
}

// generate the arrive time of a process from it's parent process and the link percent.
void CMathSimulation::GenerateArrTime(LPCTSTR lpszParentProcessName, LPCTSTR lpszProcessName,
									  int iPercent, int iDistance)
{
	CMathProcessEngine* pParentEngine = FindEngine(lpszParentProcessName);
	CMathProcessEngine* pEngine = FindEngine(lpszProcessName);
	ASSERT(pParentEngine != NULL && pEngine != NULL);

	ProbabilityDistribution* pSpeedDist = FindDefaultSpeedDist();
	if (pSpeedDist == NULL)
		return;

	srand((unsigned)time(NULL));
	for (std::vector<double>::const_iterator iter = pParentEngine->get_LeaTime().begin();
		 iter != pParentEngine->get_LeaTime().end();
		 iter++)
	{
		int iRand = 100 * rand()/(RAND_MAX+1);
		if (iRand <= iPercent)
		{
			double dArrTime = *iter + (iDistance / pSpeedDist->getRandomValue());
			pEngine->AddArrTime(dArrTime);
		}
	}
}

bool CMathSimulation::Pause()
{
	bool bRet = false;
	return bRet;
}

bool CMathSimulation::Resume()
{
	bool bRet = false;
	return bRet;
}

bool CMathSimulation::ExportLogs()
{
	bool bRet = false;
	return bRet;
}

bool CMathSimulation::PrintLogs()
{
	bool bRet = false;
	return bRet;
}

bool CMathSimulation::GenerateArrTimeOneFlight(int indexFlight, CMathProcessEngine* pEngine)
{
	if (m_pTermPlanDoc == NULL)
		return false;

	ASSERT(pEngine != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	Flight* pFlight = terminal.flightSchedule->getItem(indexFlight);

	TCHAR strACType[128];
	pFlight->getACType(strACType);
	CACType* pACType = NULL ;
	pACType = _g_GetActiveACMan(terminal.m_pAirportDB)->getACTypeItem(strACType);
	if( pACType == NULL)
	{
		CString errormsg ;
		errormsg.Format(_T("Invalid ACType,please define the actype entry for \'%s\'"),strACType) ;
		AfxMessageBox(errormsg, MB_ICONEXCLAMATION | MB_OK );
		return false;
	}
	int iCapacity = pACType->m_iCapacity;
	ProbabilityDistribution* pProbDist = FindDefaultPaxTypeDist();
	if (pProbDist == NULL)
		return false;

	srand((unsigned)time(NULL));
	double leadTime = 0.0;

	if (pFlight->isDeparting())
	{
		int iDepTime = pFlight->getDepTime().asSeconds();
		for (int i = 0; i < iCapacity; i++)
		{
			leadTime = iDepTime + (60.0 * (pProbDist->getRandomValue()));
			pEngine->AddArrTime(leadTime);
		}
	}

	return true;
}

CMathProcess* CMathSimulation::FindProcess(LPCTSTR lpszProName)
{
	ASSERT(m_pTermPlanDoc != NULL);

	for (std::vector<CMathProcess>::iterator iter = m_pTermPlanDoc->m_vProcess.begin();
		iter != m_pTermPlanDoc->m_vProcess.end();
		iter++)
	{
		if (iter->GetName().CompareNoCase(lpszProName) == 0)
			return &(*iter);
	}

	return NULL;
}

void CMathSimulation::AddProcessName(CMathProcess* pPro)
{
	ASSERT(m_pTermPlanDoc != NULL && pPro != NULL);
	std::string strName(LPCTSTR(pPro->GetName()));

	CMathResultManager& mathResultManager = m_pTermPlanDoc->m_MathResultManager;
	
	mathResultManager.GetQTimeResult().m_vstrProcess.push_back(strName);
	mathResultManager.GetQLengthResult().m_vstrProcess.push_back(strName);
	mathResultManager.GetThroughputResult().m_vstrProcess.push_back(strName);
	mathResultManager.GetPaxCountResult().m_vstrProcess.push_back(strName);
	mathResultManager.GetUtilizationResult().m_vstrProcess.push_back(strName);

}

// check if a process have been run
bool CMathSimulation::HasResult(LPCTSTR lpszProcessName)
{
	ASSERT(m_pTermPlanDoc != NULL);
	CString strName(lpszProcessName);

	std::vector<std::string>& vStr = m_pTermPlanDoc->m_MathResultManager.GetQTimeResult().m_vstrProcess;

	for (std::vector<std::string>::const_iterator iter = vStr.begin();
		 iter != vStr.end();
		 iter++) 
	{
		CString strIter(iter->c_str());
		if (strName.CompareNoCase(strIter) == 0)
		{
			return true;
		}
	}

	return false;
}

// check if a process can be run now
// if all of it's inlink processes have leave times and this process has no result, 
// then this process can be run now
bool CMathSimulation::CanRun(LPCTSTR lpszProcessName)
{
	ASSERT(m_pTermPlanDoc != NULL && m_pMathFlow != NULL);
	if (HasResult(lpszProcessName))
		return false;

	std::string strName(lpszProcessName);

	std::vector<MathLink> vInLink;
	m_pMathFlow->InLink(strName, vInLink);

	for (std::vector<MathLink>::const_iterator iter = vInLink.begin();
		 iter != vInLink.end();
		 iter++)
	{
		if (!HasResult(iter->name.first.c_str()))
			return false;
	}

	return true;
}



void CMathSimulation::ShowLogInfo(LPCTSTR lpszLogInfo)
{
	ASSERT(m_pLogWnd != NULL);
	ASSERT(::IsWindow(m_pLogWnd->m_hWnd));
	m_pLogWnd->AddLogText(lpszLogInfo);
}

void CMathSimulation::AddProgressPos()
{
	ASSERT(m_pLogWnd != NULL);
	ASSERT(::IsWindow(m_pLogWnd->m_hWnd));

	int iProgress = (int)(100.0 / m_pMathFlow->GetFlow().size());
	int iPos = min(100, m_pLogWnd->GetProgressPos() + iProgress);
	m_pLogWnd->SetProgressPos(iPos);
}

bool CMathSimulation::CreateEngines()
{
	CMathProcessEngineFactory engFactory;

	for (std::vector<CMathProcess>::iterator iter = m_pTermPlanDoc->m_vProcess.begin();
		iter != m_pTermPlanDoc->m_vProcess.end();
		iter++)
	{
		std::string strName((LPCTSTR)(iter->GetName()));
		if (CMathFlow::HasProc(*m_pMathFlow, strName))
		{
			iter->SetTerminal(&(m_pTermPlanDoc->GetTerminal()));
			ProbabilityDistribution* pProbDist = iter->GetProbDist();
			if (pProbDist == NULL)
			{
				CString strProbErrorInfo;
				strProbErrorInfo.Format(_T("Can not find the service time distribution on process %s"),
										iter->GetName());
				ShowLogInfo(strProbErrorInfo);
				return false;
			}
			
			engFactory.SetServiceTimeProbDist(pProbDist);
			CMathProcessEngine* pEngine = engFactory.CreateInstance();
			if (pEngine == NULL)
			{
				CString strErrorInfo;
				strErrorInfo.Format(_T("Failed to create engine of %s"), strName.c_str());
				ShowLogInfo(strErrorInfo);
				return false;
			}

			pEngine->set_Name(iter->GetName());
			m_vpEngines.push_back(pEngine);
		}
	}

	return true;
}

CMathProcessEngine* CMathSimulation::FindEngine(LPCTSTR lpszEngineName)
{
	CMathProcessEngine* pEngine = NULL;
	for (std::vector<CMathProcessEngine*>::const_iterator iter = m_vpEngines.begin();
		 iter != m_vpEngines.end();
		 iter++)
	{
		CString strCurEngineName((*iter)->get_Name());
		if (strCurEngineName.CompareNoCase(lpszEngineName) == 0)
		{
			pEngine = *iter;
			return pEngine;
		}
	}

	return pEngine;
}
CMathProcess* CMathSimulation::GetRootProcess()
{
	CMathProcess* pRootProcess = NULL;
	int iRootProCount = 0;
	std::vector<MathLink> vlink;
	for (std::vector<CMathProcess>::iterator iter = m_pTermPlanDoc->m_vProcess.begin();
		iter != m_pTermPlanDoc->m_vProcess.end();
		iter++)
	{
		std::string strName((LPCTSTR)(iter->GetName()));
		if (CMathFlow::HasProc(*m_pMathFlow, strName))
		{
			m_pMathFlow->InLink(strName, vlink);
			if (vlink.empty())
			{
				pRootProcess = &(*iter);
				iRootProCount++;
			}
		}
	}

	if (iRootProCount > 1)
	{
		CString strInfo;
		strInfo.Format(_T("This Flow Has %d Entry Processes!"), iRootProCount);
		ShowLogInfo(strInfo);
		return NULL;
	}

	if (pRootProcess == NULL)
		ShowLogInfo(_T("Can not find root process in this flow!"));

	return pRootProcess;
}

ProbabilityDistribution* CMathSimulation::FindDefaultPaxTypeDist()
{
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	CMobileConWithProcIDDatabase* pConDB = terminal.paxDataList->getLeadLagTime();
	int nCount = pConDB->getCount();

	for (int i = 0; i<nCount; i++)
	{
		ConstraintEntry* pEntry = pConDB->getItem(i);
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint(szName, 0, 256);
		if (szName.CompareNoCase(_T("Departure")) == 0)
			return pEntry->getValue();
	}

	AfxMessageBox(_T("Can not find the DEPARTURE passenger type!"));
	return NULL;
}

ProbabilityDistribution* CMathSimulation::FindDefaultSpeedDist()
{
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	CMobileElemConstraintDatabase* pConDB = terminal.paxDataList->getSpeed();
	int nCount = pConDB->getCount();

	CMobileElemConstraintDatabase* pImpactConDB = terminal.paxDataList->getImpactSpeed();
	int nImpactCount = pImpactConDB->getCount();

	double dImpactValue = 0;
	ProbabilityDistribution* pProDis = NULL;
    ConstantDistribution* pConstant = NULL;

	for (int i = 0; i<nCount; i++)
	{
		ConstraintEntry* pEntry = pConDB->getItem(i);
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint(szName, 0, 256);
		if (szName.CompareNoCase(_T("Default")) == 0)
		{
			pProDis = pEntry->getValue();

			for (int j = 0; j < nImpactCount; j++)//find the default impact-speed value
			{
				ConstraintEntry* pImpactEntry = pImpactConDB->getItem(j);
				const Constraint* pImpactType = pImpactEntry->getConstraint();
				CString szImpactName;
				pImpactType->screenPrint(szImpactName, 0, 256);

				if (szImpactName.CompareNoCase(_T("Default")) == 0)
				{
					ProbabilityDistribution* pProb = pImpactEntry->getValue();
					dImpactValue = pProb->getMean()/100;
					double dSpeed = pProDis->getMean();
					dSpeed = dSpeed * (1 - dImpactValue);

					pConstant->init(dSpeed);
					return pConstant;
				}					
			}

			return pProDis;
		}
	}

	AfxMessageBox(_T("Can not find the DEFAULT speed type!"));
	return NULL;
}