// DlgLandsideLayoutOptions.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideLayoutOptions.h"
#include ".\dlglandsidelayoutoptions.h"


// CDlgLandsideLayoutOptions dialog

IMPLEMENT_DYNAMIC(CDlgLandsideLayoutOptions, CDialog)
CDlgLandsideLayoutOptions::CDlgLandsideLayoutOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLandsideLayoutOptions::IDD, pParent)
{
}

CDlgLandsideLayoutOptions::~CDlgLandsideLayoutOptions()
{
}

void CDlgLandsideLayoutOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_DATA,m_wndComboxCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLandsideLayoutOptions, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA, OnCbnSelchangeComboData)
END_MESSAGE_MAP()

// CDlgLandsideLayoutOptions message handlers
BOOL CDlgLandsideLayoutOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Read Data
	m_layoutOptions.ReadData(-1);

	//init combox data
	InitCombox();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgLandsideLayoutOptions::OnOK()
{
	int nSel = m_wndComboxCtrl.GetCurSel();
	if (nSel == LB_ERR)
	{
		MessageBox(_T("Please Select Layout Options!!!"),_T("Warnning"),MB_OK);
		return;
	}
	
	m_layoutOptions.SetLayoutOptions(LandsideLayoutOptions::LAYOUTOPTION(nSel));
	try
	{
		CADODatabase::BeginTransaction();
		m_layoutOptions.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CDialog::OnOK();
}

void CDlgLandsideLayoutOptions::InitCombox()
{
	m_wndComboxCtrl.AddString(_T("Left Hand"));
	m_wndComboxCtrl.AddString(_T("Right Hand"));

	int nCurSel = (int)m_layoutOptions.GetLayoutOptions();
	m_wndComboxCtrl.SetCurSel(nCurSel);
}

void CDlgLandsideLayoutOptions::OnCbnSelchangeComboData()
{
	// TODO: Add your control notification handler code here
}
