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
	m_bModified = FALSE;
	m_compProject = NULL;
}

CCmpReport::~CCmpReport(void)
{
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
		if(::CreateDirectory(strPath, NULL) == FALSE)
			return FALSE;
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
	return TRUE;
}

BOOL CCmpReport::Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(LPCTSTR))
{
	BOOL bRet = FALSE;
	try
	{
		if (bRet = m_compProject->Run(pWndStatus,_ShowCopyInfo))
		{
			m_compProject->SetMatch(bRet);

			CCmpReportParameter* pProjectReportParam = GetComparativeProject()->GetInputParam();
			CSingleReportsManager* pRepManager = pProjectReportParam->GetReportsManager();
			int repCount = pRepManager->getCount();
			for(int i=0; i<repCount; i++)
			{
				CReportToCompare& report = pRepManager->getReport(i);
				if(report.GetChecked() == TRUE)
				{
					CString repName = report.GetName();
					break;
				}
			}
		}
	}	
	catch (ARCError* e)
	{
		char  strBuffer[256];
		e->getMessage(strBuffer);
		TRACE(strBuffer);
		if(pWndStatus)
			pWndStatus->AddLogText(strBuffer);
		delete e;
	}
	catch(ARCError& e)
	{
		char  strBuffer[256];
		e.getMessage(strBuffer);
		TRACE(strBuffer);
		if(pWndStatus)
			pWndStatus->AddLogText(strBuffer);
	}
	catch(std::exception& e)
	{
		if(pWndStatus)
			pWndStatus->AddLogText(e.what());
	}
	

	return bRet;
}

Terminal* CCmpReport::GetTerminal()
{
	//return Terminal of first model
	//is it right? please check it before use it
	return m_compProject->GetInputParam()->getTerminal();
}

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
	return TRUE;
}


