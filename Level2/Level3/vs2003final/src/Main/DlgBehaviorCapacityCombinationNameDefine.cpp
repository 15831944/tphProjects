// DlgBehaviorCapacityCombinationNameDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgBehaviorCapacityCombinationNameDefine.h"


// CDlgBehaviorCapacityCombinationNameDefine dialog

IMPLEMENT_DYNAMIC(CDlgBehaviorCapacityCombinationNameDefine, CDialog)
CDlgBehaviorCapacityCombinationNameDefine::CDlgBehaviorCapacityCombinationNameDefine(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBehaviorCapacityCombinationNameDefine::IDD, pParent)
{
}

CDlgBehaviorCapacityCombinationNameDefine::~CDlgBehaviorCapacityCombinationNameDefine()
{
}

void CDlgBehaviorCapacityCombinationNameDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Name);
}


BEGIN_MESSAGE_MAP(CDlgBehaviorCapacityCombinationNameDefine, CDialog)
END_MESSAGE_MAP()

void CDlgBehaviorCapacityCombinationNameDefine::OnOK()
{
	m_Name.GetWindowText(m_StrName) ;
	CDialog::OnOK() ;
}

// CDlgBehaviorCapacityCombinationNameDefine message handlers
