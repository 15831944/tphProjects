#include "stdafx.h"
#include "TermPlan.h"
#include "DlgHoldShortLines.h"
#include <Common/ARCVector.h>
#include <Common/WinMsg.h>
#include <CommonData/Fallback.h>
#include <InputAirside/IntersectionNode.h>
#include <InputAirside/taxiway.h>
#include <InputAirside\ARCUnitManager.h>
#include "ViewMsg.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgHoldShortLines, CXTResizeDialog)

BEGIN_MESSAGE_MAP(CDlgHoldShortLines, CXTResizeDialog)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDeleteItem)
	ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_COMMAND(IDC_BUTTON_SAVE,OnSave)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_HOLDSHORTLINE, OnNMRclickListHoldShort)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_HOLDSHORTLINE,OnEndEditLabelListHoldShort)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CDlgHoldShortLines::CDlgHoldShortLines(int nProjID,CWnd * pParent/* = NULL*/) 
:CXTResizeDialog(CDlgHoldShortLines::IDD, pParent),m_nProjID(nProjID)
{
	m_pSelHoldShortLineNode = NULL;
	m_nRow = -1;
	m_nColumn = -1;
}

CDlgHoldShortLines::~CDlgHoldShortLines(void)
{
}

void CDlgHoldShortLines::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HOLDSHORTLINE, m_wndListCtrl);
}

BOOL CDlgHoldShortLines::OnInitDialog(void)
{
	CXTResizeDialog::OnInitDialog();
	SetWindowText(_T("Taxi Interrupt Lines"));

	InitToolbar();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TOOLBAR, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_HOLDSHORTLINE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_GROUPBOX_HOLDSHORTLINES, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	
	m_holdShortLines.ReadData(m_nProjID);
	InitListCtrl();
	SetListCtrlContent();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return (TRUE);
}

void CDlgHoldShortLines::InitListCtrl(void)
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);


	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = "Name";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = "Position(right click to pick from map)";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	TCHAR Coordinate[1024] = {0};
	wsprintf(Coordinate,"Coordinates(%s)",CARCLengthUnit::GetLengthUnitString(GetCurUnit()));
	lvc.pszText = Coordinate;
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);
}

void CDlgHoldShortLines::SetListCtrlContent(void)
{
	m_wndListCtrl.DeleteAllItems();
	
	size_t nElementCount = m_holdShortLines.GetElementCount();

	for (size_t j = 0;j < nElementCount;++j) 
	{
		CTaxiInterruptLine*	pItem = m_holdShortLines.GetItem(j);
		if (pItem) 
		{
			m_wndListCtrl.InsertItem(j,pItem->m_strName);
			m_wndListCtrl.SetItemText(j,1,pItem->m_strDescription);

			CString strTemp;
			ARCUnit_Length CurUnit = GetCurUnit() ;
			strTemp.Format(_T("%.2f,%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CurUnit,pItem->m_Position.getX()),
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CurUnit,pItem->m_Position.getY()));
			m_wndListCtrl.SetItemText(j,2,strTemp);
			m_wndListCtrl.SetItemData(j,(DWORD_PTR)pItem);
		}
	}
}

LRESULT CDlgHoldShortLines::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	//?????
	EnableWindow();

	if(m_pSelHoldShortLineNode == NULL)//??????HoldShortLineNode?
		return (false);

	ALTObject* pObj = reinterpret_cast<ALTObject*>(wParam);//???????Taxiway?
	if (pObj->GetType() != ALT_TAXIWAY)
	{
		MessageBox("Please select a point on Taxiway!");
		return false;
	}

	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>*>(lParam);//????????(?????????????)
	if(!pData)
		return (false);

	size_t nSize = pData->size();//???????
	if (nSize != 1)
		return (false);

	CPoint2008 ptSelect;
	ARCVector3 v3 = pData->at(0);//??????
	ptSelect.setPoint(v3[VX] ,v3[VY],v3[VZ]);


	Taxiway* pTaxiway = (Taxiway*)pObj;
	CString strTemp = _T("");

	IntersectionNodeList nodeList = pTaxiway->GetIntersectNodes();//??Taxiway???????
	std::sort(nodeList.begin(),nodeList.end(), IntersectionNodeDistCompareInTaxiway(pTaxiway->getID()));

	IntersectionNode* pStartNode = NULL;
	IntersectionNode* pEndNode = NULL;

	double dDistInTaxiway = pTaxiway->getPath().GetPointDist(ptSelect);//??Taxiway?????????????

	int nCount = nodeList.size();
	for (int i = 0; i < nCount-1; i++)//??Taxiway?????????????,?????????????
	{
		pStartNode = &nodeList.at(i);
		pEndNode = &nodeList.at(i+1);

		double dDistStart= pTaxiway->getPath().GetPointDist(pStartNode->GetPosition());
		double dDistEnd = pTaxiway->getPath().GetPointDist(pEndNode->GetPosition());
		if (dDistInTaxiway >= dDistStart && dDistInTaxiway <= dDistEnd)
			break;
	}

	if(pStartNode == NULL)
	{
		MessageBox("Invalid position, please select a point between intersection nodes of taxiway or near by an intersection node of taxiway!");
		return false;
	}

	double dDist1 = pTaxiway->getPath().GetPointDist(pStartNode->GetPosition());
	double dDist2 = pTaxiway->getPath().GetPointDist(ptSelect);

	double dDistToStart = dDist2 - dDist1;
	strTemp.Format("%s < %.2f < %s",pStartNode->GetName(),dDistToStart/100,pEndNode->GetName());

	m_pSelHoldShortLineNode->m_nFrontInterNodeID = pStartNode->GetID();
	if (pEndNode)
		m_pSelHoldShortLineNode->m_nBackInterNodeID = pEndNode->GetID();
	m_pSelHoldShortLineNode->m_dDistToFrontInterNode = dDistToStart;
	m_pSelHoldShortLineNode->m_Position = ptSelect;
	m_pSelHoldShortLineNode->m_strDescription = strTemp;
	m_pSelHoldShortLineNode->m_nTaxiwayID = pTaxiway->getID();

	m_wndListCtrl.SetItemText(m_nRow,1,strTemp);
	strTemp.Format("%.2f,%.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurUnit(),ptSelect.getX()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurUnit(),ptSelect.getY()));
	m_wndListCtrl.SetItemText(m_nRow,2,strTemp);		


	return true;

}


void CDlgHoldShortLines::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgHoldShortLines::ShowDialog(CWnd* parentWnd)
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

void CDlgHoldShortLines::OnProcpropPickfrommap() 
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

	FallbackReason enumReason = PICK_ALTOBJECT;


	//	enumReason= PICK_ONEPOINT;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CDlgHoldShortLines::OnNewItem(void)
{
	int nItemCount = m_wndListCtrl.GetItemCount();
	CTaxiInterruptLine * pItem =  new CTaxiInterruptLine;
	if (pItem) 
	{
		pItem->m_strName = _T("Unnamed");
		m_holdShortLines.AddNewItem(pItem);

		m_wndListCtrl.InsertItem(nItemCount,pItem->m_strName);
		CString strTemp = _T("");		
		m_wndListCtrl.SetItemText(nItemCount,1,strTemp);
		strTemp.Format(_T("(%d,%d)"),0,0);
		m_wndListCtrl.SetItemText(nItemCount,2,strTemp);
		m_wndListCtrl.SetItemData(nItemCount,(DWORD_PTR)pItem);
	} 

}

void CDlgHoldShortLines::OnDeleteItem(void)
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
		CTaxiInterruptLine * pItemData = (CTaxiInterruptLine*)(m_wndListCtrl.GetItemData(nItem));
		m_holdShortLines.DeleteItem(pItemData);
		m_wndListCtrl.DeleteItemEx(nItem);
	}
}

void CDlgHoldShortLines::OnEditItem(void)
{
}

void CDlgHoldShortLines::OnSave(void)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_holdShortLines.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->UpdateAllViews((CView*)m_pParentWnd,VM_MODIFY_HOLDSHORTLINE,NULL);
}

void CDlgHoldShortLines::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgHoldShortLines::OnCancel()
{
	CXTResizeDialog::OnCancel();
}


int CDlgHoldShortLines::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this) ||	!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MAIN_ADD_DEL_EDIT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}


void CDlgHoldShortLines::InitToolbar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	toolbar.EnableButton(ID_NEW_ITEM, TRUE);
	toolbar.EnableButton(ID_DEL_ITEM, TRUE);
	toolbar.HideButton(ID_EDIT_ITEM,TRUE);
}

void CDlgHoldShortLines::OnNMRclickListHoldShort(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	//	CCursor cursor = ::GetCursor();
	CPoint point,ptCursor;
	GetCursorPos(&ptCursor);
	point = ptCursor;
	m_wndListCtrl.ScreenToClient(&point);

	m_pSelHoldShortLineNode = NULL;
	m_nRow = -1;
	m_nColumn = -1;
	int index = -1;
	int colnum = -1;
	if((index = m_wndListCtrl.HitTestEx(point, &colnum)) != -1)
	{
		if(colnum == 1)
		{
			m_pSelHoldShortLineNode = (CTaxiInterruptLine *)m_wndListCtrl.GetItemData(index);
			m_nRow = index;
			m_nColumn = colnum;
			CMenu menuPopup;
			menuPopup.LoadMenu(IDR_MENU_POPUP);
			CMenu* pSubMenu = NULL;
			pSubMenu = menuPopup.GetSubMenu(10);
			if (pSubMenu != NULL)
				pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCursor.x, ptCursor.y, this);
		}
	}

	*pResult = 0;
}

void CDlgHoldShortLines::OnEndEditLabelListHoldShort(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO * pDispInfo = (LV_DISPINFO *)pNMHDR;
	if (pDispInfo) {		
		CTaxiInterruptLine * pItemData = (CTaxiInterruptLine *)m_wndListCtrl.GetItemData(pDispInfo->item.iItem);
		if (pItemData) {
			pItemData->m_strName = pDispInfo->item.pszText;
		}
	}
	*pResult = 0;
}

ARCUnit_Length CDlgHoldShortLines::GetCurUnit()
{
	CARCUnitBaseManager mannger ;
	mannger.ReadData(m_nProjID,UM_ID_GLOBAL_DEFAULT);
	//mannger.SetUnitInUse(ARCUnit_Length_InUse);
	return  mannger.GetCurSelLengthUnit();
}

bool IntersectionNodeDistCompareInTaxiway::operator()( IntersectionNode& Node1, IntersectionNode& Node2 ) const
{
	return Node1.GetDistance(m_nTaxiId) < Node2.GetDistance(m_nTaxiId);
}