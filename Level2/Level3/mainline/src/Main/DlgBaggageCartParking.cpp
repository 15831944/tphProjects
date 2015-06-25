// DlgBaggageCartParking.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgBaggageCartParking.h"
#include ".\dlgbaggagecartparking.h"
#include ".\ProcesserDialog.h"
#include "TermPlanDoc.h"
#include "DlgSelectALTObject.h"
#include "MainFrm.h"
#include "3DView.h"
#include <CommonData/Fallback.h>
#include "../Common/WinMsg.h"
#include "ChildFrm.h"
#include "Airside3D.h"
const static CString DEFINED = "DEFINED";
const static CString NOTDEFINED = "NOT DEFINED";
// DlgBaggageCartParking dialog
const char* sBagRelateBuffer[] = {"1:1","random"};
IMPLEMENT_DYNAMIC(CDlgBaggageCartParking, CDialogResize)
CDlgBaggageCartParking::CDlgBaggageCartParking(int nPrjID, InputTerminal* pInterm,CWnd* pParent /*=NULL*/)
	: CDialogResize(CDlgBaggageCartParking::IDD, pParent),
	m_nPrjID(nPrjID),
	m_pInterm(pInterm)
{
	m_bDataChanged = false;
	m_iSelectItem = -1;
	m_vTypeList.push_back(ConveyorProc);
	m_vTypeList.push_back(LineProc);
}

CDlgBaggageCartParking::~CDlgBaggageCartParking()
{
}

void CDlgBaggageCartParking::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCTRL_BAGPARKPOS, m_wndListCtrl);

}

BEGIN_MESSAGE_MAP(CDlgBaggageCartParking, CDialogResize)

	ON_WM_CREATE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnNewItem)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDeleteItem)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE, OnEditItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL_BAGPARKPOS, OnNMDblclkListctrlBagparkpos)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_BAGPARKPOS, OnLvnItemchangedListctrlBagparkpos)
	ON_NOTIFY(NM_RCLICK, IDC_LISTCTRL_BAGPARKPOS, OnNMRclickListctrlBagparkpos)
	ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_PASTE, OnBnClickedPaste)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
END_MESSAGE_MAP()

BEGIN_DLGRESIZE_MAP(CDlgBaggageCartParking)
	DLGRESIZE_CONTROL(IDC_STATIC_TOOLBARCONTENTER, DLSZ_SIZE_X)
	DLGRESIZE_CONTROL(IDC_LISTCTRL_BAGPARKPOS, DLSZ_SIZE_X  | DLSZ_SIZE_Y)
	DLGRESIZE_CONTROL(IDC_COPY, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDC_PASTE, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDC_SAVE, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
END_DLGRESIZE_MAP()



BOOL CDlgBaggageCartParking::OnInitDialog()
{
	CDialogResize::OnInitDialog();
	SetWindowText(_T("Baggage Cart Spot-Linkages"));
	InitResizing(FALSE, FALSE, WS_THICKFRAME);
	m_BaggageCartSpec.ReadData(-1);

	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	//C3DView* p3DView = pDoc->Get3DView();
	//if(p3DView && p3DView->GetParentFrame())
	//{
	//	p3DView->GetParentFrame()->GetAirside3D()->SetTempBagCartParkings(&m_BaggageCartSpec);
	//}
	//
	InitToolBar();
	InitListCtrl();
	UpdateClickButton();

	return TRUE;
}

int CDlgBaggageCartParking::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CDialogResize::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !m_wndToolBar.CreateEx(this) || !m_wndToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	return 0;
}

void CDlgBaggageCartParking::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_PEOPLEMOVER_NEW);
	toolbar.SetCmdID(1, ID_PEOPLEMOVER_DELETE);
	toolbar.SetCmdID(2, ID_PEOPLEMOVER_CHANGE);

	UpdateToolBar();

}

void CDlgBaggageCartParking::InitListCtrl()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = "Bagtrain Spot";
	lvc.cx = 120;
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

	LoadListContent();
}

void CDlgBaggageCartParking::UpdateToolBar()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	// if the item count > MAXITEMCOUNT, disable the new item button.
	BOOL bEnable = TRUE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW, bEnable);

	// if there's no item selected, all of other button should be disable.
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE, FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE);
	}
}

void CDlgBaggageCartParking::LoadListContent()
{
	m_wndListCtrl.DeleteAllItems();
	
	int nCount = m_BaggageCartSpec.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		BaggageParkingPlace* pItem = m_BaggageCartSpec.GetItem(i);
		InsertParkingPlaceItem(i,pItem);
	}
}

void CDlgBaggageCartParking::InsertParkingPlaceItem(int idx,BaggageParkingPlace* pItem)
{
	m_wndListCtrl.InsertItem(idx,pItem->GetParkingProcessor());
	CString strParkingSpot;
	if (pItem->GetBagTrainSpot().IsBlank())
	{
		strParkingSpot = _T("All");
	}
	else
	{
		strParkingSpot = pItem->GetBagTrainSpot().GetIDString();
	}
	m_wndListCtrl.SetItemText(idx,1,strParkingSpot);
	m_wndListCtrl.SetItemText(idx,2,sBagRelateBuffer[pItem->GetParkingRelate()]);
	m_wndListCtrl.SetItemData(idx,(DWORD_PTR)pItem);
}

void CDlgBaggageCartParking::OnNewItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));
	
	CProcesserDialog processdlg(m_pInterm,this);

	processdlg.SetOnlyShowPusherConveyor(true);
	processdlg.SetTypeList(m_vTypeList);

	if( processdlg.DoModal() == IDOK )
	{
		int nItemCount = m_wndListCtrl.GetItemCount();
		ProcessorID procID;
		processdlg.GetProcessorID(procID);
		CString strProc = procID.GetIDString();
		BaggageParkingPlace * newItem  = new BaggageParkingPlace();
		newItem->SetParkingProcessor(strProc);
		m_BaggageCartSpec.AddNewItem(newItem);
		
// 		m_wndListCtrl.InsertItem(nItemCount,strPusher);
// 		m_wndListCtrl.SetItemText(nItemCount,1,NOTDEFINED);
// 	
// 		m_wndListCtrl.SetItemData(nItemCount,(DWORD_PTR)newItem);
		InsertParkingPlaceItem(nItemCount,newItem);
		m_bDataChanged = true;
	}

	UpdateClickButton();
}


void CDlgBaggageCartParking::OnDeleteItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));
	
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			BaggageParkingPlace* pNewItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(nItem));
			m_BaggageCartSpec.DeleteItem(pNewItem);
			m_wndListCtrl.DeleteItemEx(nItem);
			m_bDataChanged = true;
		}
	}

	UpdateToolBar();
	UpdateClickButton();

}

void CDlgBaggageCartParking::OnEditItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);

			CProcesserDialog processdlg(m_pInterm,this);
			processdlg.SetOnlyShowPusherConveyor(true);
			processdlg.SetTypeList(m_vTypeList);
	

			if( processdlg.DoModal() == IDOK )
			{				
				ProcessorID procID;
				processdlg.GetProcessorID(procID);
				CString strProc = procID.GetIDString();
				BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(nItem));
				ptheItem->SetParkingProcessor(strProc);						
				m_wndListCtrl.SetItemText(nItem,0,strProc);	
				m_bDataChanged = true;
			}
		}
	}
	UpdateClickButton();
}

// DlgBaggageCartParking message handlers
void CDlgBaggageCartParking::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_BaggageCartSpec.SaveData(m_nPrjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	OnOK();
}

void CDlgBaggageCartParking::OnBnClickedCancel()
{
	m_BaggageCartSpec.ReadData(m_nPrjID);
	OnCancel();
}
void CDlgBaggageCartParking::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_BaggageCartSpec.SaveData(m_nPrjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	m_bDataChanged = false;
	UpdateClickButton();
}



void CDlgBaggageCartParking::OnNMDblclkListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	int iItem = pnmv->iItem;
	int iColumn = pnmv->iSubItem;

	if(iItem < m_wndListCtrl.GetItemCount() && iItem >=0 )
	{
		BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(iItem));
		if (iColumn ==0)
		{
			CProcesserDialog processdlg(m_pInterm,this);
			processdlg.SetOnlyShowPusherConveyor(true);
			processdlg.SetTypeList(m_vTypeList);
			if( processdlg.DoModal() == IDOK )
			{				
				ProcessorID procID;
				processdlg.GetProcessorID(procID);
				CString strProc = procID.GetIDString();
			
				ptheItem->SetParkingProcessor(strProc);						
				m_wndListCtrl.SetItemText(iItem,0,strProc);			
				m_bDataChanged = true;		
			}
		}
		
		if (iColumn == 1)
		{
			CDlgSelectALTObject dlg(m_nPrjID,ALT_ABAGCARTSPOT);
			if (IDOK == dlg.DoModal())
			{
				ptheItem->SetBagTrainSpot(dlg.GetALTObject());
				m_wndListCtrl.SetItemText(iItem,1,dlg.GetALTObject().GetIDString());
				m_bDataChanged = true;
			}
		}
	}


	*pResult = 0;
	UpdateToolBar();
	UpdateClickButton();
}


void CDlgBaggageCartParking::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgBaggageCartParking::ShowDialog(CWnd* parentWnd)
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

void CDlgBaggageCartParking::OnProcpropPickfrommap() 
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

	enum FallbackReason enumReason= PICK_MANYPOINTS;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}	

}

LRESULT CDlgBaggageCartParking::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	//std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	//size_t nSize = pData->size();

	//Path path;
	//Point *pointList = new Point[nSize];
	//for (size_t i =0; i < nSize; ++i)
	//{
	//	ARCVector3 v3 = pData->at(i);
	//	pointList[i].setPoint(v3[VX] ,v3[VY],v3[VZ]);
	//}
	//path.init(nSize,pointList); 
	//int iSelectItem = m_wndListCtrl.GetCurSel();

	//if(path.getCount() > 2 )
	//{
	//	BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(iSelectItem));
	//	ptheItem->SetParkArea(path);
	//	m_wndListCtrl.SetItemText(iSelectItem,1,DEFINED);
	//}
	//else
	//{
	//	AfxMessageBox("Baggage Cart position must have at least 3 points !");
	//	BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(iSelectItem));
	//	ptheItem->SetParkArea(path);
	//	m_wndListCtrl.SetItemText(iSelectItem,1,NOTDEFINED);
	//}


	//ShowWindow(SW_SHOW);	
	//EnableWindow();
	//m_bDataChanged = true;
	//UpdateClickButton();
	return true;
}


void CDlgBaggageCartParking::OnLvnItemchangedListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	//m_iSelectItem = pnmv->iItem;

	*pResult = 0;
	UpdateToolBar();
	UpdateClickButton();
}

void CDlgBaggageCartParking::OnNMRclickListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	//NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	//int iItem = pnmv->iItem;
	//int iColumn = pnmv->iSubItem;
	//
	//if( iColumn == 1 && iItem < m_wndListCtrl.GetItemCount() &&  iItem >=0 )
	//{		

	//	CPoint pt;//(pnmv->ptAction);
	//	GetCursorPos(&pt);
	//	CNewMenu menu, *pCtxMenu = NULL;
	//	menu.LoadMenu(IDR_MENU_POPUP);
	//	pCtxMenu =DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(10));
	//	pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, this );
	//	
	//}	
	
	*pResult = 0;
}
void CDlgBaggageCartParking::UpdateClickButton()
{
	int iSelect = m_wndListCtrl.GetCurSel();
	//Paste button
	//if(m_copyPath.getCount() > 2 && iSelect >=0 && iSelect < m_wndListCtrl.GetItemCount() )
	//{
	//	GetDlgItem(IDC_PASTE)->EnableWindow(TRUE);
	//}
	//else
	//{
	//	GetDlgItem(IDC_PASTE)->EnableWindow(FALSE);
	//}
	//copy button
	if(iSelect >=0 && iSelect < m_wndListCtrl.GetItemCount())
	{
		GetDlgItem(IDC_COPY)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COPY)->EnableWindow(FALSE);
	}
	//
	if(m_bDataChanged)
	{
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	}
}

void CDlgBaggageCartParking::OnBnClickedCopy()
{
	// TODO: Add your control notification handler code here
//	int iSelectItem = -1;
	if( m_wndListCtrl.GetSelectedCount() > 0)
	{
		m_iSelectItem = m_wndListCtrl.GetCurSel();
		/*BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(iSelectItem));
		m_copyPath = ptheItem->GetParkArea();*/
	}
	UpdateClickButton();
}

void CDlgBaggageCartParking::OnBnClickedPaste()
{
	// TODO: Add your control notification handler code here
//	ASSERT(m_copyPath.getCount() > 2 );
	
	if(m_iSelectItem != -1)
	{
		int index = m_wndListCtrl.GetItemCount();
		BaggageParkingPlace* pNewItem = new BaggageParkingPlace();
		BaggageParkingPlace* ptheItem = (BaggageParkingPlace*)(m_wndListCtrl.GetItemData(m_iSelectItem));
		*pNewItem = *ptheItem;
		m_BaggageCartSpec.AddNewItem(pNewItem);
		InsertParkingPlaceItem(index,pNewItem);
	/*	ptheItem->SetParkArea(m_copyPath);
		m_wndListCtrl.SetItemText(iSelectItem,1, DEFINED);*/
		m_bDataChanged = true;
	}	
	UpdateClickButton();
}

LRESULT CDlgBaggageCartParking::OnMsgComboChange( WPARAM wParam, LPARAM lParam )
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nSub = dispinfo->item.iSubItem;
	//	int nCurSubSel = dispinfo->item.iSubItem;	
	BaggageParkingPlace* pItem = (BaggageParkingPlace*)m_wndListCtrl.GetItemData(nCurSel);
	if(nSub == 2)
	{
		pItem->SetParkingRelate(BaggageParkingPlace::ParkingRelateType(nComboxSel));
	}
	m_bDataChanged = true;
	UpdateClickButton();
	return 0;
}

