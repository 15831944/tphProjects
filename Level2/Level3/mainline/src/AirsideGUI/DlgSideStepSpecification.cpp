// DlgSideStepSpecification.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgSideStepSpecification.h"
#include ".\dlgsidestepspecification.h"
#include "DlgSelectRunway.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "DlgSelectSideStepToRunway.h"
#include "..\common\AirportDatabase.h"

static const UINT ID_NEW_FLIGHTTYPE = 10;
static const UINT ID_DEL_FLIGHTTYPE = 11;
static const UINT ID_EDIT_FLIGHTTYPE = 12;
static const UINT ID_NEW_RUNWAYDATA = 20;
static const UINT ID_DEL_RUNWAYDATA = 21;
static const UINT ID_EDIT_RUNWAYDATA = 22;

// CDlgSideStepSpecification dialog

IMPLEMENT_DYNAMIC(CDlgSideStepSpecification, CXTResizeDialog)
CDlgSideStepSpecification::CDlgSideStepSpecification(int nProjID,PSelectFlightType pSelectFlightType,CAirportDatabase* pAirportDB,CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgSideStepSpecification::IDD, pParent)
,m_nProjID(nProjID)
,m_pSelectFlightType(pSelectFlightType)
,m_SideStepSpecification(nProjID)
,m_pAirportDB(pAirportDB)
{
}

CDlgSideStepSpecification::~CDlgSideStepSpecification()
{
}

void CDlgSideStepSpecification::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FLTRUNWAY, m_wndTreeFltRunway);
	DDX_Control(pDX, IDC_TREE_RUNWAYDATA, m_wndTreeRunwayData);

}


BEGIN_MESSAGE_MAP(CDlgSideStepSpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_FLIGHTTYPE, OnNewFlightType)
	ON_COMMAND(ID_DEL_FLIGHTTYPE, OnDelFlightType)
	ON_COMMAND(ID_EDIT_FLIGHTTYPE, OnEditFlightType)
	ON_COMMAND(ID_NEW_APPROCHRUNWAY, OnNewApprochRunway)
	ON_COMMAND(ID_DEL_APPROCHRUNWAY, OnDelApprochRunway)
	ON_COMMAND(ID_EDIT_APPROCHRUNWAY, OnEditApprochRunway)
	ON_COMMAND(ID_NEW_RUNWAYDATA, OnNewToRunway)
	ON_COMMAND(ID_DEL_RUNWAYDATA, OnDelToRunway)
	ON_COMMAND(ID_EDIT_RUNWAYDATA, OnEditToRunway)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLTRUNWAY, OnTvnSelchangedTreeFltrunway)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RUNWAYDATA, OnTvnSelchangedTreeRunwaydata) 
END_MESSAGE_MAP()

int CDlgSideStepSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	if (!m_wndFltRunwayToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltRunwayToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	if (!m_wndRunwayDataToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_wndFltRunwayToolbar.GetDlgCtrlID()+1)
		|| !m_wndRunwayDataToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& fltToolbar = m_wndFltRunwayToolbar.GetToolBarCtrl();
	fltToolbar.SetCmdID(0, ID_NEW_FLIGHTTYPE);
	fltToolbar.SetCmdID(1, ID_DEL_FLIGHTTYPE);
	fltToolbar.SetCmdID(2, ID_EDIT_FLIGHTTYPE);

	CToolBarCtrl& runwayToolbar = m_wndRunwayDataToolbar.GetToolBarCtrl();
	runwayToolbar.SetCmdID(0, ID_NEW_RUNWAYDATA);
	runwayToolbar.SetCmdID(1, ID_DEL_RUNWAYDATA);
	runwayToolbar.SetCmdID(2, ID_EDIT_RUNWAYDATA);

	return 0;
}
void CDlgSideStepSpecification::InitSideStepToolBar()
{
	CRect rcToolbar;

	// set the position of the SideStep FltRunway toolbar	
	m_wndTreeFltRunway.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 4;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_wndFltRunwayToolbar.MoveWindow(rcToolbar);

	m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLIGHTTYPE);
	m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLIGHTTYPE, FALSE);
	m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLIGHTTYPE, FALSE);

	// set the position of the SideStep RunwayData toolbar	
	m_wndTreeRunwayData.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 4;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22; 
	m_wndRunwayDataToolbar.MoveWindow(rcToolbar);

	m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, FALSE);
	m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
	m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);

}

BOOL CDlgSideStepSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitSideStepToolBar();
	m_SideStepSpecification.SetAirportDB(m_pAirportDB);
	m_SideStepSpecification.ReadData();
	InitFltRunwayTree();

	SetResize(m_wndFltRunwayToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndRunwayDataToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_STATIC_SIDESTEPTORUNWAY, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_FLTRUNWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_RUNWAYDATA, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_LANDINGRUNWAY,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSideStepSpecification::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction() ;
		m_SideStepSpecification.SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CXTResizeDialog::OnOK();
}

void CDlgSideStepSpecification::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void CDlgSideStepSpecification::OnContextMenu(CWnd* pWnd, CPoint point)
{
	OnContextMenuForRunwayDate(pWnd,point);
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeFltRunway.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeFltRunway.SetFocus();
	CPoint pt = point;
	m_wndTreeFltRunway.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeFltRunway.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeFltRunway.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu(IDR_MENU_SIDESTEPSPECIFICATION);
	CMenu* pSubMenu = NULL;

	if (m_wndTreeFltRunway.GetParentItem(hRClickItem) == NULL)
		pSubMenu = menuPopup.GetSubMenu(0);
	else
		pSubMenu = menuPopup.GetSubMenu(1);

	if (pSubMenu != NULL)
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}
void CDlgSideStepSpecification::OnNewFlightType()
{
	if (m_pSelectFlightType == NULL)
		return;
	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
	CString strFlightType;
	fltType.screenPrint(strFlightType);

	HTREEITEM hItem = m_wndTreeFltRunway.GetRootItem();
	while (hItem)
	{
		if (m_wndTreeFltRunway.GetItemText(hItem) == strFlightType)
		{
			MessageBox(_T("Flight Type: \" ") +strFlightType+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeFltRunway.GetNextItem(hItem , TVGN_NEXT);
	}

	CSideStepFlightType* pNewItem = new CSideStepFlightType;
	pNewItem->SetFltType(fltType);

	m_SideStepSpecification.AddFlightTypeItem(pNewItem);
	HTREEITEM hFltTypeItem = m_wndTreeFltRunway.InsertItem(strFlightType);
	m_wndTreeFltRunway.SetItemData(hFltTypeItem,(DWORD_PTR)pNewItem);
}
void CDlgSideStepSpecification::OnDelFlightType()
{
	HTREEITEM hTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepFlightType* pItem = (CSideStepFlightType*)m_wndTreeFltRunway.GetItemData(hTreeItem);
	m_wndTreeFltRunway.DeleteItem(hTreeItem);
	m_SideStepSpecification.DeleteFlightTypeItem(pItem);
}

void CDlgSideStepSpecification::OnEditFlightType()
{
	HTREEITEM hTreeItem = m_wndTreeFltRunway.GetSelectedItem();

	CSideStepFlightType* pItem =
		(CSideStepFlightType*)m_wndTreeFltRunway.GetItemData(hTreeItem);

	if (m_pSelectFlightType == NULL)
		return;
	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	CString strFlightType;
	fltType.screenPrint(strFlightType);
	HTREEITEM hItem = m_wndTreeFltRunway.GetRootItem();
	while (hItem)
	{
		if (m_wndTreeFltRunway.GetItemText(hItem) == strFlightType)
		{
			MessageBox(_T("Flight Type: \" ") +strFlightType+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeFltRunway.GetNextItem(hItem , TVGN_NEXT);
	}
	m_wndTreeFltRunway.SetItemText(hTreeItem, strFlightType);
	pItem->SetFltType(fltType);

}

void CDlgSideStepSpecification::OnNewApprochRunway()
{
	HTREEITEM hFltTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepFlightType* pFltItem = (CSideStepFlightType*)m_wndTreeFltRunway.GetItemData(hFltTreeItem);
	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway runway;
	runway.ReadObject(nRunwayID);

	CString strRunwayMark;
	if(nRunwayMarkIndex == 0)
		strRunwayMark = runway.GetMarking1().c_str();
	else
		strRunwayMark = runway.GetMarking2().c_str();
	CString strApproachRunway;
	strApproachRunway.Format("%s:%s","ApproachRunway",strRunwayMark);

	if(m_wndTreeFltRunway.ItemHasChildren(hFltTreeItem))
	{
		HTREEITEM hChildItem = m_wndTreeFltRunway.GetChildItem(hFltTreeItem);
		while (hChildItem)
		{
			if (m_wndTreeFltRunway.GetItemText(hChildItem) == strApproachRunway)
			{
				MessageBox(_T("\" ") +strApproachRunway+ _T(" \" already exists."));
				return;
			}
			hChildItem = m_wndTreeFltRunway.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}

	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.InsertItem(strApproachRunway, hFltTreeItem, TVI_LAST);
	m_wndTreeFltRunway.Expand(hFltTreeItem, TVE_EXPAND);	
	CSideStepApproachRunway* pNewItem = new CSideStepApproachRunway;
	pNewItem->SetRunwayID(nRunwayID);
	pNewItem->SetMarkIndex(nRunwayMarkIndex);
	m_wndTreeFltRunway.SetItemData(hApproachRunwayTreeItem, (DWORD_PTR)pNewItem);
	pFltItem->AddApproachRunwayItem(pNewItem);
}

void CDlgSideStepSpecification::OnDelApprochRunway()
{
	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepApproachRunway* pApproachRunwayItem = (CSideStepApproachRunway*)m_wndTreeFltRunway.GetItemData(hApproachRunwayTreeItem);
	HTREEITEM hFltTreeItem = m_wndTreeFltRunway.GetParentItem(hApproachRunwayTreeItem); 
	CSideStepFlightType* pFltItem = (CSideStepFlightType*)m_wndTreeFltRunway.GetItemData(hFltTreeItem);
	m_wndTreeFltRunway.DeleteItem(hApproachRunwayTreeItem);
	pFltItem->DeleteApproachRunwayItem(pApproachRunwayItem);
}

void CDlgSideStepSpecification::OnEditApprochRunway()
{
	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepApproachRunway* pApproachRunwayItem = (CSideStepApproachRunway*)m_wndTreeFltRunway.GetItemData(hApproachRunwayTreeItem);
	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway runway;
	runway.ReadObject(nRunwayID);

	CString strRunwayMark;
	if(nRunwayMarkIndex == 0)
		strRunwayMark = runway.GetMarking1().c_str();
	else
		strRunwayMark = runway.GetMarking2().c_str();
	CString strApproachRunway;
	strApproachRunway.Format("%s:%s","ApproachRunway",strRunwayMark);

	HTREEITEM hFltTreeItem = m_wndTreeFltRunway.GetParentItem(hApproachRunwayTreeItem);
	if (m_wndTreeFltRunway.ItemHasChildren(hFltTreeItem))
	{
		HTREEITEM hChildItem = m_wndTreeFltRunway.GetChildItem(hFltTreeItem);
		while (hChildItem)
		{
			if (m_wndTreeFltRunway.GetItemText(hChildItem) == strApproachRunway)
			{
				MessageBox(_T("\" ") +strApproachRunway+ _T(" \" already exists."));
				return;
			}
			hChildItem = m_wndTreeFltRunway.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}

	m_wndTreeFltRunway.SetItemText(hApproachRunwayTreeItem, strApproachRunway);
	pApproachRunwayItem->SetRunwayID(nRunwayID);
	pApproachRunwayItem->SetMarkIndex(nRunwayMarkIndex);
}

void CDlgSideStepSpecification::OnNewToRunway()
{
	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepApproachRunway* pApproachRunwayItem = (CSideStepApproachRunway*)m_wndTreeFltRunway.GetItemData(hApproachRunwayTreeItem);
	CString strApproachRunway = m_wndTreeFltRunway.GetItemText(hApproachRunwayTreeItem);
	strApproachRunway = strApproachRunway.Mid(15,3);
	CDlgSelectSideStepToRunway dlg(m_nProjID,strApproachRunway);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway runway;
	runway.ReadObject(nRunwayID);

	CString strRunwayMark;
	if(nRunwayMarkIndex == 0)
		strRunwayMark = runway.GetMarking1().c_str();
	else
		strRunwayMark = runway.GetMarking2().c_str();
	CString strToRunway;
	strToRunway.Format("%s:%s","Side Step To Runway",strRunwayMark);

	HTREEITEM hItem = m_wndTreeRunwayData.GetRootItem();
	while (hItem)
	{
		if (m_wndTreeRunwayData.GetItemText(hItem) == strToRunway)
		{
			MessageBox(_T("\" ") +strToRunway+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeRunwayData.GetNextItem(hItem , TVGN_NEXT);
	}
	
	COOLTREE_NODE_INFO cni;	
	CCoolTree::InitNodeInfo(this,cni);
	m_wndTreeRunwayData.SetRedraw(FALSE);
	HTREEITEM hToRunwayTreeItem = m_wndTreeRunwayData.InsertItem(strToRunway,cni,FALSE);
	CSideStepToRunway* pNewItem = new CSideStepToRunway;
	pNewItem->SetRunwayID(nRunwayID);
	pNewItem->SetMarkIndex(nRunwayMarkIndex);
	pNewItem->SetMinFinalLeg(300);
	pNewItem->SetMaxTurnAngle(30);
	m_wndTreeRunwayData.SetItemData(hToRunwayTreeItem, (DWORD_PTR)pNewItem);
	CString strMinFinalLeg,strMaxTurnAngle;
	strMinFinalLeg.Format("Min Final Leg (m) : %d",300);
	strMaxTurnAngle.Format("Max Turn Degree : %d",30);
	cni.nt=NT_NORMAL; 
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.fMaxValue = 1000;
	HTREEITEM hMinFinalLegTreeItem = m_wndTreeRunwayData.InsertItem(strMinFinalLeg,cni,FALSE,FALSE, hToRunwayTreeItem, TVI_LAST);
	m_wndTreeRunwayData.SetItemData(hMinFinalLegTreeItem,300);
	HTREEITEM hMaxTurnAngleTreeItem = m_wndTreeRunwayData.InsertItem(strMaxTurnAngle,cni,FALSE,FALSE, hToRunwayTreeItem, TVI_LAST);
	m_wndTreeRunwayData.SetItemData(hMaxTurnAngleTreeItem, 30);
	m_wndTreeRunwayData.Expand(hToRunwayTreeItem, TVE_EXPAND);
	pApproachRunwayItem->AddToRunwayItem(pNewItem);
	m_wndTreeRunwayData.SetRedraw(TRUE);
}

void CDlgSideStepSpecification::OnDelToRunway()
{
	HTREEITEM hToRunwayTreeItem = m_wndTreeRunwayData.GetSelectedItem();
	CSideStepToRunway* pToRunwayItem = (CSideStepToRunway*)m_wndTreeRunwayData.GetItemData(hToRunwayTreeItem);
	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	CSideStepApproachRunway* pApproachRunwayItem = (CSideStepApproachRunway*)m_wndTreeFltRunway.GetItemData(hApproachRunwayTreeItem);
	m_wndTreeRunwayData.DeleteItem(hToRunwayTreeItem);
	pApproachRunwayItem->DeleteToRunwayItem(pToRunwayItem);
}

void CDlgSideStepSpecification::OnEditToRunway()
{
	HTREEITEM hToRunwayTreeItem = m_wndTreeRunwayData.GetSelectedItem();
	if(!hToRunwayTreeItem)return;
	HTREEITEM hParent = m_wndTreeRunwayData.GetParentItem(hToRunwayTreeItem);
	if(hParent)
	{		
		m_wndTreeRunwayData.SetWidth(m_wndTreeRunwayData.GetItemData(hToRunwayTreeItem));
		COOLTREE_NODE_INFO* pCNI = 0;
		pCNI = m_wndTreeRunwayData.GetItemNodeInfo(hToRunwayTreeItem);
		if(!pCNI) return;
		m_wndTreeRunwayData.ShowEditSpinWnd(hToRunwayTreeItem,pCNI);
		return;
	}

	CSideStepToRunway* pToRunwayItem = (CSideStepToRunway*)m_wndTreeRunwayData.GetItemData(hToRunwayTreeItem);
	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway runway;
	runway.ReadObject(nRunwayID);

	CString strRunwayMark;
	if(nRunwayMarkIndex == 0)
		strRunwayMark = runway.GetMarking1().c_str();
	else
		strRunwayMark = runway.GetMarking2().c_str();
	CString strToRunway;
	strToRunway.Format("%s:%s","Side Step To Runway",strRunwayMark);

	HTREEITEM hItem = m_wndTreeRunwayData.GetRootItem();
	while (hItem)
	{
		if (m_wndTreeRunwayData.GetItemText(hItem) == strToRunway)
		{
			MessageBox(_T("\" ") +strToRunway+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeRunwayData.GetNextItem(hItem , TVGN_NEXT);
	}

	m_wndTreeRunwayData.SetItemText(hToRunwayTreeItem, strToRunway);
	pToRunwayItem->SetRunwayID(nRunwayID);
	pToRunwayItem->SetMarkIndex(nRunwayMarkIndex);
}
void CDlgSideStepSpecification::UpdateToolBar()
{
	HTREEITEM hFltRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	if (hFltRunwayTreeItem != NULL)
	{
		if(m_wndTreeFltRunway.GetParentItem(hFltRunwayTreeItem) == NULL)
		{
			m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLIGHTTYPE, TRUE);
			m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLIGHTTYPE, TRUE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);
		}
		else
		{
			m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLIGHTTYPE, TRUE);
			m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLIGHTTYPE, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, TRUE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);
			
		}
	}
	else
	{
		m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLIGHTTYPE, FALSE);
		m_wndFltRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLIGHTTYPE, FALSE);
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, FALSE);
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);
	}
}
void CDlgSideStepSpecification::InitFltRunwayTree()
{
	for(int i=0;i<m_SideStepSpecification.GetFlightTypeCount();i++)
	{
		CSideStepFlightType* pSideStepFlightType = m_SideStepSpecification.GetFlightTypeItem(i);
		FlightConstraint& fltType = pSideStepFlightType->GetFltType();
		CString strFltType;
		fltType.screenPrint(strFltType);
		HTREEITEM hFltTreeItem = m_wndTreeFltRunway.InsertItem(strFltType);
		m_wndTreeFltRunway.SetItemData(hFltTreeItem, (DWORD_PTR)pSideStepFlightType);
		for (int j=0; j<pSideStepFlightType->GetApproachRunwayCount(); j++)
		{
			CSideStepApproachRunway* pSideStepApproachRunway
				= pSideStepFlightType->GetApproachRunwayItem(j);
			Runway runway;
			runway.ReadObject(pSideStepApproachRunway->GetRunwayID());
			CString strRunwayMark;
			if(pSideStepApproachRunway->GetMarkIndex() == 0)
				strRunwayMark = runway.GetMarking1().c_str();
			else
				strRunwayMark = runway.GetMarking2().c_str();
			CString strApproachRunway;
			strApproachRunway.Format("%s:%s","ApproachRunway",strRunwayMark);
			HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.InsertItem(strApproachRunway, hFltTreeItem, TVI_LAST);
			m_wndTreeFltRunway.SetItemData(hApproachRunwayTreeItem, (DWORD_PTR)pSideStepApproachRunway);
		}
		m_wndTreeFltRunway.Expand(hFltTreeItem, TVE_EXPAND);		
	}
}
void CDlgSideStepSpecification::InitRunwayDataTree()
{
	HTREEITEM hApproachRunwayTreeItem = m_wndTreeFltRunway.GetSelectedItem();
	if (hApproachRunwayTreeItem == NULL)
		return;

	if(m_wndTreeFltRunway.GetParentItem(hApproachRunwayTreeItem) == NULL)
		return;

	m_wndTreeRunwayData.DeleteAllItems();
	m_wndTreeRunwayData.SetRedraw(FALSE);

	CSideStepApproachRunway* pApproachRunway =
		(CSideStepApproachRunway*)m_wndTreeFltRunway.GetItemData(hApproachRunwayTreeItem);

	for (int i=0; i<pApproachRunway->GetToRunwayCount(); i++)
	{
		CSideStepToRunway* pSideStepToRunway = pApproachRunway->GetToRunwayItem(i);
		Runway runway;
		runway.ReadObject(pSideStepToRunway->GetRunwayID());
		CString strRunwayMark;
		if(pSideStepToRunway->GetMarkIndex() == 0)
			strRunwayMark = runway.GetMarking1().c_str();
		else
			strRunwayMark = runway.GetMarking2().c_str();
		CString strToRunway;
		strToRunway.Format("%s:%s","Side Step To Runway",strRunwayMark);
		COOLTREE_NODE_INFO cni;	
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hToRunwayTreeItem = m_wndTreeRunwayData.InsertItem(strToRunway,cni,FALSE);
		m_wndTreeRunwayData.SetItemData(hToRunwayTreeItem, (DWORD_PTR)pSideStepToRunway);
		CString strMinFinalLeg,strMaxTurnAngle;
		strMinFinalLeg.Format("Min Final Leg (m) : %d",pSideStepToRunway->GetMinFinalLeg());
		strMaxTurnAngle.Format("Max Turn Degree : %d",pSideStepToRunway->GetMaxTurnAngle());
		cni.nt=NT_NORMAL; 
		cni.net=NET_EDITSPIN_WITH_VALUE;
		cni.fMaxValue = 1000;
		HTREEITEM hMinFinalLegTreeItem = m_wndTreeRunwayData.InsertItem(strMinFinalLeg,cni,FALSE,FALSE, hToRunwayTreeItem, TVI_LAST);
		m_wndTreeRunwayData.SetItemData(hMinFinalLegTreeItem,pSideStepToRunway->GetMinFinalLeg() );
		HTREEITEM hMaxTurnAngleTreeItem = m_wndTreeRunwayData.InsertItem(strMaxTurnAngle,cni,FALSE,FALSE, hToRunwayTreeItem, TVI_LAST);
		m_wndTreeRunwayData.SetItemData(hMaxTurnAngleTreeItem, pSideStepToRunway->GetMaxTurnAngle());

		m_wndTreeRunwayData.Expand(hToRunwayTreeItem, TVE_EXPAND);
	}
	m_wndTreeRunwayData.SetRedraw(TRUE);
}



void CDlgSideStepSpecification::OnTvnSelchangedTreeFltrunway(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	InitRunwayDataTree();
	UpdateToolBar();
	*pResult = 0;
}

LRESULT CDlgSideStepSpecification::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message==UM_CEW_EDITSPIN_BEGIN)
	{

	}
	else if(message == UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		CString strValue = *((CString*)lParam);
		int ntemp = (int)atoi( strValue.GetBuffer() );
		
		HTREEITEM hToRunwayTreeItem = m_wndTreeRunwayData.GetParentItem(hItem);
		CSideStepToRunway* pToRunwayItem = (CSideStepToRunway*)m_wndTreeRunwayData.GetItemData(hToRunwayTreeItem);
		HTREEITEM hNextTreeItem = m_wndTreeRunwayData.GetNextSiblingItem(hItem);
		CString strtemp;
		if(hNextTreeItem != NULL)
		{
			pToRunwayItem->SetMinFinalLeg(ntemp);
			strtemp.Format("%s : %s","Min Final Leg (m)",strValue);
		}
		else
		{
			pToRunwayItem->SetMaxTurnAngle(ntemp);
			strtemp.Format("%s : %s","Max Turn Degree",strValue);
		}
		m_wndTreeRunwayData.SetItemText(hItem,strtemp);
		m_wndTreeRunwayData.SetItemData(hItem,ntemp);
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgSideStepSpecification::OnTvnSelchangedTreeRunwaydata(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateRunwayTree();
	*pResult = 0;
}

void CDlgSideStepSpecification::UpdateRunwayTree()
{
	HTREEITEM hRunwayDataTreeItem = m_wndTreeRunwayData.GetSelectedItem();
	if (hRunwayDataTreeItem != NULL && m_wndTreeRunwayData.GetParentItem(hRunwayDataTreeItem) == NULL)
	{
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, TRUE);
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, TRUE);
		m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);
	}
	else
	{

		if(hRunwayDataTreeItem != NULL && m_wndTreeRunwayData.GetParentItem(hRunwayDataTreeItem) != NULL)
		{
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, TRUE);
		}else
		{
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYDATA, FALSE);
			m_wndRunwayDataToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAYDATA, FALSE);
		}
	}
}
void CDlgSideStepSpecification::OnContextMenuForRunwayDate(CWnd* pWnd, CPoint& point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeRunwayData.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeRunwayData.SetFocus();
	CPoint pt = point;
	m_wndTreeRunwayData.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeRunwayData.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeRunwayData.SelectItem(hRClickItem);	
	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 

	HTREEITEM hParentItem = 0;
	hParentItem = m_wndTreeRunwayData.GetParentItem(hRClickItem);
	if(!hParentItem)
	{		
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_RUNWAYDATA, _T("Add Runway Data...") ); 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_RUNWAYDATA, _T("Delete Runway Data") ); 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_RUNWAYDATA, _T("Edit Runway Data...") ); 
	}		
	else
	{
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_RUNWAYDATA, _T("Edit Runway Data...") ); 
	}

	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}
