// LayoutOverUpToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LayoutOverUpToolBar.h"
#include ".\layoutoveruptoolbar.h"
#include ".\layoutoveruptoolbar.h"
#include "../Common/WinMsg.h"
#include "LayOutFrame.h"
#include "../InputOnBoard/AircraftFurnishings.h"
#include "TermPlanDoc.h"
#include "../InputOnBoard/CInputOnboard.h"
// CLayoutOverUpToolBar dialog

IMPLEMENT_DYNAMIC(CLayoutOverUpToolBar, CXTResizeDialog)
CLayoutOverUpToolBar::CLayoutOverUpToolBar(CTermPlanDoc* _TermPlan ,CWnd* pParent /*=NULL*/)
	: m_pDragImage(NULL),m_Doc(_TermPlan),CXTResizeDialog(CLayoutOverUpToolBar::IDD, pParent)
{
   m_FurnishingDataSet = NULL;//m_Doc->GetInputOnboard()->GetFurnishingData() ;
}

CLayoutOverUpToolBar::~CLayoutOverUpToolBar()
{
}

void CLayoutOverUpToolBar::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_LAYOUT, m_tree);
}

BOOL CLayoutOverUpToolBar::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	InitTree() ;
	SetResize(IDC_TREE_LAYOUT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;

}
void CLayoutOverUpToolBar::InitTree()
{
   HTREEITEM root =  m_tree.InsertItem(_T("furnishings list")) ;
   m_image.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
   m_tree.SetImageList ( &m_image,TVSIL_NORMAL );
   InitToolNode(root) ;
}
void CLayoutOverUpToolBar::InitToolNode(HTREEITEM root)
{
	HTREEITEM item = NULL ;
	CString FurnType ;
   for (int i = 0 ; i < m_FurnishingDataSet->GetTypeCount() ; i++)
   {
	   FurnType = m_FurnishingDataSet->GetType(i) ;
	   item =  m_tree.InsertItem(FurnType,root) ;
	   AddTypeNode(FurnType,item) ;
	   m_tree.Expand(item,TVE_EXPAND) ;
   }
   m_tree.Expand(root,TVE_EXPAND) ;
}
void CLayoutOverUpToolBar::AddTypeNode(const CString& _type , HTREEITEM  _item )
{
	CAircraftFurnishingDataSet::TY_FURNISHING_VECTOR FurnishTools = m_FurnishingDataSet->GetItemsByType(_type) ;
	CAircraftFurnishingDataSet::TY_FURNISHING_VECTOR_ITER iter = FurnishTools.begin() ;
	CString ToolName ;
	for ( ; iter != FurnishTools.end() ; iter++)
	{
       ToolName = (*iter)->GetIDName().GetIDString() ;
		HTREEITEM newItem =  m_tree.InsertItem(ToolName,_item) ;
	   m_tree.SetItemData(newItem, (DWORD_PTR)(*iter) );
	}
}
void CLayoutOverUpToolBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY(m_pDragImage->DragMove(pt));
		// unlock window updates
		m_pDragImage->DragShowNolock(FALSE);
		//////////////////////////////////////////////////////
		CRect rcSM;
		GetWindowRect( &rcSM );
		if( rcSM.PtInRect( pt ) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ICON));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_tree.Invalidate(FALSE);
	}

	CXTResizeDialog::OnMouseMove(nFlags, point);
}

void CLayoutOverUpToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave( GetDesktopWindow() );
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = NULL;

		CWnd* pWnd = m_pParentWnd;

		while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CLayOutFrame))) {
			pWnd = pWnd->GetParent();
		}
		if(pWnd == NULL) {
			TRACE0("Error: Could not get MainFrame to send message!\n");
			return ;
		}
		
		CAircraftFurnishing* pFurnishing = (CAircraftFurnishing*)m_tree.GetItemData(m_hItemDragSrc);
		pWnd->SendMessage( WM_AIR_SELCHANGED, (WPARAM) pFurnishing, (LPARAM) NULL);
	}	
	CXTResizeDialog::OnLButtonUp(nFlags, point);
}
void CLayoutOverUpToolBar::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
BEGIN_MESSAGE_MAP(CLayoutOverUpToolBar, CXTResizeDialog)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_TREE_LAYOUT, OnNMClickTreeLayout)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LAYOUT, OnTvnSelchangedTreeLayout)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_LAYOUT, OnTvnBegindragTreeLayout)
END_MESSAGE_MAP()


// CLayoutOverUpToolBar message handlers

void CLayoutOverUpToolBar::OnNMClickTreeLayout(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

}

void CLayoutOverUpToolBar::OnTvnSelchangedTreeLayout(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CLayoutOverUpToolBar::OnTvnBegindragTreeLayout(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	if (m_tree.GetChildItem(m_hItemDragSrc))
		return;

	if(m_pDragImage)
		delete m_pDragImage;

	CPoint ptTemp(0,0);
	m_pDragImage = m_tree.CreateDragImage(m_hItemDragSrc);
	if( !m_pDragImage )
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0, CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter( GetDesktopWindow(), pt );  

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
