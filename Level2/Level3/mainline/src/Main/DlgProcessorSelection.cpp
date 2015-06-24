// DlgProcessorSelection.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgProcessorSelection.h"
#include ".\dlgprocessorselection.h"
#include "../Engine/proclist.h"
#include "inputs/IN_TERM.H"

// CDlgProcessorSelection dialog

IMPLEMENT_DYNAMIC(CDlgProcessorSelection, CDialog)
CDlgProcessorSelection::CDlgProcessorSelection(const ProcessorID& ProcID, InputTerminal* _pInTerm, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessorSelection::IDD, pParent)
	,m_pInTerm(_pInTerm)
{
	m_ProcName = ProcID;

}

CDlgProcessorSelection::~CDlgProcessorSelection()
{
}

void CDlgProcessorSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PROC, m_ctlProcTree);
}


BEGIN_MESSAGE_MAP(CDlgProcessorSelection, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROC, OnTvnSelchangedTreeProc)
END_MESSAGE_MAP()


// CDlgProcessorSelection message handlers

void CDlgProcessorSelection::OnBnClickedOk()
{


	OnOK();
}

BOOL CDlgProcessorSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pInTerm == NULL)
		return TRUE;

	// Set data to processor tree
	AddProcToTree(NULL, m_ProcName);
	
	return TRUE;  
}

const CString& CDlgProcessorSelection::GetSelProcName()
{
	return m_strSelProc;
}

void CDlgProcessorSelection::OnTvnSelchangedTreeProc(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hCurSelItem = m_ctlProcTree.GetSelectedItem();
	if( hCurSelItem == NULL )
		return;

	m_strSelProc = m_ctlProcTree.GetItemText( hCurSelItem );


	*pResult = 0;
}

void CDlgProcessorSelection::AddProcToTree( HTREEITEM _hParent, const ProcessorID& _id )
{
	StringList strDict;
	ProcessorID ProcID;
	ProcID.SetStrDict( m_pInTerm->inStrDict );

	// insert item to tree
	CString strProc = _id.GetIDString();
	HTREEITEM hProcItem = m_ctlProcTree.InsertItem( strProc, _hParent );

	m_pInTerm->procList->getMemberNames( _id, strDict, -1);
	for( int i= 0; i<strDict.getCount(); i++ )
	{
		CString strTemp = strDict.getString (i);
		strTemp.TrimLeft(); 
		strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;

		if (strProc != "All Processors")
			strTemp = strProc + "-" + strTemp;
		
		ProcID.setID(strTemp);
		AddProcToTree( hProcItem, ProcID );
	}

	m_ctlProcTree.Expand( hProcItem, TVE_EXPAND);
}