// DlgHoldShortLineSel.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgHoldShortLineSel.h"
#include ".\dlgholdshortlinesel.h"


// CDlgHoldShortLineSel dialog

IMPLEMENT_DYNAMIC(CDlgHoldShortLineSel, CXTResizeDialog)
CDlgHoldShortLineSel::CDlgHoldShortLineSel(TaxiInterruptLineList* _holdshortLines ,CWnd* pParent /*=NULL*/)
	:m_holdshortLines(_holdshortLines),m_SelShortLine(NULL), CXTResizeDialog(CDlgHoldShortLineSel::IDD, pParent)
{
}

CDlgHoldShortLineSel::~CDlgHoldShortLineSel()
{
}

void CDlgHoldShortLineSel::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_HOLD_SHORTLINE, m_treeCtrl);
	DDX_Control(pDX,IDOK,m_button_Sel) ;
}


BEGIN_MESSAGE_MAP(CDlgHoldShortLineSel, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_HOLD_SHORTLINE, OnTvnSelchangedTreeHoldShortline)
	ON_WM_SIZE() 
END_MESSAGE_MAP()


// CDlgHoldShortLineSel message handlers
BOOL CDlgHoldShortLineSel::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog() ;
	InitTree() ;
	m_button_Sel.EnableWindow(FALSE) ;
	SetResize(IDC_TREE_HOLD_SHORTLINE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgHoldShortLineSel::InitTree()
{
   int count =  m_holdshortLines->GetElementCount() ;
   for (int i = 0 ; i < count ; i++)
   {
        AddItem(m_holdshortLines->GetItem(i)) ;
   }
}
void CDlgHoldShortLineSel::AddItem(CTaxiInterruptLine* _lineNode )
{
  if ( _lineNode == NULL)
     return ;
  HTREEITEM item = m_treeCtrl.InsertItem(_lineNode->GetName()) ;
  m_treeCtrl.SetItemData(item , (DWORD_PTR)_lineNode) ;

}
void CDlgHoldShortLineSel::OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(m_treeCtrl.ItemHasChildren(m_treeCtrl.GetSelectedItem()))
		m_button_Sel.EnableWindow(FALSE) ;
	else
		m_button_Sel.EnableWindow(TRUE) ;
	*pResult = 0;
}
void CDlgHoldShortLineSel::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
void CDlgHoldShortLineSel::OnOK()
{
	HTREEITEM item = m_treeCtrl.GetSelectedItem() ;
    m_SelShortLine = (CTaxiInterruptLine*)m_treeCtrl.GetItemData(item) ;
	CXTResizeDialog::OnOK() ;
}
CTaxiInterruptLine* CDlgHoldShortLineSel::GetSelHoldShortLine() 
{
	return m_SelShortLine ;
}