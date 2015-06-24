#include "StdAfx.h"
#include "TermPlan.h"
#include ".\dlgbaseresident.h"
#include "Inputs/IN_TERM.H"
#include "Landside/InputLandside.h"
#include "Landside/LandsideLayoutObject.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "CommonData/Fallback.h"
#include "Common/WinMsg.h"
#include "CommonData/QueryData.h"
#include "Landside/LandsideQueryData.h"

namespace 
{
	UINT ID_FACILITY_COPY = WM_USER + 1001;
	UINT ID_FACILITY_ADDCOPY = WM_USER + 1002;
	UINT ID_FACILITY_INSERTCOPY = WM_USER + 1003;
}
IMPLEMENT_DYNAMIC(CDlgBaseResident, CDialog)
CDlgBaseResident::CDlgBaseResident(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent /*=NULL*/)
: CDialog(CDlgBaseResident::IDD, pParent)
,m_pInTerm(pInTerm)
,m_pInputLandside(pInputLandside)
{
	m_bDragging				= false;
	m_pDragImage			= NULL;
	m_hItemDragSrc			= NULL;
	m_hItemDragDes			= NULL;

	m_vLandsideObjectType.push_back(ALT_LEXT_NODE);
	m_vLandsideObjectType.push_back(ALT_LINTERSECTION);
	m_vLandsideObjectType.push_back(ALT_LDECISIONPOINT);
	m_vLandsideObjectType.push_back(ALT_LPARKINGLOT);
	m_vLandsideObjectType.push_back(ALT_LCURBSIDE);
	m_vLandsideObjectType.push_back(ALT_LBUSSTATION);
	m_vLandsideObjectType.push_back(ALT_LROUNDABOUT);
	m_vLandsideObjectType.push_back(ALT_LTAXIQUEUE);
	m_vLandsideObjectType.push_back(ALT_LTAXIPOOL);

	LandsideFacilityObject startObject;
	startObject.SetType(LandsideFacilityObject::Landside_MoveStart_Facility);
	m_startRoute.SetFacilityObject(startObject);
	m_pPickLayoutObject = NULL;
}

CDlgBaseResident::~CDlgBaseResident(void)
{
	for(UINT i = 0; i < m_vTreeNodes.size(); i++)
	{
		delete m_vTreeNodes[i];
	}
	m_vTreeNodes.clear();
}

void CDlgBaseResident::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
	DDX_Control(pDX,IDC_TREE_FACILITY,m_wndFacilityCtrl);
	DDX_Control(pDX,IDC_TREE_PLAN,m_wndPlanCtrl);
}

BEGIN_MESSAGE_MAP(CDlgBaseResident, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_FACILITY, OnBegindragTreeFacility)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_DATA, OnLvnDeleteitemListData)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_PLAN,OnLvnPlanTreeChanged)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_PLAN, OnDeletePlanTreeItem)
	ON_MESSAGE(TP_DATA_BACK,OnTempFallbackFinished)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_COMMAND(ID_ADD_DEST,OnAddDestination)
	ON_COMMAND(ID_INSERT_BEFORE,OnInsertBefore)
	ON_COMMAND(ID_INSERT_AFTER,OnInsertAfter)
	ON_COMMAND(ID_PRUNE_BRANCH,OnPruneBrance)
	ON_COMMAND(ID_REMOVE_BRANCH,OnRemoveFromBranch)
	ON_COMMAND(ID_REMOVE_ALLBRANCH,OnRemoveFromAllBranch)
	ON_COMMAND(ID_MODIFY_PERCENT,OnModifyPercent)
	ON_COMMAND(ID_RESIDENT_PERCENT,OnEvenPercent)
	ON_COMMAND(ID_PICK_MAP,OnPickFromMap)
	ON_COMMAND(ID_FACILITY_COPY,OnCopyFacility)
	ON_COMMAND(ID_FACILITY_ADDCOPY,OnAddCopyFacility)
	ON_COMMAND(ID_FACILITY_INSERTCOPY,OnInsertCopyFacility)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PLAN, OnClickTreeProcess)
END_MESSAGE_MAP()

int CDlgBaseResident::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndVehicleBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP | CBRS_BORDER_TOP)
		|| !m_wndVehicleBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	if (!m_wndPlanBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_BORDER_TOP | CBRS_TOOLTIPS)
		|| !m_wndPlanBar.LoadToolBar(IDR_TOOLBAR_RESIDENTPLAN))
	{
		return -1;
	}
	return 0;
}

void CDlgBaseResident::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	if(!m_wndFacilityCtrl.GetSafeHwnd() == NULL)
	{
		DoResize(cx,cy);
	}
}

void CDlgBaseResident::DoResize( int cx,int cy )
{
	CRect rect;
	GetDlgItem(IDC_STATIC_FACILITY)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	int nBottom = 0;
	CRect rectWnd;
	m_wndFacilityCtrl.SetWindowPos(NULL,rect.left,rect.bottom + 4,cx/3 - 20,cy - rect.Height() - 60,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndFacilityCtrl.GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);
	nBottom = rectWnd.bottom;
	//Splitter1
	if(!m_wndSplitter1.m_hWnd)//Create Splitter1
	{
		m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_VERTICAL_SPLITTER);
		m_wndSplitter1.SetStyle(SPS_VERTICAL);
	}

	CRect sp1Rect;
	m_wndSplitter1.SetWindowPos(NULL,rectWnd.right,rectWnd.top,4,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter1.GetWindowRect(&sp1Rect);
	ScreenToClient(&sp1Rect);
	m_wndSplitter1.SetRange(rectWnd.left,cx - 2 * 5);

	CRect typeRect;
	GetDlgItem(IDC_STATIC_TYPE)->GetWindowRect(&typeRect);
	GetDlgItem(IDC_STATIC_TYPE)->SetWindowPos(NULL,rectWnd.right + 5,rect.top,cx - 2*rect.left - rectWnd.Width() - 5,typeRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	ScreenToClient(&typeRect);

	CRect barRect;
	m_wndVehicleBar.GetWindowRect(&barRect);
	m_wndVehicleBar.SetWindowPos(NULL,typeRect.left + 2,typeRect.bottom + 4,cx - 2*rect.left - rectWnd.Width() - 5,barRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndVehicleBar.GetWindowRect(&barRect);
	ScreenToClient(&barRect);

	m_wndListCtrl.SetWindowPos(NULL,barRect.left,barRect.bottom + 4,cx - 2*rect.left - rectWnd.Width() - 5,cy/3 - 20,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndListCtrl.GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);	
	//Splitter2
	if(!m_wndSplitter2.m_hWnd)//Create Splitter2
	{
		m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_HORIZONTAL_SPLITTER);
		m_wndSplitter2.SetStyle(SPS_HORIZONTAL);
	}

	CRect sp2Rect;
	m_wndSplitter2.SetWindowPos(NULL,rectWnd.left,rectWnd.bottom,rectWnd.Width(),4,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter2.GetWindowRect(&sp2Rect);
	ScreenToClient(&sp2Rect);
	m_wndSplitter2.SetRange(rectWnd.top,cy - 2*5);

	CRect textPlanRect;
	GetDlgItem(IDC_STATIC_PLAN)->SetWindowPos(NULL,rectWnd.left,rectWnd.bottom +6,rectWnd.Width(),rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	GetDlgItem(IDC_STATIC_PLAN)->GetWindowRect(&textPlanRect);
	ScreenToClient(&textPlanRect);

	CRect planBarRect;
	m_wndPlanBar.SetWindowPos(NULL,textPlanRect.left + 2, textPlanRect.bottom + 4,barRect.Width(),barRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndPlanBar.GetWindowRect(&planBarRect);
	ScreenToClient(&planBarRect);

	m_wndPlanCtrl.SetWindowPos(NULL,textPlanRect.left,planBarRect.bottom + 2,rectWnd.Width(),nBottom - planBarRect.bottom - 2,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndPlanCtrl.GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);

	CRect rcBtn;
	GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
	GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rectWnd.right - rcBtn.Width() - 2,rectWnd.bottom + 8,rcBtn.Width(),rcBtn.Height(),NULL);
	GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
	ScreenToClient(rcBtn);
	GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 8,rectWnd.bottom + 8,rcBtn.Width(),rcBtn.Height(), NULL);
	GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
	ScreenToClient(rcBtn);
	CRect rcSave;
	GetDlgItem(IDC_BUTTON_SAVE)->GetWindowRect(&rcSave);
	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowPos(NULL,rcBtn.left - rcSave.Width() - 12,rectWnd.bottom+8,rcSave.Width(),rcSave.Height(),NULL);

	GetDlgItem(IDCANCEL)->Invalidate(FALSE);
	GetDlgItem(IDOK)->Invalidate(FALSE);
	GetDlgItem(IDC_BUTTON_SAVE)->Invalidate(FALSE);
}

BOOL CDlgBaseResident::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_imageList.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	CBitmap bm;
	bm.LoadBitmap(IDB_PROCESSES);
	m_imageList.Add(&bm,RGB(255,0,255));

	ReadInputData();
	m_wndPlanCtrl.SetReflectMsg(true);
	SetWindowText(GetTitle());
	InitListCtrlHeader();
	InitToolbar();
	FillFacilityTreeContent();
	FllListCtrlContent();

	LayoutFigure();

	CRect rect;
	GetClientRect(&rect);
	DoResize(rect.Width(),rect.Height());

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgBaseResident::InitListCtrlHeader()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);
}

void CDlgBaseResident::OnBegindragTreeFacility( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_wndFacilityCtrl.SelectItem(m_hItemDragSrc);
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndFacilityCtrl.GetItemData(m_hItemDragSrc);
	if (pNodeData == NULL)
		return;

	if (pNodeData->m_emTye == TreeNodeData::Group_Facility)
	{
		MessageBox(_T("Can't select group of lanside object!!"),_T("Warning"),MB_OK);
		return;
	}

	m_hItemDragDes = NULL;

	if(m_pDragImage)
		delete m_pDragImage;

	m_pDragImage = m_wndFacilityCtrl.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
}

void CDlgBaseResident::OnMouseMove( UINT nFlags, CPoint point )
{
	HTREEITEM  hItem;
	UINT       flags;

	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);
		//////////////////////////////////////////////////////
		CRect rectTree;
		m_wndPlanCtrl.GetWindowRect( &rectTree );
		if( rectTree.PtInRect( pt) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen(&ptTest);
			m_wndPlanCtrl.ScreenToClient(&ptTest);

			hItem = m_wndPlanCtrl.HitTest( ptTest,&flags);
			if( hItem != NULL )
			{
				m_wndPlanCtrl.SelectDropTarget( hItem );
				m_hItemDragDes = hItem;
			}
			else
			{
				m_wndPlanCtrl.SelectDropTarget( NULL );
				m_hItemDragDes = NULL;
			}
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_wndFacilityCtrl.Invalidate(FALSE);
	//	m_wndPlanCtrl.Invalidate(FALSE);
	}


	CDialog::OnMouseMove(nFlags, point);
}

void CDlgBaseResident::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		CPoint curPt(pt);

		UINT iRet;
		m_wndPlanCtrl.ScreenToClient(&pt);
		HTREEITEM hItem = m_wndPlanCtrl.HitTest(pt, &iRet);
		m_wndPlanCtrl.SelectItem(hItem);
		if( hItem && std::find(m_vRoute.begin(),m_vRoute.end(),hItem) != m_vRoute.end())
		{
			PopupRouteMenu(curPt);
		}

		m_bDragging = false;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage;
		m_pDragImage = NULL;
		m_wndPlanCtrl.SelectDropTarget( NULL );
		ReleaseCapture();
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		m_wndPlanCtrl.Invalidate(FALSE);
	}	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgBaseResident::FillFacilityTreeContent()
{
	m_wndFacilityCtrl.SetImageList ( &m_imageList,TVSIL_NORMAL );

	//entry and exit noe
	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hExtItem = m_wndFacilityCtrl.InsertItem(_T("All Entries and Exits "));
	m_wndFacilityCtrl.SetItemData(m_hExtItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hInterSectItem = m_wndFacilityCtrl.InsertItem(_T("All Intersections"));
	m_wndFacilityCtrl.SetItemData(m_hInterSectItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hDecisionItem = m_wndFacilityCtrl.InsertItem(_T("All Decision Point"));
	m_wndFacilityCtrl.SetItemData(m_hDecisionItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hParkLotItem = m_wndFacilityCtrl.InsertItem(_T("All Parking Lots"));
	m_wndFacilityCtrl.SetItemData(m_hParkLotItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hCurbsideItem = m_wndFacilityCtrl.InsertItem(_T("All Curbsides"));
	m_wndFacilityCtrl.SetItemData(m_hCurbsideItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hRoundaboutItem = m_wndFacilityCtrl.InsertItem(_T("All Roundabouts"));
	m_wndFacilityCtrl.SetItemData(m_hRoundaboutItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hBusStationItem = m_wndFacilityCtrl.InsertItem(_T("All Bus Stations"));
	m_wndFacilityCtrl.SetItemData(m_hBusStationItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hTaxiQueueItem = m_wndFacilityCtrl.InsertItem(_T("All Taxi Queue"));
	m_wndFacilityCtrl.SetItemData(m_hTaxiQueueItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Group_Facility;
	m_hTaxiPoolItem = m_wndFacilityCtrl.InsertItem(_T("All Taxi Pool"));
	m_wndFacilityCtrl.SetItemData(m_hTaxiPoolItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
	for (int i = 0; i < layoutObjectList.getCount(); i++)
	{
		LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObject(i);
		ALTObjectID objName = pLandsideObject->getName();
		if (TypeNeedToShow(pLandsideObject->GetType()))
		{
			HTREEITEM hRootItem;
			switch (pLandsideObject->GetType())
			{
			case ALT_LEXT_NODE:
				hRootItem = m_hExtItem;
				break;
			case ALT_LINTERSECTION:
				hRootItem = m_hInterSectItem;
				break;
			case ALT_LDECISIONPOINT:
				hRootItem = m_hDecisionItem;
				break;
			case ALT_LPARKINGLOT:
				hRootItem = m_hParkLotItem;
				break;
			case ALT_LCURBSIDE:
				hRootItem = m_hCurbsideItem;
				break;
			case ALT_LBUSSTATION:
				hRootItem = m_hBusStationItem;
				break;
			case ALT_LROUNDABOUT:
				hRootItem = m_hRoundaboutItem;
				break;
			case ALT_LTAXIQUEUE:
				hRootItem = m_hTaxiQueueItem;
				break;
			case ALT_LTAXIPOOL:
				hRootItem = m_hTaxiPoolItem;
				break;
			default :
				ASSERT(FALSE);
				break;
			}

			AddObjectToTree(pLandsideObject,hRootItem,objName);
		}

	}
}

void CDlgBaseResident::InsertFacilityTreeItem( LandsideFacilityLayoutObject* pLandsideObject,HTREEITEM hItem,int nLevel )
{
	ALTObjectID objName = pLandsideObject->getName();
	int nLength = objName.idLength();
	if (nLevel >= nLength)
		return;

	CString szName = objName.at(nLevel).c_str();
	HTREEITEM hChildItem = m_wndFacilityCtrl.InsertItem(szName,hItem);
	nLevel++;
	if (nLevel == nLength)
	{
		m_wndFacilityCtrl.SetItemData(hChildItem,(DWORD)pLandsideObject);
	}
	InsertFacilityTreeItem(pLandsideObject,hChildItem,nLevel);
}

bool CDlgBaseResident::TypeNeedToShow( int nType ) const
{
	if (m_vLandsideObjectType.size() == 0)
		return true;

	if(std::find(m_vLandsideObjectType.begin(), m_vLandsideObjectType.end(), nType) != m_vLandsideObjectType.end())
		return true;

	return false;
}

void CDlgBaseResident::AddObjectToTree( LandsideFacilityLayoutObject* pLandsideObject,HTREEITEM hObjRoot,ALTObjectID objName )
{
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");
	CString strname="";
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		strname=objName.m_val[nLevel].c_str();
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (strname != _T(""))
			{
				TreeNodeData* pNodeData = new TreeNodeData();
				pNodeData->m_pLandsideObject = pLandsideObject;
				int idLength = objName.idLength();
				if (idLength - 1 == nLevel )
				{
					pNodeData->m_emTye = TreeNodeData::Single_Facility;
				}	
				else 
				{
					pNodeData->m_emTye = TreeNodeData::Group_Facility;
				}

				HTREEITEM hTreeItem = m_wndFacilityCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				m_wndFacilityCtrl.SetItemData(hTreeItem,(DWORD)pNodeData);
				m_vTreeNodes.push_back(pNodeData);
				m_wndFacilityCtrl.Expand(hParentItem,TVE_EXPAND);
				hParentItem = hTreeItem;
				continue;
			}
			else 
			{
				break;
			}

		}
		else if (strname!=_T(""))
		{
			TreeNodeData* pNodeData = new TreeNodeData();
			pNodeData->m_pLandsideObject = pLandsideObject;
			pNodeData->m_emTye = TreeNodeData::Single_Facility;
			HTREEITEM hTreeItem = m_wndFacilityCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			m_wndFacilityCtrl.SetItemData(hTreeItem,(DWORD)pNodeData);
			m_vTreeNodes.push_back(pNodeData);
			m_wndFacilityCtrl.Expand(hParentItem,TVE_EXPAND);
			break;
		}
	}
}

HTREEITEM CDlgBaseResident::FindObjNode( HTREEITEM hParentItem,const CString& strNodeText )
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_wndFacilityCtrl.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_wndFacilityCtrl.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_wndFacilityCtrl.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CDlgBaseResident::InitToolbar()
{
	CRect vehicleRect;
	m_wndListCtrl.GetWindowRect( &vehicleRect );
	ScreenToClient( &vehicleRect );
	vehicleRect.top -= 26;
	vehicleRect.bottom = vehicleRect.top + 22;
	vehicleRect.left += 2;
	m_wndVehicleBar.MoveWindow(&vehicleRect);

	CToolBarCtrl& vehicleBarCtrl = m_wndVehicleBar.GetToolBarCtrl();
	vehicleBarCtrl.SetCmdID(0,ID_VEHICLE_NEW);
	vehicleBarCtrl.SetCmdID(1,ID_VEHICLE_DEL);

	m_wndVehicleBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_NEW, TRUE);
	m_wndVehicleBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_DEL, FALSE);


	CRect planRect;
	m_wndPlanCtrl.GetWindowRect( &planRect );
	ScreenToClient( &planRect );
	planRect.top -= 26;
	planRect.bottom = planRect.top + 22;
	planRect.left += 2;
	m_wndPlanBar.MoveWindow(&planRect);

	CToolBarCtrl& planBarCtrl = m_wndPlanBar.GetToolBarCtrl();

	m_wndPlanBar.GetToolBarCtrl().EnableButton( ID_RESIDENT_ADD, FALSE);
	m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
	m_wndPlanBar.GetToolBarCtrl().EnableButton( ID_RESIDENT_DEL, FALSE);
}

void CDlgBaseResident::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
		m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
			m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);

			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);

			m_wndListCtrl.SetCurSel(-1);
			FillPlanTreeContent();
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
		m_wndVehicleBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,TRUE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		FillPlanTreeContent();
	}
	else
		ASSERT(0);
}

void CDlgBaseResident::OnLvnPlanTreeChanged( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			std::vector<HTREEITEM>::iterator iter = std::find(m_vTimeRange.begin(),m_vTimeRange.end(),pNMTreeView->itemNew.hItem);
			if (iter != m_vTimeRange.end())
			{
				m_wndPlanCtrl.SelectItem(pNMTreeView->itemNew.hItem );
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,FALSE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
				return;
			}
			iter = std::find(m_vRoute.begin(),m_vRoute.end(),pNMTreeView->itemNew.hItem);
			if(iter != m_vRoute.end())
			{
				m_wndPlanCtrl.SelectItem(pNMTreeView->itemNew.hItem );
				int nSelType = PopChildMenu();
				if (nSelType == 0)
				{
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);	
				}
				else if (nSelType == 1)
				{
					if (FacilitySelect() || m_pPickLayoutObject)
					{
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,TRUE);
					}
					else
					{
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
					}
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,FALSE);
					m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);	
				}
				else if (nSelType == 2)
				{
					if (FacilitySelect() || m_pPickLayoutObject)
					{
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
					}
					else
					{
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,TRUE);
						m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
					}
					ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(pNMTreeView->itemNew.hItem);
					if (pVehicleRoute)
					{
						HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(pNMTreeView->itemNew.hItem);
						if(IfHaveCircle(hParentItem,*pVehicleRoute->GetFacilityObject()))
						{
							m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
							m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
						}
					}
				}
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
				return;
			}

			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		}
	}
	else
	{
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_ADD_DEST,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_BEFORE,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_INSERT_AFTER,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_BRANCH,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_REMOVE_ALLBRANCH,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PRUNE_BRANCH,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_MODIFY_PERCENT,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_PICK_MAP,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
	}
}

void CDlgBaseResident::OnDeletePlanTreeItem( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(m_wndPlanBar.GetSafeHwnd() == NULL)
		return;

	if (m_wndListCtrl.GetCurSel() == LB_ERR)
	{
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		return;
	}

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),pNMTreeView->itemNew.hItem) != m_vTimeRange.end() )
			{
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
				return;
			}

			if(std::find(m_vRoute.begin(),m_vRoute.end(),pNMTreeView->itemNew.hItem) != m_vRoute.end())
			{
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,TRUE);
				m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,TRUE);
				return;
			}
			
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
			
		}
	}
	else
	{
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
	}
}

void CDlgBaseResident::AdjustPlanTreeItemPercent( HTREEITEM hItem )
{
	HTREEITEM hChild = m_wndPlanCtrl.GetChildItem(hItem);
	while(hChild)
	{
		ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hChild);
		LandsideFacilityObject::FacilityType emType = pVehicleRoute->GetFacilityObject()->GetType();
		CString strRoute;

		if (emType == LandsideFacilityObject::Landside_HomeBase_Facility )
		{
			strRoute.Format(_T("HomeBase (%d%%)"),pVehicleRoute->GetPercent());
		}
		else if (emType == LandsideFacilityObject::Landside_ServiceStart_Facility)
		{
			strRoute.Format(_T("ServiceStart (%d%%)"),pVehicleRoute->GetPercent());
		}
		else 
		{
			LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
			LandsideFacilityObject* pFacilityObect = pVehicleRoute->GetFacilityObject();
			if (pFacilityObect == NULL)
				return;

			int nFacilityID = pFacilityObect->GetFacilityID();
			LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
			if (pLandsideObject == NULL)
				return;
			strRoute.Format(_T("%s: %s ( %d%% )"),pVehicleRoute->GetTypeString(),pLandsideObject->getName().GetIDString(),pVehicleRoute->GetPercent());
		}
		m_wndPlanCtrl.SetItemText(hChild,strRoute);
		hChild = m_wndPlanCtrl.GetNextSiblingItem(hChild);
	}
}

void CDlgBaseResident::DeleteRouteItem( HTREEITEM hItem )
{
	std::vector<HTREEITEM>::iterator iter = std::find(m_vRoute.begin(),m_vRoute.end(),hItem);
	if(iter != m_vRoute.end())
	{
		m_vRoute.erase(iter);
	}

	HTREEITEM hChild = m_wndPlanCtrl.GetChildItem(hItem);
	while(hChild)
	{
		DeleteRouteItem(hChild);
		hChild = m_wndPlanCtrl.GetNextSiblingItem(hChild);
	}
}

void CDlgBaseResident::FillPlanTreeContent()
{
	m_wndPlanCtrl.DeleteAllItems();

	m_vTimeRange.clear();
	m_vRoute.clear();

	m_wndPlanCtrl.SetImageList(m_wndPlanCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
}

LRESULT CDlgBaseResident::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_NOTIFY)
	{
		if (wParam == ID_VERTICAL_SPLITTER)
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
			int delta = pHdr->delta;

			CSplitterControl::ChangeWidth(&m_wndFacilityCtrl, delta, CW_LEFTALIGN);

			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_TYPE),-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndVehicleBar,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndListCtrl,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter2,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_PLAN),-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndPlanBar,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndPlanCtrl,-delta,CW_RIGHTALIGN);
			Invalidate();
		}
		else if (wParam == ID_HORIZONTAL_SPLITTER)
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
			int delta = pHdr->delta;

			CSplitterControl::ChangeHeight(&m_wndListCtrl,delta,CW_TOPALIGN);

			CRect planRect;
			GetDlgItem(IDC_STATIC_PLAN)->GetWindowRect(&planRect);
			ScreenToClient(&planRect);
			planRect.bottom += delta;
			planRect.top += delta;
			GetDlgItem(IDC_STATIC_PLAN)->MoveWindow(planRect);

			CRect barRect;
			m_wndPlanBar.GetWindowRect(&barRect);
			ScreenToClient(&barRect);
			barRect.bottom += delta;
			barRect.top += delta;
			m_wndPlanBar.MoveWindow(barRect);

			CSplitterControl::ChangeHeight(&m_wndPlanCtrl,-delta,CW_BOTTOMALIGN);
			Invalidate();
		}
	}

	return CDialog::DefWindowProc(message,wParam,lParam);
}


void CDlgBaseResident::OnLvnDeleteitemListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	int nCount = m_wndListCtrl.GetItemCount();
	if (pnmv->iItem >= nCount || pnmv->iItem < 0)
		return;

	int iStart = pnmv->iItem + 1;
	for (int i = iStart; i < nCount; i++)
	{
		CString strIndex;
		strIndex.Format(_T("%d"),i);
		m_wndListCtrl.SetItemText(i,0,strIndex);
	}
}

BOOL CDlgBaseResident::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	if (GetRoutingFrame() != NULL) return FALSE;

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

	if (nID != 0) // will be zero on a separator
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

void CDlgBaseResident::OnPaint()
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
			|| pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl))
			|| pWnd->IsKindOf(RUNTIME_CLASS(CButton)))
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
}

BOOL CDlgBaseResident::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CDlgBaseResident::OnAddDestination()
{
	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;
	
	HTREEITEM hItem;
	hItem = m_wndPlanCtrl.GetSelectedItem();
	
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	LandsideFacilityObject sourceObject = *pVehicleRoute->GetFacilityObject();
	ResidentVehicleRoute vehicleRoute;
	if (m_pPickLayoutObject)
	{
		LandsideFacilityObject::FacilityType emType = LandsideFacilityObject::convertLandObjectType(m_pPickLayoutObject->GetType());
		vehicleRoute.GetFacilityObject()->SetFacilityID(m_pPickLayoutObject->getID());
		vehicleRoute.GetFacilityObject()->SetType(emType);
	}
	else
	{
		HTREEITEM hDragItem;
		hDragItem = m_wndFacilityCtrl.GetSelectedItem();
	
		if (hDragItem == NULL)
			return;
		vehicleRoute = CreateVehicleRoute(hDragItem);
	}
	
	pVehicleRouteFlow->AddDestinationObject(sourceObject,vehicleRoute);
	m_pPickLayoutObject = NULL;

	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnInsertBefore()
{
	HTREEITEM hItem;
	hItem = m_wndPlanCtrl.GetSelectedItem();


	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	
	ResidentVehicleRoute* pParentRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hParentItem);
	if (pParentRoute == NULL)
		return;
	
	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	ResidentVehicleRoute vehicleRoute;
	if (m_pPickLayoutObject)
	{
		LandsideFacilityObject::FacilityType emType = LandsideFacilityObject::convertLandObjectType(m_pPickLayoutObject->GetType());
		vehicleRoute.GetFacilityObject()->SetFacilityID(m_pPickLayoutObject->getID());
		vehicleRoute.GetFacilityObject()->SetType(emType);
	}
	else
	{
		HTREEITEM hDragItem;
		hDragItem = m_wndFacilityCtrl.GetSelectedItem();

		if (hDragItem == NULL)
			return;
		vehicleRoute = CreateVehicleRoute(hDragItem);
	}

	LandsideFacilityObject sourceObject = *pParentRoute->GetFacilityObject();
	LandsideFacilityObject destObject = *pVehicleRoute->GetFacilityObject();
	pVehicleRouteFlow->InsertObjectBefore(sourceObject,destObject,vehicleRoute);
	m_pPickLayoutObject = NULL;

	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnInsertAfter()
{
	HTREEITEM hItem;
	hItem = m_wndPlanCtrl.GetSelectedItem();
	
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	ResidentVehicleRoute vehicleRoute;
	if (m_pPickLayoutObject)
	{
		LandsideFacilityObject::FacilityType emType = LandsideFacilityObject::convertLandObjectType(m_pPickLayoutObject->GetType());
		vehicleRoute.GetFacilityObject()->SetFacilityID(m_pPickLayoutObject->getID());
		vehicleRoute.GetFacilityObject()->SetType(emType);
	}
	else
	{
		HTREEITEM hDragItem;
		hDragItem = m_wndFacilityCtrl.GetSelectedItem();

		if (hDragItem == NULL)
			return;
		vehicleRoute = CreateVehicleRoute(hDragItem);
	}

	LandsideFacilityObject sourceObject = *pVehicleRoute->GetFacilityObject();
	LandsideFacilityObject destObject = *vehicleRoute.GetFacilityObject();
	pVehicleRouteFlow->InsertObjectAfter(sourceObject,destObject);
	m_pPickLayoutObject = NULL;


	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnPruneBrance()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;

	ResidentVehicleRoute* pParentRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hParentItem);
	if (pParentRoute == NULL)
		return;


	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	LandsideFacilityObject sourceObject = *pParentRoute->GetFacilityObject();
	LandsideFacilityObject destObject = *pVehicleRoute->GetFacilityObject();
	pVehicleRouteFlow->PruneBranch(sourceObject,destObject);
	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnRemoveFromBranch()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;

	ResidentVehicleRoute* pParentRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hParentItem);
	if (pParentRoute == NULL)
		return;
	
	
	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	LandsideFacilityObject sourceObject = *pParentRoute->GetFacilityObject();
	LandsideFacilityObject destObject = *pVehicleRoute->GetFacilityObject();
	pVehicleRouteFlow->RemoveObjectBranch(sourceObject,destObject);


	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnRemoveFromAllBranch()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	LandsideFacilityObject sourceObject = *pVehicleRoute->GetFacilityObject();
	pVehicleRouteFlow->RemoveObjectAllBranch(sourceObject);


	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnModifyPercent()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	m_wndPlanCtrl.DoEdit(hItem);
}

void CDlgBaseResident::OnEvenPercent()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	ResidentVehicleRoute* pParentVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hParentItem);
	if (pParentVehicleRoute == NULL)
		return;
	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	if (pVehicleRoute == NULL)
		return;

	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	ResidentVehicleRouteList* pRouteList = pVehicleRouteFlow->GetDestVehicleRoute(*pParentVehicleRoute->GetFacilityObject());
	if (pRouteList == NULL)
		return;
	
	pRouteList->EventPercent(pVehicleRoute);
	AdjustPlanTreeItemPercent(hParentItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnPickFromMap()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	CMDIChildWnd* pWnd = pDoc->GetMainFrame()->CreateOrActiveRender3DView();
	if(!pWnd)
		return;

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)PICK_ALTOBJECT ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CDlgBaseResident::OnContextMenu( CWnd* pWnd, CPoint point )
{
	//check facility single object whether been selected
	//if (FacilitySelect() == false)
	//	return;
	
	CRect planRect;
	m_wndPlanCtrl.GetWindowRect(&planRect);
	if (!planRect.PtInRect(point))
		return;
	
	m_wndPlanCtrl.SetFocus();

	CPoint pt = point;
	m_wndPlanCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_wndPlanCtrl.HitTest(pt, &iRet);
	if( hItem == NULL )
		return;

	if (iRet != TVHT_ONITEMLABEL)
		return;

	m_wndPlanCtrl.SelectItem(hItem);
	if(std::find(m_vRoute.begin(),m_vRoute.end(),hItem) != m_vRoute.end())
		PopupRouteMenu(point);
}

bool CDlgBaseResident::FacilitySelect() const
{
	HTREEITEM hItem = m_wndFacilityCtrl.GetSelectedItem();
	if (hItem == NULL)
		return false;
	
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndFacilityCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return false;
	
	if (pNodeData->m_emTye == TreeNodeData::Group_Facility)
		return false;

	return true;
}

void CDlgBaseResident::OnCopyFacility()
{
	m_vCopyProcessors.clear();

	std::deque<TreeNodeInfor> vDeque = m_vCopyItem;
	while (!vDeque.empty())
	{
		TreeNodeInfor nodeInfor = vDeque.front();
		ResidentVehicleRoute* pNodeData = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData( nodeInfor.m_hTreeNode); 
		if (pNodeData == NULL)
			return;

		vDeque.pop_front();
		HTREEITEM hParentITem = m_wndPlanCtrl.GetParentItem(nodeInfor.m_hTreeNode);
		if (hParentITem == NULL)
			return;

		ResidentVehicleRoute* pParentNodeData = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData( hParentITem);
		if (pParentNodeData == NULL)
			return;

		TreeNodeInfor parentInfor;
		parentInfor.m_hTreeNode = hParentITem;
		if (std::find(vDeque.begin(),vDeque.end(),parentInfor) != vDeque.end())
		{
			vDeque.push_back(nodeInfor);
		}
		else
		{
			if (!m_vCopyProcessors.empty())
			{
				ResidentVehicleRoute vehicleRoute = m_vCopyProcessors.back();
				LandsideFacilityObject facilityObject = *(vehicleRoute.GetFacilityObject());
				if (pParentNodeData->GetFacilityObject()->GetFacilityID() != facilityObject.GetFacilityID() )
				{
					m_vCopyProcessors.clear();
					MessageBox( "Sequence facility is allowed in this case", "Error", MB_OK|MB_ICONWARNING );
					return ;
				}
			}
			ResidentVehicleRoute copyRoute;
			copyRoute.InitVehicleRoute(*pNodeData);
			m_vCopyProcessors.push_back(copyRoute);
		}
	}

	m_vCopyItem.clear();
}

void CDlgBaseResident::OnAddCopyFacility()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	LandsideFacilityObject preFacility;
	preFacility = *pVehicleRoute->GetFacilityObject();

	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;
	
	for (unsigned i = 0; i < m_vCopyProcessors.size(); i++)
	{
		LandsideFacilityObject facilityObject = *(m_vCopyProcessors[i].GetFacilityObject());
		pVehicleRouteFlow->AddDestinationObject(preFacility,m_vCopyProcessors[i]);

		ResidentVehicleRouteList* pRouteList = pVehicleRouteFlow->GetDestVehicleRoute(preFacility);
		if (pRouteList)
		{
			pRouteList->EventPercent(&m_vCopyProcessors[i]);
		}

		preFacility = facilityObject;
	}
	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBaseResident::OnInsertCopyFacility()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
	LandsideFacilityObject preFacility;
	preFacility = *pVehicleRoute->GetFacilityObject();

	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	for (unsigned i = 0; i < m_vCopyProcessors.size(); i++)
	{
		LandsideFacilityObject facilityObject = *(m_vCopyProcessors[i].GetFacilityObject());

		pVehicleRouteFlow->InsertObjectAfter(preFacility,facilityObject);
		
		SetCopyFlowDestion(pVehicleRouteFlow,preFacility,m_vCopyProcessors[i]);
		preFacility = facilityObject;
	}
	HTREEITEM hRouteRoot;
	if (FindRouteRoot(hItem,hRouteRoot))
	{
		ReloadVehicleRoute(hRouteRoot);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

bool CDlgBaseResident::FindTimeRangeItem( HTREEITEM hItem, HTREEITEM& hTimeRangeItem )
{
	while(hItem)
	{
		std::vector<HTREEITEM>::iterator iter = std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem); 
		if (iter != m_vTimeRange.end())
		{
			hTimeRangeItem = *iter;
			return true;
		}
		hItem = m_wndPlanCtrl.GetParentItem(hItem);
	}
	return false;
}

bool CDlgBaseResident::FindRouteRoot(HTREEITEM hItem, HTREEITEM& hRootItem)
{
	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	while(hParentItem)
	{
		std::vector<HTREEITEM>::iterator iter = std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hParentItem); 
		if (iter != m_vTimeRange.end())
		{
			hRootItem = hItem;
			return true;
		}
		hItem = hParentItem;
		hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	}
	return false;
}

ResidentVehicleRoute CDlgBaseResident::CreateVehicleRoute( HTREEITEM hItem )
{
	ResidentVehicleRoute vehicleRoute;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndFacilityCtrl.GetItemData(hItem);
	LandsideFacilityLayoutObject* pLandsideObject = pNodeData->m_pLandsideObject;

	LandsideFacilityObject::FacilityType emType = LandsideFacilityObject::convertLandObjectType(pLandsideObject->GetType());
	vehicleRoute.GetFacilityObject()->SetFacilityID(pLandsideObject->getID());
	vehicleRoute.GetFacilityObject()->SetType(emType);

	return vehicleRoute;
}

void CDlgBaseResident::ReloadVehicleRoute(HTREEITEM hRouteRoot)
{
	HTREEITEM hTimeRange = m_wndPlanCtrl.GetParentItem(hRouteRoot);
	if (hTimeRange == NULL)
		return;
	
	if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hTimeRange) == m_vTimeRange.end())
		return;
	
	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow(hTimeRange);

	DeleteRouteItem(hRouteRoot);
	m_wndPlanCtrl.DeleteItem(hRouteRoot);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	//route
	HTREEITEM hRouteItem = m_wndPlanCtrl.InsertItem(_T("Route"),cni,FALSE,FALSE,hTimeRange);
	m_vRoute.push_back(hRouteItem);
	{
		ResidentVehicleRouteList* pStartRouteList = pVehicleRouteFlow->GetStartRoute();
		m_wndPlanCtrl.SetItemData(hRouteItem,(DWORD)&m_startRoute);
		if(pStartRouteList)
		{
			for (int i = 0; i < pStartRouteList->GetDestCount(); i++)
			{
				ResidentVehicleRoute* pVehicleRoute = pStartRouteList->GetDestObjectInfo(i);
				InsertVehicleRouteItem(hRouteItem,pVehicleRoute,pVehicleRouteFlow);
			}
		}
	}
}

ResidentVehicleRouteFlow* CDlgBaseResident::GetVehicleRouteFlow()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return NULL;

	HTREEITEM hTimeRangeItem;
	if (FindTimeRangeItem(hItem,hTimeRangeItem))
	{
		return GetVehicleRouteFlow(hTimeRangeItem);
	}
	return NULL;
}

void CDlgBaseResident::OnClickTreeProcess( NMHDR *pNMHDR, LRESULT *pResult )
{
	CPoint pt = GetMessagePos();
	m_wndPlanCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_wndPlanCtrl.HitTest(pt, &iRet);
	if( hItem == NULL )
		return;

	if (hItem == m_wndPlanCtrl.GetRootItem())
		return;

	if (std::find(m_vRoute.begin(),m_vRoute.end(),hItem) == m_vRoute.end())
		return;
	
	ResidentVehicleRouteFlow* pVehicleRouteFlow = GetVehicleRouteFlow();
	CWaitCursor	wCursor;
	if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
	{
		COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(hItem);
		if (pCNI)
		{
			if (!m_vCopyItem.empty())
			{
				TreeNodeInfor backInfo = m_vCopyItem.back();
				if (backInfo.m_pVehicleRouteFlow != pVehicleRouteFlow)
				{
					return;
				}
			}
			TreeNodeInfor nodeInfor;
			nodeInfor.m_hTreeNode = hItem;
			nodeInfor.m_color = pCNI->clrItem;
			nodeInfor.m_bBold = FALSE;
			nodeInfor.m_pVehicleRouteFlow = pVehicleRouteFlow;
			pCNI->clrItem = RGB(128,0,0);
			pCNI->lfontItem.lfWeight = FW_BOLD;
			if (std::find(m_vCopyItem.begin(),m_vCopyItem.end(),nodeInfor) == m_vCopyItem.end())
			{
				m_vCopyItem.push_back(nodeInfor);
			}
		}
		
	}
	else
	{
		for (size_t i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor nodeInfor = m_vCopyItem.at(i);
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(nodeInfor.m_hTreeNode);
			if (pCNI)
			{
				pCNI->clrItem = nodeInfor.m_color;
				pCNI->lfontItem.lfWeight = FW_NORMAL;
			}
		}
		m_vCopyItem.clear();
	}

	m_wndPlanCtrl.Invalidate(FALSE);
}

LRESULT CDlgBaseResident::OnTempFallbackFinished( WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	QueryData& qdata = *((QueryData*)wParam);
	LandsideFacilityLayoutObject* pObject = NULL;
	if (qdata.GetData(KeyAltObjectPointer,(int&)pObject))
	{
		if (TypeNeedToShow(pObject->GetType()))
		{
			m_pPickLayoutObject = pObject;
		}
	}
	return true;
}

void CDlgBaseResident::HideDialog( CWnd* parentWnd )
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgBaseResident::ShowDialog( CWnd* parentWnd )
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

void CDlgBaseResident::PopupRouteMenu( CPoint point )
{
	CMenu menuPopup;
	menuPopup.CreatePopupMenu();

	menuPopup.AppendMenu(MF_POPUP,ID_ADD_DEST,_T("Add Dest Facility"));
	menuPopup.AppendMenu(MF_POPUP,ID_INSERT_BEFORE,_T("Insert Facility Before"));
	menuPopup.AppendMenu(MF_POPUP,ID_INSERT_AFTER,_T("Insert Facility After"));

	menuPopup.AppendMenu(MF_SEPARATOR);

	menuPopup.AppendMenu(MF_POPUP,ID_PRUNE_BRANCH,_T("Prune The Branch"));
	menuPopup.AppendMenu(MF_POPUP,ID_REMOVE_BRANCH,_T("Remove Facility From This Branch"));
	menuPopup.AppendMenu(MF_POPUP,ID_REMOVE_ALLBRANCH,_T("Remove Facility From All Braches"));

	menuPopup.AppendMenu(MF_SEPARATOR);

	menuPopup.AppendMenu(MF_POPUP,ID_FACILITY_COPY,_T("Copy Facility"));
	menuPopup.AppendMenu(MF_POPUP,ID_FACILITY_ADDCOPY,_T("Add Copy Facility"));
	menuPopup.AppendMenu(MF_POPUP,ID_FACILITY_INSERTCOPY,_T("Insert Copy Facility"));

	menuPopup.EnableMenuItem(ID_FACILITY_COPY,MF_GRAYED);
	menuPopup.EnableMenuItem(ID_FACILITY_ADDCOPY,MF_GRAYED);
	menuPopup.EnableMenuItem(ID_FACILITY_INSERTCOPY,MF_GRAYED);

	int nSelType = PopChildMenu();
	HTREEITEM hRightSelect = m_wndPlanCtrl.GetSelectedItem();
	if (FacilitySelect() || m_pPickLayoutObject)
	{
		if (nSelType == 0)
		{
			menuPopup.EnableMenuItem(ID_ADD_DEST,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_INSERT_BEFORE,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_INSERT_AFTER,MF_GRAYED);
		}
		else if (nSelType == 1)
		{
			menuPopup.EnableMenuItem(ID_ADD_DEST,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_INSERT_BEFORE,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_INSERT_AFTER,MF_ENABLED);
		}
		else if (nSelType == 2)
		{
			menuPopup.EnableMenuItem(ID_ADD_DEST,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_INSERT_BEFORE,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_INSERT_AFTER,MF_ENABLED);
		}
	}
	else
	{
		menuPopup.EnableMenuItem(ID_ADD_DEST,MF_GRAYED);
		menuPopup.EnableMenuItem(ID_INSERT_BEFORE,MF_GRAYED);
		menuPopup.EnableMenuItem(ID_INSERT_AFTER,MF_GRAYED);
	}

	if (hRightSelect)
	{
		if (nSelType == 0)
		{
			menuPopup.EnableMenuItem(ID_PRUNE_BRANCH,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_REMOVE_BRANCH,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_REMOVE_ALLBRANCH,MF_GRAYED);
		}
		else if (nSelType == 1)
		{
			menuPopup.EnableMenuItem(ID_PRUNE_BRANCH,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_REMOVE_BRANCH,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_REMOVE_ALLBRANCH,MF_GRAYED);
		}
		else if (nSelType == 2)
		{
			menuPopup.EnableMenuItem(ID_PRUNE_BRANCH,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_REMOVE_BRANCH,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_REMOVE_ALLBRANCH,MF_ENABLED);
			ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hRightSelect);
			if (pVehicleRoute)
			{
				HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hRightSelect);
				if(IfHaveCircle(hParentItem,*pVehicleRoute->GetFacilityObject()))
				{
					menuPopup.EnableMenuItem(ID_REMOVE_BRANCH,MF_GRAYED);
					menuPopup.EnableMenuItem(ID_REMOVE_ALLBRANCH,MF_GRAYED);
				}
			}
		}
		
	}
	else
	{
		menuPopup.EnableMenuItem(ID_PRUNE_BRANCH,MF_GRAYED);
		menuPopup.EnableMenuItem(ID_REMOVE_BRANCH,MF_GRAYED);
		menuPopup.EnableMenuItem(ID_REMOVE_ALLBRANCH,MF_GRAYED);
	}

	
	if (!m_vCopyItem.empty())
	{
		if (nSelType == 0)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_COPY,MF_GRAYED);
		}
		else if (nSelType == 1)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_COPY,MF_GRAYED);
		}
		else if (nSelType == 2)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_COPY,MF_ENABLED);
		}

	}
	if (!m_vCopyProcessors.empty())
	{
		if (nSelType == 0)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_ADDCOPY,MF_GRAYED);
			menuPopup.EnableMenuItem(ID_FACILITY_INSERTCOPY,MF_GRAYED);
		}
		else if (nSelType == 1)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_ADDCOPY,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_FACILITY_INSERTCOPY,MF_ENABLED);
		}
		else if (nSelType == 2)
		{
			menuPopup.EnableMenuItem(ID_FACILITY_ADDCOPY,MF_ENABLED);
			menuPopup.EnableMenuItem(ID_FACILITY_INSERTCOPY,MF_ENABLED);
		}
	}
	
	
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	
}

void CDlgBaseResident::SetCopyFlowDestion(ResidentVehicleRouteFlow* pVehicleRouteFlow, const LandsideFacilityObject& facilityObject, ResidentVehicleRoute vehicleRoute )
{
	if (pVehicleRouteFlow == NULL)
		return;	

	ResidentVehicleRouteList* pRouteList = pVehicleRouteFlow->GetDestVehicleRoute(facilityObject);
	if (pRouteList == NULL)
		return;

	LandsideFacilityObject desObject = *(vehicleRoute.GetFacilityObject());
	ResidentVehicleRoute destRoute;
	if(pRouteList->GetDestObjectInfo(desObject,destRoute))
	{
		pRouteList->AddDestObject(vehicleRoute);
	}
}

void CDlgBaseResident::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

bool CDlgBaseResident::IfHaveCircle( HTREEITEM _testItem,const LandsideFacilityObject& facilityObject )
{
	if (_testItem == NULL)
		return false;
	
	if (std::find(m_vRoute.begin(),m_vRoute.end(),_testItem) == m_vRoute.end())
		return false;

	ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(_testItem);
	if (pVehicleRoute == NULL)
		return false;
	
	if (*pVehicleRoute->GetFacilityObject() == facilityObject)
		return true;
	
	HTREEITEM hFather = m_wndPlanCtrl.GetParentItem( _testItem );

	return IfHaveCircle( hFather, facilityObject );
}

void CDlgBaseResident::LayoutFigure()
{

}
