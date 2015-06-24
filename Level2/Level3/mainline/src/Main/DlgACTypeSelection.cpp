// DlgACTypeSelection.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgACTypeSelection.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/IN_TERM.H"
#include <algorithm>
// DlgACTypeSelection dialog

IMPLEMENT_DYNAMIC(DlgACTypeSelections, CDialog)
DlgACTypeSelections::DlgACTypeSelections(InputTerminal *pTerm, CWnd* pParent /*=NULL*/)
	: CDialog(DlgACTypeSelections::IDD, pParent)
{
	if(pTerm)
		 m_pACManager = ((CAirportDatabase*)(pTerm->m_pAirportDB))->getAcMan();
	else
		m_pACManager = NULL;
	m_strSelACTypes.RemoveAll();
}

DlgACTypeSelections::~DlgACTypeSelections()
{
}

void DlgACTypeSelections::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACTYPES, m_wndListBox);
}


BEGIN_MESSAGE_MAP(DlgACTypeSelections, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL DlgACTypeSelections::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitListBox();

	return TRUE;
}

void DlgACTypeSelections::InitListBox()
{
	m_wndListBox.ResetContent();
	if (m_pACManager == NULL)
		return;
	m_wndListBox.InsertString(0,"All AC Types");
	int nCount = m_pACManager->getACTypeCount();
	std::vector<CString> vACTypeList;
	vACTypeList.clear();
	for (int i =0; i < nCount; i++)
	{
		CACType* pACType = m_pACManager->getACTypeItem(i);
		if (pACType)
		{
			vACTypeList.push_back(pACType->GetIATACode());
		}
	}

	std::sort(vACTypeList.begin(), vACTypeList.end());
	for (int j =0,nIdx = 1; j < nCount; j++)
	{
		m_wndListBox.InsertString(nIdx,vACTypeList.at(j));
		nIdx++;
	}

}

// DlgACTypeSelection message handlers
void DlgACTypeSelections::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int nSelCount = m_wndListBox.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nSelCount);
	m_wndListBox.GetSelItems(nSelCount, aryListBoxSel.GetData());

	std::vector<CString> vDelString;
	for(int i=0; i<aryListBoxSel.GetSize(); i++)
	{
		int nSelIndex = aryListBoxSel.GetAt(i);
		CString strACName = _T("");
		m_wndListBox.GetText(nSelIndex, strACName);
		m_strSelACTypes.AddTail(strACName);
	}
	OnOK();
}

void DlgACTypeSelections::OnBnClickedCancel()
{
	m_strSelACTypes.RemoveAll();
	OnCancel();
}

const CStringList& DlgACTypeSelections::getSelACTyeps()
{
	return m_strSelACTypes;
}