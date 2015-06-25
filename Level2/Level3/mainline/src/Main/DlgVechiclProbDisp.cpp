// DlgVechiclProbDisp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVechiclProbDisp.h"
#include "../Landside/LandSideVehicleProbDispDetail.h"
#include "../Landside/LandSideVehicleProbDispList.h"
#include "../landside/LandSideVehicleProbDisp.h"
#include "../Common/ViewMsg.h"
#include "DlgSelectLandsideVehicleType.h"
// CDlgVechiclProbDisp dialog

IMPLEMENT_DYNAMIC(CDlgVechiclProbDisp, CXTResizeDialog)
CDlgVechiclProbDisp::CDlgVechiclProbDisp(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVechiclProbDisp::IDD, pParent)
{
	 m_landsideVehicleDispProps.ReadData();
}

CDlgVechiclProbDisp::~CDlgVechiclProbDisp()
{
}

void CDlgVechiclProbDisp::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVechiclProbDisp)
	DDX_Control(pDX, IDC_LIST_VEHICLEPROBS, m_listNamedSets);
	DDX_Control(pDX, IDC_LIST_VEHICLEPROBDETAIL, m_listctrlProp);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_BUTTON_VEHICLEPROBDISPSAVE, m_btnSave);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_VEHICLEPROBDETAIL, m_btnBarFrame);
	DDX_Control(pDX, IDC_STATIC_VEHICLEPROBS, m_btnBarFrame2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgVechiclProbDisp, CXTResizeDialog)
	//{{AFX_MSG_MAP(CDlgVechiclProbDisp)
	ON_COMMAND(IDC_BUTTON_VEHICLEPROBDISPSAVE,OnSave)

	ON_COMMAND(ID_TOOLBARBUTTONADD,OnProbDispsAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnProbDispsDel)


	ON_COMMAND(ID_PEOPLEMOVER_DELETE,OnProbDispDetailDel)
	ON_COMMAND(ID_PEOPLEMOVER_NEW,onProbDispDetailAdd)

	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_VEHICLEPROBDETAIL, OnLvnEndlabeleditListVehicleprobdetail)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VEHICLEPROBDETAIL, OnLvnItemchangedListVehicleprobdetail)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_VEHICLEPROBS, OnNMCustomdrawRecQueryList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_VEHICLEPROBS, OnLvnEndlabeleditListVehicleprobs)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VEHICLEPROBS, OnLvnItemchangedListVehicleprobs)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VEHICLEPROBS, OnNMClickListVehicleprobs)
END_MESSAGE_MAP()

BOOL CDlgVechiclProbDisp::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	if (!m_DispProbsBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP) ||
		!m_DispProbsBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_DispProbDetailBar.CreateEx(this, TBSTYLE_FLAT, CBRS_ALIGN_TOP) ||
		!m_DispProbDetailBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	DWORD dwStyle = m_listctrlProp.GetExtendedStyle();
	
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlProp.SetExtendedStyle( dwStyle );
	// Create column
	m_listctrlProp.InsertColumn( 0, "Vehicle Type", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 1, "Visible", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 2, "Shape", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 3, "Line Type", LVCFMT_CENTER, 150 );
	m_listctrlProp.InsertColumn( 4, "Color", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 5, "Leave Trail", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 6, "Tracer Density", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 7, "Scale Size", LVCFMT_CENTER, 100 );
	dwStyle = m_listNamedSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listNamedSets.SetExtendedStyle( dwStyle );
	CStringList strList;
	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Name");
	lvc.cx = 200;
	lvc.fmt = LVCFMT_EDIT;
	lvc.csList = &strList;
	m_listNamedSets.InsertColumn( 0, &lvc );
		
	UpdateVehicleprob();
	InitToolbar();
   
	SetResize(IDC_STATIC_VEHICLEPROBS,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT);
	SetResize(m_DispProbsBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_VEHICLEPROBS,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT);
	
	SetResize(IDC_STATIC_VEHICLEPROBDETAIL,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_DispProbDetailBar.GetDlgCtrlID(),SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT);
	SetResize(IDC_LIST_VEHICLEPROBDETAIL,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT);
	
	

	SetResize(IDC_BUTTON_VEHICLEPROBDISPSAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return true;
}

void CDlgVechiclProbDisp::InitToolbar()
{
	CRect rc;
	m_listNamedSets.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.left=rc.left+2;
	rc.top=rc.top-26;
	rc.bottom=rc.top+22;
	m_DispProbsBar.MoveWindow(&rc);
	m_DispProbsBar.ShowWindow(SW_SHOW);
	m_DispProbsBar.BringWindowToTop();
	m_DispProbsBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_DispProbsBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	m_DispProbsBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );

	m_listctrlProp.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.left=rc.left+2;
	rc.top=rc.top-26;
	rc.bottom=rc.top+22;
	m_DispProbDetailBar.MoveWindow(&rc);
	m_DispProbDetailBar.ShowWindow(SW_SHOW);
    m_DispProbDetailBar.BringWindowToTop();
	m_DispProbDetailBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_DispProbDetailBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
	m_DispProbDetailBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}
void CDlgVechiclProbDisp::OnSave()
{
	m_landsideVehicleDispProps.SaveData();
}
void CDlgVechiclProbDisp::OnOK()
{
	OnSave();
	CDocument* pDoc	= ((CView*)m_pParentWnd)->GetDocument();
	if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
	{
		((CTermPlanDoc*)pDoc)->UpdateAllViews(NULL, VM_RELOAD_LANDSIDE_VEHICLE_DISPLAY_PROPERTY);
	}
	CXTResizeDialog::OnOK();
}
void CDlgVechiclProbDisp::OnCancel()
{
	CXTResizeDialog::OnCancel();		
}
void CDlgVechiclProbDisp::onProbDispDetailAdd()
{
	int nCurSel=m_listNamedSets.GetCurSel();
	if (nCurSel<0)
	{
		return;
	}
	CLandSideVehicleProbDisp *tmpDisp=(CLandSideVehicleProbDisp*)m_landsideVehicleDispProps.GetVehicleProbDispList().at(nCurSel);
	CDlgSelectLandsideVehicleType dlg;
	if(dlg.DoModal() == IDOK)
	{	
		CString strName;
		strName = dlg.GetName();
		if (strName.IsEmpty())
		{
			MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
		}
		else if (tmpDisp->ExistSameName(strName))
		{
			MessageBox(_T("Exist the same group name!."),_T("Warning"),MB_OK);
			return;
		}
	
	tmpDisp->AddItem(strName);
	CLandSideVehicleProbDispDetail *tmpDetail=tmpDisp->getItem(tmpDisp->getItemCount()-1);
	int allCount=m_listctrlProp.GetItemCount();
	m_listctrlProp.InsertPaxDispItem(allCount,tmpDetail);
	}
}

void CDlgVechiclProbDisp::OnProbDispDetailDel()
{
	POSITION pos = m_listctrlProp.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	else
	{
		int nItem = m_listctrlProp.GetNextSelectedItem(pos);
		if( nItem == 0 )
		{
			MessageBox( "Can't delete Default item!" );
			return;
		}
		
		if( nItem == 1 )
		{
			MessageBox( "Can't delete Overlap item!" );
			return;
		}

		// delete from db
		int nCursel=m_listNamedSets.GetCurSel();
		if (nCursel == LB_ERR)
		{
			MessageBox("Please select one name");
			return;
		}
		
		CLandSideVehicleProbDisp *tmpDisp=(CLandSideVehicleProbDisp*)m_landsideVehicleDispProps.GetVehicleProbDispList().at(nCursel);
		tmpDisp->deleteItem(nItem-2);
		m_listctrlProp.DeleteItem( nItem );
	}
	if (m_landsideVehicleDispProps.getItemCount()<=0)
	{
		m_landsideVehicleDispProps.ReadData();
	}
	UpdateDispProbDetailBar();
}
void CDlgVechiclProbDisp::OnProbDispsAdd()
{

	CLandSideVehicleProbDisp* pProDisp = new CLandSideVehicleProbDisp();
	CLandSideVehicleProbDispDetail DefaultDetail;
	CLandSideVehicleProbDispDetail OverlapDetail;
	int nCount = m_landsideVehicleDispProps.getItemCount();
	m_landsideVehicleDispProps.AddItem(pProDisp, DefaultDetail, OverlapDetail);
	m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nCount, "         ", 0, 0, 0, nCount);
	m_listNamedSets.SetItemState(nCount, LVIS_SELECTED, LVIS_SELECTED);
	m_listNamedSets.SetFocus();
	m_listNamedSets.EditLabel(nCount);
}
void CDlgVechiclProbDisp::OnProbDispsDel()
{
    int nCount = m_listNamedSets.GetItemCount();
	if( nCount < 2 )
	{
		MessageBox( "At least one name exit!" );
		return;
	}
	int nCursel = m_landsideVehicleDispProps.GetCurrentPROSetIdx();
	m_landsideVehicleDispProps.deleteItem(nCursel);
	int CurrentSel = m_landsideVehicleDispProps.getItemCount();
	m_landsideVehicleDispProps.SetCurrentPROSet(CurrentSel-1);
	UpdateVehicleprob();
	m_listNamedSets.SetCurSel(CurrentSel);
	UpdateDispProbBar();

}
// CDlgVechiclProbDisp message handlers

void CDlgVechiclProbDisp::OnLvnEndlabeleditListVehicleprobdetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;
}
void CDlgVechiclProbDisp::UpdateDispProbBar()
{
	POSITION pos = m_listNamedSets.GetFirstSelectedItemPosition();
	BOOL bEnable = TRUE;
	if(pos == NULL)
		bEnable = FALSE;
	m_DispProbsBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL ,bEnable);
}
void CDlgVechiclProbDisp::UpdateDispProbDetailBar()
{
	POSITION pos = m_listctrlProp.GetFirstSelectedItemPosition();
	BOOL bEnable = TRUE;
	if(pos == NULL)
		bEnable = FALSE;

	m_DispProbDetailBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE, bEnable);
	m_DispProbDetailBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, bEnable);
}
void CDlgVechiclProbDisp::OnLvnItemchangedListVehicleprobdetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LV_DISPINFO *lvipInfo=(LV_DISPINFO*)(pNMHDR);
	LV_ITEM* plvItem = &lvipInfo->item;

     UpdateDispProbDetailBar();
	*pResult = 0;
}

void CDlgVechiclProbDisp::OnLvnEndlabeleditListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	LV_DISPINFO *lvipInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&lvipInfo->item;
	int nSubItem=plvItem->iSubItem;
	int nItem=plvItem->iItem;
	if (nItem<1)
	{
		return;
	}
	nItem=m_listNamedSets.GetCurSel();
	if (nItem<1)
	{
		return;
	}
	m_listNamedSets.OnEndlabeledit(pNMHDR,pResult);
	CLandSideVehicleProbDisp *tmpDisp=(CLandSideVehicleProbDisp*)m_landsideVehicleDispProps.GetVehicleProbDispList().at(nItem);
     tmpDisp->setName(m_listNamedSets.GetItemText(nItem,nSubItem));
	*pResult = 0;
}

void CDlgVechiclProbDisp::OnLvnItemchangedListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		int CurrentSel = (int) m_listNamedSets.GetItemData(pnmv->iItem);
		m_landsideVehicleDispProps.SetCurrentPROSet(CurrentSel);			
	}
	else
		ASSERT(0);

	UpdateVehicleprobDetail();
	
}

void CDlgVechiclProbDisp::OnNMClickListVehicleprobs(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdateDispProbBar();
	*pResult = 0;
}

LRESULT CDlgVechiclProbDisp::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	/*if (message == VEHICLE_DISP_PROP_CHANGE1)
	{
		if (m_pParentWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			CDocument* pDoc	= ((CView*)m_pParentWnd)->GetDocument();
			if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
			{
				((CTermPlanDoc*)pDoc)->UpdateAllViews(NULL, VM_RELOAD_VEHICLE_DISPLAY_PROPERTY);
			}
		}
	}*/

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgVechiclProbDisp::UpdateVehicleprobDetail()
{
	m_listctrlProp.DeleteAllItems();
	int CurrentSel = m_landsideVehicleDispProps.GetCurrentPROSetIdx();
	CLandSideVehicleProbDisp* tmpDisp=(CLandSideVehicleProbDisp*)m_landsideVehicleDispProps.GetVehicleProbDispList().at(CurrentSel);

	m_listctrlProp.InsertPaxDispItem(0,tmpDisp->GetDefaultDetail());
	m_listctrlProp.InsertPaxDispItem(1,tmpDisp->GetOverlapDetail());
	
	for (int i=0;i<tmpDisp->getItemCount();i++)
	{
		CLandSideVehicleProbDispDetail *tmpDetail=tmpDisp->getItem(i);
		m_listctrlProp.InsertPaxDispItem(i+2,tmpDetail);
		m_listctrlProp.SetItemData(i+2,(DWORD_PTR)tmpDetail);

	}
	UpdateDispProbBar();
}

void CDlgVechiclProbDisp::UpdateVehicleprob()
{
	m_listNamedSets.DeleteAllItems();
	for (int i=0;i<m_landsideVehicleDispProps.getItemCount();i++)
	{
		CLandSideVehicleProbDisp *tmpDisp = m_landsideVehicleDispProps.getItem(i);
		m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, tmpDisp->getName(), 0, 0, 0, (LPARAM) i);
	}
	int CurrentSel = m_landsideVehicleDispProps.GetCurrentPROSetIdx();
	m_listNamedSets.SetCurSel(CurrentSel);
/*	m_listNamedSets.SetItemState(CurrentSel, LVIS_SELECTED, LVIS_SELECTED );	*/
}

void CDlgVechiclProbDisp::OnNMCustomdrawRecQueryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{

		COLORREF clrNewTextColor, clrNewBkColor;

		int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

		POSITION pos = m_listNamedSets.GetFirstSelectedItemPosition();
		int index = m_listNamedSets.GetNextSelectedItem(pos);

		if (index == nItem)//refresh the current item
		{
			clrNewTextColor = RGB(255,255,255);        //Set the text to white
			clrNewBkColor = RGB(49,106,197);        //Set the background color to blue
		}
		else
		{
			clrNewTextColor = RGB(0,0,0);        //set the text black
			clrNewBkColor = RGB(255,255,255);    //leave the background color white
		}

		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;

		*pResult = CDRF_DODEFAULT;
	}
}