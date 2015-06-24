// LogDisplayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LogDisplayDlg.h"
#include ".\logdisplaydlg.h"

#include "..\common\ProjectManager.h"
#include "common\exeption.h"

// CLogDisplayDlg dialog

IMPLEMENT_DYNAMIC(CLogDisplayDlg, CDialog)
CLogDisplayDlg::CLogDisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDisplayDlg::IDD, pParent)
{
	filePath.Format(_T("%s\\Log"), PROJMANAGER->GetAppPath());
}

CLogDisplayDlg::~CLogDisplayDlg()
{
}

void CLogDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOGTYPE, m_lbLogType);
	DDX_Control(pDX, IDC_LIST_LOGDETAIL, m_lcLogDetail);
}


BEGIN_MESSAGE_MAP(CLogDisplayDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_LOGTYPE, OnLbnSelchangeListLogtype)
END_MESSAGE_MAP()


// CLogDisplayDlg message handlers

BOOL CLogDisplayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitListBox();
	InitListControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLogDisplayDlg::InitListBox()
{	
	CFileFind fileFind;
	CString fileName;

	for (int i = 0; i < LogTypeCount; i++)
	{
		CString logTypeName = ECHOLOG->GetStringByLogType( (EchoLogType)i );
		fileName.Format("%s\\%s.txt", filePath, logTypeName);
		if ( fileFind.FindFile(fileName) )
		{
			m_lbLogType.AddString( logTypeName );
		}
	}
}

void CLogDisplayDlg::InitListControl()
{
	char* columnLabel[] = {	"Date", "Time", "Airline", "ArrID", 
		"DepID ", "Origin", "Destination", "ACType" };
	int DefaultColumnWidth[] = { 80, 60, 105, 75, 75, 90, 165, 90 };
	for (int i = 0; i < 8; i++)
	{
		m_lcLogDetail.InsertColumn(i, columnLabel[i], LVCFMT_CENTER, DefaultColumnWidth[i]);
	}
}

void CLogDisplayDlg::OnLbnSelchangeListLogtype()
{
	// TODO: Add your control notification handler code here
	m_lcLogDetail.DeleteAllItems();

	CString itemLabel;
	int nCur = m_lbLogType.GetCurSel();
	m_lbLogType.GetText(nCur, itemLabel);

	DisplayLogDetails( itemLabel );
}

void CLogDisplayDlg::DisplayLogDetails(CString logTypeName)
{
	CString fileName;
	fileName.Format("%s\\%s.txt", filePath, logTypeName);

	char filename[_MAX_PATH];
	strcpy(filename, fileName);

	//open the Log file which type is key
	CLogFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		return;
	}

	char strLog[64];//save the file data 
	int nRowItem = 0;//the row number

	do
	{
		CString strDate, strTime, strAirline, strArrID; 
		CString strDepID, strOrigin, strDestination, strACType;

		file.getField(strLog,64);//Date
		strDate = (CString)strLog;

		file.getField(strLog,64);//Time
		strTime = (CString)strLog;

		file.getField(strLog,64);//Airline
		strAirline = (CString)strLog;

		file.getField(strLog,64);//ArrID
		strArrID = (CString)strLog;

		file.getField(strLog,64);//DepID
		strDepID = (CString)strLog;

		file.getField(strLog,64);//Origin
		strOrigin = (CString)strLog;

		file.getField(strLog,64);//Destination
		strDestination = (CString)strLog;

		file.getField(strLog,64);//ACType
		strACType = (CString)strLog;

		//insert the log information into the list
		m_lcLogDetail.InsertItem(nRowItem, strDate);
		m_lcLogDetail.SetItemText(nRowItem, 1, strTime);
		m_lcLogDetail.SetItemText(nRowItem, 2, strAirline);
		m_lcLogDetail.SetItemText(nRowItem, 3, strArrID);
		m_lcLogDetail.SetItemText(nRowItem, 4, strDepID);
		m_lcLogDetail.SetItemText(nRowItem, 5, strOrigin);
		m_lcLogDetail.SetItemText(nRowItem, 6, strDestination);
		m_lcLogDetail.SetItemText(nRowItem, 7, strACType);

		nRowItem++;

	} while( file.getLine() );

	file.closeOut();

}