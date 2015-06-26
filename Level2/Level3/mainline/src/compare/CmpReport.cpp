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
	m_compProject = NULL;
}

CCmpReport::~CCmpReport(void)
{

}

CComparativeProject* CCmpReport::GetComparativeProject()
{
//	ASSERT(m_compProject != NULL);

	return m_compProject;
}

bool CCmpReport::InitReport(const CString& strName)
{
	if(m_compProject == NULL)
	{
		 m_compProject = CMPPROJECTMANAGER->FindCmpProjectByName(strName);
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
    if(CMPPROJECTMANAGER->FindCmpProjectByName(strName) == NULL)
        return FALSE;
    return TRUE;
}

BOOL CCmpReport::SaveProject()
{
    CString strAppPath = PROJMANAGER->GetAppPath();
    // check if the comparative report exists, if does not, create it
    CString strPath = strAppPath + _T("\\Comparative Report\\");
    if (!::PathFileExists(strPath))
    {
        if(::CreateDirectory(strPath, NULL) == FALSE)
            return FALSE;
    }

    strPath += m_compProject->GetName();
    m_compProject->GetInputParam()->SaveData(strPath);
    m_compProject->SetMatch(FALSE);
    return TRUE;
}

BOOL CCmpReport::LoadProject(const CString &strName, const CString& strDesc)
{
	if(!InitReport(strName))
		return FALSE;

	return TRUE;
}

BOOL CCmpReport::Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(int, LPCTSTR))
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
					SetFocusReportName(repName);
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

void CCmpReport::SetFocusReportName( const CString& strFocusReport )
{
	m_strFocusRepName = strFocusReport;
}

CString CCmpReport::GetFocusReportName()
{
	return m_strFocusRepName;
}


