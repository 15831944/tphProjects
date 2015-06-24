// LogClearingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LogClearingDlg.h"
#include ".\logclearingdlg.h"

#include "..\common\ProjectManager.h"
#include "common\exeption.h"

// CLogClearingDlg dialog

IMPLEMENT_DYNAMIC(CLogClearingDlg, CDialog)
CLogClearingDlg::CLogClearingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogClearingDlg::IDD, pParent)
{
	filePath.Format(_T("%s\\Log"), PROJMANAGER->GetAppPath());
}

CLogClearingDlg::~CLogClearingDlg()
{
}

void CLogClearingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOGFILES, m_clbLogFiles);
}


BEGIN_MESSAGE_MAP(CLogClearingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ALL, OnBnClickedButtonAll)
	ON_BN_CLICKED(IDC_BUTTON_SELECTED, OnBnClickedButtonSelected)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CLogClearingDlg message handlers

BOOL CLogClearingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLogClearingDlg::InitList()
{
	CFileFind fileFind;
	CString fileName;

	for (int i = 0; i < LogTypeCount; i++)
	{
		CString logTypeName = ECHOLOG->GetStringByLogType( (EchoLogType)i );
		fileName.Format("%s\\%s.txt", filePath, logTypeName);
		if ( fileFind.FindFile( fileName ) )
		{
			// get size of file
			CFile* pFile = NULL;
			pFile = new CFile(_T(fileName),	CFile::modeRead | CFile::shareDenyNone);
			ULONGLONG dwLength = pFile->GetLength();
			if (dwLength > 0)
			{
				CString strTextType;
				strTextType.Format(" %s", logTypeName);
				m_clbLogFiles.AddString( strTextType );
			}
			if (pFile != NULL)
			{
				pFile->Close();
				delete pFile;
			}
		}
	}
}

void CLogClearingDlg::OnBnClickedButtonAll()
{
	// TODO: Add your control notification handler code here
	CString strItemLabel;
	CString fileName;
	FILE* pFile = NULL;

	for (int i = 0; i < m_clbLogFiles.GetCount(); i++)
	{
		m_clbLogFiles.GetText(i, strItemLabel);
		strItemLabel.TrimLeft();
		fileName.Format("%s\\%s.txt", filePath, strItemLabel);
		pFile = fopen( fileName, "w" );
		if ( !pFile )
		{
			CString strInfo;
			strInfo.Format("clear %s.txt file: when open this file,there is error!", strItemLabel);
			AfxMessageBox(_T(strInfo));
			fclose( pFile );
			return;			
		}
		fflush(pFile);
		fclose( pFile );
	}
	SendMessage(WM_COMMAND, IDOK);
}

void CLogClearingDlg::OnBnClickedButtonSelected()
{
	// TODO: Add your control notification handler code here
	CString strItemLabel;
	CString fileName;
	FILE* pFile = NULL;

	for (int i = 0; i < m_clbLogFiles.GetCount(); i++)
	{
		if ( 1 == m_clbLogFiles.GetCheck( i ) )
		{
			m_clbLogFiles.GetText(i, strItemLabel);
			strItemLabel.TrimLeft();
			fileName.Format("%s\\%s.txt", filePath, strItemLabel);
			pFile = fopen( fileName, "w" );
			if ( !pFile )
			{
				CString strInfo;
				strInfo.Format("clear %s.txt file: when open this file,there is error!", strItemLabel);
				AfxMessageBox(_T(strInfo));
				fclose( pFile );
				return;			
			}
			fflush(pFile);
			fclose( pFile );
		}
	}
	SendMessage(WM_COMMAND, IDOK);
}

void CLogClearingDlg::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_COMMAND, IDOK);
}
