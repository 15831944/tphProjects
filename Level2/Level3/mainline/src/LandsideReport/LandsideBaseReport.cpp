#include "StdAfx.h"
#include ".\landsidebasereport.h"
#include "LandsideBaseParam.h"
#include "LandsideBaseResult.h"
#include "../Common/exeption.h"
#include "../Common/fileman.h"
#include "../Common/Termfile.h"



LandsideBaseReport::LandsideBaseReport(void)
:m_pParemeter(NULL)
,m_pResult(NULL)
{
}

LandsideBaseReport::~LandsideBaseReport(void)
{
	if (m_pParemeter)
	{
		delete m_pParemeter;
		m_pParemeter = NULL;
	}
}	
CString LandsideBaseReport::GetReportFilePath()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\%s"),m_strReportFilePath,GetReportFileName());
	return strFilePath;
}
//---------------------------------------------------------------------------------
//Summary:
//		retrieve passenger log path
//Parameter:
//		pFunc: pointer of function to get path of log file
//---------------------------------------------------------------------------------
void LandsideBaseReport::SetCBGetLogFilePath(CBGetLogFilePath pFunc)
{
	m_pGetLogFilePath = pFunc;
}
CString LandsideBaseReport::GetReportFileName()
{
	return "";
}

BOOL LandsideBaseReport::WriteReportData(ArctermFile& _file)
{
	if(m_pResult)
		return m_pResult->WriteReportData(_file);

	return FALSE;
}

BOOL LandsideBaseReport::ReadReportData(ArctermFile& _file)
{
	if(m_pResult)
		return m_pResult->ReadReportData(_file);

	return FALSE;
}
BOOL LandsideBaseReport::LoadReportData()
{
	CString strFilrName = GetReportFilePath();

	ASSERT(m_pParemeter);
	if (m_pParemeter)
	{
		m_pParemeter->SetReportFileDir(m_strReportFilePath);
		m_pParemeter->LoadParameterFile();
	}
	
	return LoadReportData( strFilrName );
}

BOOL LandsideBaseReport::LoadReportData( const CString& strReportFile )
{
	ArctermFile file;
	try 
	{
		file.openFile (strReportFile, READ);
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		return FALSE;
	}
	ReadReportData(file);
	
	file.closeIn();

	return TRUE;
}
BOOL LandsideBaseReport::SaveReportData()
{
	CString strFilrName = GetReportFilePath();
	int nIndex = strFilrName.ReverseFind('\\');
	if(nIndex <= 0)
		return FALSE;


	CString strFolder = strFilrName.Left(nIndex);

	FileManager::MakePath(strFolder);
	ArctermFile arcFile;
	arcFile.openFile(strFilrName,WRITE);

	WriteReportData (arcFile);

	arcFile.endFile();

	return TRUE;
}

void LandsideBaseReport::Draw3DChart( CARC3DChart& chartWnd)
{
	ASSERT(m_pParemeter);
	if(m_pResult && m_pParemeter)
		m_pResult->Draw3DChart(chartWnd,m_pParemeter);
}

void LandsideBaseReport::Initialize( ProjectCommon *pCommonData, const CString& strReportPath )
{
	ASSERT(m_pParemeter);
	if(m_pParemeter)
	{
		m_pParemeter->SetCommonData(pCommonData);
	}

	m_strReportFilePath = strReportPath;
}

LSGraphChartTypeList LandsideBaseReport::GetChartList() const
{
	if(m_pResult)
		return m_pResult->GetChartList();

	return LSGraphChartTypeList();
}

void LandsideBaseReport::InitListHead( CListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	if(m_pResult)
		m_pResult->InitListHead(cxListCtrl,m_pParemeter,piSHC);
}

void LandsideBaseReport::FillListContent( CListCtrl& cxListCtrl )
{
	if(m_pResult)
		m_pResult->FillListContent(cxListCtrl,m_pParemeter);

}






















