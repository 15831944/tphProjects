// DlgSelMultiPaxType.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"

#include ".\dlgselmultipaxtype.h"
#include "MoblieElemDlg.h"
#include "..\inputs\MobileElemConstraint.h"
#include "PassengerTypeDialog.h"
#include "DlgSelMultiPaxType.h"
#include "..\Inputs\typelist.h"


// DlgSelMultiPaxType dialog

IMPLEMENT_DYNAMIC(DlgSelMultiPaxType, CDialog)
DlgSelMultiPaxType::DlgSelMultiPaxType(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSelMultiPaxType::IDD, pParent)
{
//	m_pTree = NULL;
	m_pInTypeList = NULL;
	m_pConstraint = NULL;
}

DlgSelMultiPaxType::~DlgSelMultiPaxType()
{
}

void DlgSelMultiPaxType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMLEVEL1,  m_allLevelCombox[0]);
	DDX_Control(pDX, IDC_COMLEVEL2,  m_allLevelCombox[1]);
	DDX_Control(pDX, IDC_COMLEVEL3,  m_allLevelCombox[2]);
	DDX_Control(pDX, IDC_COMLEVEL4,  m_allLevelCombox[3]);
	DDX_Control(pDX, IDC_COMLEVEL5,  m_allLevelCombox[4]);
	DDX_Control(pDX, IDC_COMLEVEL6,  m_allLevelCombox[5]);
	DDX_Control(pDX, IDC_COMLEVEL7,  m_allLevelCombox[6]);
	DDX_Control(pDX, IDC_COMLEVEL8,  m_allLevelCombox[7]);
	DDX_Control(pDX, IDC_COMLEVEL9,  m_allLevelCombox[8]);
	DDX_Control(pDX, IDC_COMLEVEL10, m_allLevelCombox[9]);
	DDX_Control(pDX, IDC_COMLEVEL11, m_allLevelCombox[10]);
	DDX_Control(pDX, IDC_COMLEVEL12, m_allLevelCombox[11]);
	DDX_Control(pDX, IDC_COMLEVEL13, m_allLevelCombox[12]);
	DDX_Control(pDX, IDC_COMLEVEL14, m_allLevelCombox[13]);
	DDX_Control(pDX, IDC_COMLEVEL15, m_allLevelCombox[14]);
	DDX_Control(pDX, IDC_COMLEVEL16, m_allLevelCombox[15]);
}


BEGIN_MESSAGE_MAP(DlgSelMultiPaxType, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


void DlgSelMultiPaxType::SetPointers(PassengerTypeList* pInTypeList,CMobileElemConstraint* pConstraint)
{
//	m_pTree = pTree;
	m_pInTypeList = pInTypeList;
	m_pConstraint = pConstraint;
}

BOOL DlgSelMultiPaxType::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
//	m_comLevel1.AddString("ALL");
// 	m_comLevel2.AddString("ALL");
// 	m_comLevel3.AddString("ALL");
// 	m_comLevel4.AddString("ALL");
// 	m_comLevel5.AddString("ALL");
// 	m_comLevel6.AddString("ALL");
// 	m_comLevel7.AddString("ALL");
// 	m_comLevel8.AddString("ALL");
// 	
// 	m_comLevel1.SelectString(-1,"ALL");
//	m_comLevel2.SelectString(-1,"ALL");
//	m_comLevel3.SelectString(-1,"ALL");
//	m_comLevel4.SelectString(-1,"ALL");
//	m_comLevel5.SelectString(-1,"ALL");
//	m_comLevel6.SelectString(-1,"ALL");
//	m_comLevel7.SelectString(-1,"ALL");
//	m_comLevel8.SelectString(-1,"ALL");
// 	
// 	GetDlgItem(IDC_COMLEVEL2)->EnableWindow(FALSE);
//	GetDlgItem(IDC_COMLEVEL3)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_COMLEVEL4)->EnableWindow(FALSE);
//	GetDlgItem(IDC_COMLEVEL5)->EnableWindow(FALSE);
//	GetDlgItem(IDC_COMLEVEL6)->EnableWindow(FALSE);
//	GetDlgItem(IDC_COMLEVEL7)->EnableWindow(FALSE);
//	GetDlgItem(IDC_COMLEVEL8)->EnableWindow(FALSE);

	m_allLevelCombox[0].AddString(_T("ALL"));
	m_allLevelCombox[0].SelectString(-1, _T("ALL"));
	for (int i=1;i<MULTI_PAX_TYPE_COUNT;i++)
	{
		m_allLevelCombox[i].AddString(_T("ALL"));
		m_allLevelCombox[i].SelectString(-1, _T("ALL"));
		m_allLevelCombox[i].EnableWindow(FALSE);
	}

    InsertPassengerType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DlgSelMultiPaxType::InsertPassengerType()
{
	TypeString  mhItem;
	m_TypeStrings.clear();

	for (int i = 0; i < MULTI_PAX_TYPE_COUNT; i++)
	{
		int nBranch = m_pInTypeList->getBranchesAtLevel(i);
		if (nBranch == 0)
			break;

		m_allLevelCombox[i].EnableWindow(TRUE);
		for (int j = 0; j < nBranch; j++)
		{
			m_allLevelCombox[i].AddString(m_pInTypeList->getTypeName(j,i));
			mhItem.str = m_pInTypeList->getTypeName(j,i);
			mhItem.nLevel = i;
			mhItem.nIdx = j;
			m_TypeStrings.push_back(mhItem);
		}
	}
}


void DlgSelMultiPaxType::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str = "";
	m_Sel.clear();

	for (int i=0;i<MULTI_PAX_TYPE_COUNT;i++)
	{
		int nIndex = m_allLevelCombox[i].GetCurSel();
		m_allLevelCombox[i].GetLBText(nIndex,str);
		for(size_t j = 0; j<m_TypeStrings.size(); j++)
		{
			if(str == m_TypeStrings[j].str )
			{
				m_Sel.push_back(j);
				break;
			}
		}
	}
    

	OnOK();
}
