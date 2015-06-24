// ShapesBarPFlow.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MainFrm.h"
#include "ShapesBarPFlow.h"
#include "ShapesBar.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"
#include "Common\ProjectManager.h"
#include "Ini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapesBarPFlow

CShapesBarPFlow::CShapesBarPFlow()
{
	m_nMenu.m_hMenu = NULL;
	m_pSubMenu = NULL;
}

CShapesBarPFlow::~CShapesBarPFlow()
{
	if(m_nMenu.m_hMenu != NULL)
	{
		m_nMenu.DestroyMenu();
		m_nMenu.m_hMenu = NULL;
		m_pSubMenu = NULL;
	}
}


BEGIN_MESSAGE_MAP(CShapesBarPFlow, CSizingControlBarG)
	//{{AFX_MSG_MAP(CShapesBarPFlow)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	ON_WM_NCLBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCRBUTTONDOWN()
//	ON_COMMAND(ID_MMPROC_ADD, OnMmprocAdd)
//	ON_COMMAND(ID_MMPROC_COMMENTS, OnMmprocComments)
//	ON_COMMAND(ID_MMPROC_HELP, OnMmprocHelp)
//	ON_COMMAND(ID_MMITEM_EDIT, OnMmitemEdit)
	ON_COMMAND(ID_MMITEM_DELETE, OnMmitemDelete)
//	ON_COMMAND(ID_MMITEM_COMMENTS, OnMmitemComments)
//	ON_COMMAND(ID_MMITEM_HELP, OnMmitemHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CShapesBarPFlow message handlers
void CShapesBarPFlow::CreateOutlookBar(CShape::CShapeList* pSL)
{
	DWORD dwf =CGfxOutBarCtrl::fAnimation;

	m_wndOutBarCtrl.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,IDC_LIST_SHAPESPFLOW, dwf);
	
	m_largeIL.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);//|ILC_MASK
	m_smallIL.Create(16,16, ILC_COLOR24|ILC_MASK,0,4);//|ILC_MASK

	m_wndOutBarCtrl.SetImageList(&m_largeIL, CGfxOutBarCtrl::fLargeIcon);
	m_wndOutBarCtrl.SetImageList(&m_smallIL, CGfxOutBarCtrl::fSmallIcon);
	
	m_wndOutBarCtrl.SetAnimationTickCount(6);

	m_wndOutBarCtrl.AddFolder("Processes", 0);
	m_wndOutBarCtrl.AddFolder("", 1);

//	AddNewProcess("Process1");

	m_wndOutBarCtrl.SetSelFolder(0);
	m_wndOutBarCtrl.RemoveFolder(1);

	m_nMenu.LoadMenu(IDR_CTX_MATHEMATIC);
}

LRESULT CShapesBarPFlow::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	// TRACE("CShapesBarPFlow::OnSLBSelChanged()\n");
	//ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	CWnd* pWnd = GetParent();
	while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pWnd = pWnd->GetParent();
	}
	if(pWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return 0;
	}

	CString strName = m_wndOutBarCtrl.GetItemText(m_wndOutBarCtrl.iLastItemHighlighted);
	char* pch = strName.GetBuffer(strName.GetLength());
	pWnd->SendMessage(WM_SLB_SELCHANGED, wParam, (LPARAM)pch);
	strName.ReleaseBuffer();
	return 0;
}

HBRUSH CShapesBarPFlow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSizingControlBarG::OnCtlColor(pDC, pWnd, nCtlColor);
	return m_brush;
}
int CShapesBarPFlow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	m_pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(m_pSL != NULL);
	CreateOutlookBar(m_pSL);	

	m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
    // Create an 8-point MS Sans Serif font for the list box.
    m_font.CreatePointFont (80, _T ("MS Sans Serif"));

	return 0;
}

int CShapesBarPFlow::AddNewProcess(const CString &strName)
{
	for(int i=0; i<m_wndOutBarCtrl.GetItemCount(); i++)
	{
		if(m_wndOutBarCtrl.GetItemText(i) == strName)
		{
			return -1;
		}
	}

//	HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SHAPESPFLOW), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,m_pSL->at(1)->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	CBitmap bm;
	bm.Attach(hBitmap);
	int nR=m_largeIL.Add(&bm,RGB(255,0,255));//255
	bm.Detach();
//	hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SHAPESPFLOWS), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	hBitmap = (HBITMAP) ::LoadImage(NULL,m_pSL->at(1)->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	bm.Attach(hBitmap);
	m_smallIL.Add(&bm,RGB(255,0,255));//
//	m_pSL->at(1).
	int iIndex = m_wndOutBarCtrl.InsertItem(0, -1, strName,nR, (DWORD)m_pSL->at(1));//
	
	m_wndOutBarCtrl.Invalidate();

	return iIndex;
}

UINT CShapesBarPFlow::DeleteProcess(const CString &strName)
{
	for(int i=0; i<m_wndOutBarCtrl.GetItemCount(); i++)
	{
		if(m_wndOutBarCtrl.GetItemText(i) == strName)
		{
			m_wndOutBarCtrl.RemoveItem(i);
			break;
		}
	}

	m_wndOutBarCtrl.Invalidate();

	return i;
}

BOOL CShapesBarPFlow::UpdateProcess(CString &strOld, CString &strNew)
{
	for(int i=0; i<m_wndOutBarCtrl.GetItemCount(); i++)
	{
		if(m_wndOutBarCtrl.GetItemText(i) == strOld)
		{
			m_wndOutBarCtrl.SetItemText(i, strNew);
			m_wndOutBarCtrl.Invalidate();
			return TRUE;
		}
	}

	return FALSE;
}

void CShapesBarPFlow::DeleteAllProcesses()
{
	while(m_wndOutBarCtrl.GetItemCount())
	{
		m_wndOutBarCtrl.RemoveItem(0);
	}

	m_wndOutBarCtrl.Invalidate();
}

void CShapesBarPFlow::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (nHitTest == HTCLOSE)
		((CMainFrame*)AfxGetMainWnd())->ShapesBarPFlowMsg(FALSE);

	CSizingControlBarG::OnNcLButtonUp(nHitTest, point);
}

void CShapesBarPFlow::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	if(m_nMenu == NULL)
		return ;

	CPoint pt = point;
	ScreenToClient(&pt);
	m_pSubMenu = NULL;
	int nIndex = -1;
	int nStatus = m_wndOutBarCtrl.HitTestEx(pt, nIndex);
	
	CWnd* pParentWnd = GetParent();
	while(pParentWnd != NULL && !pParentWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pParentWnd = pParentWnd->GetParent();
	}
	if(pParentWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return ;
	}

	CString strName = m_wndOutBarCtrl.GetItemText(m_wndOutBarCtrl.iLastItemHighlighted);
	char* pch = strName.GetBuffer(strName.GetLength());
	
	pParentWnd->SendMessage(WM_SLB_SELCHANGED, (WPARAM)NULL, (LPARAM)pch);
	strName.ReleaseBuffer();

	if(nStatus == m_wndOutBarCtrl.htItem )
	{
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(3));
	}
	else if(nStatus == m_wndOutBarCtrl.htNothing)
	{
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(1));
	}

	if(m_pSubMenu)
		m_pSubMenu->TrackPopupMenu(0, point.x, point.y, this, NULL);
}

void CShapesBarPFlow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CSizingControlBarG::OnRButtonDown(nFlags, point);
}

void CShapesBarPFlow::OnNcRButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CSizingControlBarG::OnNcRButtonDown(nHitTest, point);
}

void CShapesBarPFlow::OnMmprocAdd()
{

}

void CShapesBarPFlow::OnMmprocComments()
{

}

void CShapesBarPFlow::OnMmprocHelp()
{
	
}

void CShapesBarPFlow::OnMmitemEdit()
{
	
}

void CShapesBarPFlow::OnMmitemDelete()
{
	CWnd* pParentWnd = GetParent();
	while(pParentWnd != NULL && !pParentWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pParentWnd = pParentWnd->GetParent();
	}
	if(pParentWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return ;
	}

	CString strName = m_wndOutBarCtrl.GetItemText(m_wndOutBarCtrl.iLastItemHighlighted);
	
	
	((CMainFrame*)pParentWnd)->DeleteItem(strName);
	
}

void CShapesBarPFlow::OnMmitemComments()
{

}

void CShapesBarPFlow::OnMmitemHelp()
{

}
