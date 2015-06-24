// GateSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GateSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGateSelectionDlg dialog


CGateSelectionDlg::CGateSelectionDlg(CWnd* pParent /*=NULL*/, int nIdx, BOOL _bFlag)
	: CDialog(CGateSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGateSelectionDlg)
		m_nInx = nIdx;
		m_bParentIsOtherDlg = _bFlag;
	//}}AFX_DATA_INIT
	m_bStandOnly=FALSE;

	m_treeProc.setInputTerminal( GetInputTerminal() );
}


void CGateSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGateSelectionDlg)
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_treeProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGateSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CGateSelectionDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROCESSOR, OnDblclkTreeProcessor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGateSelectionDlg message handlers

BOOL CGateSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_treeProc.m_bDisplayAll=TRUE;

	//reload gate infor from the database.
	ReloadDatabase();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGateSelectionDlg::ReloadDatabase()
{
	// check if have gate with ac stand
	if( m_bStandOnly )
	{
		// check if have gate processor with ac stand flag
		StringList strList;
		GetInputTerminal()->procList->getAllGroupNames( strList, GateProc, TRUE );
		if( strList.getCount() == 0 )
		{
			// need airside stand
			m_treeProc.LoadData( GetInputTerminal(), 
				(ProcessorDatabase*)GetInputTerminal()->serviceTimeDB, StandProcessor,m_bStandOnly );
			return;
		}
	}
	m_treeProc.LoadData( GetInputTerminal(), 
		(ProcessorDatabase*)GetInputTerminal()->serviceTimeDB, GateProc,m_bStandOnly);
}

extern CTermPlanApp theApp;
InputTerminal* CGateSelectionDlg::GetInputTerminal()
{
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)(m_pParentWnd->GetParent()))->GetDocument();
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CGateSelectionDlg::OnOK() 
{
	//////////////////////////////////////////////////////////////////////////
	// add by bird 2003/5/23
	if( m_bParentIsOtherDlg )
	{
		HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
		if(hSelItem == NULL)
			return;
	
		CString strSelItem = m_treeProc.GetItemText(hSelItem);
		m_ProcID.SetStrDict(GetInputTerminal()->inStrDict);
		m_ProcID.setID(strSelItem.GetBuffer(strSelItem.GetLength() + 1));

		CDialog::OnOK();
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	

	if(m_nInx == -1)
	{
		HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
		if(hSelItem == NULL)
			return;
		CString strSelItem = m_treeProc.GetItemText(hSelItem);
		/*ProcessorID *procID = new ProcessorID;
		procID->SetStrDict(GetInputTerminal()->inStrDict);
		procID->setID(strSelItem.GetBuffer(strSelItem.GetLength() + 1));*/
		ALTObjectID objID;
		objID = strSelItem.GetBuffer(strSelItem.GetLength()+1);
		((CGatePriorityDlg *)m_pParentWnd)->m_standIDList.Add(objID);
		CDialog::OnOK();
	}

	else 
	{
		HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
		if(hSelItem == NULL)
			return;
		CString strSelItem = m_treeProc.GetItemText(hSelItem);
		//ProcessorID *procID = new ProcessorID;
		//procID->SetStrDict(GetInputTerminal()->inStrDict);
		//procID->setID(strSelItem.GetBuffer(strSelItem.GetLength() + 1));
		ALTObjectID objID;
		objID = strSelItem.GetBuffer(strSelItem.GetLength()+1);
		((CGatePriorityDlg *)m_pParentWnd)->m_standIDList[m_nInx]  = objID;
		CDialog::OnOK();
	}
}

void CGateSelectionDlg::OnDblclkTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}
