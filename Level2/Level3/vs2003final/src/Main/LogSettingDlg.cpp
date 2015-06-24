// LogSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LogSettingDlg.h"
#include ".\logsettingdlg.h"

#include "..\common\EchoSystem.h"

// CLogSettingDlg dialog

IMPLEMENT_DYNAMIC(CLogSettingDlg, CDialog)
CLogSettingDlg::CLogSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogSettingDlg::IDD, pParent)
{
	
}

CLogSettingDlg::~CLogSettingDlg()
{
}

void CLogSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOGS, m_lbLogs);
}


BEGIN_MESSAGE_MAP(CLogSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_MASTERON, OnBnClickedRadioMasteron)
	ON_BN_CLICKED(IDC_RADIO_MASTEROFF, OnBnClickedRadioMasteroff)
END_MESSAGE_MAP()


// CLogSettingDlg message handlers

BOOL CLogSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//get pointers to the radio button
	pm_MasterOn = (CButton*)GetDlgItem(IDC_RADIO_MASTERON);
	pm_MasterOff = (CButton*)GetDlgItem(IDC_RADIO_MASTEROFF);

	InitList();

	return TRUE;
}

void CLogSettingDlg::InitList()
{
	LoadData();//load the data

	bool bEnable = ECHOLOG->IsEnable();
	if ( bEnable )
	{
		pm_MasterOn->SetCheck(BST_CHECKED);
	}
	else
	{
		pm_MasterOff->SetCheck(BST_CHECKED);
	}
	
	std::vector< CEchoSystem::echoSetting>  &logData = ECHOLOG->GetLogTypeData();
	std::vector< CEchoSystem::echoSetting>::const_iterator it = logData.begin();

	for (int i = 0; i < ECHOLOG->GetLogTypeCount() && it != logData.end(); i++,it++)
	{
		CString strTextType;
		strTextType.Format(" %s", ECHOLOG->GetStringByLogType(EchoLogType(i)));

		m_lbLogs.AddString(strTextType);
		m_lbLogs.SetCheck(i, (*it).bEnable ? 1 : 0);
		m_lbLogs.Enable(i, bEnable);
	}

	m_lbLogs.UpdateWindow();

}

void CLogSettingDlg::LoadData()
{
	ECHOLOG->loadDataSet();
}

void CLogSettingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
		
	std::vector< CEchoSystem::echoSetting>  &LogData = ECHOLOG->GetLogTypeData();
	std::vector< CEchoSystem::echoSetting>  ::const_iterator it = LogData.begin();

	for (int i = 0; i < ECHOLOG->GetLogTypeCount() && it != LogData.end(); i++,it++)
	{
		LogData[i].bEnable = (1 == m_lbLogs.GetCheck( i )) ? true : false;
	}

	ECHOLOG->saveDataSet(BST_CHECKED==pm_MasterOn->GetCheck() ? true : false);

	OnOK();
}

void CLogSettingDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here

	LoadData();
	OnCancel();
}


void CLogSettingDlg::OnBnClickedRadioMasteron()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_lbLogs.GetCount(); i++)
	{
		m_lbLogs.Enable(i, true);
	}
}

void CLogSettingDlg::OnBnClickedRadioMasteroff()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_lbLogs.GetCount(); i++)
	{
		m_lbLogs.Enable(i, false);
	}
}
