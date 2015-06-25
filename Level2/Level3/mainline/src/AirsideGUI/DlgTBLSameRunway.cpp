// DlgTBLSameRunway.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTBLSameRunway.h"
#include "Resource.h"
#include ".\dlgtblsamerunway.h"

// CDlgTBLSameRunway dialog

IMPLEMENT_DYNAMIC(CDlgTBLSameRunway, CPropertyPage)
CDlgTBLSameRunway::CDlgTBLSameRunway(int nProjID, TakeoffClearanceItem* pItem)
	: CPropertyPage(CDlgTBLSameRunway::IDD)
	, m_nProjID(nProjID)
	, m_nDistApproach(0)
	, m_nTimeApproach(0)
	, m_nTimeSlot(0)
	, m_nFtAfterLander(0)
{
	m_pTBLSameRunway = (TakeoffBehindLandingSameRunway*)pItem;
}

CDlgTBLSameRunway::~CDlgTBLSameRunway()
{
}

void CDlgTBLSameRunway::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_nDistApproach);
	DDX_Text(pDX, IDC_EDIT4, m_nTimeApproach);
	DDX_Text(pDX, IDC_EDIT5, m_nTimeSlot);
	DDX_Text(pDX, IDC_EDIT1, m_nFtAfterLander);
}

void CDlgTBLSameRunway::InitEditBox()
{
	m_nDistApproach = m_pTBLSameRunway->GetDistTakeoffApproach();
	m_nTimeApproach = m_pTBLSameRunway->GetTimeTakeoffAproach();
	m_nTimeSlot = m_pTBLSameRunway->GetTimeTakeoffSlot();
	m_nFtAfterLander =  m_pTBLSameRunway->GetFtLanderDownRunway();

	if(m_pTBLSameRunway->IsDistTakeoffApproach())
		CheckDlgButton( IDC_RADIO5, TRUE );
	else
		CheckDlgButton( IDC_RADIO6, TRUE );

	UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CDlgTBLSameRunway, CPropertyPage)
END_MESSAGE_MAP()


// CDlgTBLSameRunway message handlers

BOOL CDlgTBLSameRunway::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitEditBox();

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN1))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN3))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN4))->SetRange(0,1000);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN5))->SetRange(0,1000);

	return TRUE;
}

void CDlgTBLSameRunway::GetValueBeforSave()
{
	UpdateData(TRUE);

	m_pTBLSameRunway->SetDistTakeoffApproach(m_nDistApproach);
	m_pTBLSameRunway->SetTimeTakeoffAproach(m_nTimeApproach);
	m_pTBLSameRunway->SetTimeTakeoffSlot(m_nTimeSlot);

	m_pTBLSameRunway->SetFtLanderDownRunway(m_nFtAfterLander);

	m_pTBLSameRunway->SetDistTakeoffApproachFlag( IsDlgButtonChecked(IDC_RADIO5) == BST_CHECKED);
}

BOOL CDlgTBLSameRunway::OnApply()
{
	GetValueBeforSave();
	return CPropertyPage::OnApply();
}
void CDlgTBLSameRunway::OnCancel()
{
	return CPropertyPage::OnCancel();
}
void CDlgTBLSameRunway::OnOK()
{
	GetValueBeforSave();
	return CPropertyPage::OnOK();
}

