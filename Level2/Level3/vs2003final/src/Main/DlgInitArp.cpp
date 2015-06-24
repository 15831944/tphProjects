// DlgInitArp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgInitArp.h"
#include "TermPlanDoc.h"
#include "../Inputs/AirsideInput.h"
#include "TVNode.h"
#include "ModelingSequenceManager.h"
#include "AirSideMSImpl.h"

// CDlgInitArp dialog

IMPLEMENT_DYNAMIC(CDlgInitArp, CDialog)
CDlgInitArp::CDlgInitArp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInitArp::IDD, pParent)
{
//	m_dElevation = 0L;
	m_bDefineWayPointLL = false;
}

CDlgInitArp::~CDlgInitArp()
{
}

void CDlgInitArp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInitArp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInitArp message handlers

BOOL CDlgInitArp::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rectLongitude, rectLatitude;
	GetDlgItem(IDC_LONGITUDE)->GetWindowRect(&rectLongitude);
	GetDlgItem(IDC_LATITUDE)->GetWindowRect(&rectLatitude);
	ScreenToClient(rectLongitude);
	ScreenToClient(rectLatitude);

	//	GetDlgItem(IDC_LONGITUDE)->MoveWindow(CRect(0,0,0,0));
	//	GetDlgItem(IDC_LATITUDE)->MoveWindow(CRect(0,0,0,0));
	GetDlgItem(IDC_LONGITUDE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LATITUDE)->ShowWindow(SW_HIDE);

	m_pLongitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 10);
	m_pLatitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 11);
	m_pLongitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLatitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLongitude.SetWindowPos(&CWnd::wndTop, rectLongitude.left, rectLongitude.top,
		rectLongitude.Width(), rectLongitude.Height(), SWP_SHOWWINDOW);
	m_pLatitude.SetWindowPos(&CWnd::wndTop, rectLatitude.left, rectLatitude.top, 
		rectLatitude.Width(), rectLatitude.Height(), SWP_SHOWWINDOW);

	m_pLongitude.SetComboBoxItem("E");
	m_pLongitude.SetComboBoxItem("W");
	m_pLongitude.SetItemRange(180);
	m_pLongitude.SetCtrlText(m_strLongitude);

	m_pLatitude.SetComboBoxItem("N");
	m_pLatitude.SetComboBoxItem("S");
	m_pLatitude.SetItemRange(90);
	m_pLatitude.SetCtrlText(m_strLatitude);
	m_pLatitude.m_Addr[1].SetLimitText(2);

	if(m_bDefineWayPointLL) SetWindowText("Define WayPoint Lat/Long");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInitArp::OnBnClickedOk()
{
	CString strLongitude,strLatitude;
	m_pLongitude.GetCtrlText(m_strLongitude);
	m_pLatitude.GetCtrlText(m_strLatitude);
	m_lLongitude = m_pLongitude.GetCtrlValue();
	m_lLatitude	 =	m_pLatitude.GetCtrlValue();
//	CString strElevation;
//	GetDlgItem(IDC_EDIT_ELEVATION)->GetWindowText(strElevation);
//	m_dElevation = atol(strElevation);
	OnOK();
}
