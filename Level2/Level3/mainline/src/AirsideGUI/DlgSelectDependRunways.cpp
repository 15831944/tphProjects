// DlgSelectDependRunways.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include ".\dlgselectdependrunways.h"


// CDlgSelectDependRunways dialog

IMPLEMENT_DYNAMIC(CDlgSelectDependRunways, CDialog)
CDlgSelectDependRunways::CDlgSelectDependRunways(std::vector<CString> &vAllRunways,
												 std::vector<CString>& vSelectRunway,
												 CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectDependRunways::IDD, pParent)
{
	m_vAllRunways = vAllRunways;
	m_vSelectedRunways = vSelectRunway;
}

CDlgSelectDependRunways::~CDlgSelectDependRunways()
{
}

void CDlgSelectDependRunways::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALLRUNWAYS, m_listAllRunways);
	DDX_Control(pDX, IDC_LIST_SELECTEDRUNWAYS, m_listSelectedRunways);
}


BEGIN_MESSAGE_MAP(CDlgSelectDependRunways, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDRUNWAYS, OnBnClickedButtonAddrunways)
	ON_BN_CLICKED(IDC_BUTTON_DELRUNWAYS, OnBnClickedButtonDelrunways)
END_MESSAGE_MAP()


// CDlgSelectDependRunways message handlers

void CDlgSelectDependRunways::OnBnClickedButtonAddrunways()
{
	int nIndex = m_listAllRunways.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CString strAddFix;
	m_listAllRunways.GetText(nIndex, strAddFix);
	m_listAllRunways.DeleteString(nIndex);

	m_listSelectedRunways.AddString(strAddFix);
}

void CDlgSelectDependRunways::OnBnClickedButtonDelrunways()
{
	int nIndex = m_listSelectedRunways.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CString strDelFix;
	m_listSelectedRunways.GetText(nIndex, strDelFix);
	m_listSelectedRunways.DeleteString(nIndex);
	m_listAllRunways.AddString(strDelFix);
}

BOOL CDlgSelectDependRunways::OnInitDialog()
{
	CDialog::OnInitDialog();

	//initialize list control
	int nCount = static_cast<int>(m_vAllRunways.size());

	for (int i = 0 ; i <nCount ; i++)
	{
		CString  strName= m_vAllRunways[i];
		if (IsSelected(strName))
			m_listSelectedRunways.AddString(strName);
		else
			m_listAllRunways.AddString(strName);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgSelectDependRunways::IsSelected(CString& strName)
{
	int nSize = static_cast<int>(m_vSelectedRunways.size());
	for (int i = 0; i< nSize ; i++) 
	{
		if (strName.CompareNoCase(m_vSelectedRunways[i]) == 0)
			return true;
	}
	return false;
}
void CDlgSelectDependRunways::OnOK()
{
	m_vSelectedRunways.clear();
	int nCount = m_listSelectedRunways.GetCount();
	for (int i = 0 ;i <nCount; i++)
	{
		CString strText;
		m_listSelectedRunways.GetText(i,strText);
		m_vSelectedRunways.push_back(strText);
	}

	CDialog::OnOK();
}
