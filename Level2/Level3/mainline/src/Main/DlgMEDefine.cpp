// DlgMEDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgMEDefine.h"
#include "TermPlanDoc.h"
#include "PassengerTypeDialog.h"


// CDlgMEDefine dialog

IMPLEMENT_DYNAMIC(CDlgMEDefine, CXTResizeDialog)
CDlgMEDefine::CDlgMEDefine(CTermPlanDoc* pTermDoc,bool bGenerate,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgMEDefine::IDD, pParent)
	,m_pTermDoc(pTermDoc)
	,m_bGenerate(bGenerate)
{
}

CDlgMEDefine::~CDlgMEDefine()
{
}

void CDlgMEDefine::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgMEDefine, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_FLIGHT_ADD,OnAddMoblieType)
	ON_COMMAND(ID_FLIGHT_DEL,OnDeleteMobileType)
	ON_LBN_SELCHANGE(IDC_LIST_DATA, OnLbnSelchangeListData)
END_MESSAGE_MAP()

// CDlgMEDefine message handlers
BOOL CDlgMEDefine::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_createMeList = m_pTermDoc->GetCreateMeList();

	LoadMeContent();
	OnInitToolbar();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	UpdateToolbarStatus();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgMEDefine::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolBar.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgMEDefine::OnInitToolbar()
{
	CRect rect;
	m_wndListBox.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 2;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolBar.MoveWindow(&rect,true);
	m_wndToolBar.ShowWindow(SW_SHOW);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
}

void CDlgMEDefine::OnAddMoblieType()
{
	CPassengerTypeDialog dlg( m_pParentWnd ,FALSE, TRUE);
	//dlg.SetShowFlightIDFlag( FALSE );
	if (dlg.DoModal() == IDCANCEL)
		return;

	CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();

	m_createMeList.addConstraint(mobileConstr);
	CString strMobCon;
	mobileConstr.screenPrint(strMobCon);
	int idx = m_wndListBox.AddString(strMobCon);

	if (idx != LB_ERR)
	{
		m_wndListBox.SetCurSel(idx);
		if (m_bGenerate)
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
		
	}
	UpdateToolbarStatus();
}

void CDlgMEDefine::OnDeleteMobileType()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		return;
	}

	m_createMeList.deleteConst(nSel);
	m_wndListBox.DeleteString(nSel);
	if (m_bGenerate)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}

	if (nSel != 0)
	{
		m_wndListBox.SetCurSel(nSel - 1);
	}
	else if(m_wndListBox.GetCount())
	{
		m_wndListBox.SetCurSel(0);
	}
	UpdateToolbarStatus();
}

void CDlgMEDefine::OnOK()
{
	if (m_bGenerate)
	{
		m_pTermDoc->SetCreateMeList(m_createMeList);
	}
	else
	{
		int nSel = m_wndListBox.GetCurSel();
		if (nSel == LB_ERR)
		{
			MessageBox(_T("Please Select mobile elmenet constraint!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
			return;
		}
		m_nIdxCon =  nSel;
	}
	CXTResizeDialog::OnOK();
}

void CDlgMEDefine::LoadMeContent()
{
	for (int i = 0; i < m_createMeList.getCount(); i++)
	{
		CMobileElemConstraint* pMobCon = m_createMeList.getConstraint(i);
		CString strMobCon;
		pMobCon->screenPrint(strMobCon);
		m_wndListBox.AddString(strMobCon);
	}

	if (m_wndListBox.GetCount() != 0)
	{
		m_wndListBox.SetCurSel(0);
		UpdateToolbarStatus();
	}
}

void CDlgMEDefine::OnLbnSelchangeListData()
{
	UpdateToolbarStatus();
}

void CDlgMEDefine::UpdateToolbarStatus()
{
	int nCurIndex = m_wndListBox.GetCurSel();
	if (!m_bGenerate)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,FALSE);
		if (m_wndListBox.GetCount() == 0 || nCurIndex == -1)
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
	}
	else
	{
		if (m_wndListBox.GetCount() == 0 || nCurIndex == -1)
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,FALSE);
		}
		else
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,TRUE);
		}
	}
	
}

