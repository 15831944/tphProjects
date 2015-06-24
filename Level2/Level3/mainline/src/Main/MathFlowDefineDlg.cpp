// MathFlowDefineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MathFlowDefineDlg.h"
#include "MathFlow.h"
#include "..\Inputs\TYPELIST.H"
#include "passengertypedialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMathFlowDefineDlg dialog


CMathFlowDefineDlg::CMathFlowDefineDlg(CMathFlow* pMathFlow, CWnd* pParent)
	: CDialog(CMathFlowDefineDlg::IDD, pParent)
	, m_pMathFlow(pMathFlow)
{
	m_strFlowName = _T("");
}


void CMathFlowDefineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMathFlowDefineDlg)
	DDX_Text(pDX, IDC_MATHFLOW_NAME, m_strFlowName);
	DDX_Text(pDX, IDC_EDIT_PAXTYPE, m_strPaxType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMathFlowDefineDlg, CDialog)
	//{{AFX_MSG_MAP(CMathFlowDefineDlg)
	ON_BN_CLICKED(IDC_BTN_SELECTTYPE, OnBtnSelecttype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMathFlowDefineDlg message handlers

BOOL CMathFlowDefineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strFlowName = m_pMathFlow->GetFlowName();
	if (m_strPaxType.IsEmpty())
		m_strPaxType = _T("DEFAULT");

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMathFlowDefineDlg::OnOK() 
{
	UpdateData();
	if (m_strFlowName.IsEmpty())
	{
		AfxMessageBox(_T("You must set the Flow Name!"));
		return;
	}
	
	m_pMathFlow->SetFlowName(m_strFlowName);
	m_pMathFlow->SetPaxType(m_strPaxType);

	CDialog::OnOK();
}

void CMathFlowDefineDlg::OnBtnSelecttype() 
{
	UpdateData();
	CPassengerTypeDialog dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		CString strName;
		mobileConstr.screenPrint(strName, 0, 256);
		m_strPaxType = strName;
		strName.TrimRight(_T(" "));
		if (m_strFlowName.IsEmpty())
			m_strFlowName = strName;
		if (m_strPaxType.IsEmpty())
			m_strPaxType = strName;
		
		UpdateData(FALSE);
	}
}
