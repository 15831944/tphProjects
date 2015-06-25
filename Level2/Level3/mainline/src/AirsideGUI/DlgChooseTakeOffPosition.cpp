// CDlgChooseTakeOffPosition.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgChooseTakeoffPosition.h"
#include ".\dlgchoosetakeoffposition.h"

// CDlgChooseTakeOffPosition dialog

IMPLEMENT_DYNAMIC(CDlgChooseTakeOffPosition, CDialog)
CDlgChooseTakeOffPosition::CDlgChooseTakeOffPosition(TakeoffSequencing * pTakeOffSeq ,CWnd* pParent /*=NULL*/)
: CDialog(CDlgChooseTakeOffPosition::IDD, pParent) 
,m_pTakeOffSeq(pTakeOffSeq)
{ 
}

CDlgChooseTakeOffPosition::~CDlgChooseTakeOffPosition()
{
}

void CDlgChooseTakeOffPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAKEOFFPOSI, m_wndTakeoffPosiTree);
}


BEGIN_MESSAGE_MAP(CDlgChooseTakeOffPosition, CDialog)

END_MESSAGE_MAP()
// CDlgSelectTakeoffPosi message handlers

BOOL CDlgChooseTakeOffPosition::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
//	m_wndTakeoffPosiTree.EnableMultiSelect();
	SetTreeContents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChooseTakeOffPosition::SetTreeContents(void)
{
	m_wndTakeoffPosiTree.SetRedraw(FALSE);
	m_wndTakeoffPosiTree.DeleteAllItems();
	if(!m_pTakeOffSeq)return;
	int nNodeCount = m_pTakeOffSeq->GetTakeOffPositionCount();
	for (int i = 0;i < nNodeCount;++i) 
	{
		TakeOffPositionInfo * pTakeOffPositionInfo = m_pTakeOffSeq->GetTakeOffPositionInfo(i);
		if(pTakeOffPositionInfo){
			HTREEITEM hTakeOffPosInofNode = m_wndTakeoffPosiTree.InsertItem(pTakeOffPositionInfo->m_strTakeOffPositionName);
			m_wndTakeoffPosiTree.SetItemData(hTakeOffPosInofNode,(DWORD_PTR)pTakeOffPositionInfo);
		}
	}
	m_wndTakeoffPosiTree.SetRedraw(TRUE);
}
void CDlgChooseTakeOffPosition::OnOK()
{
//	int nSelCount = m_wndTakeoffPosiTree.GetSelectedCount();
	HTREEITEM hSelItem = m_wndTakeoffPosiTree.GetSelectedItem();
//	for (int i = 0; i < nSelCount;++i) 
	{	
		m_vrSelItemInfo.push_back((TakeOffPositionInfo *)(m_wndTakeoffPosiTree.GetItemData(hSelItem)) );
		//hSelItem = m_wndTakeoffPosiTree.GetNextSelectedItem(hSelItem);
	}
	CDialog::OnOK();
}


