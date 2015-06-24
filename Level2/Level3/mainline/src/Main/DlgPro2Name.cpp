// DlgPro2Name.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "DlgPro2Name.h"
#include "Processor2.h"
#include <CommonData/Shape.h>
#include "Placement.h"
#include ".\dlgpro2name.h"

// CDlgPro2Name dialog

IMPLEMENT_DYNAMIC(CDlgPro2Name, CDialog)
CDlgPro2Name::CDlgPro2Name(CProcessor2* pProc2,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPro2Name::IDD, pParent)
	,m_pProc2(pProc2)
{
}

CDlgPro2Name::~CDlgPro2Name()
{
}

void CDlgPro2Name::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_LEVEL1,m_wndComboxLevel1);
	DDX_Control(pDX,IDC_COMBO_LEVEL2,m_wndComboxLevel2);
	DDX_Control(pDX,IDC_COMBO_LEVEL3,m_wndComboxLevel3);
	DDX_Control(pDX,IDC_COMBO_LEVEL4,m_wndComboxLevel4);
	DDX_CBString(pDX, IDC_COMBO_LEVEL1, m_csLevel1);
	DDX_CBString(pDX, IDC_COMBO_LEVEL2, m_csLevel2);
	DDX_CBString(pDX, IDC_COMBO_LEVEL3, m_csLevel3);
	DDX_CBString(pDX, IDC_COMBO_LEVEL4, m_csLevel4);
}


BEGIN_MESSAGE_MAP(CDlgPro2Name, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnDropdownComboLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnDropdownComboLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnDropdownComboLevel4)
END_MESSAGE_MAP()


// CDlgPro2Name message handlers

BOOL CDlgPro2Name::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadCombo();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}
void CDlgPro2Name::LoadCombo()
{
	m_wndComboxLevel1.ResetContent();
	m_wndComboxLevel2.ResetContent();
	m_wndComboxLevel3.ResetContent();
	m_wndComboxLevel4.ResetContent();

	// level1 
	int nCount = GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.getNodeCount();
	for(int i = 0; i < nCount; i++ )
	{
		CNudefinePro2Node* pNode = GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.getItem(i);
		m_wndComboxLevel1.AddString(pNode->getNodeName());
	}


	if( m_pProc2 == NULL)
	{
		m_wndComboxLevel1.EnableWindow();
		m_wndComboxLevel2.EnableWindow();
		m_wndComboxLevel3.EnableWindow();
		m_wndComboxLevel4.EnableWindow();
		// might need clean the test.
	}
	else
	{
		m_wndComboxLevel1.EnableWindow();
		m_wndComboxLevel2.EnableWindow();
		m_wndComboxLevel3.EnableWindow();
		m_wndComboxLevel4.EnableWindow();
		const ALTObjectID* pID = &m_pProc2->ShapeName();
		m_wndComboxLevel1.SetWindowText( pID->at(0).c_str());
		m_wndComboxLevel2.SetWindowText( pID->at(1).c_str());
		m_wndComboxLevel3.SetWindowText( pID->at(2).c_str());
		m_wndComboxLevel4.SetWindowText( pID->at(3).c_str());
	}
}
CTermPlanDoc* CDlgPro2Name::GetTermPlanDoc()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc;
}
void CDlgPro2Name::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if( m_csLevel1.IsEmpty() )
	{
		MessageBox( "Name has not been defined yet", "Error", MB_OK|MB_ICONWARNING );
		return;
	}

	CString csName = m_csLevel1;
	if( !m_csLevel2.IsEmpty() )
	{
		csName += "-" + m_csLevel2;
		if( !m_csLevel3.IsEmpty() )
		{
			csName += "-" + m_csLevel3;
			if( !m_csLevel4.IsEmpty() )
				csName += "-" + m_csLevel4;
		}
	}
	if (IsNameChanged(csName))
	{
		if (GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.IsInvalid(csName))
		{
			AfxMessageBox(_T("Undefine processor2 is invalid"));
			return;
		}
		m_pProc2->ShapeName(csName);
	}
	OnOK();
}

BOOL CDlgPro2Name::IsNameChanged(CString newName)
{
	if(m_pProc2==NULL)
		return true;

	if(newName.IsEmpty())
		return true;

	if(!(m_pProc2->ShapeName().GetIDString().CompareNoCase(newName)))
		return false;

	return true;
}

void CDlgPro2Name::OnDropdownComboLevel2()
{
	m_wndComboxLevel2.ResetContent();

	CString csLevel1;
	m_wndComboxLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	CNudefinePro2Node* pNode1 = GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.getNode(csLevel1);
	for(int i = 0; i < pNode1->getNodeCount(); i++)
	{
		CNudefinePro2Node* pNode2 = pNode1->getItem(i);
		m_wndComboxLevel2.AddString(pNode2->getNodeName());
	}
}

void CDlgPro2Name::OnDropdownComboLevel3()
{
	m_wndComboxLevel3.ResetContent();

	CString csLevel1;
	m_wndComboxLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_wndComboxLevel2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;

	CNudefinePro2Node* pNodeL1 = GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.getNode(csLevel1);
	CNudefinePro2Node* pNodeL2 = pNodeL1->getNode(csLevel2);
	for(int i = 0 ;i < pNodeL2->getNodeCount(); i++ )
	{
		CNudefinePro2Node* pNodeL3 = pNodeL2->getItem(i);
		m_wndComboxLevel3.AddString(pNodeL3->getNodeName());	
	}
}

void CDlgPro2Name::OnDropdownComboLevel4()
{
	m_wndComboxLevel4.ResetContent();

	CString csLevel1;
	m_wndComboxLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_wndComboxLevel2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_wndComboxLevel3.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;

	CNudefinePro2Node* pNodeL1 = GetTermPlanDoc()->GetTerminalPlacement()->m_vUndefined.getNode(csLevel1);
	CNudefinePro2Node* pNodeL2 = pNodeL1->getNode(csLevel2);
	CNudefinePro2Node* pNodeL3 = pNodeL2->getNode(csLevel3);
	for(int i = 0; i < pNodeL3->getNodeCount() ; i++)
	{
		CNudefinePro2Node* pNodeL4 = pNodeL3->getItem(i);
		m_wndComboxLevel4.AddString(pNodeL4->getNodeName());	
	}
}