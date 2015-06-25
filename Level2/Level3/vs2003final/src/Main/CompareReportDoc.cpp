// CompareReportDoc.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CompareReportDoc.h"
#include "../common/ProjectManager.h"
#include "../common/AirportDatabaseList.h"
#include "../common/AirportDatabase.h"
#include "../compare/ComparativeReportResult.h"
#include "../compare/ComparativeQLengthReport.h"
#include "../compare/ComparativeQTimeReport.h"
#include "../Compare/ComparativeThroughputReport.h"
#include "../Compare/ComparativeAcOperationReport.h"
#include "../Compare/ComparativePaxCountReport.h"
#include "../Compare/ComparativeDistanceTravelReport.h"
#include "../Compare/ComparativeTimeTerminalReport.h"
#include "../Compare/ModelToCompare.h"
#include "../Compare/ComparativeSpaceDensityReport.h"
#include "../Common/SimAndReportManager.h"
#include <shlwapi.h>
#include "MainFrm.h"
#include "../Main/XPStyle/StatusBarXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompareReportDoc

IMPLEMENT_DYNCREATE(CCompareReportDoc, CDocument)

CCompareReportDoc::CCompareReportDoc()
{
	m_strPrevModelName.Empty();
	m_strProjName.Empty();

	m_bModified = FALSE;
}

BOOL CCompareReportDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCompareReportDoc::~CCompareReportDoc()
{
	if (m_terminalForReportParam.m_pAirportDB != NULL)
	{	
		m_terminalForReportParam.m_pAirportDB->clearData();
	}
}


BEGIN_MESSAGE_MAP(CCompareReportDoc, CDocument)
	//{{AFX_MSG_MAP(CCompareReportDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompareReportDoc diagnostics

#ifdef _DEBUG
void CCompareReportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCompareReportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCompareReportDoc serialization

void CCompareReportDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCompareReportDoc commands

CComparativeProject* CCompareReportDoc::GetComparativeProject()
{
	return m_compProject;
}

BOOL CCompareReportDoc::SaveProject()
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
		UINT nID = MessageBox(NULL, "The folder \\arcterm\\Comparative Report do not exists! Create it?", "Warning", MB_YESNOCANCEL);
		if(nID == IDYES)
			::CreateDirectory(strPath, NULL);
		else if(nID == IDCANCEL)
			return FALSE;
		else if(nID == IDNO)
			return TRUE;			
	}

	CComparativeProjectDataSet dsProj;
	dsProj.loadDataSet(PROJMANAGER->GetAppPath());
	std::vector<CComparativeProject *> vProjs;
	BOOL bFound = FALSE;
	if (dsProj.GetProjects(vProjs))
	{
		for (int i = 0; i < static_cast<int>(vProjs.size()); i++)
		{
			if (vProjs[i]->GetName() == m_strProjName)
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

	dsProj.SetProjects(vProjs);
	dsProj.saveDataSet(PROJMANAGER->GetAppPath(), false);
	

	strPath += m_compProject->GetName();

	CFileOperation fo;
	if (m_strProjName != m_compProject->GetName())
	{
		if (m_strProjName.IsEmpty())
		{
			if (!::CreateDirectory(strPath, NULL))
				return FALSE;
		}
		else
		{
			CString strOldPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\") + m_strProjName;
			if (fo.CheckPath(strOldPath) != PATH_IS_FOLDER)
			{
				if (!::CreateDirectory(strPath, NULL))
					return FALSE;
			}
			else
				fo.Rename(strOldPath, strPath);
		}
	}

	m_strProjName = m_compProject->GetName();
	m_strProjName.MakeUpper();

	//	Save Model Information

	CModelToCompareDataSet dsModel;
	std::vector<CModelToCompare*>& vModels = m_compProject->GetInputParam()->GetModelsManagerPtr()->GetModelsList();
	dsModel.SetModels(vModels);
	dsModel.saveDataSet(strPath, false);

	//	Save Report information
	std::vector<CReportToCompare>& vReports = m_compProject->GetInputParam()->GetReportsManagerPtr()->GetReportsList();
	CReportToCompareDataSet dsReport;
	dsReport.SetReports(vReports);
	dsReport.saveDataSet(strPath, false);

	m_bModified = FALSE;
	SetModifyFlag(FALSE);

	m_compProject->SetMatch(FALSE);	
	return TRUE;
}

BOOL CCompareReportDoc::LoadProject(const CString &strName, const CString& strDesc)
{
	m_compProject->SetName(strName);
	SetTitle(strName);
	m_strProjName = strName;
	m_compProject->SetDescription(strDesc);

	CString strPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\") + strName;

	CModelToCompareDataSet dsModel;
	std::vector<CModelToCompare*> vModels;
	dsModel.loadDataSet(strPath);
	if (dsModel.GetModels(vModels))
	{
		m_compProject->GetInputParam()->GetModelsManagerPtr()->SetModels(vModels);
	}

	if (!InitTerminal())
		return FALSE;


	CReportToCompareDataSet dsReport;
	dsReport.SetInputTerminal(&m_terminalForReportParam);
	std::vector<CReportToCompare> vReports;
	dsReport.loadDataSet(strPath);
	if (dsReport.GetReports(vReports))
	{
		m_compProject->GetInputParam()->GetReportsManagerPtr()->SetReports(vReports);

		for (int i = 0; i < static_cast<int>(vReports.size()); i++)
		{
			CComparativeReportResult* pResult = NULL;
			switch (vReports[i].GetCategory())
			{
			case ENUM_QUEUELENGTH_REP:
				pResult = new CComparativeQLengthReport;
				break;

			case ENUM_QUEUETIME_REP:
				pResult = new CComparativeQTimeReport;
				break;

			case ENUM_THROUGHPUT_REP:
				pResult = new CComparativeThroughputReport;
				break;

			case ENUM_PAXDENS_REP:
				pResult = new CComparativeSpaceDensityReport;
				break;

			case ENUM_PAXCOUNT_REP:
				pResult = new CComparativePaxCountReport;
				break;
			case ENUM_ACOPERATION_REP:
				pResult = new CComparativeAcOperationReport;
				break;

			case ENUM_DURATION_REP:
				pResult = new CComparativeTimeTerminalReport;
				break;
			case ENUM_DISTANCE_REP:
				pResult = new CComparativeDistanceTravelReport;
				break;
			}

			CString strReportPath = strPath + _T("\\Results\\") + vReports[i].GetName() + _T(".txt");
			pResult->LoadReport(strReportPath.GetBuffer(0));
			m_compProject->AddReportResult(pResult);
		}
	}

	return TRUE;
}

//CStatusBarXP* __statusBar = NULL;

//static void CALLBACK _ShowCopyInfo(LPCTSTR strPath)
//{
//	__statusBar->SetPaneText(0,strPath);
//}
BOOL CCompareReportDoc::Run(HWND hwnd, CCompRepLogBar* pWndStatus)
{
	BOOL bRet = FALSE;
	//if (m_compProject.IsMatch())
	//	m_compProject.SetMatch(!m_bModified);
	
	//Get status bar
	//__statusBar = &(((CMainFrame *)AfxGetApp()->GetMainWnd())->m_wndStatusBar);

	if (bRet = m_compProject->Run(pWndStatus,NULL))
	{
		m_compProject->SetMatch(bRet);
		//m_bModified = FALSE;
	}

	return bRet;
}

Terminal& CCompareReportDoc::GetTerminal()
{
	return m_terminalForReportParam;
}

BOOL CCompareReportDoc::InitTerminal()
{
	int i = 0;
	std::vector<CModelToCompare *>& vModels = m_compProject->GetInputParam()->GetModelsManagerPtr()->GetModelsList();

	if (!vModels.size())
		return FALSE;

	for (; i < static_cast<int>(vModels.size()); i++)
	{
		if (vModels[i]->GetModelName() == m_strPrevModelName)
			return TRUE;
	}

	
	// Any Local Model?
	BOOL bHasLocalModel = FALSE;
	CString strModelName, strPath;
	for (i = 0; i < static_cast<int>(vModels.size()); i++)
	{
		if (GetHostName(vModels[i]->GetModelLocation()) == _T("LOCAL"))
		{
			bHasLocalModel = TRUE;
			strModelName = vModels[i]->GetModelName();
			break;
		}
	}

	CFileOperation fo;
	if (bHasLocalModel)
	{
		strPath = PROJMANAGER->GetAppPath() + _T("\\Project");
	}
	else
	{
		//	Copy data
		strModelName = vModels[0]->GetModelName();
		CString strRemote = vModels[0]->GetModelLocation();
		strPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\Temp");
		if (fo.CheckPath(strPath) != PATH_IS_FOLDER)
			::CreateDirectory(strPath, NULL);

		if (!fo.Copy(strRemote, strPath,NULL))
			return FALSE;
	}

	PROJECTINFO pi;
	if (!PROJMANAGER->GetProjectInfo(strModelName, &pi, strPath))
		return FALSE;
	m_terminalForReportParam.m_pAirportDB = AIRPORTDBLIST->getAirportDBByName( pi.dbname );
	if(m_terminalForReportParam.m_pAirportDB == NULL)
		return FALSE ;
	if( !m_terminalForReportParam.m_pAirportDB->hadLoadDatabase() )
		m_terminalForReportParam.m_pAirportDB->loadDatabase();
	m_terminalForReportParam.loadInputs(strPath + _T("\\") + strModelName);
	m_terminalForReportParam.GetSimReportManager()->loadDataSet(strPath + _T("\\") + strModelName);

	m_strPrevModelName = strModelName;

	if (!bHasLocalModel)
	{
		fo.SetAskIfReadOnly(false);
		fo.Delete(strPath);
	}

	return TRUE;
}

CString CCompareReportDoc::GetHostName(const CString &strFolder)
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

void CCompareReportDoc::SetModifyFlag(BOOL bModified)
{
	m_bModified = bModified;
	SetModifiedFlag(bModified);

}

BOOL CCompareReportDoc::GetModifyFlag()
{
	return m_bModified;
}

BOOL CCompareReportDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace /* = TRUE */)
{
	return SaveProject();
}

BOOL CCompareReportDoc::CheckData()
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
