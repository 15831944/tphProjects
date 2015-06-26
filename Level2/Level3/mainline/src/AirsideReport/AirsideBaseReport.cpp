#include "StdAfx.h"
#include "AirsideBaseReport.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/IntersectionNode.h"
#include "../Common/exeption.h"
#include "../Common/fileman.h"
CAirsideBaseReport::CAirsideBaseReport(CBGetLogFilePath pFunc)
:m_pCBGetLogFilePath(pFunc),m_ListCtrl(NULL),m_HasGenerate(FALSE)
{
}

CAirsideBaseReport::~CAirsideBaseReport(void)
{
}
void CAirsideBaseReport::RefreshReport(CParameters * parameter)
{

}

void CAirsideBaseReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
}

void CAirsideBaseReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{

}

CString CAirsideBaseReport::GetObjectName(int objectType,int objID)
{
	CString strObjName = _T("");
	if (objectType == 0)
	{
		ALTObject *pObject = ALTObject::ReadObjectByID(objID);
		if (pObject)
		{
			strObjName = pObject->GetObjNameString();
		}
	}
	else
	{
		IntersectionNode intersecNode;
		intersecNode.ReadData(objID);
		strObjName = intersecNode.GetName();
	}

	return strObjName;
}
BOOL CAirsideBaseReport::ExportListCtrlToCvsFile(ArctermFile& _file,CXListCtrl& cxListCtrl,CParameters * parameter)
{
	//export the lisctrl head 
	int size = cxListCtrl.GetColumns() ;
	LVCOLUMN column ;
	column.mask = LVCF_TEXT ;
	TCHAR th[1024] = {0};
	column.pszText = th ;
    column.cchTextMax = 1024 ;
	for (int i = 0 ;i < size ;i++)
	{
		cxListCtrl.GetColumn(i,&column) ;
		CString strColText(column.pszText);
		strColText.Trim();
		if(strColText.GetLength() == 0)
			strColText = _T("*");

        _file.writeField(strColText) ;
	}
	m_ListCtrl = &cxListCtrl ;
	_file.writeLine() ;
   return  ExportListData(_file,parameter) ;
}
BOOL CAirsideBaseReport::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	CString val ; 
	if(m_ListCtrl == NULL)
		return FALSE ;
	for (int line = 0 ; line < m_ListCtrl->GetItemCount() ;line++)
	{
		for (int colum = 0 ; colum < m_ListCtrl->GetColumns() ; colum++)
		{
			val = m_ListCtrl->GetItemText(line,colum) ;
			_file.writeField(val) ;
		}
		_file.writeLine() ;
	}
return TRUE;
}

void CAirsideBaseReport::SetReportFileDir( const CString& strReportPath )
{
	m_strReportFilePath = strReportPath;
}

CString CAirsideBaseReport::GetReportFilePath()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\%s"),m_strReportFilePath,GetReportFileName());
	return strFilePath;
}
CString CAirsideBaseReport::GetReportFileName()
{
	return "";
}

BOOL CAirsideBaseReport::WriteReportData(ArctermFile& _file)
{
	return FALSE;
}

BOOL CAirsideBaseReport::ReadReportData(ArctermFile& _file)
{
	return FALSE;
}
BOOL CAirsideBaseReport::LoadReportData()
{
	CString strFilrName = GetReportFilePath();

	return LoadReportData( strFilrName );
}

BOOL CAirsideBaseReport::LoadReportData( const CString& strReportFile )
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
BOOL CAirsideBaseReport::SaveReportData()
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











