// DlgGatArp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgGatArp.h"
#include "dlggatarp.h"
#include "TermPlanDoc.h"
#include "../Inputs/AirsideInput.h"
#include "TVNode.h"
#include "ModelingSequenceManager.h"
#include "AirSideMSImpl.h"
#include "../Inputs/AirportInfo.h"

// CDlgGatArp dialog

IMPLEMENT_DYNAMIC(CDlgGatArp, CDialog)
CDlgGatArp::CDlgGatArp(CWnd* pParent)
	: CDialog(CDlgGatArp::IDD, pParent)
{
	m_dElevation = 0;
}

CDlgGatArp::~CDlgGatArp()
{
}

void CDlgGatArp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGatArp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgGatArp message handlers

BOOL CDlgGatArp::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CTermPlanDoc *pDoc=(CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->GetTerminal().m_AirsideInput->GetLL(pDoc->m_ProjInfo.path,m_strLongitude,m_strLatitude);
	//latitude longitude
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


	//elevation
	pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->GetElevation(m_dElevation);
	CString strElevation ;
	strElevation.Format("%.0f",m_dElevation);
//	GetDlgItem(IDC_EDIT_ELEVATION)->SetWindowText(strElevation);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGatArp::OnBnClickedOk()
{
	CString strLongitude,strLatitude;
	double dElevation;
	m_pLongitude.GetCtrlText(strLongitude);
	m_pLatitude.GetCtrlText(strLatitude);
	CString strElevation;
//	GetDlgItem(IDC_EDIT_ELEVATION)->GetWindowText(strElevation);

	dElevation = atol(strElevation);
	if ((m_strLongitude.CompareNoCase(strLongitude)==0)&&(m_strLatitude.CompareNoCase(strLatitude)==0)&&m_dElevation == dElevation)
	{
	}
	else
	{
		CTermPlanDoc *pDoc=(CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		
		//save the elevation
		pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->SetElevation(dElevation);
		//save the latitude and longitude
		pDoc->GetTerminal().m_AirsideInput->SetLL(pDoc->m_ProjInfo.path,strLongitude,strLatitude,FALSE);

		CTVNode *pRefNode=pDoc->m_msManager.FindNodeByName(IDS_TVNN_AIRPORTREFERENCEPOINTS);
		if (pRefNode->GetChildCount()>1)
		{
			CNode* pNode=pRefNode->GetChildByIdx(0);
			CString strItemText;
			strItemText.Format("LAT:%s LONG:%s",strLatitude,strLongitude);
			pNode->Name(strItemText);
		}
	
	}
	OnOK();
}
