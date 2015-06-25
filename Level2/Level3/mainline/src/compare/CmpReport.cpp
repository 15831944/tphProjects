#include "StdAfx.h"
#include ".\cmpreport.h"
#include "../common/ProjectManager.h"
#include "../common/AirportDatabaseList.h"
#include "../common/AirportDatabase.h"
#include "../Common/SimAndReportManager.h"
#include "ComparativeReportResult.h"
#include "ComparativeQLengthReport.h"
#include "ComparativeQTimeReport.h"
#include "ComparativeThroughputReport.h"
#include "ComparativeAcOperationReport.h"
#include "ComparativePaxCountReport.h"
#include "ComparativeSpaceDensityReport.h"
#include "ComparativeDistanceTravelReport.h"
#include "ComparativeTimeTerminalReport.h"
#include <shlwapi.h>
#include "ModelToCompare.h"


CCmpReport::CCmpReport(void)
{
	m_strFocusRepName.Empty();
	m_bModified = FALSE;
	m_compProject = NULL;
}

CCmpReport::~CCmpReport(void)
{
	//if (m_terminalForReportParam.m_pAirportDB != NULL)
	//{	
	//	m_terminalForReportParam.m_pAirportDB->clearData();
	//}
}

CComparativeProject* CCmpReport::GetComparativeProject()
{
	ASSERT(m_compProject != NULL);

	return m_compProject;
}

bool CCmpReport::InitReport( const CString& strName )
{
	if(m_compProject == NULL)
	{
		 m_compProject = CMPPROJECTMANAGER->getProjectByName(strName);
		 ASSERT(m_compProject != NULL);
		 if(m_compProject)
			m_compProject->LoadData();
	}
	if(m_compProject )
		return true;
	
	return false;
}


BOOL CCmpReport::ProjExists(const CString& strName)
{
	return !(CMPPROJECTMANAGER->isNameAvailable(strName));
}

BOOL CCmpReport::SaveProject()
{
	//	Save Project Information

	if (!CheckData())
		return FALSE;

	if (!m_bModified)
		return TRUE;
	//	create folder;
	CString strPath = PROJMANAGER->GetAppPath();
	strPath += _T("\\Comparative Report\\");




	// check if the comparative report exists, if does not, create it
	if (!::PathFileExists(strPath))
	{
//		UINT nID = MessageBox(NULL, "The folder \\arcterm\\Comparative Report do not exists! Create it?", "Warning", MB_YESNOCANCEL);
//		if(nID == IDYES)
		if(::CreateDirectory(strPath, NULL) == FALSE)
			return FALSE;
//		else if(nID == IDCANCEL)
//			return FALSE;
//		else if(nID == IDNO)
//			return TRUE;

	}

	CComparativeProjectDataSet dsProj;
	dsProj.loadDataSet(PROJMANAGER->GetAppPath());


	std::vector<CComparativeProject *> vProjs;
	BOOL bFound = FALSE;
	if (dsProj.GetProjects(vProjs))
	{
		CString strOriName = m_compProject->GetOriName();
		for (int i = 0; i < static_cast<int>(vProjs.size()); i++)
		{
			if (vProjs[i]->GetName().CompareNoCase(strOriName) == 0)
			{
				vProjs[i]->SetName(m_compProject->GetName());
				vProjs[i]->SetDescription(m_compProject->GetDescription());
				vProjs[i]->SetLastModifiedTime(CTime::GetCurrentTime());
				bFound = TRUE;
				break;
			}
		}
	}

	if (!bFound)
	{
		TCHAR szName[128];
		DWORD dwBufferLen = 128;
		::GetUserName(szName, &dwBufferLen);
		m_compProject->SetUser(szName);
		::GetComputerName(szName, &dwBufferLen);
		m_compProject->SetMachine(szName);
		m_compProject->SetCreatedTime(CTime::GetCurrentTime());
		m_compProject->SetLastModifiedTime(CTime::GetCurrentTime());
		vProjs.push_back(m_compProject);
	}

	m_compProject->SetOriName(m_compProject->GetName());
	dsProj.SetProjects(vProjs);
	dsProj.saveDataSet(PROJMANAGER->GetAppPath(), false);


	strPath += m_compProject->GetName();


	m_compProject->GetInputParam()->SaveData(strPath);

	m_bModified = FALSE;
	SetModifyFlag(FALSE);

	m_compProject->SetMatch(FALSE);	

	return TRUE;
}

BOOL CCmpReport::LoadProject(const CString &strName, const CString& strDesc)
{
	if(!InitReport(strName))
		return FALSE;



//	CModelToCompareDataSet dsModel;
//	std::vector<CModelToCompare *> vModels;
//	dsModel.SetProjName(strName);
//	dsModel.loadDataSet(strPath);
//	if (dsModel.GetModels(vModels)) 
//	{
//		m_compProject->GetInputParam()->GetModelsManagerPtr()->SetModels(vModels);
//	}
//    
//	//m_compProject->Run(NULL,NULL);
//	//return TRUE;
//
//
////	if (!InitTerminal())
//	//	return FALSE;
//
//	CReportToCompareDataSet dsReport;
//	//dsReport.SetInputTerminal(&m_terminalForReportParam);
//	std::vector<CReportToCompare> vReports;
//	dsReport.SetModels(vModels);
//	dsReport.loadDataSet(strPath);
//	if (dsReport.GetReports(vReports))
//	{
//		m_compProject->GetInputParam()->GetReportsManagerPtr()->SetReports(vReports);
//
//		for (int i = 0; i < static_cast<int>(vReports.size()); i++)
//		{
//			CComparativeReportResult* pResult = NULL;
//			switch (vReports[i].GetCategory())
//			{
//			case ENUM_QUEUELENGTH_REP:
//				pResult = new CComparativeQLengthReport;
//				break;
//
//			case ENUM_QUEUETIME_REP:
//				pResult = new CComparativeQTimeReport;
//				break;
//
//			case ENUM_THROUGHPUT_REP:
//				pResult = new CComparativeThroughputReport;
//				break;
//
//			case ENUM_PAXDENS_REP:
//				pResult = new CComparativeSpaceDensityReport;
//				break;
//
//			case ENUM_PAXCOUNT_REP:
//				pResult = new CComparativePaxCountReport;
//				break;
//			case ENUM_ACOPERATION_REP:
//				pResult = new CComparativeAcOperationReport;
//				break;
//			case ENUM_DURATION_REP:
//				pResult = new CComparativeTimeTerminalReport;
//				break;
//			case ENUM_DISTANCE_REP:
//				pResult = new CComparativeDistanceTravelReport;
//				break;
//
//			}
//			if (pResult != NULL)
//			{
//				CString strReportPath = strPath + _T("\\Results\\") + vReports[i].GetName() + _T(".txt");
//				if(pResult->LoadReport(strReportPath.GetBuffer(0)) == true)
//					m_compProject->AddReportResult(pResult);
//			}
//		}
//	}




	return TRUE;
}

BOOL CCmpReport::Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(LPCTSTR))
{
	BOOL bRet = FALSE;
	//if (m_compProject.IsMatch())
	//	m_compProject.SetMatch(!m_bModified);
	if (bRet = m_compProject->Run(pWndStatus,_ShowCopyInfo))
	{
		m_compProject->SetMatch(bRet);
		//m_bModified = FALSE;
	}

	return bRet;
}

Terminal* CCmpReport::GetTerminal()
{
	//return Terminal of first model
	//is it right? please check it before use it
	return m_compProject->GetInputParam()->getTerminal();
}

//BOOL CCmpReport::InitTerminal()
//{
//	int i = 0;
//	std::vector<CModelToCompare *>& vModels = m_compProject->GetInputParam()->GetModelsManagerPtr()->GetModelsList();
//
//	if (!vModels.size())
//		return FALSE;
//
//	for (; i < static_cast<int>(vModels.size()); i++)
//	{
//		if (vModels[i]->GetModelName() == m_strPrevModelName)
//			return TRUE;
//	}
//
//
//	// Any Local Model?
//	BOOL bHasLocalModel = FALSE;
//	CString strModelName, strPath;
//	for (i = 0; i < static_cast<int>(vModels.size()); i++)
//	{
//		if (GetHostName(vModels[i]->GetModelLocation()) == _T("LOCAL"))
//		{
//			bHasLocalModel = TRUE;
//			strModelName = vModels[i]->GetModelName();
//			break;
//		}
//	}
//
//	CFileOperation fo;
//	if (bHasLocalModel)
//	{
//		strPath = PROJMANAGER->GetAppPath() + _T("\\Project");
//	}
//	else
//	{
//		//	Copy data
//		strModelName = vModels[0]->GetModelName();
//		CString strRemote = vModels[0]->GetModelLocation();
//		strPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\Temp");
//		if (fo.CheckPath(strPath) != PATH_IS_FOLDER)
//			::CreateDirectory(strPath, NULL);
//
//		if (!fo.Copy(strRemote, strPath,NULL))
//			return FALSE;
//	}
//
//	PROJECTINFO pi;
//	if (!PROJMANAGER->GetProjectInfo(strModelName, &pi, strPath))
//		return FALSE;
//	m_terminalForReportParam.m_pAirportDB = AIRPORTDBLIST->getAirportDBByID( pi.lUniqueIndex );
//	if( !m_terminalForReportParam.m_pAirportDB->hadLoadDatabase() )
//		m_terminalForReportParam.m_pAirportDB->loadDatabase();
//	m_terminalForReportParam.loadInputs(strPath + _T("\\") + strModelName);
//	m_terminalForReportParam.GetSimReportManager()->loadDataSet(strPath + _T("\\") + strModelName);
//
//	m_strPrevModelName = strModelName;
//
//	if (!bHasLocalModel)
//	{
//		fo.SetAskIfReadOnly(false);
//		fo.Delete(strPath);
//	}
//
//	return TRUE;
//}

CString CCmpReport::GetHostName(const CString &strFolder)
{
	CString str = _T("LOCAL");
	if (strFolder.Left(2) == _T("\\\\"))
	{
		int nPos = strFolder.Find(_T("\\"), 2);
		if (nPos != -1)
			str = strFolder.Mid(2, nPos - 2);
		else
			str = strFolder.Mid(2, strFolder.GetLength() - 2);
	}

	str.MakeUpper();
	return str;
}

void CCmpReport::SetModifyFlag(BOOL bModified)
{
	m_bModified = bModified;
}

BOOL CCmpReport::GetModifyFlag()
{
	return m_bModified;
}

BOOL CCmpReport::CheckData()
{
	if (m_compProject->GetName().IsEmpty())
	{
		AfxMessageBox(_T("Please fill the project name."));
		return FALSE;
	}

	//if (!m_compProject.GetInputParam()->GetModelsManagerPtr()->GetModelsList().size())
	//{
	//	AfxMessageBox(_T("Please add one or more model(s)."));
	//	return FALSE;
	//}

	//if (!m_compProject.GetInputParam()->GetReportsManagerPtr()->GetReportsList().size())
	//{
	//	AfxMessageBox(_T("Please add one or more report(s)."));
	//	return FALSE;
	//}


	return TRUE;
}

