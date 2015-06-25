// DlgTowingRouteFromAndTo.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgTowingRouteFromAndTo.h"
#include ".\dlgtowingroutefromandto.h"
#include "DlgTowingRouteStandfamilyAndTemporaryParking.h"


// CDlgTowingRouteFromAndTo dialog

IMPLEMENT_DYNAMIC(CDlgTowingRouteFromAndTo, CDialog)
CDlgTowingRouteFromAndTo::CDlgTowingRouteFromAndTo(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTowingRouteFromAndTo::IDD, pParent)
	, m_nProjID(nProjID)
	, m_strFrom(_T(""))
	, m_strTo(_T(""))
{
}

CDlgTowingRouteFromAndTo::~CDlgTowingRouteFromAndTo()
{
}

void CDlgTowingRouteFromAndTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM, m_strFrom);
	DDX_Text(pDX, IDC_EDIT_TO, m_strTo);
	DDX_Control(pDX, IDC_BUTTON_FROM, m_buttonFrom);
	DDX_Control(pDX, IDC_BUTTON_TO, m_buttonTo);
}


BEGIN_MESSAGE_MAP(CDlgTowingRouteFromAndTo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FROM, OnBnClickedButtonFrom)
	ON_BN_CLICKED(IDC_BUTTON_TO, OnBnClickedButtonTo)
END_MESSAGE_MAP()


// CDlgTowingRouteFromAndTo message handlers

void CDlgTowingRouteFromAndTo::OnBnClickedButtonFrom()
{
	// TODO: Add your control notification handler code here
	CDlgTowingRouteStandfamilyAndTemporaryParking dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	m_strFrom = dlg.GetSelALTObjectName();
	UpdateData(FALSE);
	m_nFromFlag = dlg.GetSelALTObjectFlag();
	m_nFromID = dlg.GetSelALTObjectID();
}

void CDlgTowingRouteFromAndTo::OnBnClickedButtonTo()
{
	// TODO: Add your control notification handler code here
	CDlgTowingRouteStandfamilyAndTemporaryParking dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	m_strTo = dlg.GetSelALTObjectName();
	UpdateData(FALSE);
	m_nToFlag = dlg.GetSelALTObjectFlag();
	m_nToID = dlg.GetSelALTObjectID();
}
