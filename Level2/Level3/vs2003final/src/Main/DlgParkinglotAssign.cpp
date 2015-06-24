// DlgParkinglotAssign.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgParkinglotAssign.h"
#include "Resource.h"
#include "../Landside/ParkingFligthType.h"
#include "../Landside/ParkingLotDetail.h"
#include "../Landside/ParkingLotTimeRange.h"
#include "TermPlanDoc.h"
#include "../Common/FLT_CNST.H"
#include "DlgCurbsideSelect.h"
#include "DlgTimeRange.h"
#include "../Common/ALT_TYPE.h"
#include "../Landside/LandsideLayoutObject.h"
#include "FlightDialog.h"
#include "../Landside/InputLandside.h"
#include "../Common/WinMsg.h"
// CDlgParkinglotAssign dialog

IMPLEMENT_DYNAMIC(CDlgParkinglotAssign, CXTResizeDialog)
CDlgParkinglotAssign::CDlgParkinglotAssign(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgParkinglotAssign::IDD, pParent)
{
}

CDlgParkinglotAssign::~CDlgParkinglotAssign()
{
}
CDlgParkinglotAssign::CDlgParkinglotAssign(CTermPlanDoc *tmpDoc,CWnd *pParent/* =NULL */)
:CXTResizeDialog(CDlgParkinglotAssign::IDD,pParent)
{
   pDoc=tmpDoc;
   m_bExpand=FALSE;
   m_onInit=FALSE;
   m_sizeLastWnd=CSize(0,0);
}

int CDlgParkinglotAssign::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    
	
	return 0;
}
BOOL CDlgParkinglotAssign::OnInitDialog()
{
	
	CXTResizeDialog::OnInitDialog();
	if(!m_treeToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_treeToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		TRACE(_T("Create tool bar error "));
		return FALSE;
	}


	if (!m_listToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_listToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE(_T("Create tool bar error"));
		return FALSE;
	}

	InitToolBar();
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);

	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE);
	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle(dwStyle);

	// set the runway assign list header contents
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Parkinglot");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Percent");
	lvc.fmt = LVCFMT_NUMBER;
	m_listCtrl.InsertColumn(1, &lvc);
	m_pLandSide=pDoc->getARCport()->m_pInputLandside;
	InitTree();
	SetResize(IDC_STATIC_PARKINTLOTFLIGHT, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(m_treeToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_PARKINGLOTASSIGN, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_STATIC_PARKINGLOTDETAIL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_listToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_PARKINGLOTASSIGN,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDPARKINGLOTASSIGNSAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	m_listRightWnd.Add(&m_treeCtrl); 
    m_listRightWnd.Add(&m_frameFlight);

	
	m_listLeftBtn.Add(&m_btnCancel);
	m_listLeftBtn.Add(&m_btnOk);
	m_listLeftBtn.Add(&m_btnSave);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return true;
}
void CDlgParkinglotAssign::ResizeToHideRightWnd()
{
	/*CRect rectBtn,rectWnd;
	m_butAdvance.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);

	GetWindowRect(rectWnd);
	SetWindowPos(NULL,0,0,rectBtn.right +20,rectWnd.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	ShowRightWnd(FALSE);*/
}
void CDlgParkinglotAssign::ShowRightWnd(BOOL bShow)
{
	m_bExpand=bShow;
	for(int i=0;i<m_listRightWnd.GetSize();i++)
	{
		m_listRightWnd[i]->ShowWindow(bShow?SW_SHOW:SW_HIDE);
	}
}
void CDlgParkinglotAssign::InitTree()
{
	CParkingFligthType *tmpFlight;
	//CParkingLotTimeRange   *timeRang;
	HTREEITEM hParentItem=NULL;
	HTREEITEM hChildItem=NULL;
	CString strTimeName;
	m_treeCtrl.DeleteAllItems();
	//m_treeCtrl.SetRedraw(FALSE);
	int j=0;
	CString strFilghtType;
	for (int i=0;i<m_pLandSide->getParkingFlightTypeList().getItemCount();i++)
	{
		//pStrategyItem->GetFlightType().screenPrint();
		tmpFlight=m_pLandSide->getParkingFlightTypeList().getItem(i);
		tmpFlight->getFlightConstraint().screenPrint(strFilghtType.GetBuffer(1024));
		if (hParentItem==NULL)
		{
			hParentItem=m_treeCtrl.InsertItem(strFilghtType,TVI_ROOT);
			m_treeCtrl.SetItemData(hParentItem,(DWORD_PTR) tmpFlight);
		}
		else 
		{
			hParentItem=m_treeCtrl.InsertItem(strFilghtType,TVI_ROOT,hParentItem);
			m_treeCtrl.SetItemData(hParentItem,(DWORD_PTR)tmpFlight);
		}

		/*for ( j=0;j<tmpFlight->getItemCount();j++)
		{
			timeRang=tmpFlight->getItem(j);
			timeRang->getName(strTimeName);
			if(j==0)
			{

				hChildItem=m_treeCtrl.InsertItem(strTimeName,hParentItem);
			}
			else 
			{
				hChildItem=m_treeCtrl.InsertItem(strTimeName,hParentItem);
			}
			m_treeCtrl.SetItemData(hChildItem,(DWORD_PTR)timeRang);
			m_treeCtrl.Expand(hChildItem,TVE_EXPAND);
		}*/
		m_treeCtrl.Expand(hParentItem,TVE_EXPAND);
	}
	//m_treeCtrl.SetRedraw(TRUE);
}
void CDlgParkinglotAssign::InitToolBar()
{
	//CRect rcDlg;
	//GetWindowRect(&rcDlg);
	//ScreenToClient(&rcDlg);

	//CRect rcToolbar(12, 15,rcDlg.right-10,40);
	CRect rect;
	CRect listRect;
	m_treeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_treeToolBar.MoveWindow(rect);
	m_treeToolBar.ShowWindow(SW_SHOW);
	//m_treeCtrl(&rect);
	m_listCtrl.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	listRect.top=listRect.top-26;
	listRect.left=listRect.left+4;
	listRect.bottom=listRect.top+22;
	m_listToolBar.MoveWindow(listRect);
	/*CRect rect;
	CRect listRect;
	m_treeCtrl.GetClientRect(&rect);
	m_treeToolBar.MoveWindow(12,25,rect.right-10,40);
	m_treeToolBar.ShowWindow(SW_SHOW);
	m_treeCtrl.GetClientRect(&rect);
	m_listCtrl.GetClientRect(&listRect);
	m_listToolBar.MoveWindow(rect.right+30,25,listRect.right-10,40);*/
}
void CDlgParkinglotAssign::OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	//CParkingLotTimeRange *timeRange;
	CParkingFligthType *flightType;
	m_listCtrl.DeleteAllItems();
	if(getTreeItemByType(TREENODE_FLTTYPE, m_hTimeRangeTreeItem))
	{
		flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		CString strPercent;
		for(int i=0;i<flightType->getItemCount();i++)
		{
			CParkingLotDetail *detail=flightType->getItem(i);
			m_listCtrl.InsertItem(i,"");
			m_listCtrl.SetItemText(i,0,detail->getName());
			strPercent.Format("%d",detail->getPecent());
			m_listCtrl.SetItemText(i,1,strPercent);
		}
	}
}
void CDlgParkinglotAssign::OnTvnSelChangedFlight(NMHDR*pNMHDR,LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateTreeToolBar();
	OnSelChangedFltTimeTree(pNMHDR,pResult);
	*pResult=0;

}
BOOL CDlgParkinglotAssign::getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem )
{
	HTREEITEM selectItem=m_treeCtrl.GetSelectedItem();
	HTREEITEM hParentItem;
	if (selectItem==NULL)
		return false;
	switch(nodeType)
	{
	case TREENODE_FLTTYPE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if (hParentItem!=NULL)
		{
			return FALSE;
		}
		hItem=selectItem;
		break;
	case TREENODE_TIMERANGE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if(hParentItem==NULL)
		{
			return FALSE;
		}
		hItem=selectItem;

		break;
	default:
		return FALSE;


	}
	return TRUE;
}

void CDlgParkinglotAssign::OnNewFlightType()
{
	CFlightDialog dlg( m_pParentWnd );
    CString strConPrintName;
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint fltConstr = dlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltConstr.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (hItem)
		{
			if (m_treeCtrl.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
 
		CParkingFligthType *tmpFlight=new CParkingFligthType();
		fltConstr.WriteSyntaxStringWithVersion(szBuffer);
		tmpFlight->setAirportDataBase(pDoc->GetTerminal().m_pAirportDB);
		tmpFlight->setName(szBuffer);
		
		hItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT);
		m_treeCtrl.SetItemData(hItem,(DWORD_PTR)tmpFlight);
		m_treeCtrl.SelectItem(hItem);
		m_pLandSide->getParkingFlightTypeList().AddItem(tmpFlight);
		//m_treeCtrl.SetRedraw(true);
		m_treeCtrl.Expand(hItem,TVE_EXPAND);
         if (m_listCtrl.GetItemCount()>=0)
		 {
			 m_listCtrl.DeleteAllItems();
		 }

		//m_btnSave.EnableWindow( TRUE );
		// InsertTreeItemorder(tmpFlight);
	}	
}
void CDlgParkinglotAssign::InsertTreeItemorder(CParkingFligthType *tmpFligType)
{
	CString strBuffer;
	HTREEITEM hBrotherItem;
       CParkingFligthType *flightType=NULL;
		for (int i=0;i<m_pLandSide->getParkingFlightTypeList().getItemCount();i++)
		{
			if (i==0&&m_pLandSide->getParkingFlightTypeList().getItem(i)==tmpFligType)
			{
				break;
			}
			if (m_pLandSide->getParkingFlightTypeList().getItem(i)==tmpFligType)
			{
				flightType=m_pLandSide->getParkingFlightTypeList().getItem(i-1);
				break;
			}
		}
		HTREEITEM hitem=NULL;
		if (flightType)
		{
			hitem=m_treeCtrl.GetRootItem();
		}
		
		while (true&&flightType)
		{
			CParkingFligthType *fType=(CParkingFligthType*)m_treeCtrl.GetItemData(hitem);
			if (fType==flightType)
			{
				break;
			}
			hitem=m_treeCtrl.GetNextItem(hitem,TVGN_NEXT);
		}
		tmpFligType->getFlightConstraint().screenPrint(strBuffer.GetBuffer(1024));
		if (hitem)
		{
			
			hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT,hitem);
		}
		else
		{
			
			hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT,TVI_FIRST);
		}
		m_treeCtrl.SetItemData(hBrotherItem,(DWORD_PTR)tmpFligType);
		m_treeCtrl.SetItemState(hBrotherItem,TVIS_SELECTED,TVIS_STATEIMAGEMASK);
}
void CDlgParkinglotAssign::EditTreeItemOrder(CParkingFligthType *tmpFlightType)
{
	CString strBuffer;
	HTREEITEM hBrotherItem;
	CParkingFligthType *flightType=NULL;
	for (int i=0;i<m_pLandSide->getParkingFlightTypeList().getItemCount();i++)
	{
		if (i==0&&m_pLandSide->getParkingFlightTypeList().getItem(i)==tmpFlightType)
		{
			break;
		}
		if (m_pLandSide->getParkingFlightTypeList().getItem(i)==tmpFlightType)
		{
			flightType=m_pLandSide->getParkingFlightTypeList().getItem(i-1);
			break;
		}
	}
	HTREEITEM hitem=m_treeCtrl.GetRootItem();
	while (true&&flightType)
	{
		CParkingFligthType *fType=(CParkingFligthType*)m_treeCtrl.GetItemData(hitem);
		if (fType==flightType)
		{
			m_treeCtrl.DeleteItem(hitem);
			break;
		}
		hitem=m_treeCtrl.GetNextItem(hitem,TVGN_NEXT);
	}
	tmpFlightType->getFlightConstraint().screenPrint(strBuffer.GetBuffer(1024));
	if (hitem)
	{

		hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT,hitem);
	}
	else
	{

		hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT);
	}
	m_treeCtrl.SetItemData(hBrotherItem,(DWORD_PTR)tmpFlightType);
	m_treeCtrl.SetItemState(hBrotherItem,TVIS_SELECTED,TVIS_STATEIMAGEMASK);
}
void CDlgParkinglotAssign::ResizeRightWnd(CSize size,CSize sizeLast)
{
	if(!m_bExpand) return;

	CRect rectWnd;
	int nTop;
	int nSplitterWidth=9;
	int nSubWidth,nSubHeight=48,nWidthCur,nHeightCur;
	/*m_FlowTree.GetWindowRect(rectWnd);*/
	ScreenToClient(rectWnd);
	int nLeft=rectWnd.right+nSplitterWidth;
	nSubWidth=nSplitterWidth*4;
	nWidthCur=int((size.cx-nSubWidth)*3/13.0)+2;

	nHeightCur=int((size.cy-nSubHeight)*3/10.0)+26;
	//FlightTitle Static
	//m_staticFlightTitle.GetWindowRect(rectWnd);
	//m_staticFlightTitle.SetWindowPos(NULL,nLeft,nSplitterWidth,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	//m_staticFlightTitle.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);
	//Flight List
	nTop=rectWnd.bottom+2;
	m_listCtrl.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_listCtrl.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter5
	int nSplitter5Top=rectWnd.top+nSplitterWidth;
	if(!m_wndSplitter5.m_hWnd)//Create Splitter1
	{
		m_wndSplitter5.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER5);
		m_wndSplitter5.SetStyle(SPS_HORIZONTAL);
		m_listRightWnd.Add(&m_wndSplitter5);
	}


	nTop=rectWnd.bottom;
	nHeightCur=nSplitterWidth;
	m_wndSplitter5.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter5.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	//GateTitle Static
	nTop=rectWnd.bottom;
	//m_staticGateTitle.GetWindowRect(rectWnd);
	//m_staticGateTitle.SetWindowPos(NULL,nLeft,nTop,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	//m_staticGateTitle.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);

	//int nSplitter5Bottom=size.cy-nSubHeight-rectWnd.Height()-2-nSplitterWidth;
	//m_wndSplitter5.SetRange(nSplitter5Top,nSplitter5Bottom);
	////Gate Tree
	//nTop=rectWnd.bottom+2;
	//nHeightCur=size.cy-nSubHeight-nTop;
	//m_treeGate.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	//m_treeGate.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);
	//Splitter4-------------------------------------------------------------------------------------------
	//if(!m_wndSplitter4.m_hWnd)
	//{
	//	m_wndSplitter4.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER4);
	//	m_wndSplitter4.SetStyle(SPS_VERTICAL);
	//	m_listRightWnd.Add(&m_wndSplitter4);
	//}
	//m_wndSplitter4.SetWindowPos(NULL,rectWnd.left-nSplitterWidth,nSplitterWidth,nSplitterWidth,
	//	rectWnd.bottom-nSplitterWidth,SWP_NOACTIVATE|SWP_NOZORDER);
	//int nSplitter4Left,nSplitter4Right;


	//nSplitter4Right=rectWnd.right-nSplitterWidth;
	//m_butRestore.GetClientRect(rectWnd);
	//nSplitter4Right-=rectWnd.Width();
	//m_FlowTree.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);
	//nSplitter4Left=rectWnd.left+nSplitterWidth;
	//m_butAdvance.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);
	//int nRightTemp=rectWnd.right;
	//m_btnSave.GetWindowRect(rectWnd);
	//ScreenToClient(rectWnd);
	//int nLeftTemp=rectWnd.left;
	//int nWidthTemp=nRightTemp-nLeftTemp+nSplitterWidth;
	//if(nWidthTemp>nSplitter4Left)
	//	nSplitter4Left=nWidthTemp;
	//m_wndSplitter4.SetRange(nSplitter4Left,nSplitter4Right);
	//----------------------------------------------------------------------------------------------------

}
void CDlgParkinglotAssign::ResizeLeftWnd(CSize size,CSize sizeLast)
{
	CRect rectWnd;
	int nTop;
	int nSplitterWidth=9;
	int nLeft=nSplitterWidth;
	int nSubWidth,nSubHeight=48,nWidthCur,nHeightCur;
	if(m_bExpand)
	{
		nSubWidth=nSplitterWidth*4;
		nWidthCur=int((size.cx-nSubWidth)*2.5/13.0);

	}
	else
	{
		nSubWidth=nSplitterWidth*3;
		nWidthCur=int((size.cx-nSubWidth)*2.5/10.0);
	}
	
	m_treeCtrl.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	//Splitter1
	int nSplitter1Top=rectWnd.top+nSplitterWidth;
	if(!m_wndSplitter1.m_hWnd)//Create Splitter1
	{
		m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER1);
		m_wndSplitter1.SetStyle(SPS_HORIZONTAL);
	}


	nTop=rectWnd.bottom;
	nHeightCur=rectWnd.bottom+5;
	m_wndSplitter1.SetWindowPos(NULL,rectWnd.left,rectWnd.top,rectWnd.right+5,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter1.GetWindowRect(&rectWnd);
	ScreenToClient(rectWnd);
	nTop=rectWnd.bottom;


	int nSplitter1Bottom=size.cy-nSubHeight-rectWnd.Height()-2-nSplitterWidth;
	m_wndSplitter1.SetRange(nSplitter1Top,nSplitter1Bottom);


	
}
LRESULT CDlgParkinglotAssign::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	//if (message == WM_INPLACE_SPIN)
	//{
	//	LPSPINTEXT pst = (LPSPINTEXT) lParam;
	///*	if (pst->iPercent >= 0)
	//	{
	//		SetPercent( pst->iPercent );
	//	}*/
	//	//		m_OperatedPaxFlow.SetChangedFlag( true );		
	//	//		OnPaxflowEvenPercent();
	//	return TRUE;
	//}
	//else if (message == WM_NOTIFY)
	//{

	//}
	if (message == WM_NOTIFY)
	{
		switch(wParam)
		{
		case ID_WND_SPLITTER1:
		case ID_WND_SPLITTER2:
		case ID_WND_SPLITTER3:
		case ID_WND_SPLITTER4:
		case ID_WND_SPLITTER5:
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}
			break;
		}
	}
	if (message == WM_KEYDOWN )
	{
		char chCharCode = (TCHAR) wParam;    // character code 
		long lKeyData = lParam;   
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
void CDlgParkinglotAssign::DoResize(int delta,UINT nIDSplitter)
{
	CRect rectWnd;
	switch(nIDSplitter)
	{
	case ID_WND_SPLITTER1:
		{
			/*CSplitterControl::ChangeHeight(&m_ProcTree, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_ProcessTree,-delta,CW_BOTTOMALIGN);
			m_staticProcess.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticProcess.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);*/
		}
		break;
	case ID_WND_SPLITTER2:
		{
			/*CSplitterControl::ChangeWidth(&m_ProcTree, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ProcessTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticProcessor, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticProcess,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter1,delta,CW_LEFTALIGN);

			CSplitterControl::ChangeWidth(&m_listCtrlPaxType, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_FlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticMobileTag,- delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter3,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarFlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarMEType,-delta,CW_RIGHTALIGN);*/


		}
		break;
	case ID_WND_SPLITTER3:
		{
			/*CSplitterControl::ChangeHeight(&m_listCtrlPaxType, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_FlowTree,-delta,CW_BOTTOMALIGN);
			m_staticFlowTree.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticFlowTree.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);

			m_ToolBarFlowTree.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_ToolBarFlowTree.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);*/
		}
		break;
	case ID_WND_SPLITTER4:
		{

			/*CSplitterControl::ChangeWidth(&m_listCtrlPaxType, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_FlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticMobileTag,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter3,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarFlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarMEType,delta,CW_LEFTALIGN);
			for(int i=0;i<m_listLeftBtn.GetSize();i++)
			{
				m_listLeftBtn[i]->GetWindowRect(rectWnd);
				ScreenToClient(rectWnd);
				m_listLeftBtn[i]->SetWindowPos(NULL,rectWnd.left+delta,rectWnd.top,0,0,
					SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			}
			CSplitterControl::ChangeWidth(&m_listFlight, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_treeGate,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticGateTitle, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlightTitle,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter5,-delta,CW_RIGHTALIGN);*/

		}
		break;
	case ID_WND_SPLITTER5:
		/*{
			CSplitterControl::ChangeHeight(&m_listFlight, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_treeGate,-delta,CW_BOTTOMALIGN);
			m_staticGateTitle.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticGateTitle.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
		}*/
		break;
	}

	Invalidate();
}
void CDlgParkinglotAssign::OnSizeEx(int cx, int cy)
{
	if(m_sizeLastWnd.cx!=0&&m_sizeLastWnd.cy!=0)
	{
		ResizeLeftWnd(CSize(cx,cy),m_sizeLastWnd);
	
		ResizeRightWnd(CSize(cx,cy),m_sizeLastWnd);
		
	}
	// TODO: Add your message handler code here
	m_sizeLastWnd=CSize(cx,cy);
}
void CDlgParkinglotAssign::OnEditFlightType()
{
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
		return;
	if (m_hTimeRangeTreeItem==NULL)
		return;
	// HTREEITEM hItem;
	CParkingFligthType *tmpFlight=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CFlightDialog dlg( m_pParentWnd );

	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint fltConstr = dlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltConstr.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (hItem)
		{
			if (m_treeCtrl.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		fltConstr.WriteSyntaxStringWithVersion(szBuffer);
		tmpFlight->setName(szBuffer);
		m_treeCtrl.SetItemText(m_hTimeRangeTreeItem,strBuffer);
	    //m_pLandSide->getParkingFlightTypeList().Sort();
		
		/*InitTree();
		m_treeCtrl.SelectItem(m_hTimeRangeTreeItem);*/
		/*m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
		InsertTreeItemorder(tmpFlight);*/
	}
}

void CDlgParkinglotAssign::OnDelFlightType()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	//	 HTREEITEM hitem;
	CParkingFligthType *tmpFLight;
	tmpFLight=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for(int i=0;i<m_pLandSide->getParkingFlightTypeList().getItemCount();i++)
	{
		if(tmpFLight==m_pLandSide->getParkingFlightTypeList().getItem(i))
		{
			m_pLandSide->getParkingFlightTypeList().deleteItem(i);
			m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
			m_listCtrl.DeleteAllItems();
			break;
		}
	}
	
}
void CDlgParkinglotAssign::UpdateTreeToolBar()
{
	BOOL bEnabled=getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,bEnabled);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,bEnabled);
}

void CDlgParkinglotAssign::OnNewTimeRange()
{

	/*ElapsedTime startTime ;
	ElapsedTime endtime ;
	HTREEITEM hItem;
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return ;
	CDlgTimeRange DlgTimeRanger(startTime,endtime) ;
	CParkingLotTimeRange *timeRange=new CParkingLotTimeRange();
	CParkingFligthType *tmpFlight=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	hItem=m_treeCtrl.GetChildItem(m_hTimeRangeTreeItem);
	if(DlgTimeRanger.DoModal() == IDOK)
	{
		CString strTimeRange;
		strTimeRange.Format("%s - %s", DlgTimeRanger.GetStartTimeString(), DlgTimeRanger.GetEndTimeString());
		while (hItem)
		{
			if (!IsvalidTimeRange(m_treeCtrl.GetItemText(hItem), strTimeRange)&&m_hTimeRangeTreeItem!=hItem)
			{
				MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		timeRange->setName(strTimeRange);
		timeRange->setParentID(tmpFlight->getID());
		tmpFlight->AddItem(timeRange);
		HTREEITEM hItem=m_treeCtrl.InsertItem(strTimeRange,m_hTimeRangeTreeItem);
		m_treeCtrl.SetItemData(hItem,(DWORD_PTR) timeRange);
		m_treeCtrl.Expand(m_hTimeRangeTreeItem,TVE_EXPAND) ;
		if (m_listCtrl.GetItemCount()>=0)
		{
			m_listCtrl.DeleteAllItems();
		}

	}*/
}
void CDlgParkinglotAssign::UpdateListToolBar()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	BOOL bEnable = TRUE;
	if(pos == NULL)
		bEnable = FALSE;

	m_listToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE, bEnable);
	m_listToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, bEnable);
}

void CDlgParkinglotAssign::OnLvnItemChangedParkinglotList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	//OnListItemChanged(nItem, nSubItem);
	UpdateListToolBar();
	*pResult = 0;

}
void CDlgParkinglotAssign::OnEditTimeRange()
{
	ElapsedTime startTime;
	ElapsedTime endTime;
	HTREEITEM hItem;
	if(!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return ;
	CDlgTimeRange dlg(startTime,endTime);
	CParkingLotTimeRange *timeRange=(CParkingLotTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	hItem=m_treeCtrl.GetChildItem(m_treeCtrl.GetParentItem(m_hTimeRangeTreeItem));
	if (dlg.DoModal()==IDOK)
	{
		CString strTimeRange;
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());
		while (hItem)
		{
			if (!IsvalidTimeRange(m_treeCtrl.GetItemText(hItem), strTimeRange)&&m_hTimeRangeTreeItem!=hItem)
			{
				MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		timeRange->setName(strTimeRange);
		m_treeCtrl.SetItemText(m_hTimeRangeTreeItem,strTimeRange);
	}

}
void CDlgParkinglotAssign::OnDelTimeRange()
{
	/*if (!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	HTREEITEM hParentItem=m_treeCtrl.GetParentItem(m_hTimeRangeTreeItem);
	CParkingLotTimeRange *timeRange=(CParkingLotTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CParkingFligthType *tmpFlight=(CParkingFligthType*)m_treeCtrl.GetItemData(hParentItem);
	for(int i=0;i<tmpFlight->getItemCount();i++)
	{
		if (timeRange==tmpFlight->getItem(i))
		{
			tmpFlight->deleteItem(i);
			m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
			m_listCtrl.DeleteAllItems();
			break;
		}
	}*/
}
void CDlgParkinglotAssign::OnSave()
{
	int nCount = 0;
	int iTotals = 0;
	/*nCount = m_listCtrl.GetItemCount();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount ; i++)
		{
			iTotals += _ttol(m_listCtrl.GetItemText(i,1) );
		}
		if (iTotals != 100)
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}*/
	for (int i=0;i<m_pLandSide->getParkingFlightTypeList().getItemCount();i++)
	{
		CParkingFligthType *flightType=m_pLandSide->getParkingFlightTypeList().getItem(i);
		iTotals=0;
		for (int j=0;j<flightType->getItemCount();j++)
		{
			
			CParkingLotDetail *detail=flightType->getItem(j);
			iTotals+=detail->getPecent();
			
			
		}
		if (iTotals!=100 && flightType->getItemCount()>0)
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}

	m_pLandSide->getParkingFlightTypeList().SaveData();
	if (m_pLandSide->getParkingFlightTypeList().getItemCount()<=0)
	{
		m_pLandSide->getParkingFlightTypeList().ReadData();
	}
}
void CDlgParkinglotAssign::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();

}
void CDlgParkinglotAssign::OnCancel()
{   
	m_pLandSide->getParkingFlightTypeList().ReadData();
	CXTResizeDialog::OnCancel();
}
void CDlgParkinglotAssign::OnNewParkinglot()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	LandsideLayoutObjectList *objectList= &(m_pLandSide->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LPARKINGLOT,m_pParentWnd);
	int nCount = m_listCtrl.GetItemCount();	
	//int averPercent;
	if (dlg.DoModal()==IDOK)
	{
		m_listCtrl.InsertItem(m_listCtrl.GetItemCount(),"");
		CString strPercent = _T("");
		CParkingLotDetail *detail=new CParkingLotDetail();
		int i = 0;
		strPercent.Format(_T("%d"),100/(nCount + 1));
		for (i = 0; i < nCount + 1; i++)
		{
			if(i == nCount)
				strPercent.Format(_T("%d"),100-nCount*(100/(nCount + 1)));
			m_listCtrl.SetItemText(i,1,strPercent);
		}
		CParkingFligthType *flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		flightType->AddItem(detail);
		//detail->setCurbSideID(dlg.getLayoutObject()->getID());
		detail->setName(dlg.getCurbSideName());
		m_listCtrl.SetItemText(m_listCtrl.GetItemCount()-1,0,dlg.getCurbSideName());
		i = 0;
		for(int j=0;j<flightType->getItemCount();j++)
		{
			CParkingLotDetail *itrPercent=flightType->getItem(j);
			if(j == nCount)
				//(itrPercent) = 1.0*100-1.0*nCount*100/(nCount + 1);
				itrPercent->setPercent(100-nCount*(100/(nCount + 1)));
			else
				//	(itrPercent)->m_dPercent = 1.0*100/(nCount + 1);
				itrPercent->setPercent(100/(nCount + 1));
			itrPercent->setParentID(flightType->getID());

		}
	}

}
void CDlgParkinglotAssign::OnEditParkinglot()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
	{
		return ;
	}
	int nCursel=m_listCtrl.GetCurSel();
	if (nCursel<0)
		return;
	CParkingFligthType *flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);

	//CParkingLotTimeRange *timeRange=(CParkingLotTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CParkingLotDetail *detail=flightType->getItem(nCursel);
	LandsideLayoutObjectList *objectList=&(pDoc->getARCport()->m_pInputLandside->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LPARKINGLOT,m_pParentWnd);
	if (dlg.DoModal()==IDOK)
	{
		detail->setName(dlg.getCurbSideName());
		//detail->setCurbSideID(dlg.getLayoutObject()->getID());
		m_listCtrl.SetItemText(nCursel,0,dlg.getCurbSideName());
		m_pLandSide->getParkingFlightTypeList().Sort();
	}
	InitTree();
	m_treeCtrl.SelectItem(m_hTimeRangeTreeItem);
}
void CDlgParkinglotAssign::OnDelParkinglot()
{
	int averPercent=0;
	int alTotal=0;
	int nCursel=m_listCtrl.GetCurSel();
	if (nCursel<0)
		return;
	m_listCtrl.DeleteItem(nCursel);
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	CParkingFligthType *flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);

	//CParkingLotTimeRange *timeRange=(CParkingLotTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	flightType->deleteItem(nCursel);
	 alTotal=flightType->getItemCount();
	 if (alTotal<=0)
	 {
		 return;
	 }
	 averPercent=100/alTotal;
	 CParkingLotDetail *detail;
	 CString strPercent="";
	for (int i=0;i<alTotal-1;i++)
	{
		
		detail=flightType->getItem(i);
		detail->setPercent(averPercent);
		strPercent.Format("%d",averPercent);
		m_listCtrl.SetItemText(i,1,strPercent);
	}
	detail=flightType->getItem(i);
	detail->setPercent(100-(i*averPercent));
	strPercent.Format("%d",detail->getPecent());
	m_listCtrl.SetItemText(i,1,strPercent);
}
void CDlgParkinglotAssign::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals=0,nCount=0;
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nsubItem=plvItem->iSubItem;
	if (nItem<0)
		return ;
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	CParkingFligthType *flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	//CParkingLotTimeRange *timeRange=(CParkingLotTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for (int j=0;j<flightType->getItemCount();j++)
	{
		CParkingLotDetail *detail=flightType->getItem(j);
		if(plvItem->iItem!=j)
			iTotals+=detail->getPecent();
	}
	if(iTotals+_ttoi(m_listCtrl.GetItemText(nItem,1))>100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_listCtrl.SetItemText(nItem,1,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}
	m_listCtrl.OnEndlabeledit(pNMHDR,pResult);
	OnListItemEndLabelEdit(nItem,nsubItem);
}
bool CDlgParkinglotAssign::IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2)
{
	CString str1 = _T("");
	CString str2 = _T("");
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Left(str1.Find(' '));
	str2 = str2.Left(str2.Find(' '));
	str1.MakeLower();
	str2.MakeLower();
	if(str1 > str2)
		return (false);
	else if(str1 < str2)
		return (true);
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Mid(str1.Find(' ') + 1);
	str2 = str2.Mid(str2.Find(' ') + 1);
	if(str1 > str2)
		return (false);
	else
		return (true);
}
void CDlgParkinglotAssign::OnListItemEndLabelEdit(int nItem, int nSubItem)
{
	CString strValue = m_listCtrl.GetItemText(nItem, nSubItem);
	int nCurSel=m_listCtrl.GetCurSel();
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	CParkingFligthType *flightType=(CParkingFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CParkingLotDetail *detail=flightType->getItem(nCurSel);
	detail->setPercent(atoi(strValue));
}
void CDlgParkinglotAssign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PARKINGLOTASSIGN, m_treeCtrl);
	DDX_Control(pDX, IDC_LIST_PARKINGLOTASSIGN, m_listCtrl);
	DDX_Control(pDX,IDPARKINGLOTASSIGNSAVE,m_btnSave);
	DDX_Control(pDX,IDOK,m_btnOk);
	DDX_Control(pDX,IDCANCEL,m_btnCancel);
	DDX_Control(pDX,IDC_STATIC_PARKINTLOTFLIGHT,m_frameFlight);
	DDX_Control(pDX,IDC_STATIC_PARKINGLOTDETAIL,m_frameDetail);
}

void CDlgParkinglotAssign::OnContextMenu(CWnd*pWnd , CPoint pos)
{
	CRect rect;
	m_treeCtrl.GetWindowRect(&rect);
	if (!rect.PtInRect(pos))
		return;
	m_treeCtrl.SetFocus();
	CPoint point;
	point=pos;
	m_treeCtrl.ScreenToClient(&point);
	UINT iRet;
	HTREEITEM hitem=m_treeCtrl.HitTest(point,&iRet);
	if (iRet!=TVHT_ONITEMLABEL)
		hitem=NULL;
	if (hitem==NULL)
		return;
	/*m_treeCtrl.SelectItem(hitem);
	CMenu menu;
	menu.LoadMenu(IDR_MENU27);
	CMenu *pMenu=menu.GetSubMenu(0);
	if (getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		pMenu->EnableMenuItem(ID_LANDSIDE_DELTIMERANGE,MF_GRAYED);
		pMenu->EnableMenuItem(ID_LANDSIDE_EDITTIMERAGE,MF_GRAYED);

	}
	else if(getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem))
	{
		pMenu->EnableMenuItem(ID_LANDSIDE_NEWTIMERANGE,MF_GRAYED);
	}
	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pos.x,pos.y,this);*/
}
void CDlgParkinglotAssign::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	//OnSizeEx(cx,cy);
	
	Invalidate(FALSE);
}
BEGIN_MESSAGE_MAP(CDlgParkinglotAssign, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_PARKINGLOTASSIGN,OnTvnSelChangedFlight)
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnNewFlightType)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDelFlightType)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditFlightType)
	ON_COMMAND(ID_LANDSIDE_EDITTIMERAGE,OnEditTimeRange)
	ON_COMMAND(ID_LANDSIDE_NEWTIMERANGE,OnNewTimeRange)
	ON_COMMAND(ID_LANDSIDE_DELTIMERANGE,OnDelTimeRange)
	ON_COMMAND(ID_PEOPLEMOVER_NEW,OnNewParkinglot)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE,OnEditParkinglot)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE,OnDelParkinglot)
	ON_BN_CLICKED(IDPARKINGLOTASSIGNSAVE,OnSave)
	ON_WM_SIZE()
	ON_WM_CREATE()
	/*ON_BN_CLICKED(IDC_BUTTON_CURBOK,OnOk)
	ON_BN_CLICKED(IDC_BUTTON_CURBCANCEL,OnCancel)*/
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_PARKINGLOTASSIGN,OnLvnItemChangedParkinglotList)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_PARKINGLOTASSIGN,OnLvnEndlabeleditListContents)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgParkinglotAssign message handlers
