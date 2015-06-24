// DlgPipeAttachCrosswalk.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgPipeAttachCrosswalk.h"
#include "../Landside/LandsideCrosswalk.h"
#include "../Inputs/Pipe.h"
#include "ConDBListCtrlWithCheckBox.h"
#include "../Inputs/IN_TERM.H"
#include "TermPlanDoc.h"
#include "../Inputs/PipeDataSet.h"

CPipeAttachCrosswalkListCtrl::CPipeAttachCrosswalkListCtrl()
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CPipeAttachCrosswalkListCtrl::~CPipeAttachCrosswalkListCtrl()
{

}

BEGIN_MESSAGE_MAP(CPipeAttachCrosswalkListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE( UM_ITEM_CHECK_STATUS_CHANGED, OnCheckStateChanged )
END_MESSAGE_MAP()

void CPipeAttachCrosswalkListCtrl::InsertItem( int _nIdx,CrossWalkAttachPipe* pCrossPipe)
{
	int nIdx = CListCtrl::InsertItem( _nIdx, LPCTSTR(""));
	ASSERT(nIdx == _nIdx);
	SetItemData(nIdx,(DWORD)pCrossPipe);
}


void CPipeAttachCrosswalkListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis )
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


	CrossWalkAttachPipe* pCrossPipe = (CrossWalkAttachPipe*)lvi.lParam;
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
	CString strValue;;
	CString strCrosswalk;
	strCrosswalk = pCrossPipe->GetAttachWalk()->getName().GetIDString();
	pDC->DrawText(strCrosswalk, rcVisible1, DT_LEFT| DT_VCENTER | DT_SINGLELINE );

	//draw checkbox
	CDC bitmapDC;
	HBITMAP hOldBM;
	GroupIndex groupIndex = pCrossPipe->GetGroupIndex();
	int nCheck = 0;
	if (groupIndex.start != -1 && groupIndex.end != -1)
	{
		nCheck = 1;
	}
	rcVisible2.left+=(rcVisible2.Width()-m_sizeCheck.cx)/2;
	rcVisible2.top+=(rcVisible2.Height()-m_sizeCheck.cy)/2;
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcVisible2.left, rcVisible2.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
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

BOOL CPipeAttachCrosswalkListCtrl::GetItemCheck( int nItem )
{
	CString strCheck=GetItemText(nItem,1);
	if(strCheck==CString("1"))
		return TRUE;
	return FALSE;
}

LRESULT CPipeAttachCrosswalkListCtrl::OnCheckStateChanged( WPARAM wParam, LPARAM lParam )
{
	int iItem = (int) wParam;
	bool flag = lParam ? true : false;


	if( iItem != -1 )
	{
 		CrossWalkAttachPipe* pCrossPipe = (CrossWalkAttachPipe*)GetItemData(iItem);
 		GroupIndex groupIndex = pCrossPipe->GetGroupIndex();
		if (flag)
		{
			groupIndex.end = m_nIdx;
			pCrossPipe->SetGroupIndex(groupIndex);
		}
		else
		{
			groupIndex.end = -1;
			pCrossPipe->SetGroupIndex(groupIndex);
		}
	}

	return TRUE;
}

void CPipeAttachCrosswalkListCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	int nItem=HitTest(point);
	if(nItem != -1) 
	{
		CRect rcVisible;
		GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
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

void CPipeAttachCrosswalkListCtrl::SetItemCheck( int nItem,BOOL bCheck )
{
	if(bCheck)
		SetItemText(nItem,1,"1");
	else
		SetItemText(nItem,1,"0");
}

void CPipeAttachCrosswalkListCtrl::SetEndIndex( int nEndIndex )
{
	m_nIdx = nEndIndex;
}


// CDlgPipeAttachCrosswalk dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgPipeAttachCrosswalk, CXTResizeDialog)
CDlgPipeAttachCrosswalk::CDlgPipeAttachCrosswalk(CPipe* pPipe,int nEndIndex,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPipeAttachCrosswalk::IDD, pParent)
{
	m_pPipe = pPipe;
	m_nIdx = nEndIndex;
}

CDlgPipeAttachCrosswalk::~CDlgPipeAttachCrosswalk()
{
}

void CDlgPipeAttachCrosswalk::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgPipeAttachCrosswalk, CXTResizeDialog)
END_MESSAGE_MAP()


// CDlgPipeAttachCrosswalk message handlers



BOOL CDlgPipeAttachCrosswalk::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_wndListCtrl.SetEndIndex(m_nIdx);
	InitListCtrlHead();
	SetListCtrlContent();

	CString strPipe = m_pPipe->GetPipeName();
	GetDlgItem(IDC_STATIC_GROUP)->SetWindowText(strPipe);
	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

InputTerminal* CDlgPipeAttachCrosswalk::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

CString CDlgPipeAttachCrosswalk::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CDlgPipeAttachCrosswalk::OnOK()
{
	GetInputTerminal()->m_pPipeDataSet->saveDataSet(GetProjPath(),true);
	CXTResizeDialog::OnOK();
}

void CDlgPipeAttachCrosswalk::OnCancel()
{
	GetInputTerminal()->m_pPipeDataSet->loadDataSet(GetProjPath());
	CDialog::OnCancel();
}

void CDlgPipeAttachCrosswalk::InitListCtrlHead()
{
 	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
 	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
 	m_wndListCtrl.SetExtendedStyle(dwStyle);
 
 	m_wndListCtrl.InsertColumn(0, "Cross walk", LVCFMT_LEFT, 150);
	m_wndListCtrl.InsertColumn(1, "Attach Crosswalk",LVCFMT_LEFT,150);

}

void CDlgPipeAttachCrosswalk::SetListCtrlContent()
{
	int itemidx =0;
	for (int i = 0; i < m_pPipe->GetAttachCrosswalkCount(); i++)
	{
		CrossWalkAttachPipe& crossPipe = m_pPipe->GetAtachCrosswalk(i);

		m_wndListCtrl.InsertItem(itemidx,&crossPipe);
		itemidx++;
	}
}
