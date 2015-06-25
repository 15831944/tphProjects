// PickConveyorTree.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PickConveyorTree.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "TermPlanDoc.h"
#include "PaxFlowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickConveyorTree

CPickConveyorTree::CPickConveyorTree()
{
	m_nInsertType=0;
	m_pProc2=NULL;
	m_pParentWnd=NULL;
	m_bSendMessage = false;
}

CPickConveyorTree::~CPickConveyorTree()
{
	for(int l=0;l<m_arrayProcessorID.GetSize();l++)
	{
		delete m_arrayProcessorID[l];
	}
	m_arrayProcessorID.RemoveAll();
}


BEGIN_MESSAGE_MAP(CPickConveyorTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CPickConveyorTree)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickConveyorTree message handlers

BOOL CPickConveyorTree::Create(DWORD dwStyle, CWnd *pParentWnd)
{
	m_pParentWnd=pParentWnd;
	m_ImageList.Create(IDB_SMALLNODEIMAGES,16,1,RGB(255,0,255));
	BOOL bCreate=CreateEx(NULL, 
		TVS_HASBUTTONS|TVS_HASLINES |
		TVS_LINESATROOT|TVS_SHOWSELALWAYS|
		/*WS_POPUP|*/WS_BORDER|dwStyle,CRect(),
		pParentWnd,0);
	if(bCreate)
	{
		SetImageList(&m_ImageList,TVSIL_NORMAL);
		return TRUE;
	}
	return FALSE;
}

void CPickConveyorTree::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CNodeView* pNodeView = pDoc->GetNodeView();
	HTREEITEM hItemSeled=GetSelectedItem();
	ProcessorID* pProcID=(ProcessorID*)GetItemData(hItemSeled);
	if(!m_bSendMessage)
		pNodeView->m_pDlgFlow->SendMessage(UM_PCTREE_SELECT_PROC,(WPARAM)pProcID,m_nInsertType);
	
	ShowWindow(SW_HIDE);
	
	*pResult = 0;
}

void CPickConveyorTree::Init(CProcessor2 *pProc2,int nInsertType)
{
	m_pProc2=pProc2;
	m_nInsertType=nInsertType;
	DeleteAllItems();
	for(int l=0;l<m_arrayProcessorID.GetSize();l++)
	{
		delete m_arrayProcessorID[l];
	}
	m_arrayProcessorID.RemoveAll();
	Processor* pProc=pProc2->GetProcessor();
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ProcessorID	procIDNew(*(pProc->getID()));
	procIDNew.getSuperGroup();
	GroupIndex group=pDoc->GetTerminal().procList->getGroupIndex(procIDNew);
	ASSERT(group.start>=0);
	HTREEITEM hItem=TVI_ROOT;
	HTREEITEM hLeafItem=NULL;

	Processor* pProcNew=pDoc->GetTerminal().procList->getProcessor(group.start);
	int nLevel=pProcNew->getID()->idLength();
	HTREEITEM hItemSeled=NULL;
	for(int j=0;j<nLevel;j++)
	{
		ProcessorID*	pProcIDNew2=new ProcessorID(*(pProcNew->getID()));
		for(int k=0;k<nLevel-j-1;k++)
			pProcIDNew2->getSuperGroup();
		m_arrayProcessorID.Add(pProcIDNew2);

		char szName[128];
		pProcNew->getID()->getNameAtLevel(szName,j);
		hItem=InsertItem(szName,0,0,hItem);
		if(j==nLevel-1)
			if(pProc->getID()==pProcNew->getID())
				hItemSeled=hItem;
		SetItemData(hItem,(DWORD)pProcIDNew2);
	}
	HTREEITEM hItemPrev=GetParentItem(hItem);	
	for(int i=group.start+1;i<=group.end;i++)
	{
		Processor* pProcNew=pDoc->GetTerminal().procList->getProcessor(i);
		ProcessorID* pProcIDNew2=new ProcessorID(*(pProcNew->getID()));
		m_arrayProcessorID.Add(pProcIDNew2);

		char szName[128];
		pProcNew->getID()->getNameAtLevel(szName,nLevel-1);
		hItem=InsertItem(szName,0,0,hItemPrev);
		if(pProc->getID()==pProcNew->getID())
			hItemSeled=hItem;
		SetItemData(hItem,(DWORD)pProcIDNew2);
	}

	hItem=TVI_ROOT;
	while(hItem)
	{
		Expand(hItem,TVE_EXPAND);
		hItem=GetChildItem(hItem);
	}
	SelectItem(hItemSeled);	
	m_bSendMessage = false;
}

void CPickConveyorTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CNodeView* pNodeView = pDoc->GetNodeView();
	HTREEITEM hItemSeled=GetSelectedItem();
	ProcessorID* pProcID=(ProcessorID*)GetItemData(hItemSeled);
	pNodeView->m_pDlgFlow->SendMessage(UM_PCTREE_SELECT_PROC,(WPARAM)pProcID,m_nInsertType);
	m_bSendMessage = true;
	ShowWindow(SW_HIDE);
	
	*pResult = 0;
}
