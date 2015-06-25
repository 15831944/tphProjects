// DlgSelectTakeoffPosi.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgSelectTakeoffPosi.h"
#include ".\dlgselecttakeoffposi.h"


// CDlgSelectTakeoffPosi dialog

IMPLEMENT_DYNAMIC(CDlgSelectTakeoffPosi, CDialog)
CDlgSelectTakeoffPosi::CDlgSelectTakeoffPosi(TakeoffQueues* pTakeoffQueues,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectTakeoffPosi::IDD, pParent)
	, m_pTakeoffQueues(pTakeoffQueues)
{

	for (int i=0; i<m_pTakeoffQueues->GetTakeoffQueuePosiCount(); i++)
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = m_pTakeoffQueues->GetItem(i);
		AddToMap(pTakeoffQueuePosi);	
	}
}

CDlgSelectTakeoffPosi::~CDlgSelectTakeoffPosi()
{
}

void CDlgSelectTakeoffPosi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAKEOFFPOSI, m_wndTakeoffPosiTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectTakeoffPosi, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TAKEOFFPOSI, OnTvnSelchangedTreeTakeoffposi)
END_MESSAGE_MAP()
// CDlgSelectTakeoffPosi message handlers


void CDlgSelectTakeoffPosi::AddToMap(TakeoffQueuePosi* pItem)
{
	CString strRunTaxiway;// = pItem->GetStrRunTaxiway();
	CString strRunwayMark, strTaxiwayName;
	int n = strRunTaxiway.Find("&");
	strRunwayMark = strRunTaxiway.Left(n);
	strTaxiwayName = strRunTaxiway.Right(strRunTaxiway.GetLength()-n-1);

	TakeoffQueuesPosiMapIter iter = m_TakeoffQueuesPosiMap.begin();
	for(; iter != m_TakeoffQueuesPosiMap.end(); iter++)
	{
		if(iter->first == strRunwayMark)
		break;
	}

	//TakeoffQueuesPosiMapIter iter = 
	//	std::find(m_TakeoffQueuesPosiMap.begin(),m_TakeoffQueuesPosiMap.end(), pItem);
	//if (iter == m_TakeoffQueuesPosiMap.end())
	//	return;

	if (iter == m_TakeoffQueuesPosiMap.end())
	{
		TakeoffQueuesTaxiVector vec;
		vec.push_back(strTaxiwayName);

		m_TakeoffQueuesPosiMap.insert(std::make_pair(strRunwayMark,vec));
	}
	else
	{
		TakeoffQueuesTaxiVector& vec = iter->second;
		vec.push_back(strTaxiwayName);
	}
}

BOOL CDlgSelectTakeoffPosi::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTreeContents();
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgSelectTakeoffPosi::SetTreeContents()
{
	TakeoffQueuesPosiMapIter iter = m_TakeoffQueuesPosiMap.begin();
	for(; iter != m_TakeoffQueuesPosiMap.end(); iter++)
	{
		HTREEITEM hRunMark = m_wndTakeoffPosiTree.InsertItem(iter->first);

		LPCSTR pstr = iter->first;
		m_wndTakeoffPosiTree.SetItemData(hRunMark, (DWORD_PTR)pstr);

		TakeoffQueuesTaxiVector& vec = iter->second;
		TakeoffQueuesTaxiIter iterTaxi = vec.begin();
		for(; iterTaxi != vec.end(); iterTaxi++)
		{
			HTREEITEM hTaxiway = m_wndTakeoffPosiTree.InsertItem(*iterTaxi,hRunMark);
			pstr = *iterTaxi;
			m_wndTakeoffPosiTree.SetItemData(hTaxiway, (DWORD_PTR)pstr);
		}
		m_wndTakeoffPosiTree.Expand(hRunMark, TVE_EXPAND);
	}
}

void CDlgSelectTakeoffPosi::OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndTakeoffPosiTree.GetSelectedItem();
	if (hSelItem != NULL && IsTaxiwayItem(hSelItem))
	{
		LPCSTR pstrTaxiwayName = (LPCSTR)(m_wndTakeoffPosiTree.GetItemData(hSelItem));
	
		HTREEITEM hRunMarkItem = m_wndTakeoffPosiTree.GetParentItem(hSelItem);
		LPCSTR pstrRunwayMark = (LPCSTR)(m_wndTakeoffPosiTree.GetItemData(hRunMarkItem));
		m_strRunTaxiway.Format("%s&%s",pstrRunwayMark, pstrTaxiwayName);
		GetDlgItem(IDOK)->EnableWindow();
	}
	else
		GetDlgItem(IDOK)->EnableWindow(FALSE);

	*pResult = 0;
}
bool CDlgSelectTakeoffPosi::IsTaxiwayItem(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_wndTakeoffPosiTree.GetParentItem(hItem);
	return (hParentItem != NULL);
}
