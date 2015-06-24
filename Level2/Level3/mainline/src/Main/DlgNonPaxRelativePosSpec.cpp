// DlgNonPaxRelativePosSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgNonPaxRelativePosSpec.h"
#include ".\dlgnonpaxrelativeposspec.h"
#include "..\AirsideGUI\DragDropImpl.h"
#include <inputs\MobileElemTypeStrDB.h>
#include <Inputs\IN_TERM.H>
#include <Inputs\NonPaxRelativePosSpec.h>
#include "TermPlanDoc.h"
#include "MobileCountDlg.h"
using namespace ElementDragSource;
// CDlgNonPaxRelativePosSpec dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
CNonPaxListCtrl::CNonPaxListCtrl()
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CNonPaxListCtrl::~CNonPaxListCtrl()
{

}

BEGIN_MESSAGE_MAP(CNonPaxListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CItinerantFlightListCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE( UM_ITEM_CHECK_STATUS_CHANGED, OnCheckStateChanged )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNonPaxListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem=HitTest(point);
	if(nItem != -1) 
	{
		CRect rcVisible;
		GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible);
		if(rcVisible.PtInRect(point))
		{
			BOOL bCheck=GetItemCheck(nItem);
			if(bCheck)
				SetItemCheck( nItem,FALSE);
			else
				SetItemCheck( nItem,TRUE);
			SendMessage(UM_ITEM_CHECK_STATUS_CHANGED,(WPARAM)nItem,(LPARAM)!bCheck);
		}
		Invalidate();
	}
	CListCtrl::OnLButtonDown(nFlags,point);

}

void CNonPaxListCtrl::SetItemCheck(int nItem,BOOL bCheck)
{
	if(bCheck)
		SetItemText(nItem,2,"1");
	else
		SetItemText(nItem,2,"0");
}

BOOL CNonPaxListCtrl::GetItemCheck(int nItem)
{
	CString strCheck=GetItemText(nItem,2);
	if(strCheck==CString("1"))
		return TRUE;
	return FALSE;
}
LRESULT CNonPaxListCtrl::OnCheckStateChanged( WPARAM wParam, LPARAM lParam )
{
	int iItem = (int) wParam;
	bool flag = lParam ? true : false;
	

	if( iItem != -1 )
	{
		int nTypeIndex = (int)GetItemData(iItem);
		m_pPosSpec->SetTypeItemVisible(nTypeIndex,flag);
	}

	return TRUE;
}

void CNonPaxListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
{
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	this->GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED); 

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 
	CRect rcVisible1;
	this->GetItemRect(nItem,rcVisible1,LVIR_LABEL);
	rcVisible1.left+=2;
	CRect rcVisible2;
	this->GetSubItemRect( nItem,1,LVIR_LABEL,rcVisible2 );
	rcVisible2.left+=2;
	CRect rcVisible3;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcVisible3 );

	rcAllLabels.left = rcVisible1.left;
	if(bSelected) { 
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
	} 
	else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	int nTypeIndex = (int)lvi.lParam;
	CString strValue(_T(""));
	pDC->DrawText( m_pMobElemTypeDB->GetString(nTypeIndex), rcVisible1, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	strValue.Format(_T("%d"),m_pPosSpec->GetTypeItemCount(nTypeIndex));
	pDC->DrawText( strValue, rcVisible2, DT_LEFT| DT_VCENTER | DT_SINGLELINE );

	//draw checkbox
	CDC bitmapDC;
	HBITMAP hOldBM;
	rcVisible3.left+=(rcVisible3.Width()-m_sizeCheck.cx)/2;
	rcVisible3.top+=(rcVisible3.Height()-m_sizeCheck.cy)/2;
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcVisible3.left, rcVisible3.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * GetItemCheck(nItem), 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void CNonPaxListCtrl::InsertItem(int _nIdx,int nTypeIndex)
{
	int nIdx = CListCtrl::InsertItem( _nIdx, LPCTSTR(""));
	ASSERT(nIdx == _nIdx);
	SetItemData(nIdx,(DWORD)nTypeIndex);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgNonPaxRelativePosSpec, CXTResizeDialog)
CDlgNonPaxRelativePosSpec::CDlgNonPaxRelativePosSpec(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgNonPaxRelativePosSpec::IDD, pParent)
{
	mpDoc = pDoc;

	m_bDragging = false;
	m_pDragImage = NULL;
	m_nDragIndex = -1;

	if(mpDoc)
	{
		CNonPaxRelativePosSpec* pPosSpec =  mpDoc->GetTerminal().GetNonPaxRelativePosSpec();
		m_editPosCtrl.SetPosSpec(pPosSpec);
	}

}

CDlgNonPaxRelativePosSpec::~CDlgNonPaxRelativePosSpec()
{
}

void CDlgNonPaxRelativePosSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POSVIEW, m_editPosCtrl);
	DDX_Control(pDX, IDC_LIST_NONPAX, m_nonPaxListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgNonPaxRelativePosSpec, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_NONPAX, OnLvnBegindragListRelative)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RADIO_TOP, OnBnClickedRadioTop)
	ON_BN_CLICKED(IDC_RADIO_LEFT, OnBnClickedRadioLeft)
	ON_BN_CLICKED(IDC_RADIO_FRONT, OnBnClickedRadioFront)
	ON_BN_CLICKED(IDC_RADIO_PERS, OnBnClickedRadioPers)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(ID_NONPAXPICK_DELETE, OnDeletePosItem)
END_MESSAGE_MAP()


// CDlgNonPaxRelativePosSpec message handlers

void CDlgNonPaxRelativePosSpec::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(mpDoc )
	{
		CNonPaxRelativePosSpec* pPosSpec =  mpDoc->GetTerminal().GetNonPaxRelativePosSpec();
		if(pPosSpec)
		{
			pPosSpec->saveDataSet(mpDoc->m_ProjInfo.path,false);
		}
	}

	OnOK();
}



BOOL CDlgNonPaxRelativePosSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();


	SetResize(IDC_LIST_NONPAX,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_NPT,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_CAM,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_LOAD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_EDIT_POSVIEW,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_EDITP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_RADIO_FRONT,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_RADIO_LEFT,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_RADIO_TOP,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_RADIO_PERS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	InitListCtrlHead();

	((CButton*)GetDlgItem(IDC_RADIO_PERS))->SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgNonPaxRelativePosSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void CDlgNonPaxRelativePosSpec::OnLvnBegindragListRelative( NMHDR *pNMHDR, LRESULT *pResult )
{

	//This routine sets the parameters for a Drag and Drop operation.
	//It sets some variables to track the Drag/Drop as well
	// as creating the drag image to be shown during the drag.

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMListView->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if(m_nonPaxListCtrl.GetSelectedCount() > 1) //more than one item is selected
		pt.x = nOffset;
	pt.y = nOffset;

	m_pDragImage = m_nonPaxListCtrl.CreateDragImage(m_nDragIndex, &pt);
	ASSERT(m_pDragImage); //make sure it was created
	//We will call delete later (in LButtonUp) to clean this up

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_PROCESSES);
	m_pDragImage->Replace(0, &bitmap, &bitmap);

	//// Change the cursor to the drag image
	////	(still must perform DragMove() in OnMouseMove() to show it moving)
	m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
	m_pDragImage->DragEnter(GetDesktopWindow(), pNMListView->ptAction);

	//// Set dragging flag and others
	m_bDragging = TRUE;	//we are in a drag and drop operation

	//// Capture all mouse messages
	SetCapture ();

	*pResult = 0;
}

void CDlgNonPaxRelativePosSpec::InitListCtrlHead()
{
	DWORD dwStyle = m_nonPaxListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_nonPaxListCtrl.SetExtendedStyle(dwStyle);

	m_nonPaxListCtrl.InsertColumn(0, "Type", LVCFMT_LEFT, 80);
	m_nonPaxListCtrl.InsertColumn(1, "Count", LVCFMT_LEFT, 50);
	m_nonPaxListCtrl.InsertColumn(2, "Visible",LVCFMT_LEFT,50);

	m_nonPaxListCtrl.SetMobileElemTypeStrDB(GetTypeDB());
	m_nonPaxListCtrl.SetNonPaxPosSpec(GetPosSpec());
	SetListCtrlContent();
}

void CDlgNonPaxRelativePosSpec::SetListCtrlContent()
{
	CMobileElemTypeStrDB* pMobileElemTypeStrDB= GetTypeDB();
	CNonPaxRelativePosSpec* pPosSpec =  GetPosSpec();

	if(!pMobileElemTypeStrDB || !pPosSpec)
		return ;

	int endIndex = MIN(NOPAX_COUNT_EXCLUDE_USER_DEFINE,pMobileElemTypeStrDB->GetCount());
	int itemidx =0;
	m_nonPaxListCtrl.DeleteAllItems();
	for(int i=1;i<=endIndex;++i)
	{

		if( i>=4 && i<= 7)continue;

		CString strType = pMobileElemTypeStrDB->GetString(i);
		m_nonPaxListCtrl.InsertItem(itemidx,i);
		m_nonPaxListCtrl.SetItemCheck(itemidx,GetPosSpec()->GetTypeItemVisible(i));
	
		itemidx ++;
	}
}
void CDlgNonPaxRelativePosSpec::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//// If we are in a drag/drop procedure (m_bDragging is true)
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);


		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		if(pDropWnd == &m_editPosCtrl)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen(&ptTest);			
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
	
	}
	CXTResizeDialog::OnMouseMove(nFlags, point);
}

void CDlgNonPaxRelativePosSpec::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();

		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;

		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		m_pDragImage = NULL;

		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		
		
		// If window is CListCtrl, we perform the drop
		if (pDropWnd==&m_editPosCtrl)
		{			
			pDropWnd->ScreenToClient(&pt);

		//	int nIdx = m_nonPaxListCtrl.GetItemData(m_nDragIndex);
			CMobileElemTypeStrDB* pMobileElemTypeStrDB= GetTypeDB();

			if(pMobileElemTypeStrDB)
			{
				//CString strType = pMobileElemTypeStrDB->GetString(nIdx);
				int nTypeIndex = m_nonPaxListCtrl.GetItemData(m_nDragIndex);
				m_editPosCtrl.AddNewNonPaxPos(nTypeIndex,pt);
				SetListCtrlContent();
				//UpdateListCtrl();
			}
		}
	}

	CXTResizeDialog::OnLButtonUp(nFlags, point);
}

void CDlgNonPaxRelativePosSpec::UpdateListCtrl()
{
	CMobileElemTypeStrDB* pMobileElemTypeStrDB= GetTypeDB();
	CNonPaxRelativePosSpec* pPosSpec = GetPosSpec();

	if(!pMobileElemTypeStrDB || !pPosSpec)
		return ;

	int endIndex = MIN(NOPAX_COUNT_EXCLUDE_USER_DEFINE,pMobileElemTypeStrDB->GetCount());
	int itemidx =0;
	for(int i=1;i<=endIndex;++i)
	{

		if( i>=4 && i<= 7)continue;
		

		//CString strType = pMobileElemTypeStrDB->GetString(i);
	
		m_nonPaxListCtrl.SetItemData(itemidx, (DWORD_PTR)i);
		int nTypeCount = pPosSpec->GetTypeItemCount(i);
		CString strCount ;strCount.Format("%d",nTypeCount);
		m_nonPaxListCtrl.SetItemText(itemidx,1,strCount);
		itemidx ++;
	}
}
void CDlgNonPaxRelativePosSpec::OnBnClickedRadioTop()
{
	// TODO: Add your control notification handler code here
	m_editPosCtrl.SetSceneCameType(camTop);
}

void CDlgNonPaxRelativePosSpec::OnBnClickedRadioLeft()
{
	// TODO: Add your control notification handler code here
	m_editPosCtrl.SetSceneCameType(camLeft);
}

void CDlgNonPaxRelativePosSpec::OnBnClickedRadioFront()
{
	// TODO: Add your control notification handler code here
	m_editPosCtrl.SetSceneCameType(camFront);
}

void CDlgNonPaxRelativePosSpec::OnBnClickedRadioPers()
{
	// TODO: Add your control notification handler code here
	m_editPosCtrl.SetSceneCameType(camPerspect);
}

void CDlgNonPaxRelativePosSpec::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CNonPaxRelativePosSpec* pSpec =	GetPosSpec();
	if(pSpec && mpDoc)
	{
		pSpec->loadDataSet(mpDoc->m_ProjInfo.path );
	}

	OnCancel();
}

CMobileElemTypeStrDB* CDlgNonPaxRelativePosSpec::GetTypeDB() const
{
	if(mpDoc)
	{
		return mpDoc->GetTerminal().m_pMobElemTypeStrDB;
	}
	return NULL;

}

CNonPaxRelativePosSpec* CDlgNonPaxRelativePosSpec::GetPosSpec() const
{
	if(mpDoc)
	{
		return mpDoc->GetTerminal().GetNonPaxRelativePosSpec();
	}

	return NULL;
}

LRESULT CDlgNonPaxRelativePosSpec::OnDeletePosItem( WPARAM,LPARAM )
{
	//UpdateListCtrl();
	SetListCtrlContent();
	return -1;
}