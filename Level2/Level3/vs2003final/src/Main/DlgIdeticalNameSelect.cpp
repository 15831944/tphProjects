// DlgIdeticalNameSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgIdeticalNameSelect.h"
#include ".\dlgideticalnameselect.h"


// CDlgIdeticalNameSelect dialog

IMPLEMENT_DYNAMIC(CDlgIdeticalNameSelect, CDialog)
CDlgIdeticalNameSelect::CDlgIdeticalNameSelect(const IdenticalNameList& nameList, CString strDialogTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIdeticalNameSelect::IDD, pParent)
	, m_nameList(nameList)
	, m_strTitle(strDialogTitle)
	, m_nSelIndex(-1)
{
	ASSERT(!m_strTitle.IsEmpty());
}

CDlgIdeticalNameSelect::~CDlgIdeticalNameSelect()
{
}

void CDlgIdeticalNameSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NAME, m_listNameList);
}


BEGIN_MESSAGE_MAP(CDlgIdeticalNameSelect, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_NAME, OnLbnDblclkListName)
END_MESSAGE_MAP()


// CDlgIdeticalNameSelect message handlers

BOOL CDlgIdeticalNameSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(m_strTitle);

	m_listNameList.ResetContent();
	size_t nCount = m_nameList.size();
	for(size_t i=0;i<nCount;i++)
	{
		int nIndex = m_listNameList.AddString(m_nameList[i]);
		m_listNameList.SetItemData(nIndex, i);
	}

	//------------------------------------------------------------
	// 
	CPoint pt;
	::GetCursorPos(&pt);
	CRect rc;
	GetWindowRect(rc);

	rc.MoveToXY(pt.x - rc.Width()/2, pt.y - rc.Height()/2);
	// 	ScreenToClient(rc);

	MoveWindow(rc);
	//------------------------------------------------------------


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgIdeticalNameSelect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int nSelIndex = m_listNameList.GetCurSel();
	if (LB_ERR == nSelIndex)
	{
		MessageBox(_T("Please select one item!"));
		return;
	}

	m_listNameList.GetText(nSelIndex, m_strSelName);
	m_nSelIndex = m_listNameList.GetItemData(nSelIndex);
	CDialog::OnOK();
}

void CDlgIdeticalNameSelect::OnLbnDblclkListName()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
