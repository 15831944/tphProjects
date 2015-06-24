// DlgAircraftModelDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgComponentDBDefine.h"
#include "DlgAircraftModelItemDefine.h"
#include <inputonboard/AircraftComponentModelDatabase.h>
#include <inputonboard/ComponentModel.h>
#include "../Common/WinMsg.h"
#include "../Common/fileman.h"
//#include "Onboard/ComponentModelEditView.h"
#include "ViewMsg.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"
//#include "./DlgEditAircraftComponent.h"
#include "Onboard/OnboardViewMsg.h"
#include "../InputOnBoard/CInputOnboard.h"
//#include "../InputOnBoard/ComponentEditContext.h"
#include "onboard/MeshEdit3DView.h"

#include <Renderer/RenderEngine/RenderEngine.h>
#include <common/BitmapSaver.h>
#define  TIME_FORMAT _T("%m/%d/%Y %H:%M")

// CDlgAircraftModelDefine dialog
IMPLEMENT_DYNAMIC(CDlgComponentDBDefine, CDialog)

CDlgComponentDBDefine::CDlgComponentDBDefine(CAircraftComponentModelDatabase* pAircraftComponentModelDB,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComponentDBDefine::IDD, pParent)
	, m_pModelToEditShape(NULL)
{
	ASSERT(pAircraftComponentModelDB);
	m_pAircraftComponentModelDB = pAircraftComponentModelDB;
	ClearBmps(false);
}

CDlgComponentDBDefine::~CDlgComponentDBDefine()
{
	ClearBmps(true);
}

void CDlgComponentDBDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AIRCRAFT, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgComponentDBDefine, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText ) 
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_AIRCRAFT,OnSelChangeACTypeModel)
	ON_COMMAND(ID_AIRCRAFT_NEW,OnAddAircraftComponentItem)
	ON_COMMAND(ID_AIRCRAFT_EDIT,OnEditAircraftComponentShape)
	ON_COMMAND(ID_AIRCRAFT_DELETE,OnRemoveAircraftComponentItem)
	ON_COMMAND(ID_EDIT_COMPONENT,OnEditAircraftComponentShape)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDBClick)
	ON_NOTIFY(NM_RCLICK,IDC_LIST_AIRCRAFT, OnRClick)
END_MESSAGE_MAP()


// CDlgAircraftModelDefine message handler

BOOL CDlgComponentDBDefine::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitToolbar();
	OnInitListCtrl();
	PopulateACModelItem();
	UpdateToolbarState();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgComponentDBDefine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_FLYBY | CBRS_SIZE_DYNAMIC
		| CBRS_TOOLTIPS ) ||
		!m_wndToolbar.LoadToolBar(IDR_AIRCRAFT_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}
BOOL CDlgComponentDBDefine::OnEraseBkgnd(CDC*  pDC)     
{   
	return TRUE;   
}   
void CDlgComponentDBDefine::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_DELETE,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_EDIT,TRUE);
}

void CDlgComponentDBDefine::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Index", "Level 1 name", "Level 2 name","Level 3 name","Level 4 name","Creation date","Modified date","Creator"};
	int DefaultColumnWidth[] = { 50, 100, 100,100,100,120,120,120};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT
	};

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	lvc.csList = &strList;

	for (int i = 0; i < 8; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

BOOL CDlgComponentDBDefine::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	//if (GetRoutingFrame() != NULL) return FALSE;

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID!= 0) // will be zero on a separator
	{

		AfxLoadString(nID, szFullText);
		strTipText=szFullText;

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}

void CDlgComponentDBDefine::OnPaint()
{	
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))
			|| pWnd == GetDlgItem(IDC_STATIC_SIDE)
			|| pWnd == GetDlgItem(IDC_STATIC_FRONT)
			|| pWnd == GetDlgItem(IDC_STATIC_ISOMETRIC)
			|| pWnd == GetDlgItem(IDC_STATIC_PLAN))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);

	DisplayAllBmpInCtrl();
}

void CDlgComponentDBDefine::DisplayBmpInCtrl(HBITMAP hBmp,UINT nID)
{
	if (hBmp != NULL)
	{
		CBitmap bmpObj;
		bmpObj.Attach(hBmp);
		CWnd *pWnd = GetDlgItem(nID);
		CDC *pDC = pWnd->GetDC();

		CDC DCCompatible;
		DCCompatible.CreateCompatibleDC(pDC);
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)DCCompatible.m_hDC , 0);

		CRect rect;
		pWnd->GetClientRect(rect);
		CBitmap *pOldBmp = DCCompatible.SelectObject(&bmpObj);
		BITMAP bmpInfo;
		bmpObj.GetObject(sizeof(bmpInfo),&bmpInfo);

		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &DCCompatible, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );
		DCCompatible.SelectObject(pOldBmp);
		bmpObj.Detach();
		return;

	}
	// bkcolor
	CRect rcClient;
	CWnd *pWnd = GetDlgItem(nID);
	CDC *dc = pWnd->GetDC();
	pWnd->GetClientRect(&rcClient);

	CBrush pOrgBrush(RGB(255,255,255));
	dc->FillRect(rcClient,&pOrgBrush);
}

void CDlgComponentDBDefine::OnAddAircraftComponentItem()
{
	CComponentMeshModel* pModel = new CComponentMeshModel(m_pAircraftComponentModelDB);
    CDlgAircraftModelItemDefine dlg(m_pAircraftComponentModelDB,pModel);
	dlg.SetNewComponent(true);
	if(dlg.DoModal() == IDOK)
	{
		ALTObjectID altObjID;
		altObjID.FromString(pModel->m_sModelName);
		if (!m_pAircraftComponentModelDB->getModelByName(altObjID))
		{
			m_pAircraftComponentModelDB->AddModel(pModel);
			PopulateACModelItem();
			return;

		}
	}
	delete pModel;
}

void CDlgComponentDBDefine::OnEditAircraftComponentItem()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CComponentMeshModel* pModel = (CComponentMeshModel*)m_wndListCtrl.GetItemData(nSel);
		ASSERT(pModel);
		CDlgAircraftModelItemDefine dlg(m_pAircraftComponentModelDB,pModel);
		if (dlg.DoModal() == IDOK)
		{
			SetListCtrlItemText(nSel,pModel);
		}
	}
}

void CDlgComponentDBDefine::OnRemoveAircraftComponentItem()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CComponentMeshModel* pModel = (CComponentMeshModel*)m_wndListCtrl.GetItemData(nSel);
		ASSERT(pModel);

		m_pAircraftComponentModelDB->DelModel(pModel);
		m_wndListCtrl.DeleteItemEx(nSel);
	}
}

void CDlgComponentDBDefine::OnCancel()
{
	CWaitCursor wc;
	m_pAircraftComponentModelDB->ClearData();
	m_pAircraftComponentModelDB->loadDatabase(m_pAircraftComponentModelDB->m_pAirportDB);
	CDialog::OnCancel();
}

void CDlgComponentDBDefine::UpdateToolbarState()
{
	if (m_wndListCtrl.GetCurSel() == LB_ERR)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_EDIT,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_DELETE,FALSE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_EDIT,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_DELETE,TRUE);
	}
}

void CDlgComponentDBDefine::OnSelChangeACTypeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	UpdateToolbarState();

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
	{
		GetDlgItem(IDC_STATIC_PLAN)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
		return;
	}

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		GetDlgItem(IDC_STATIC_PLAN)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			DisplayBmpInCtrl(NULL,IDC_STATIC_PLAN);
			DisplayBmpInCtrl(NULL,IDC_STATIC_SIDE);
			DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT);
			DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC);
			

			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_EDIT,FALSE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_DELETE,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		CComponentMeshModel* pModel = (CComponentMeshModel*)m_wndListCtrl.GetItemData(pnmv->iItem);
		if (pModel)
		{
			LoadBmp(pModel);
		}
		DisplayAllBmpInCtrl();
	}

	GetDlgItem(IDC_STATIC_PLAN)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
}

void CDlgComponentDBDefine::PopulateACModelItem()
{
	int nCount = m_pAircraftComponentModelDB->GetModelCount();
	m_wndListCtrl.DeleteAllItems();

	for(int i=0; i < nCount; i++)
	{
		CComponentMeshModel* pModel = m_pAircraftComponentModelDB->GetModel(i);

		CString strValue = _T("");
		strValue.Format(_T("%d"),i+1);
		m_wndListCtrl.InsertItem(i, strValue);
		ALTObjectID altObjID;
		altObjID.FromString(pModel->m_sModelName);
		m_wndListCtrl.SetItemText(i, 1, altObjID.at(0).c_str());
		m_wndListCtrl.SetItemText(i, 2, altObjID.at(1).c_str());
		m_wndListCtrl.SetItemText(i, 3, altObjID.at(2).c_str());
		m_wndListCtrl.SetItemText(i, 4, altObjID.at(3).c_str());
		m_wndListCtrl.SetItemText(i, 5, pModel->GetCreateTime().Format(TIME_FORMAT) );
		m_wndListCtrl.SetItemText(i, 6, pModel->GetLastModifiedTime().Format(TIME_FORMAT) );
		m_wndListCtrl.SetItemText(i, 7, pModel->GetCreator() );
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pModel);
	}
}

void CDlgComponentDBDefine::SetListCtrlItemText(int nItem,const CComponentMeshModel* pModel)
{
	CString strValue = _T("");
	strValue.Format(_T("%d"),nItem + 1);
	ALTObjectID altObjID;
	altObjID.FromString(pModel->m_sModelName);
	m_wndListCtrl.SetItemText(nItem, 0,strValue);
	m_wndListCtrl.SetItemText(nItem, 1, altObjID.at(0).c_str());
	m_wndListCtrl.SetItemText(nItem, 2, altObjID.at(1).c_str());
	m_wndListCtrl.SetItemText(nItem, 3, altObjID.at(2).c_str());
	m_wndListCtrl.SetItemText(nItem, 4, altObjID.at(3).c_str());
	m_wndListCtrl.SetItemText(nItem, 5, pModel->GetCreateTime().Format(TIME_FORMAT));
	m_wndListCtrl.SetItemText(nItem, 6, pModel->GetLastModifiedTime().Format(TIME_FORMAT));
	m_wndListCtrl.SetItemText(nItem, 7, pModel->GetCreator());
	m_wndListCtrl.SetItemData(nItem,(DWORD_PTR)pModel);
}
void CDlgComponentDBDefine::OnOK()
{
	CWaitCursor wc;
	m_pAircraftComponentModelDB->SaveData();
	CDialog::OnOK();
}

LRESULT CDlgComponentDBDefine::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	OnEditAircraftComponentItem();
	return 0;
}

void CDlgComponentDBDefine::DisplayAllBmpInCtrl()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CComponentMeshModel*  pModel= (CComponentMeshModel*)m_wndListCtrl.GetItemData(nSel);
		
		ALTObjectID altObjID;
		altObjID.FromString(pModel->m_sModelName);
	    if (pModel)
	    {
			HBITMAP* hBmp = m_hBmp;
		
			
			DisplayBmpInCtrl(hBmp[0],IDC_STATIC_PLAN);
			DisplayBmpInCtrl(hBmp[1],IDC_STATIC_SIDE);
			DisplayBmpInCtrl(hBmp[2],IDC_STATIC_FRONT);
			DisplayBmpInCtrl(hBmp[3],IDC_STATIC_ISOMETRIC);

			GetDlgItem(IDC_STATIC_PLAN)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
			return;
	    }
	}

	DisplayBmpInCtrl(NULL,IDC_STATIC_PLAN);
	DisplayBmpInCtrl(NULL,IDC_STATIC_SIDE);
	DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT);
	DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC);

	GetDlgItem(IDC_STATIC_PLAN)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);

}

void CDlgComponentDBDefine::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint cliPt;
	GetCursorPos(&cliPt);
	CPoint point = cliPt;
	m_wndListCtrl.ScreenToClient(&cliPt);
	int index;
	if((index = m_wndListCtrl.HitTestEx(cliPt, NULL)) != -1)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_COMPONENT);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);
	}
}

void CDlgComponentDBDefine::OnEditAircraftComponentShape()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
	{
		MessageBox(_T("Please select one component model item to modify the shape!"), NULL, MB_OK);
		return;
	}

	m_pModelToEditShape = (CComponentMeshModel*)m_wndListCtrl.GetItemData(nSel);
	OnOK();
}

// void CDlgAircraftModelDefine::HideDialog(CWnd* parentWnd)
// {
// 	ShowWindow(SW_HIDE);
// 	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
// 		parentWnd->ShowWindow(SW_HIDE);
// 		parentWnd = parentWnd->GetParent();
// 	}
// 	parentWnd->EnableWindow(TRUE);
// 	parentWnd->SetActiveWindow();
// }

// void CDlgAircraftModelDefine::ShowDialog(CWnd* parentWnd)
// {
// 	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
// 	{
// 		parentWnd->ShowWindow(SW_SHOW);
// 		parentWnd->EnableWindow(FALSE);
// 		parentWnd = parentWnd->GetParent();
// 	}
// 	parentWnd->EnableWindow(FALSE);
// 	ShowWindow(SW_SHOW);	
// 	EnableWindow();
// }

void CDlgComponentDBDefine::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);

	if (m_wndListCtrl.m_hWnd != NULL)
	{
		CRect rect;
		CRect rc;
		GetDlgItem(IDC_STATIC_COMPONENTS)->SetWindowPos(NULL,10,11,cx-2*10,16,NULL);
		GetDlgItem(IDC_STATIC_COMPONENTS)->GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndToolbar.SetWindowPos(NULL,rect.left+1,rect.bottom,cx-22,26,NULL);
		m_wndToolbar.GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndListCtrl.SetWindowPos(NULL,rect.left+1,rect.bottom + 2,cx-2*rect.left-4,cy/2-rect.bottom - 8,NULL);
		m_wndListCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);
		
		GetDlgItem(IDC_STATIC_GROUP)->SetWindowPos(NULL,rc.left+1,rc.bottom + 5,cx - 2*rect.left-4,cy- rc.bottom -45,NULL);
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rc);
		ScreenToClient(rc);

		GetDlgItem(IDC_STATIC_Z)->SetWindowPos(NULL,rc.left+2,rc.top + 20,rc.Width()/4 -2,16,NULL);
		GetDlgItem(IDC_STATIC_Y)->SetWindowPos(NULL,rc.left+rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_X)->SetWindowPos(NULL,rc.left+rc.Width()/2,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_ISO)->SetWindowPos(NULL,rc.right-rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);

		CRect rcView;
		GetDlgItem(IDC_STATIC_PLAN)->SetWindowPos(NULL,rc.left,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_PLAN)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_SIDE)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_SIDE)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_FRONT)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_FRONT)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.right-rcView.right,rc.Height()-38,NULL);

		CRect rcBtn;
		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rc.right - rcBtn.Width() - 2,rc.bottom + 2,rcBtn.Width(),rcBtn.Height(),NULL);
	}
}

void CDlgComponentDBDefine::ClearBmps( bool bDelete )
{
	if(bDelete)
	{
		for(int i=0;i<ACTYPEBMP_COUNT;i++)
			::DeleteObject(m_hBmp[i]);
	}
	for(int i=0;i<ACTYPEBMP_COUNT;i++)
		m_hBmp[i] = NULL;
}

void CDlgComponentDBDefine::LoadBmp( CComponentMeshModel* pModel )
{
	ClearBmps(true);
	HBITMAP* hBmp = m_hBmp;
	if(!pModel->MakeSureFileOpen())
	{
		return;
	}


	CString sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_X;			
	hBmp[0] = BitmapSaver::FromFile(sBmpFile);	
	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Y;
	hBmp[1] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Z;
	hBmp[2] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_P;
	hBmp[3] = BitmapSaver::FromFile(sBmpFile);	
}