// DlgPaxBusParking.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgPaxBusParking.h"
#include <InputAirside\PaxBusParkingPlaceList.h>
#include "DlgSelectALTObject.h"
#include <Common/WinMsg.h>
#include "ProcesserDialog.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"
#include "ChildFrm.h"
#include "Airside3D.h"

// CDlgPaxBusParking dialog
static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;

const char* sGateBuffer[] = {"depgate","arrgate"};
const char* sRelateBuffer[] = {"1:1","random"};
IMPLEMENT_DYNAMIC(CDlgPaxBusParking, CXTResizeDialog)
CDlgPaxBusParking::CDlgPaxBusParking(int nProjID, InputTerminal* _pInTerm, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPaxBusParking::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pInTerm(_pInTerm)
	,n_selectedItem(0)
{
	m_pPaxBusParkingList = new PaxBusParkingPlaceList;
	m_nFlag = 0;
}

CDlgPaxBusParking::~CDlgPaxBusParking()
{
	delete m_pPaxBusParkingList;
}

void CDlgPaxBusParking::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PAXBUSPARKING, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgPaxBusParking, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDelItem)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnNoEditDBClick)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnProcpropPickfrommap)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAXBUSPARKING, OnLvnItemchangedListPaxbusparking)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	//ON_NOTIFY(NM_RCLICK, IDC_LIST_PAXBUSPARKING, OnNMRclickListPaxbusparking)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, OnBnClickedButtonPaste)
END_MESSAGE_MAP()


// CDlgPaxBusParking message handlers

BOOL CDlgPaxBusParking::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(_T("Passenger Bus Spot-Gate Linkages"));
	InitToolBar();	
	InitListCtrl();
	m_pPaxBusParkingList->ReadData(-1);

	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	//C3DView* p3DView = pDoc->Get3DView();
	//if(p3DView && p3DView->GetParentFrame())
	//{
	//	p3DView->GetParentFrame()->GetAirside3D()->SetTempPaxBusParkings(m_pPaxBusParkingList);
	//}

	SetListContent();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_PAXBUSPARKING, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_GROUPBOX, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_COPY, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_PASTE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgPaxBusParking::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDEL_PROJECT))
	{
		return -1;
	}

	CToolBarCtrl& toolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	toolbarCtrl.SetCmdID(0, ID_NEW_ITEM);
	toolbarCtrl.SetCmdID(1, ID_DEL_ITEM);

	return 0;
}

void CDlgPaxBusParking::InitToolBar()
{
	CRect rcToolbar;

	m_wndListCtrl.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_wndToolBar.MoveWindow(rcToolbar);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,FALSE);
}

void CDlgPaxBusParking::UpdateToolBar()
{
	int nSel = GetListCtrlSelectedItem();
	if(nSel != -1)
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM);
	else
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,FALSE);
}

void CDlgPaxBusParking::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);


	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCFMT_CENTER;
	CStringList strList;
	lvc.csList = &strList;

	lvc.cx = 166;
	lvc.pszText = _T("Gate");	
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);
	
	//lvc.cx = 80;
	//CStringList strGateList;
	//strGateList.InsertAfter(strGateList.GetTailPosition(), "depgate");
	//strGateList.InsertAfter(strGateList.GetTailPosition(), "arrgate");
	//lvc.csList = &strGateList;
	//lvc.pszText = _T("Type");
	//lvc.fmt = LVCFMT_DROPDOWN;
	//m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.cx = 120;
	lvc.pszText = _T("Terminal Spot");
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1,&lvc);

	lvc.cx = 80;
	CStringList strRelateList;
	strRelateList.InsertAfter(strRelateList.GetTailPosition(),"1:1");
	strRelateList.InsertAfter(strRelateList.GetTailPosition(),"random");
	lvc.csList = &strRelateList;
	lvc.pszText = _T("Relation");
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(2, &lvc);
	//lvc.cx = 230;
	//lvc.csList = &strList;
	//lvc.pszText = _T("Bus park position (RButtonDown to pick from map)");
	//lvc.fmt = LVCFMT_NOEDIT;
	//m_wndListCtrl.InsertColumn(2, &lvc);

}

int CDlgPaxBusParking::GetListCtrlSelectedItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}

void CDlgPaxBusParking::SetListContent()
{
	int nCount = m_pPaxBusParkingList->GetItemCount();

	CString strValue;
	for (int i = 0; i < nCount; i++)
	{
	//	PaxBusParkingPlace* pItem = m_pPaxBusParkingList->GetItem(i);
		PaxBusParkingPlace* pItem = m_pPaxBusParkingList->GetItem(i);
		//m_wndListCtrl.InsertItem(i, pItem->GetGate());

		////Gatetype enumGateType = pItem->GetGatetype();
		////if(enumGateType == depgate)
		////	strValue = _T("depgate");
		////else
		////	strValue = _T("arrgate");
		////m_wndListCtrl.SetItemText((int)i, 1, strValue);

		////if((pItem->GetPath()).getCount() > 2)
		////	strValue = _T("defined");
		////else
		////	strValue = _T("Not defined");
		////m_wndListCtrl.SetItemText((int)i, 2, strValue);
		//m_wndListCtrl.SetItemText(i,1,sGateBuffer[pItem->GetGateType()]);
		//m_wndListCtrl.SetItemText(i,2,pItem->GetTerminalSpot().GetIDString());
		//m_wndListCtrl.SetItemText(i,3,sRelateBuffer[pItem->GetParkingRelate()]);

		//m_wndListCtrl.SetItemData((int)i,(DWORD_PTR)pItem);
		InsertParkingPlaceItem(i,pItem);
	}
}

void CDlgPaxBusParking::InsertParkingPlaceItem(int idx,PaxBusParkingPlace* pItem)
{
	m_wndListCtrl.InsertItem(idx, pItem->GetGate());
//	m_wndListCtrl.SetItemText(idx,1,sGateBuffer[pItem->GetGateType()]);
	CString strSpot;
	if (pItem->GetTerminalSpot().IsBlank())
	{
		strSpot = "All";
	}
	else
	{
		strSpot = pItem->GetTerminalSpot().GetIDString();
	}
	m_wndListCtrl.SetItemText(idx,1,strSpot);
	m_wndListCtrl.SetItemText(idx,2,sRelateBuffer[pItem->GetParkingRelate()]);

	m_wndListCtrl.SetItemData(idx,(DWORD_PTR)pItem);
}

void CDlgPaxBusParking::OnNewItem()
{
	CProcesserDialog dlg(m_pInTerm,this);
	dlg.SetType(GateProc);
	if(IDOK == dlg.DoModal())
	{
		PaxBusParkingPlace* pNewItem = new PaxBusParkingPlace;
		m_pPaxBusParkingList->AddNewItem(pNewItem);
		int nIndex = m_wndListCtrl.GetItemCount();
		CString strValue;

		ProcessorID processorID;
		dlg.GetProcessorID(processorID);
		strValue = processorID.GetIDString("-");
		pNewItem->SetGate(strValue);
		InsertParkingPlaceItem(nIndex,pNewItem);
		//m_wndListCtrl.InsertItem(nIndex, strValue);

		//strValue = _T("depgate");
		//m_wndListCtrl.SetItemText(nIndex, 1, strValue);

		//strValue = _T("Not defined");
		//m_wndListCtrl.SetItemText(nIndex, 2, strValue);

		//m_wndListCtrl.SetItemData(nIndex,(DWORD_PTR)pNewItem);

	}

	UpdateToolBar();
}

void CDlgPaxBusParking::OnDelItem()
{
	int nItem = GetListCtrlSelectedItem();
	m_wndListCtrl.DeleteItemEx(nItem);
	m_pPaxBusParkingList->DeleteItem(nItem);
	UpdateToolBar();
}

LRESULT CDlgPaxBusParking::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nSub = dispinfo->item.iSubItem;
//	int nCurSubSel = dispinfo->item.iSubItem;	
	PaxBusParkingPlace* pItem = (PaxBusParkingPlace*)m_wndListCtrl.GetItemData(nCurSel);
	
	//if (nComboxSel == 0)
	//	pItem->SetGatetype(depgate);
	//else if (nComboxSel == 1)
	//	pItem->SetGatetype(arrgate);

	//if (nSub == 1)
	//{
	//	pItem->SetGateType(PaxBusParkingPlace::PakingPlaceGate(nComboxSel));
	//}
	if(nSub == 2)
	{
		pItem->SetParkingRelate(PaxBusParkingPlace::ParkingRelateType(nComboxSel));
	}
	return 0;
}

LRESULT CDlgPaxBusParking::OnNoEditDBClick(WPARAM wParam, LPARAM lParam)
{
	int nCurSel = (int)wParam;
	int nCurSubSel = (int)lParam;	

	PaxBusParkingPlace* pItem = (PaxBusParkingPlace*)m_wndListCtrl.GetItemData(nCurSel);
	CString strValue;
	if(nCurSubSel == 0)
	{
		CProcesserDialog dlg(m_pInTerm,this);
		dlg.SetType(GateProc);
		if(IDOK == dlg.DoModal())
		{
			ProcessorID processorID;
			dlg.GetProcessorID(processorID);
			strValue = processorID.GetIDString("-");
			pItem->SetGate(strValue);
		}
		m_wndListCtrl.SetItemText(nCurSel, 0, strValue);
	}

	if (nCurSubSel == 1)
	{
		CDlgSelectALTObject dlg(m_nProjID,ALT_APAXBUSSPOT);
		if (IDOK == dlg.DoModal())
		{
			pItem->SetTerminalSpot(dlg.GetALTObject());
			m_wndListCtrl.SetItemText(nCurSel,1,dlg.GetALTObject().GetIDString());
		}
	}
	return 0;
}

void CDlgPaxBusParking::OnProcpropPickfrommap() 
{

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = PICK_MANYPOINTS;


	//	enumReason= PICK_ONEPOINT;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}


void CDlgPaxBusParking::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pPaxBusParkingList->SaveData(-1);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CXTResizeDialog::OnOK();
}

void CDlgPaxBusParking::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pPaxBusParkingList->SaveData(-1);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
//	CXTResizeDialog::OnOK();
}

void CDlgPaxBusParking::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgPaxBusParking::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

LRESULT CDlgPaxBusParking::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	DoTempFallBackFinished(wParam,lParam);
	return true;
}

bool CDlgPaxBusParking::DoTempFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();

	Path path;
	Point *pointList = new Point[nSize];
	for (size_t i =0; i < nSize; ++i)
	{
		ARCVector3 v3 = pData->at(i);
		pointList[i].setPoint(v3[VX] ,v3[VY],v3[VZ]);
	}
	path.init(nSize,pointList);
	SetObjectPath(path);	
	delete []pointList;

	return true;
}

void CDlgPaxBusParking::SetObjectPath(Path& path)
{
	//int nItem = GetListCtrlSelectedItem();
	//PaxBusParkingPlace* pItem = (PaxBusParkingPlace*)m_wndListCtrl.GetItemData(nItem);
	//pItem->SetPath(path);
	//if(path.getCount() > 2 )
	//{
	//	CString strValue = _T("defined");
	//	m_wndListCtrl.SetItemText(nItem, 2, strValue);
	//}
	//else
	//{
	//	AfxMessageBox("Bus park position must have at least 3 points !");
	//	CString strValue = _T("Not defined");
	//	m_wndListCtrl.SetItemText(nItem, 2, strValue);
	//}
}

void CDlgPaxBusParking::OnLvnItemchangedListPaxbusparking(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

// void CDlgPaxBusParking::OnNMRclickListPaxbusparking(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	// TODO: Add your control notification handler code here
// //	CCursor cursor = ::GetCursor();
// 	CPoint point;
// 	GetCursorPos(&point);
// 
// 	//int index;
// 	//int colnum;
// 	//if((index = m_wndListCtrl.HitTestEx(point, &colnum)) != -1)
// 	if(GetListCtrlSelectedItem() != -1)
// 	{
// 		//if(colnum == 2)
// 		//{
// 			CMenu menuPopup;
// 			menuPopup.LoadMenu(IDR_MENU_POPUP);
// 			CMenu* pSubMenu = NULL;
// 			pSubMenu = menuPopup.GetSubMenu(10);
// 			if (pSubMenu != NULL)
// 				pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
// 		//}
// 	}
// 
// 	*pResult = 0;
//}

void CDlgPaxBusParking::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here
	n_selectedItem = GetListCtrlSelectedItem();
}

void CDlgPaxBusParking::OnBnClickedButtonPaste()
{
	// TODO: Add your control notification handler code here
	if(n_selectedItem != -1)
	{
        int index = m_wndListCtrl.GetItemCount();
	/*	m_wndListCtrl.InsertItem(index,m_wndListCtrl.GetItemText(n_selectedItem,0));
		m_wndListCtrl.SetItemText(index,1,m_wndListCtrl.GetItemText(n_selectedItem,1));
		m_wndListCtrl.SetItemText(index,2,m_wndListCtrl.GetItemText(n_selectedItem,2));*/
       PaxBusParkingPlace* copenew = new PaxBusParkingPlace;
       PaxBusParkingPlace* selected = (PaxBusParkingPlace*)m_wndListCtrl.GetItemData(n_selectedItem);
	   *copenew = *selected; 
	   m_pPaxBusParkingList->AddItem(copenew);
	   InsertParkingPlaceItem(index,copenew);
	/*   copenew->SetGate(selected->GetGate());
	   copenew->SetGatetype(selected->GetGatetype());
	   copenew->SetPath(selected->GetPath());
       	m_pPaxBusParkingList->AddNewItem(copenew);
		m_wndListCtrl.SetItemData(index,(DWORD_PTR)(copenew));*/
	}
	//	pItem->SetPath(m_copypath);
	//	if(m_copypath.getCount() > 2 )
	//	{
	//		CString strValue = _T("defined");
	//		m_wndListCtrl.SetItemText(nItem, 2, strValue);
	//	}
	//	else
	//	{
	//		AfxMessageBox("Bus park position must have at least 3 points !");
	//		CString strValue = _T("Not defined");
	//		m_wndListCtrl.SetItemText(nItem, 2, strValue);
	//	}
	//}
	//else
	//	AfxMessageBox("You haven't copy anyone.");

}

