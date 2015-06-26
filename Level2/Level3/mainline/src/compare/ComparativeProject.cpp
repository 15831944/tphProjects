// ComparativeProject.cpp: implementation of the CComparativeProject class.
// Author: Kevin Chang
// Copyright (c) 2005		ARC Inc.,
// Last Modified: 2005-5-13 11:14
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Engine/terminal.h"
#include "ComparativeProject.h"
#include "../common/exeption.h"
#include "../reports/MutiRunReportAgent.h"
#include "../common/ProjectManager.h"
#include "../Reports/ReportParaOfTime.h"
#include "../Reports/ReportParaOfReportType.h"
#include "../Reports/ReportParaOfThreshold.h"
#include "../Reports/ReportParaOfPaxType.h"
#include "../Reports/ReportParaWithPortal.h"
#include "../Reports/ReportParaOfTwoGroupProcs.h"


#include "../Reports/ReportParaOfProcs.h"
#include "../common/SimAndReportManager.h"
#include "../common/AirportDatabase.h"
#include "../common/AirportDatabaseList.h"

#include "comparativepaxcountreport.h"

#include "ComparativeReportResult.h"
#include "ComparativeQLengthReport.h"
#include "ComparativeQTimeReport.h"
#include "ComparativeThroughputReport.h"
#include "ComparativeAcOperationReport.h"
#include "ComparativeDistanceTravelReport.h"
#include "ComparativeTimeTerminalReport.h"
#include "ComparativSpaceThroughputReport.h"
#include "ComparativeProcUtilizationReport.h"


#include "ComparativeSpaceDensityReport.h"
#include "ReportDef.h"
#include "../Reports/ReportParaWithArea.h"
#include "ModelToCompare.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComparativeProjectDataSet* CComparativeProjectDataSet::m_pInstance = NULL;

CComparativeProject::CComparativeProject()
{
	m_bMatch = FALSE;
	m_bStop = FALSE;
	m_bRunning = FALSE;
}

CComparativeProject::~CComparativeProject()
{

}

const CString& CComparativeProject::GetName() const
{
	return m_strName;
}

void CComparativeProject::SetName(const CString& strName)
{
	m_strName = strName;
}

const CString& CComparativeProject::GetDescription() const
{
	return m_strDesc;
}

void CComparativeProject::SetDescription(const CString& strDesc)
{
	m_strDesc = strDesc;
}

const CString& CComparativeProject::GetUser() const
{
	return m_strUser;
}

void CComparativeProject::SetUser(const CString& strUser)
{
	m_strUser = strUser;
}

const CString& CComparativeProject::GetServerName() const
{
	return m_strServerName;
}

void CComparativeProject::SetServerName(const CString& strServerName)
{
	m_strServerName = strServerName;
}

const CTime& CComparativeProject::GetCreatedTime() const
{
	return m_tmCreated;
}

void CComparativeProject::SetCreatedTime(const CTime& t)
{
	m_tmCreated = t;
}

const CTime& CComparativeProject::GetLastModifiedTime() const
{
	return m_tmLastModified;
}

void CComparativeProject::SetLastModifiedTime(const CTime& t)
{
	m_tmLastModified = t;
}

CString CComparativeProject::getProjectPath() const
{
	CString strPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\") + m_strName;
	return strPath;
}
BOOL CComparativeProject::LoadData()
{
	CString strProjPath =getProjectPath();
	if(!m_inputParam.LoadData(m_strName, strProjPath))
		return FALSE;

	m_rptList.LoadData(&m_inputParam, strProjPath);

	return TRUE;

}

BOOL CComparativeProject::IsMatch() const
{
	return m_bMatch;
}

void CComparativeProject::AddModel()
{
}

void CComparativeProject::RemoveModel()
{
}

void CComparativeProject::GetModel()
{
}

//	Transfer files
BOOL CComparativeProject::TransferFiles(const CString& strSource, const CString& strDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	CFileOperation fo;
	fo.SetAskIfReadOnly(false);
	fo.SetOverwriteMode(true);
	return fo.Copy(strSource, strDest,_ShowCopyInfo);
}
BOOL CComparativeProject::TransferLogFiles(CModelToCompare *pCmpModel, const CString& strDest,const CString& strSimResult,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	
	CString strSource = pCmpModel->GetModelLocation();
	CString strSourcePath = _T("");
	strSourcePath.Format("%s\\SimResult\\%s",strSource,strSimResult);

	CString strDestPath = _T("");
	strDestPath.Format("%s\\SimResult",strDest);

	CString strDestSimResultPath;
	strDestSimResultPath.Format("%s\\SimResult\\%s",strDest,strSimResult);
	
	CFileOperation fo;
	if(fo.CheckPath(strDestSimResultPath) == PATH_IS_FOLDER)//this path exists 
	{
		if ( (pCmpModel->IsNeedToCopy() == TRUE) )
		{
			return TRUE;
		}
	}

	if(!fo.CheckPath(strSourcePath))
	{
		return FALSE;
	}

	if(!fo.CheckPath(strDestPath))
	{
		::CreateDirectory(strDestPath,NULL);
	}

	fo.SetAskIfReadOnly(false);
	fo.SetOverwriteMode(true);
	return fo.Copy(strSourcePath, strDestPath,_ShowCopyInfo);
}

void CComparativeProject::GenerateReportParameter(const CReportParamToCompare &inParam, CReportParameter* pOutParam, CModelToCompare* pModel)
{
	pOutParam->SetStartTime(inParam.GetStartTime());
	pOutParam->SetEndTime(inParam.GetEndTime());
	pOutParam->SetInterval(inParam.GetInterval().asSeconds());
	pOutParam->SetReportType(inParam.GetReportDetail());


	pOutParam->SetThreshold(LONG_MAX);
	CModelParameter modelParam;
	inParam.GetModelParamByModelName(pModel->GetUniqueName(),modelParam);
	
	std::vector<CString> vsAreas;
	vsAreas.push_back(modelParam.GetArea());
	pOutParam->SetAreas(vsAreas);
	std::vector<CString>vPortals;
	vPortals.push_back(modelParam.GetPortal());
	pOutParam->SetPortals(vPortals);
	std::vector<CMobileElemConstraint> vPaxType;
	modelParam.GetPaxType(vPaxType);

	pOutParam->SetPaxType(vPaxType);

	std::vector<ProcessorID> vProcGroups;
	modelParam.GetProcessorID(vProcGroups);
	pOutParam->SetProcessorGroup(vProcGroups);

	CReportParameter::FROM_TO_PROCS _fromtoProcs;
	modelParam.GetFromToProcs(_fromtoProcs);
	pOutParam->SetFromToProcs(_fromtoProcs);
}

BOOL CComparativeProject::Run(CCompRepLogBar* pWndStatus ,void (CALLBACK * _ShowCopyInfo)(int, LPCTSTR))
{
	CString strStatus;
	int	nStart = 0, nEnd = 0, nProgressStatus = 0;

#ifndef _DEBUG
	if (m_bMatch)
		return TRUE;
#endif

	m_bRunning = TRUE;
	CString strPath = PROJMANAGER->GetAppPath();
	strPath += _T("\\Comparative Report\\") + m_strName;

	CString strTempPath = strPath;
	
	m_rptList.ClearReportResult();

	std::vector<CModelToCompare *>& vModels = m_inputParam.GetModelsManager()->GetModelsList();
	std::vector<CReportToCompare>& vReports = m_inputParam.GetReportsManager()->GetReportsList();
	for(int ns = 0; ns < (int)vReports.size(); ns++)
	{
		vReports[ns].m_vstrOutput.clear();
	}
	
	//get total progress		
	int nTotalRuns = 0;
	{

		for (unsigned nModel = 0; nModel < vModels.size(); nModel++)
		{
			CModelToCompare* pCmpModel =  vModels[nModel];
			nTotalRuns += pCmpModel->GetSimResultCount();
		}

		//each report, increment 1
		nEnd = vReports.size() * nTotalRuns + vReports.size();
	}

	pWndStatus->SetProgressRange(nStart, nEnd);

	SendMessage(pWndStatus->GetSafeHwnd(), WM_COMPARE_RUN, 0, 0);
	
	for (unsigned nReport = 0; nReport < vReports.size(); nReport++)
	{
		try
		{
			CReportToCompare& reportCmp = vReports[nReport];
			if (reportCmp.GetChecked() == FALSE)
				{
					UpdateProgressBar(pWndStatus, nTotalRuns);
					continue;
			}

			for (unsigned nModel = 0; nModel < vModels.size(); nModel++)
			{	
				CModelToCompare* pCmpModel =  vModels[nModel];

				SendMessage(pWndStatus->GetSafeHwnd(), WM_COMPARE_RUN, 0, 0);
				if (m_bStop)
					break;

				if(!pCmpModel->GetChecked())
				{
					//skip all sim result
					UpdateProgressBar(pWndStatus, pCmpModel->GetSimResultCount());
					continue;
				}

				int iReportIndex = -1;
				switch(vReports[nReport].GetCategory())
				{
				case ENUM_QUEUELENGTH_REP:
					iReportIndex = 0;
					break;
				case ENUM_QUEUETIME_REP:
					iReportIndex = 1;
					break;
				case ENUM_THROUGHPUT_REP:
					iReportIndex = 2;
					break;
				case ENUM_PAXDENS_REP:
					iReportIndex = 3;
					break;
				case ENUM_PAXCOUNT_REP:
					iReportIndex = 4;
					break;
				case ENUM_ACOPERATION_REP:
					iReportIndex = 5;
					break;
				case ENUM_DURATION_REP:
					iReportIndex = 6;
					break;
				case ENUM_DISTANCE_REP:
					iReportIndex = 7;
					break;
				case ENUM_SPACETHROUGHPUT_REP:
					iReportIndex = 8;
					break;
                case ENUM_UTILIZATION_REP:
                    iReportIndex = 9;
                    break;
				default:
					break;
				}

				CReportParameter* pOutParam = NULL;
				switch (vReports[nReport].GetCategory())
				{
				case ENUM_QUEUETIME_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( 
										          new CReportParaOfPaxType( new CReportParaOfProcs( NULL) ))));
					break;
				case ENUM_PAXDENS_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaWithArea( new CReportParaOfReportType( NULL)) ) );
					break;
				case ENUM_SPACETHROUGHPUT_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfPaxType( new CReportParaWithPortal( new CReportParaOfReportType( NULL)) ) );
					break;
				case ENUM_ACOPERATION_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfPaxType( NULL) ) );
					break;
				case ENUM_PAXCOUNT_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfPaxType( NULL))));
					break;
				case ENUM_DURATION_REP:
				case ENUM_DISTANCE_REP:
					pOutParam =new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfPaxType( new CReportParaOfTwoGroupProcs( NULL ) ) ) ) );
					break;
				case ENUM_THROUGHPUT_REP: //8
				case ENUM_QUEUELENGTH_REP:
                case ENUM_UTILIZATION_REP:
					pOutParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs(new CReportParaOfPaxType(NULL)))));
					break;

				default:
					pOutParam = new CReportParaOfTime( new CReportParaOfReportType( new CReportParaOfThreshold( new CReportParaOfProcs(NULL) )));
					break;
				}
				GenerateReportParameter(vReports[nReport].GetParameter(), pOutParam,pCmpModel);
				
				int nSimResultCount = pCmpModel->GetSimResultCount();

				for (int nResult = 0; nResult < nSimResultCount; ++nResult )
				{
					if (pCmpModel->GetSimResult(nResult).GetChecked() == FALSE)
					{
						UpdateProgressBar(pWndStatus);
						continue;
					}
					
					CString strSimResult = pCmpModel->GetSimResultName(nResult);
					strStatus.Format(_T("%s%s%s%s :%s"), _T("Generating "), 
						s_szReportCategoryName[iReportIndex],
						_T(" of "), pCmpModel->GetModelName(),strSimResult);
					pWndStatus->AddLogText(strStatus);

					TransferLogFiles(pCmpModel,pCmpModel->GetLocalPath(),strSimResult,_ShowCopyInfo);
					
				
					CString projPath = pCmpModel->GetLocalPath();
					Terminal* pTerm = pCmpModel->GetTerminal();
					ASSERT(pTerm);
					ASSERT(!projPath.IsEmpty());

					pTerm->GetSimReportManager()->SetCurrentSimResult(strSimResult);
					CMutiRunReportAgent tempMultiRunAgent;					
					tempMultiRunAgent.Init(projPath, pTerm);
					tempMultiRunAgent.AddReportWhatToGen((ENUM_REPORT_TYPE)vReports[nReport].GetCategory(), pOutParam);
					tempMultiRunAgent.GenerateAll();
					CString strReportPath = pTerm->GetSimReportManager()->GetCurrentReportFileName(pCmpModel->GetLocalPath());
				
					strReportPath = SaveTempReport(strReportPath, nModel, nReport, nResult,_ShowCopyInfo);
					ASSERT(!strReportPath.IsEmpty());
					if(strReportPath.IsEmpty()) // failed to generate this report
					{
						strStatus.Empty();
						CString strSimResult = pCmpModel->GetSimResultName(nResult);
						strStatus.Format(_T("%s%s%s%s :%s"), _T("Failed to generate "), 
							s_szReportCategoryName[iReportIndex],
							_T(" of "), pCmpModel->GetModelName(),strSimResult);
						pWndStatus->AddLogText(strStatus);
					}
					AddReportPath(vReports[nReport], strReportPath);
			
					UpdateProgressBar(pWndStatus);

				}
				delete pOutParam;
				pOutParam = NULL;



				if (m_bStop)
					break;
			}

			strStatus.Empty();
			strStatus.Format(_T("Generating comparative report: %s "), vReports[nReport].GetName());
			pWndStatus->AddLogText(strStatus);
			MergeReport(vReports[nReport], strTempPath);
			UpdateProgressBar(pWndStatus);

		}
		catch( FileOpenError* pError )
		{
			MessageBox( NULL, "No Data Available", "Error", MB_OK | MB_ICONWARNING );
			delete pError;
			return FALSE;
		}
		catch( FileVersionTooNewError* _pError )
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
			delete _pError;
			return FALSE;	
		}
		catch(StringError* _pError)
		{
			AfxMessageBox("The log file is old, run simulation please!",MB_OK|MB_ICONWARNING);
			delete _pError;
			return FALSE;
		}
		
		SendMessage(pWndStatus->GetSafeHwnd(), WM_COMPARE_RUN, 0, 0);
		if (m_bStop)
			break;
	}

	pWndStatus->SetProgressPos(nEnd);
	if (m_bStop)
		strStatus = _T("Canceled");
	else
		strStatus = _T("Done.");
	
	pWndStatus->AddLogText(strStatus);

	if (!m_bStop)
		m_bMatch = TRUE;
	
	m_bStop = FALSE;
	m_bRunning = FALSE;

	return m_bMatch;
}

void CComparativeProject::AddReportPath(CReportToCompare& rptToCmpare, const CString& strPath)
{
	rptToCmpare.m_vstrOutput.push_back(strPath);
}

void CComparativeProject::MergeReport(CReportToCompare& ReportToMerge, const CString& sOutputPath)
{
	if (m_fo.CheckPath(sOutputPath + _T("\\Results")) != PATH_IS_FOLDER)
	{
		::CreateDirectory(sOutputPath + _T("\\Results"), NULL);
	}

		CCmpBaseReport* pResult = NULL;
		switch (ReportToMerge.GetCategory())
		{
		case ENUM_QUEUETIME_REP:
			pResult = new CComparativeQTimeReport;
			break;

		case ENUM_QUEUELENGTH_REP:
			pResult = new CComparativeQLengthReport;
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
		case ENUM_SPACETHROUGHPUT_REP:
			pResult = new CComparativSpaceThroughputReport;
			break;
		case ENUM_UTILIZATION_REP:
			pResult = new CComparativeProcUtilizationReport;
			break;
		default:
			ASSERT(0);
			break;
		}

		pResult->m_cmpParam = ReportToMerge.GetParameter();
		pResult->m_ReportStartTime = ReportToMerge.GetParameter().GetStartTime();
		pResult->m_ReportEndTime = ReportToMerge.GetParameter().GetEndTime();//elapsed time.

		pResult->SetCmpReportName(ReportToMerge.GetName());
		std::vector<CModelToCompare*>& vModels = m_inputParam.GetModelsManager()->GetModelsList();
		CString simName;
		for(std::vector<CModelToCompare*>::iterator itor=vModels.begin(); itor!=vModels.end(); itor++)
		{
			if ((*itor)->GetChecked() == FALSE)
				continue;

			for(int nSimResult=0; nSimResult<(*itor)->GetSimResultCount(); nSimResult++)
			{
				if((*itor)->GetSimResult(nSimResult).GetChecked() == FALSE)
					continue;

				simName = _T("");
				simName += (*itor)->GetModelName();
				simName += ("(");
				simName += (*itor)->GetSimResultName(nSimResult);
				simName += (")");
				pResult->AddSimName(simName);
			}
		}

		std::vector<std::string> vstrOutput;
		for (unsigned nReportResultFile = 0; nReportResultFile < ReportToMerge.m_vstrOutput.size(); nReportResultFile++)
		{
			// TRACE("%s\n", vReports[i].m_vstrOutput[j]);
			std::string tstr = ReportToMerge.m_vstrOutput[nReportResultFile];
			vstrOutput.push_back( tstr );
		}
		pResult->AddSamplePath(vstrOutput);

		//distance travel handle
		if (ReportToMerge.GetCategory() == ENUM_DISTANCE_REP)
		{
			long lInterval = 0;
			ReportToMerge.GetParameter().GetInterval(lInterval);
			pResult->MergeSample(lInterval);
		}
		else
		{
			pResult->MergeSample(ReportToMerge.GetParameter().GetInterval());
		}

		CString strResult = sOutputPath + _T("\\Results\\") + ReportToMerge.GetName() + _T(".txt");
		pResult->SaveReport(strResult.GetBuffer(0));
		m_rptList.AddReportResult(pResult);
	
}

CString CComparativeProject::SaveTempReport(const CString &strReportPath, int iModelIndex,int iReportIndex,int nSimResult, void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	CString strDest;
	CString strPath = PROJMANAGER->GetAppPath();
	strPath += _T("\\Comparative Report\\") + m_strName;
	strPath += _T("\\Reports");
	if (m_fo.CheckPath(strPath) != PATH_IS_FOLDER)
		::CreateDirectory(strPath, NULL);

	int nPos = strReportPath.ReverseFind(_T('\\'));
	CString strName = strReportPath.Mid(nPos + 1, strReportPath.GetLength() - nPos);
	strDest = strPath + _T("\\") + strName;

	strDest.Format(_T("%s%s%d%d%d%s"), strPath, _T("\\"), iModelIndex,nSimResult ,iReportIndex, strName);

	if(_ShowCopyInfo != NULL)
	{
		CString strMessage = _T("Saving report result ") + strName;
		(*_ShowCopyInfo)(0, strMessage);
	}
	
	if (CopyFile(strReportPath, strDest, FALSE))
		return strDest;

	ASSERT(0);
	return _T("");
}


void CComparativeProject::RemoveTempReport()
{
	CString strTempPath = PROJMANAGER->GetAppPath();
	strTempPath += _T("\\Comparative Report\\") + m_strName;
	strTempPath += _T("\\Reports");
	m_fo.SetAskIfReadOnly(false);
	m_fo.Delete(strTempPath);
}

void CComparativeProject::RemoveTempData()
{
	CFileFind finder;
	CString strPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report");
	strPath += _T("\\") + m_strName;
	BOOL bFound = finder.FindFile(strPath + _T("\\*.*"));

	m_fo.SetAskIfReadOnly(false);

	while (bFound)
	{
		bFound = finder.FindNextFile();
		if (finder.IsDirectory() && !finder.IsDots())
		{
			if (finder.GetFileName() != _T("Results"))
			{
				m_fo.Delete(finder.GetFilePath());
			}
		}
	}
}


BOOL CComparativeProject::IsLocalModel(const CString &strPath)
{
	CString str = _T("(LOCAL)");
	if (strPath.Left(2) == _T("\\\\"))
	{
		int nPos = strPath.Find(_T("\\"), 2);
		if (nPos != -1)
			str = strPath.Mid(2, nPos - 2);
		else
			str = strPath.Mid(2, strPath.GetLength() - 2);
	}

	str.MakeUpper();
	
	return ((str == _T("(LOCAL)")) ? TRUE : FALSE);
}

const CCmpReportManager& CComparativeProject::GetCompReportResultList() const
{
	return m_rptList;
}

void CComparativeProject::AddReportResult( CCmpBaseReport* pResult )
{
	m_rptList.AddReportResult(pResult);
}

void CComparativeProject::UpdateProgressBar( CCompRepLogBar* pWndStatus, int nIncrement /*= 1*/ )
{
	if(pWndStatus)
	{
		int nPos = pWndStatus->GetProgressPos();
		pWndStatus->SetProgressPos(nPos + nIncrement);
	}
}
BOOL CComparativeProject::DeleteProjectPath()
{
    CString strProjPath = PROJMANAGER->GetAppPath();
    strProjPath += _T("\\Comparative Report\\") + m_strName;
    m_fo.SetAskIfReadOnly(false);
    if(!m_fo.Delete(strProjPath))
        return FALSE;
    return TRUE;
}

BOOL CComparativeProject::RenameProjectPath(const CString& strNewName)
{
    CString strProjPath = PROJMANAGER->GetAppPath();

    CString strSrc = strProjPath + _T("\\Comparative Report\\") + m_strName;
    CString strDest = strProjPath += _T("\\Comparative Report\\") + strNewName;
    if(m_fo.Rename(strSrc, strDest))
        return TRUE;
    else
        return FALSE;
}



//////////////////////////////////////////////////////////////////////////
//	CComparativeProjectDataSet

CComparativeProjectDataSet::CComparativeProjectDataSet() : DataSet(ProjectInfoFile, 2.1f)
{
	//m_pInstance = NULL;
}

CComparativeProjectDataSet::~CComparativeProjectDataSet()
{
}

void CComparativeProjectDataSet::clear()
{
}

void CComparativeProjectDataSet::readData(ArctermFile& p_file)
{
	char buf[256];

	m_vProjs.clear();

	
	while (p_file.getLine() == 1)
	{
		CComparativeProject* proj = new CComparativeProject;

		// read the Project name;
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proj->SetName(buf);
		
		// read the project description
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proj->SetDescription(buf);

		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proj->SetUser(buf);

		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			proj->SetServerName(buf);
		
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
		{
			char* b = buf;
			char* p = NULL;
			int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;
			int c = 1;
			while((p = strchr(b, ' ')) != NULL) {
				*p = '\0';
				switch(c)
				{
				case 1: //year
					nYear = atoi(b);
					break;
				case 2: //month
					nMonth = atoi(b);
					break;
				case 3: //day
					nDay = atoi(b);
					break;
				case 4: //hour
					nHour = atoi(b);
					break;
				case 5: //min
					nMin = atoi(b);
					break;
				}
				b = ++p;
				c++;
			}
			nSec = atoi(b);
			CTime t(nYear, nMonth, nDay, nHour, nMin, nSec);
			proj->SetCreatedTime(t);
		}

		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
		{
			char* b = buf;
			char* p = NULL;
			int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;
			int c = 1;
			while((p = strchr(b, ' ')) != NULL) {
				*p = '\0';
				switch(c)
				{
				case 1: //year
					nYear = atoi(b);
					break;
				case 2: //month
					nMonth = atoi(b);
					break;
				case 3: //day
					nDay = atoi(b);
					break;
				case 4: //hour
					nHour = atoi(b);
					break;
				case 5: //min
					nMin = atoi(b);
					break;
				}
				b = ++p;
				c++;
			}
			nSec = atoi(b);
			CTime t(nYear, nMonth, nDay, nHour, nMin, nSec);
			proj->SetLastModifiedTime(t);
		}

		m_vProjs.push_back(proj);
	}
}

void CComparativeProjectDataSet::readObsoleteData(ArctermFile& p_file)
{
}

void CComparativeProjectDataSet::writeData(ArctermFile& p_file) const
{
	for (unsigned i = 0; i < m_vProjs.size(); i++)
	{
		p_file.writeField(m_vProjs[i]->GetName());
		p_file.writeField(m_vProjs[i]->GetDescription());
		p_file.writeField(m_vProjs[i]->GetUser());
		p_file.writeField(m_vProjs[i]->GetServerName());
		CTime tm = m_vProjs[i]->GetCreatedTime();
		p_file.writeField(tm.Format("%Y %m %d %H %M %S"));
		tm = m_vProjs[i]->GetLastModifiedTime();
		p_file.writeField(tm.Format("%Y %m %d %H %M %S"));
		p_file.writeInt(m_vProjs[i]->IsMatch());
		p_file.writeLine();
	}
	
	if (m_vProjs.empty())
		p_file.writeLine();
}

int CComparativeProjectDataSet::GetProjects(OUT std::vector<CComparativeProject *>& vProjs)
{
	vProjs = m_vProjs;
	return m_vProjs.size();
}

void CComparativeProjectDataSet::SetProjects(const std::vector<CComparativeProject *>& vProjs)
{
	m_vProjs = vProjs;
}

void CComparativeProjectDataSet::AddProject(CComparativeProject* proj)
{
	m_vProjs.push_back(proj);
}

CComparativeProjectDataSet* CComparativeProjectDataSet::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CComparativeProjectDataSet();
		CString strAppPath = PROJMANAGER->GetAppPath();
		m_pInstance->loadDataSet(strAppPath);
	}
	return m_pInstance;
}

void CComparativeProjectDataSet::DeleteInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

int CComparativeProjectDataSet::getProjectCount() const
{
	return static_cast<int>(m_vProjs.size());
}

CComparativeProject * CComparativeProjectDataSet::getCmpProject( int nIndex )
{
	ASSERT(nIndex >= 0 && nIndex < getProjectCount());
	if(nIndex >= 0 && nIndex < getProjectCount())
	{
		return m_vProjs.at(nIndex);
	}
	return NULL;
}

CComparativeProject* CComparativeProjectDataSet::FindCmpProjectByName(const CString& strName)
{
    for(size_t i=0; i<m_vProjs.size(); i++)
    {
        CComparativeProject* pProject = m_vProjs.at(i);
        ASSERT(pProject != NULL);
        if (strName.CompareNoCase(pProject->GetName()) == 0)
        {
            return pProject;
        }
    }
    return NULL;
}

CComparativeProject * CComparativeProjectDataSet::AddNewProject( const CString& strName, const CString& strDesc )
{
    ASSERT(FindCmpProjectByName(strName) == NULL);
	CComparativeProject* pNewProj = new CComparativeProject;

	pNewProj->SetName(strName);
	pNewProj->SetDescription(strDesc);

	TCHAR szName[128];
	DWORD dwBufferLen = 128;
	::GetUserName(szName, &dwBufferLen);
	pNewProj->SetUser(szName);
	::GetComputerName(szName, &dwBufferLen);
	pNewProj->SetServerName(szName);
	pNewProj->SetCreatedTime(CTime::GetCurrentTime());
	pNewProj->SetLastModifiedTime(CTime::GetCurrentTime());
	m_vProjs.push_back(pNewProj);

	return pNewProj;
}

bool CComparativeProjectDataSet::DeleteProjectByName(const CString& strProjName)
{
    std::vector<CComparativeProject *>::iterator itor = m_vProjs.begin();
    for(; itor!=m_vProjs.end(); ++itor)
    {
        if(strProjName.CompareNoCase((*itor)->GetName()) == 0)
        {
            (*itor)->DeleteProjectPath();
            delete (*itor);
            m_vProjs.erase(itor);
            return true;
        }
    }
    return false;
}
