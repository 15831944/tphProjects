// AirsideObjectBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AirsideObjectBaseDlg.h"
#include "Common\WinMsg.h"
#include "DlgInitArp.h"
#include "DlgPositionEdit.h"
#include "3DView.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "AirsideRunwayDlg.h"
#include "AirsideHeliportDlg.h"
#include "AirsideTaxiwayDlg.h"
#include "AirsideGateDlg.h"
#include "AirsideDeicePadDlg.h"
#include "AirsideWaypointDlg.h"
#include "AirsideContourDlg.h"
#include "AirsideHoldDlg.h"
#include "AirsideSurfaceDlg.h"
#include "AirsideStretchDlg.h"
#include "AirsideIntersectionsDlg.h"
#include "AirsideRoundaboutDlg.h"
#include "AirsideTurnoffDlg.h"
#include "AirsideLaneAdapterDlg.h"
#include "AirsideLineParkingDlg.h"
#include "AirsideNoseInParkingDlg.h"
#include "AirsideTollGateDlg.h"
#include "AirsideStopSignDlg.h"
#include "AirsideYieldSignDlg.h"
#include "AirsideVehiclePoolParkingDlg.h"
#include "AirsideParkingPlaceDlg.h"
#include "AirsideDriveRoadDlg.h"
#include "DlgReportingArea.h"
#include "StartPositionsDlg.h"
#include "MeetingPointDlg.h"
#include "AirsideSectorDlg.h"
#include "AirsideStructureDlg.h"
#include "AirsideTrafficLightDlg.h"
#include "ChildFrm.h"
#include "Airside3D.h"
#include "ALTObject3D.h"
#include "Common\ViewMsg.h"













#include "AirsideBagCartParkPosDlg.h"
#include "AirsidePaxBusParkingPosDlg.h"
IMPLEMENT_DYNAMIC(CAirsideObjectBaseDlg, CXTResizeDialog)
CAirsideObjectBaseDlg::CAirsideObjectBaseDlg(int nObjID,int nAirportID,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CAirsideObjectBaseDlg::IDD, pParent)
{
	m_nProjID = nProjID;
	m_nObjID = nObjID;
	m_pObject = NULL;
	m_hRClickItem = NULL;
	m_nAirportID = nAirportID;
	m_bPropModified = false;
	m_bNameModified = false;
	m_bPathModified = false;
	m_bTypeModify = false;
}

//CAirsideObjectBaseDlg::CAirsideObjectBaseDlg( ALTObject * pObj,int nProjID, CWnd * pParent /*= NULL*/ ): CXTResizeDialog(CAirsideObjectBaseDlg::IDD, pParent)
//{
//	ASSERT(pObj);
//	
//	m_nProjID = nProjID;
//	m_nObjID = pObj->getID();
//	m_pObject = pObj;
//	m_hRClickItem = NULL;
//	m_nAirportID = pObj->getAptID();
//	m_bPropModified = false;
//	m_bNameModified = false;
//	UpdateTempObjectInfo();
//}

CAirsideObjectBaseDlg::~CAirsideObjectBaseDlg()
{
}

void CAirsideObjectBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeProp);
	DDX_Control(pDX, IDC_OBJECT_IMAGE, m_ImageStatic);
	DDX_Control(pDX, IDC_CHECK_LOCKED, m_btnCheckLocked);
}


BEGIN_MESSAGE_MAP(CAirsideObjectBaseDlg, CXTResizeDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnPropTreeDoubleClick)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
	ON_BN_CLICKED(IDC_CHECK_LOCKED, OnBnClickedCheckLocked)
END_MESSAGE_MAP()


// CAirsideObjectBaseDlg message handlers
bool CAirsideObjectBaseDlg::OnPropModifyLatLong(CLatitude& lat, CLongitude& longt)
{
	CDlgInitArp initArp;
	lat.GetValue(initArp.m_strLatitude);
	longt.GetValue(initArp.m_strLongitude);

	if (initArp.DoModal()==IDOK)
	{
		lat.SetValue(initArp.m_strLatitude);
		longt.SetValue(initArp.m_strLongitude);		
		return true;
	}
	return false;
}

bool CAirsideObjectBaseDlg::OnPropModifyPosition(CPoint2008& pt,ARCUnit_Length emType)
{
	CDlgPositionEdit positionEdit(emType,pt);

	if (positionEdit.DoModal() == IDOK)
	{
		pt = positionEdit.GetPoint();
		return true;
	}
	return false;
}

void CAirsideObjectBaseDlg::OnCbnKillfocusComboNameLevel1()
{
	// TODO: Add your control notification handler code here
}

void CAirsideObjectBaseDlg::OnCbnDropdownComboNameLevel1()
{

}

void CAirsideObjectBaseDlg::OnCbnDropdownComboNameLevel2()
{	
	m_cmbName2.ResetContent();
	SortedStringList levelName;	

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	m_lstExistObjectName.GetLevel2StringList(csLevel1,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName2.AddString(*iter);
	}
}

void CAirsideObjectBaseDlg::OnCbnDropdownComboNameLevel3()
{
	m_cmbName3.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;	
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel3StringList(csLevel1,csLevel2,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName3.AddString(*iter);
	}
}

void CAirsideObjectBaseDlg::OnCbnDropdownComboNameLevel4()
{
	m_cmbName4.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_cmbName3.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel4StringList(csLevel1,csLevel2,csLevel3,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName4.AddString(*iter);
	}
}

BOOL CAirsideObjectBaseDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	try
	{
		m_pObject->ReadObject(m_nObjID);
		
		ALTObject::GetObjectNameList(m_pObject->GetType(),m_nAirportID,m_lstExistObjectName);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the Object."));
	}
	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();
	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_HIDE);

	if (m_pObject != NULL)
	{
		ALTObjectID objName;
		m_pObject->getObjName(objName);
		m_cmbName1.SetWindowText(objName.at(0).c_str());
		m_cmbName2.SetWindowText(objName.at(1).c_str());
		m_cmbName3.SetWindowText(objName.at(2).c_str());
		m_cmbName4.SetWindowText(objName.at(3).c_str());
	}	
	InitComboBox();
	m_btnCheckLocked.SetCheck(m_pObject->GetLocked());

	SetResize(IDC_STATIC, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL4, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROPER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROP, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_PROP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_PROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_OBJECT_IMAGE, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	SetResize(IDC_CHECK_LOCKED, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	CRect rect;
	GetClientRect(rect);

	AdjustCombox(rect.right - rect.left);	

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideObjectBaseDlg::InitComboBox()
{
	//level 1
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel1StringList(levelName);
	
	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName1.AddString(*iter);
	}
	levelName.clear();
}

LRESULT CAirsideObjectBaseDlg::OnPropTreeDoubleClick(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hTreeItem = (HTREEITEM)lParam;
	OnDoubleClickPropTree(hTreeItem);
	return 0;
}

void CAirsideObjectBaseDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	
}

void CAirsideObjectBaseDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	m_hRClickItem = NULL;

	CRect rectTree;
	m_treeProp.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeProp.SetFocus();
	CPoint pt = point;
	m_treeProp.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeProp.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	if (m_hRClickItem==NULL)
		return;

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;

	OnContextMenu(menuPopup,pMenu);
	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}
void CAirsideObjectBaseDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{


}
LRESULT CAirsideObjectBaseDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CAirsideObjectBaseDlg::OnProcpropPickfrommap() 
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

	enum FallbackReason enumReason = GetFallBackReason();


//	enumReason= PICK_ONEPOINT;
	
	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CAirsideObjectBaseDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CAirsideObjectBaseDlg::ShowDialog(CWnd* parentWnd)
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
LRESULT CAirsideObjectBaseDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	DoTempFallBackFinished(wParam,lParam);
	UpdateTempObjectInfo();
	return true;
}
void CAirsideObjectBaseDlg::OnProcpropDelete() 
{
}

void CAirsideObjectBaseDlg::OnOK()
{
//	ALTOBJECT_NAME objName;
	CString strName;
	m_cmbName1.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("please input object name.");
		return ;
	}
	m_objName.m_val[0] = strName.MakeUpper();

	m_cmbName2.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[1] = strName.MakeUpper();

	m_cmbName3.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[2] = strName.MakeUpper();

	m_cmbName4.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[3] = strName.MakeUpper();

	

	if (m_nObjID != -1)
	{	
		ALTObjectID oldName;
		m_pObject->getObjName(oldName);
		if(m_objName.m_val != oldName.m_val)
		{

			
			if(!ALTObject::CheckObjNameIsValid(m_objName,GetAirportID(),m_nObjID))
			{
				MessageBox("The ObjName is not valid,please re-input.");
				return ;
			}
			m_bNameModified = true;
			m_pObject->setObjName(m_objName);
		}

		try
		{	
			CADODatabase::BeginTransaction();
			if(!UpdateOtherData())
			{
				CADODatabase::RollBackTransation();
				return;
			}
			if (m_bPropModified || m_bNameModified)
			{
				m_pObject->UpdateObject(m_nObjID);
			}
			CADODatabase::CommitTransaction();
		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
			MessageBox(_T("Cann't update the Object."));
			return;
		}

	}
	else
	{	
		if(!ALTObject::CheckObjNameIsValid(m_objName,GetAirportID()))
		{
			MessageBox("The ObjName is not valid,please re-input.");
			return ;
		}		
		m_pObject->setObjName(m_objName);

		try
		{
			CADODatabase::BeginTransaction();	
			if(!UpdateOtherData())
			{
				CADODatabase::RollBackTransation();
				return;
			}
			m_pObject->SaveObject(GetAirportID());
			CADODatabase::CommitTransaction();	
		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
			MessageBox(_T("Cann't save the Object."));
			return;
		}
	}
	DoneEditTempObject();
	CXTResizeDialog::OnOK();
}

CAirsideObjectBaseDlg * CAirsideObjectBaseDlg::NewObjectDlg(int nObjID,ALTOBJECT_TYPE objType, int nParentID,int nAirportID,int nProjID, CWnd *pParent /* = NULL */)
{
	CAirsideObjectBaseDlg *pDlg = NULL;
	switch (objType)
	{
	case ALT_UNKNOWN:
		//		enumProcType = ArpProcessor;
		break;
	case ALT_RUNWAY:
		{
			pDlg = new CAirsideRunwayDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_HELIPORT:
		{
			pDlg = new CAirsideHeliportDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_TAXIWAY:
		{
			pDlg = new CAirsideTaxiwayDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_STAND:
		{
			pDlg = new CAirsideGateDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_DEICEBAY:
		{
			pDlg = new CAirsideDeicePadDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_GROUNDROUTE:
		break;
	case ALT_WAYPOINT:
		{
			pDlg = new CAirsideWaypointDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_CONTOUR:
		{
			pDlg = new CAirsideContourDlg(nObjID,nAirportID, nParentID, nProjID,pParent);
		}
		break;
	case ALT_HOLD:
		{
			pDlg = new CAirsideHoldDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_SECTOR:
		{
			pDlg = new CAirsideSectorDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;

	case ALT_SURFACE:
		{
			pDlg = new CAirsideSurfaceDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;

	case ALT_STRUCTURE:
		{
			pDlg = new CAirsideStructureDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_STRETCH:
	case ALT_CIRCLESTRETCH:
		{
			pDlg = new CAirsideStretchDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_INTERSECTIONS:
		{
			pDlg = new CAirsideIntersectionsDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_ROUNDABOUT:
		{
			pDlg = new CAirsideRoundaboutDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_TURNOFF:
		{
			pDlg = new CAirsideTurnoffDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_LANEADAPTER:
		{
			pDlg = new CAirsideLaneAdapterDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_LINEPARKING:
		{
			pDlg = new CAirsideLineParkingDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_NOSEINPARKING:
		{
			pDlg = new CAirsideNoseInParkingDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_TRAFFICLIGHT:
		{
			pDlg = new CAirsideTrafficLightDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_TOLLGATE:
		{
			pDlg = new CAirsideTollGateDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_STOPSIGN:
		{
			pDlg = new CAirsideStopSignDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_YIELDSIGN:
		{
			pDlg = new CAirsideYieldSignDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_VEHICLEPOOLPARKING:
		{
			pDlg = new CAirsideVehiclePoolParkingDlg(nObjID,nAirportID,nProjID,pParent);
		}
		break;
	case ALT_PARKINGPLACE:
		{
			pDlg = new AirsideParkingPlaceDlg(nObjID, nParentID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_DRIVEROAD:
		{
			pDlg = new AirsideDriveRoadDlg(nObjID,nParentID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_REPORTINGAREA:
		{
			pDlg = new DlgReportingArea(nObjID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_STARTPOSITION:
		{
			pDlg = new CStartPositionDlg(nObjID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_MEETINGPOINT:
		{
			pDlg = new CMeetingPointDlg(nObjID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_ABAGCARTSPOT:
		{
			pDlg = new CAirsideBagCartParkingPosDlg(nObjID, nAirportID,nProjID,pParent);
		}
		break;
	case ALT_APAXBUSSPOT:
		{
			pDlg = new CAirsidePaxBusParkingPosDlg(nObjID, nAirportID,nProjID,pParent);
		}
		break;
	default:
		break;
	}
	return pDlg;
}


void CAirsideObjectBaseDlg::UpdateTempObjectInfo()
{
	CAirside3D * pAirside3D = NULL;
	
	if(GetDocument() && GetDocument()->Get3DViewParent() )
	{
		pAirside3D = GetDocument()->Get3DViewParent()->GetAirside3D();
	}
	if(pAirside3D)
	{
		ALTObject3D * pObj3D = pAirside3D->GetObject3D(m_pObject->getID());
		if(pObj3D)
		{
			pObj3D->DoSync();
		}
		else
		{
			pObj3D = pAirside3D->AddObject(GetObject());
			if (pObj3D)
			{
				pObj3D->DoSync();
			}
		}
		//if(pObj3D)
		//	pObj3D->SetInEdit(false);
	}

}

CTermPlanDoc * CAirsideObjectBaseDlg::GetDocument()
{
	return (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
}

void CAirsideObjectBaseDlg::DoneEditTempObject()
{
	GetDocument()->SetTempObject(NULL);
}

void CAirsideObjectBaseDlg::OnCancel()
{
// 	try
// 	{
// 		m_pObject->ReadObject(m_nObjID);
// 		UpdateObjectViews();
// 	}
// 	catch (CADOException& /*e*/)
// 	{
// 	}

	DoneEditTempObject();
	CXTResizeDialog::OnCancel();
}

void CAirsideObjectBaseDlg::AdjustCombox(int nWidth)
{
	const int nIconWidth = 80;

	int iLevelWidth = (nWidth- 2*15 - 3*9 - nIconWidth)/4;

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL1))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName1.MoveWindow(rect.left,25,iLevelWidth,12);
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE1))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE1)->MoveWindow( rect.left+iLevelWidth+2,32,5,3 );
	}

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL2))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName2.MoveWindow(rect.left+iLevelWidth+9,25,iLevelWidth,12 );
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE2))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE2)->MoveWindow( rect.left+2*iLevelWidth+11,32,5,3 );
	}

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL3))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName3.MoveWindow(rect.left+2*iLevelWidth+18, 25, iLevelWidth,12 );
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE3))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE3)->MoveWindow( rect.left+3*iLevelWidth+20,32,5,3 );
	}	

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL4))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName4.MoveWindow(rect.left+3*iLevelWidth+27, 25, iLevelWidth,12 );
	}
}

void CAirsideObjectBaseDlg::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	AdjustCombox(cx);	
}

bool CAirsideObjectBaseDlg::DoTempFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();

	CPath2008 path;
	CPoint2008 *pointList = new CPoint2008[nSize];
	for (size_t i =0; i < nSize; ++i)
	{
		ARCVector3 v3 = pData->at(i);
		pointList[i].setPoint(v3[VX] ,v3[VY],0.0);
	}
	path.init(nSize,pointList);
	SetObjectPath(path);	
	delete []pointList;

	return true;
}

void CAirsideObjectBaseDlg::UpdateObjectViews()
{
	GetDocument()->UpdateAllViews(NULL,VM_MODIFY_ALTOBJECT, (CObject*)GetObject());
}

void CAirsideObjectBaseDlg::UpdateAddandRemoveObject()
{
	GetDocument()->UpdateAllViews(NULL,VM_NEW_SINGLE_ALTOBJECT, (CObject*)GetObject());
}

void CAirsideObjectBaseDlg::UpdateNewViews()
{
	GetDocument()->UpdateAllViews(NULL,NULL,NULL);
}

void CAirsideObjectBaseDlg::OnBnClickedCheckLocked()
{
	// TODO: Add your control notification handler code here
	m_pObject->SetLocked(BST_CHECKED == m_btnCheckLocked.GetCheck());
	m_bPropModified = TRUE;
}
