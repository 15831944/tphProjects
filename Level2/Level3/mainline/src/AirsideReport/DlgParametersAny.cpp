// DlgParametersAny.cpp : implementation file
//

#include "stdafx.h"
#include "../AirsideReport/Resource.h"
#include "DlgParametersAny.h"
#include ".\dlgparametersany.h"


// CDlgParametersAny dialog
static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgParametersAny, CDialog)
CDlgParametersAny::CDlgParametersAny(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_REPORT_ANYEXT, pParent)
{
}

CDlgParametersAny::~CDlgParametersAny()
{
}

void CDlgParametersAny::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANY, m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgParametersAny, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDlgParametersAny message handlers

BOOL CDlgParametersAny::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_wndListBox.AddString(_T("DEFAULT"));

	InitToolBar();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgParametersAny::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL);

	return 0;
}

void CDlgParametersAny::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);
	toolbar.EnableButton(ID_NEW_ITEM,FALSE);
	toolbar.EnableButton(ID_DEL_ITEM,FALSE);
	toolbar.EnableButton(ID_EDIT_ITEM,FALSE);
}
