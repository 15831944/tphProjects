// DlgGateSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgGateSelect.h"
#include "../Inputs/GateAssignmentMgr.h"
#include <Inputs/IN_TERM.H>

// CDlgGateSelect dialog

IMPLEMENT_DYNAMIC(CDlgGateSelect, CXTResizeDialog)
CDlgGateSelect::CDlgGateSelect(InputTerminal* _pInTerm ,int _Type ,CWnd* pParent /*=NULL*/)
	: m_pInTerm(_pInTerm),m_Type(_Type),CXTResizeDialog(CDlgGateSelect::IDD, pParent),
    m_bSelLeafOnly(FALSE)
{

	m_Caption.Format(_T("Gate Select")) ;
	m_treeProc.setInputTerminal(m_pInTerm );
}

CDlgGateSelect::~CDlgGateSelect()
{
}

void CDlgGateSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_GATE,m_treeProc) ;
}


BEGIN_MESSAGE_MAP(CDlgGateSelect, CDialog)
ON_WM_SIZE()
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GATE, &CDlgGateSelect::OnTvnSelchangedTreeGate)
END_MESSAGE_MAP()
void CDlgGateSelect::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
BOOL CDlgGateSelect::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	InintListBox() ;
	SetWindowText(m_Caption) ;
	SetResize(IDC_TREE_GATE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	m_treeProc.m_bDisplayAll = TRUE ;
    if(m_bSelLeafOnly)
        GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE ;
}
void CDlgGateSelect::InintListBox()
{
	
	m_treeProc.LoadGateData(m_pInTerm, 
		(ProcessorDatabase*)m_pInTerm->serviceTimeDB, m_Type);
}
void CDlgGateSelect::OnOK()
{
	HTREEITEM _item = m_treeProc.GetSelectedItem() ;
	if(_item != NULL)
	{
		m_SelGateName = m_treeProc.GetItemText(_item) ;
	}
	
	CXTResizeDialog::OnOK() ;
}

void CDlgGateSelect::SetSelectLeafNodeOnly(BOOL bSel)
{
    m_bSelLeafOnly = bSel;
}

// CDlgGateSelect message handlers


void CDlgGateSelect::OnTvnSelchangedTreeGate(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
    if(hSelItem == NULL)
        return;
    if(m_bSelLeafOnly)
    {
        GetDlgItem(IDOK)->EnableWindow(FALSE);
        if(m_treeProc.GetChildItem(hSelItem) == NULL)
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        *pResult = 0;
    }
}
