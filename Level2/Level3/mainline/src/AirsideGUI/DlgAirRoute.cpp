// DlgAirRoute.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "DlgAirRoute.h"
#include ".\dlgairroute.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/AirWayPoint.h"
#include "DlgAirRouteWayPointDefine.h"
#include "../inputairside/Runway.h"
#include "../InputAirside/AirRouteIntersection.h"
#include "DlgSelectRunway.h"
#include <Common/ARCMathCommon.h>

// CDlgAirRoute dialog
using namespace AirsideGUI;

const UINT ID_NEW_RUNWAYPORT  = 20;
const UINT ID_DEL_RUNWAYPORT  = 21;

IMPLEMENT_DYNAMIC(CDlgAirRoute, CXTResizeDialog)
CDlgAirRoute::CDlgAirRoute(BOOL bEditAirRoute,int nAirspaceID,int nAirRouteID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAirRoute::IDD, pParent)
	,m_strAirRouteName(_T(""))
{
	m_nAirspaceID = nAirspaceID;
	m_nAirRouteID = nAirRouteID;
	m_bEidtAirRoute = bEditAirRoute;
	m_pCurAirRoute = NULL;
	m_vConnectRunway.clear();
}

CDlgAirRoute::~CDlgAirRoute()
{
}

void CDlgAirRoute::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_LIST_ARWAYPOINT,m_lstWaypoint);
	DDX_Control(pDX, IDC_LIST_RUNWAYPORT, m_lstRunway);
	DDX_Control(pDX,IDC_COMBO_TYPE,m_cmbType);
	//DDX_Control(pDX,IDC_COMBO_RUNWAY,m_cmbRunway);
	//DDX_Control(pDX,IDC_COMBO_RUNWAYMARK,m_cmbRunwayMark);
	DDX_Text(pDX,IDC_EDIT_AIRROUTE_NAME,m_strAirRouteName);
	CXTResizeDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAirRoute, CXTResizeDialog)
	ON_WM_CREATE()

	ON_COMMAND(ID_NEW_ARCWAYPOINT,OnNewARWaypoint)
	ON_COMMAND(ID_EDIT_ARWAYPOINT,OnEditARWaypoint)
	ON_COMMAND(ID_DEL_ARWAYPOINT,OnDelARWaypoint)
	ON_COMMAND(ID_INSERT_ARWAYPOINT,OnInsertARWaypoint)	

	ON_COMMAND(ID_NEW_RUNWAYPORT, OnNewRunway)
	ON_COMMAND(ID_DEL_RUNWAYPORT, OnDelRunway)

	ON_MESSAGE(WM_LBITEMCHANGED,OnMessageNewAirRoute)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText )
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	//ON_CBN_SELCHANGE(IDC_COMBO_RUNWAY, OnCbnSelchangeComboRunway)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ARWAYPOINT, OnLvnItemchangedARWayPoint)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ARWAYPOINT, OnNMDblclkARWayPointList)
	ON_EN_KILLFOCUS(IDC_EDIT_AIRROUTE_NAME, OnKillEidtFocus)
	ON_LBN_SETFOCUS(IDC_LIST_RUNWAYPORT, OnLbnSetfocusListRunway)
	ON_LBN_KILLFOCUS(IDC_LIST_RUNWAYPORT, OnLbnKillfocusListRunway)
END_MESSAGE_MAP()


// CDlgAirRoute message handlers

int AirsideGUI::CDlgAirRoute::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolBaWayPoint.CreateEx(this,
		TBSTYLE_FLAT,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),456)||
		!m_toolBaWayPoint.LoadToolBar(IDR_TOOLBAR_AIRROUTE_ARWAYPOINT))
	{
		return -1;
	}

	if (!m_toolBarRunway.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),IDR_TOOLBAR_RUNWAYPOINT_ADD_DEL)
		|| !m_toolBarRunway.LoadToolBar(IDR_TOOLBAR_RUNWAYPOINT_ADD_DEL))
	{
		return -1;
	}
 
 	CToolBarCtrl& toolbarAssignment = m_toolBarRunway.GetToolBarCtrl();
 	toolbarAssignment.SetCmdID(0, ID_NEW_RUNWAYPORT);
 	toolbarAssignment.SetCmdID(1, ID_DEL_RUNWAYPORT);

	return 0;
}

BOOL AirsideGUI::CDlgAirRoute::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CRect rcWaypoint;
	GetDlgItem(IDC_STATIC_TOOLBARWAYPOINT)->GetWindowRect(&rcWaypoint);
	ScreenToClient(&rcWaypoint);
	m_toolBaWayPoint.MoveWindow(&rcWaypoint);
	m_toolBaWayPoint.ShowWindow(SW_SHOW);

	m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_DEL_ARWAYPOINT);
	m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_NEW_ARCWAYPOINT);
	m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_EDIT_ARWAYPOINT);
	m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_INSERT_ARWAYPOINT);

	CRect rcRunway;
	GetDlgItem(IDC_STATIC_RUNWAYTOOLBAR)->GetWindowRect(&rcRunway);
	ScreenToClient(&rcRunway);
	m_toolBarRunway.MoveWindow(&rcRunway);

	m_toolBarRunway.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RUNWAYTOOLBAR)->ShowWindow(SW_HIDE);

	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD);
	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,FALSE);

	//init combobox
	
 	m_cmbType.AddString(_T("STAR"));
	m_cmbType.AddString(_T("SID"));
	m_cmbType.AddString(_T("ENROUTE"));
	m_cmbType.AddString(_T("MISSEDAPCH"));
	m_cmbType.AddString(_T("CIRCUIT"));
	m_cmbType.SetCurSel(0);
	//m_cmbRunway.SetCurSel(0);

	try
	{

		std::vector<ALTAirport> vAirport;
		//the airspace id is equal to the project id
		//ALTAirport::GetAirportList(m_nAirspaceID,vAirport);
		//
		//for (int i = 0; i < static_cast<int>(vAirport.size()); ++i)
		//{
		//	CString strAirportName = vAirport[i].getName();
		//	
		//	std::vector<ALTObject> vRunwayObject;
		//	ALTObject::GetObjectList(ALT_RUNWAY,vAirport[i].getID(),vRunwayObject);

		//	for (int j= 0; j < static_cast<int>(vRunwayObject.size()); ++j)
		//	{
		//		CString strRunwayName = _T("");

		//		strRunwayName.Format(_T("%s-%s"),strAirportName,vRunwayObject[j].GetObjNameString());

		//		int nItem = m_cmbRunway.AddString(strRunwayName);
		//		m_cmbRunway.SetItemData(nItem,vRunwayObject[j].getID());
		//	}
		//}
		AirWayPoint::GetWayPointList(m_nAirspaceID,m_vWayPoint);
		
	}	
	catch (CADOException &e) 
	{
		e.ErrorMessage();
		MessageBox(_T("Load Waypoint List Error;"));
	}
 
	SetResize(IDC_STATIC_PROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_TYPE,SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(IDC_COMBO_TYPE, SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(IDC_STATIC_AIRROUTE_NAME,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_EDIT_AIRROUTE_NAME,SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_RUNWAY, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_RUNWAYTOOLBAR,SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(m_toolBarRunway.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_RUNWAYPORT,SZ_TOP_CENTER,SZ_TOP_RIGHT);
	//SetResize(IDC_COMBO_RUNWAY, SZ_TOP_LEFT, SZ_TOP_CENTER);
	//SetResize(IDC_STATIC_MARK, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	//SetResize(IDC_COMBO_RUNWAYMARK, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_TOOLBARWAYPOINT, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_toolBaWayPoint.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_ARWAYPOINT, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	CUnitPiece::InitializeUnitPiece(m_nAirspaceID,UM_ID_17,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	GetDlgItem(IDC_LIST_ARWAYPOINT)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;	
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse|ARCUnit_Velocity_InUse);


	// set list control window style
	DWORD dwStyle = m_lstWaypoint.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstWaypoint.SetExtendedStyle(dwStyle);
	InitARWaypointList();
	//init list	
	InitNewAirRoute();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirRoute::InitProperties(CAirRoute *pAirRoute)
{
	//InitDefaultProperties();

	//type
	CAirRoute::RouteType routeType = pAirRoute->getRouteType();
	m_cmbType.SetCurSel((int)routeType);
	//runway
	if (routeType == CAirRoute::EN_ROUTE)
	{
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,FALSE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, FALSE);
		//m_cmbRunway.EnableWindow(FALSE);
		//m_cmbRunwayMark.EnableWindow(FALSE);
	}
	else
	{
		//m_cmbRunway.EnableWindow(TRUE);
		//m_cmbRunwayMark.EnableWindow(TRUE);
		//int nRunwayID = pAirRoute->getRunwayID();
		//SelectRunwayComboboxByRunwayID(nRunwayID);
		//OnCbnSelchangeComboRunway();
		//m_cmbRunwayMark.SetCurSel(pAirRoute->getRunwayMarkIndex());
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,TRUE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, FALSE);
	}



	const std::vector<ARWaypoint *>& vARWaypoint = pAirRoute->getARWaypoint();

	for (int i = 0; i < static_cast<int>(vARWaypoint.size()); ++i)
	{
		InsertARWaypointIntoList(vARWaypoint[i], i);
	}

	int nRwy = pAirRoute->GetConnectRunwayCount();
	for (int i =0; i<nRwy; i++)
	{
		LogicRunway_Pair logicrunway = pAirRoute->getLogicRunwayByIdx(i);

		Runway altRunway;
		altRunway.ReadObject(logicrunway.first);	

		CString strMarking = (RUNWAYMARK_FIRST == logicrunway.second ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
		CString strRunwayInfo;
		strRunwayInfo.Format("Runway:%s", strMarking);

		m_vConnectRunway.push_back(logicrunway);
		int nlstItemCnt = m_lstRunway.GetCount();
		m_lstRunway.InsertString(nlstItemCnt,strRunwayInfo);
		m_lstRunway.SetItemData(nlstItemCnt,logicrunway.first);
	}

	AdjustARWayPointToolBarState();

}
int CDlgAirRoute::InsertARWaypointIntoList(ARWaypoint * arwaypoint, int nRowNum)
{
	//int nItem =  m_lstWaypoint.GetItemCount();
	const ALTObject& waypoint = arwaypoint->getWaypoint();
	m_lstWaypoint.InsertItem(nRowNum,waypoint.GetObjNameString());
	if (waypoint.GetObjectName().IsBlank())
	{
		MessageBox("Please select a Way Point!", "Error", MB_OK);
	}

	CString strText = _T("");
	double lMinSpeed = arwaypoint->getMinSpeed();
	if (lMinSpeed == -1)
	{	
		m_lstWaypoint.SetItemText(nRowNum,1,_T("No Limit"));
	}
	else
	{
		lMinSpeed = (CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)lMinSpeed));
		strText.Format(_T("%.2f"),lMinSpeed);
		m_lstWaypoint.SetItemText(nRowNum,1,strText);
	}

	double lMaxSpeed = arwaypoint->getMaxSpeed();
	if (lMaxSpeed == -1)
	{	
		m_lstWaypoint.SetItemText(nRowNum,2,_T("No Limit"));
	}
	else
	{
		lMaxSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)lMaxSpeed);
		strText.Format(_T("%.2f"),lMaxSpeed);
		m_lstWaypoint.SetItemText(nRowNum,2,strText);
	}

	double lMinHeight = arwaypoint->getMinHeight();
	if (lMinHeight == -1)
	{
		m_lstWaypoint.SetItemText(nRowNum,3,_T("No Limit"));
	}
	else
	{
		lMinHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)lMinHeight);
		strText.Format(_T("%.2f"),lMinHeight);
		m_lstWaypoint.SetItemText(nRowNum,3,strText);
	}

	double lMaxHeight = arwaypoint->getMaxHeight();
	if (lMaxHeight == -1)
	{
		m_lstWaypoint.SetItemText(nRowNum,4,_T("No Limit"));
	}
	else
	{
		lMaxHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)lMaxHeight);
		strText.Format(_T("%.2f"),lMaxHeight);
		m_lstWaypoint.SetItemText(nRowNum,4,strText);
	}

	if (arwaypoint->getDepartType() == ARWaypoint::NextWaypoint)
	{
		m_lstWaypoint.SetItemText(nRowNum,5, _T("Next Waypoint"));
	}
	else
	{

		m_lstWaypoint.SetItemText(nRowNum,5,_T("Heading"));

		strText.Format(_T("%d"),arwaypoint->getDegrees());
		m_lstWaypoint.SetItemText(nRowNum,6,strText);

		if (arwaypoint->getHeadingType() == ARWaypoint::Direct)
			m_lstWaypoint.SetItemText(nRowNum,7,_T("Direct"));
		else
			m_lstWaypoint.SetItemText(nRowNum,7,_T("Aligned"));

		double lVisDis = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)arwaypoint->getVisDistance());
		strText.Format(_T("%.2f"),lVisDis);
		m_lstWaypoint.SetItemText(nRowNum,8,strText);
	}

	return nRowNum;
}

void CDlgAirRoute::InitARWaypointList()
{
	m_lstWaypoint.DeleteAllItems(); 
	for (int i = m_lstWaypoint.GetHeaderCtrl()->GetItemCount();i > 0;--i) 
		m_lstWaypoint.DeleteColumn(0);

	CString strTitle = _T("");

	strTitle.Format(_T("%s"),_T("Way point"));
	m_lstWaypoint.InsertColumn(0,strTitle,LVCFMT_CENTER,100);
	strTitle.Format(_T("Min Speed (%s)"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()) );
	m_lstWaypoint.InsertColumn(1,strTitle,LVCFMT_CENTER,80);
	strTitle.Format(_T("Max Speed (%s)"), CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()));
	m_lstWaypoint.InsertColumn(2,strTitle,LVCFMT_CENTER,80);
	strTitle.Format(_T("Min Height (%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) );
	m_lstWaypoint.InsertColumn(3,strTitle,LVCFMT_CENTER,80);
	strTitle.Format(_T("Max Height (%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) );
	m_lstWaypoint.InsertColumn(4,strTitle,LVCFMT_CENTER,80);
	m_lstWaypoint.InsertColumn(5,_T("Depart for"),LVCFMT_CENTER,80);
	m_lstWaypoint.InsertColumn(6,_T("Heading Degree"),LVCFMT_CENTER,80);
	m_lstWaypoint.InsertColumn(7,_T("Sequencing to next wp"),LVCFMT_CENTER, 100);
	strTitle.Format(_T("Visual Distance (%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) );
	m_lstWaypoint.InsertColumn(8,strTitle,LVCFMT_CENTER,80);
}
//void CDlgAirRoute::SelectRunwayComboboxByRunwayID(int nRunwayID)
//{
//	m_cmbRunway.SetCurSel(-1);
//	int nCount = m_cmbRunway.GetCount();
//	for (int i =0; i < nCount; ++i)
//	{
//		if (nRunwayID == m_cmbRunway.GetItemData(i))
//		{
//			m_cmbRunway.SetCurSel(i);
//			break;
//		} 
//	}
//}
void CDlgAirRoute::InitDefaultProperties()
{
	m_cmbType.EnableWindow(TRUE);	
	m_cmbType.SetCurSel(0);
	//m_cmbRunway.SetCurSel(-1);
	//m_cmbRunway.EnableWindow(TRUE);
	//m_cmbRunwayMark.ResetContent();
	//m_cmbRunwayMark.EnableWindow(TRUE);

	InitARWaypointList();

	AdjustARWayPointToolBarState();
}

void AirsideGUI::CDlgAirRoute::OnOK()
{
	//check heading valid
	if (m_pCurAirRoute && !m_pCurAirRoute->HeadingValid())
	{
		MessageBox(_T("Heading cannot be defined in the first segment or the last segment of the Air Route"),_T("Error"),MB_OK);
		return;
	}

	if (m_lstRunway.GetCount() == 0 )
	{
		int nCurSel = m_cmbType.GetCurSel();
		if (nCurSel != CAirRoute::EN_ROUTE && nCurSel != CAirRoute::CIRCUIT)
		{
			MessageBox("Please add at least one runway!", "Error", MB_OK);
			return ;
		}
	}

	if (m_lstWaypoint.GetItemCount() == 0)
	{
		MessageBox("Please add a Way Point!", "Error", MB_OK);
		return ;
	}
	else
	{
		SaveProPerties(m_pCurAirRoute);
		try
		{
			CADODatabase::BeginTransaction();
			int nIndex = -1;
			nIndex = m_airRoutelst.GetAirRouteCount();
			m_airRoutelst.SaveData(m_nAirspaceID);
			m_delAirRoutelst.DeleteData();

			m_nAirRouteID = m_pCurAirRoute->getID();
			m_airRoutelst.CalAirRouteIntersections();

			for (int i = 0; i < static_cast<int>(m_vDelARWaypoint.size()); ++i)
			{
				m_vDelARWaypoint[i]->DeleteData();
			} 
			CADODatabase::CommitTransaction();
		}
		catch (CADOException &e) 
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
			MessageBox(_T("Save Data Error."));
			return;
		}

	}

	CXTResizeDialog::OnOK();
}

void AirsideGUI::CDlgAirRoute::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void AirsideGUI::CDlgAirRoute::InitNewAirRoute()
{
	CAirRoute *pAirRoute = NULL;
	if (!m_bEidtAirRoute)
	{
		CString strName = _T("AirRoute");
		pAirRoute = new CAirRoute(-1);
		m_strAirRouteName = strName;
		pAirRoute->setName(strName);
		InitDefaultProperties();
		m_airRoutelst.AddAirRoute(pAirRoute);
	}
	else
	{
		m_airRoutelst.ReadData(m_nAirspaceID);
		pAirRoute = m_airRoutelst.GetAirRouteByID(m_nAirRouteID);
		m_strAirRouteName = pAirRoute->getName();
		InitProperties(pAirRoute);
	}
	UpdateData(FALSE);
	m_pCurAirRoute = pAirRoute;
	m_nAirRouteID = pAirRoute->getID();
}
LRESULT AirsideGUI::CDlgAirRoute::OnMessageNewAirRoute( WPARAM wParam, LPARAM lParam )
{
	
	CAirRoute* pAirRoute = m_airRoutelst.GetAirRoute((int)wParam);
	pAirRoute->setName((LPCSTR)lParam);


	return 1;
}

void AirsideGUI::CDlgAirRoute::SaveProPerties(CAirRoute *pAirRoute)
{
	int nType = m_cmbType.GetCurSel();
	if (nType != LB_ERR)
	{
		pAirRoute->setRouteType((CAirRoute::RouteType)nType);
		if (nType != CAirRoute::EN_ROUTE)
		{
			pAirRoute->SetConnectRunway(m_vConnectRunway);

			//int nRunwayItem =  m_cmbRunway.GetCurSel();
			//if (nRunwayItem != LB_ERR)
			//{
			//	 int nRunwayID = m_cmbRunway.GetItemData(nRunwayItem);
			//	 int nMarkIndex = m_cmbRunwayMark.GetCurSel();
			//	 if (nMarkIndex == LB_ERR)
			//	 {
			//		 nMarkIndex = 0;
			//	 }
			//	 pAirRoute->setRunwayID(nRunwayID,nMarkIndex);
			//}
		}
	}

}
void AirsideGUI::CDlgAirRoute::OnNewARWaypoint()
{
	if ((int)m_vWayPoint.size() == 0)
	{
		return;
	}
	m_cmbType.EnableWindow(FALSE);

	bool bStar = false;

	int nType = m_cmbType.GetCurSel();
	if (nType == CAirRoute::STAR)
		bStar = true;

	ARWaypoint *pARWayPoint = new ARWaypoint(-1);

	CDlgAirRouteWayPointDefine dlg(pARWayPoint,m_vWayPoint, bStar,this);
	if (dlg.DoModal() == IDOK)
	{
		//CAirRoute* pAirRoute = m_airRoutelst.GetAirRoute(m_nListBoxSel);
		int nRowCount = m_lstWaypoint.GetItemCount();
		if(pARWayPoint->getWaypoint().GetObjectName().GetIDString().IsEmpty())
		{
			MessageBox("Please select a Way Point!", "Error", MB_OK);
		}
		else
		{
			m_pCurAirRoute->getARWaypoint().push_back(pARWayPoint);
			int nItem = InsertARWaypointIntoList(pARWayPoint, nRowCount);
		}
	}
	else
	{
		delete pARWayPoint;
	}

	AdjustARWayPointToolBarState();

}

void AirsideGUI::CDlgAirRoute::OnNewRunway()
{
	CDlgSelectRunway dlg(m_nAirspaceID);
	if(IDOK != dlg.DoModal())
		return;
	//int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nMark = dlg.GetSelectedRunwayMarkingIndex();

	//ALTAirport altAirport;
	//altAirport.ReadAirport(nAirportID);
	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	

	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
	CString strRunwayInfo;
	strRunwayInfo.Format("Runway:%s", strMarking);

	int nCount = (int)m_vConnectRunway.size();	
	for(int j = 0;j < nCount;j++ )
	{
		LogicRunway_Pair logicrunway = m_vConnectRunway.at(j);
		if(logicrunway.first == nRunwayID)
		{
			MessageBox(_T("Can not select exist runway!"));
			return;
		}
	}
	m_vConnectRunway.push_back(LogicRunway_Pair(nRunwayID,nMark));
	int nlstItemCnt = m_lstRunway.GetCount();
	m_lstRunway.InsertString(nlstItemCnt,strRunwayInfo);
	m_lstRunway.SetItemData(nlstItemCnt,nRunwayID);
}

void AirsideGUI::CDlgAirRoute::OnDelRunway()
{
	int nIdx = m_lstRunway.GetCurSel();
	int nDelRunway = m_lstRunway.GetItemData(nIdx);
	
	for(int j = 0;j < (int)m_vConnectRunway.size();j++ )
	{
		LogicRunway_Pair logicrunway = m_vConnectRunway.at(j);
		if(logicrunway.first == nDelRunway)
		{
			m_vConnectRunway.erase(m_vConnectRunway.begin()+ j);
			m_lstRunway.DeleteString(nIdx);
			break;
		}
	}
}

void AirsideGUI::CDlgAirRoute::OnEditARWaypoint()
{
	bool bStar = false;

	int nType = m_cmbType.GetCurSel();
	if (nType == CAirRoute::STAR)
		bStar = true;

	POSITION pos = m_lstWaypoint.GetFirstSelectedItemPosition();
	if (pos)
	{
		int nItem = m_lstWaypoint.GetNextSelectedItem(pos);
		
		ARWaypoint *pARWayPoint = m_pCurAirRoute->getARWaypoint().at(nItem);
		CDlgAirRouteWayPointDefine dlg(pARWayPoint,m_vWayPoint,bStar,this);
		if (dlg.DoModal() == IDOK)
		{
			const ALTObject& waypoint = pARWayPoint->getWaypoint();
			m_lstWaypoint.SetItemText(nItem,0,waypoint.GetObjNameString());

			CString strText = _T("");
			double lMinSpeed = pARWayPoint->getMinSpeed();
			if (lMinSpeed == -1)
			{	
				m_lstWaypoint.SetItemText(nItem,1,_T("No Limit"));
			}
			else
			{
				lMinSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)lMinSpeed);
				strText.Format(_T("%.2f"),lMinSpeed);
				m_lstWaypoint.SetItemText(nItem,1,strText);
			}

			double lMaxSpeed = pARWayPoint->getMaxSpeed();
			if (lMaxSpeed == -1)
			{	
				m_lstWaypoint.SetItemText(nItem,2,_T("No Limit"));
			}
			else
			{
				lMaxSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),(double)lMaxSpeed);
				strText.Format(_T("%.2f"),lMaxSpeed);
				m_lstWaypoint.SetItemText(nItem,2,strText);
			}

			double lMinHeight = pARWayPoint->getMinHeight();
			if (std::abs(lMinHeight+1) < ARCMath::EPSILON)
			{
				m_lstWaypoint.SetItemText(nItem,3,_T("No Limit"));
			}
			else
			{
				lMinHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)lMinHeight);
				strText.Format(_T("%.2f"),lMinHeight);
				m_lstWaypoint.SetItemText(nItem,3,strText);
			}

			double lMaxHeight = pARWayPoint->getMaxHeight();
			if (std::abs(lMaxHeight+1) < ARCMath::EPSILON)
			{
				m_lstWaypoint.SetItemText(nItem,4,_T("No Limit"));
			}
			else
			{
				lMaxHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)lMaxHeight);
				strText.Format(_T("%.2f"),lMaxHeight);
				m_lstWaypoint.SetItemText(nItem,4,strText);
			}

			if (pARWayPoint->getDepartType() == ARWaypoint::NextWaypoint)
			{
				m_lstWaypoint.SetItemText(nItem,5, _T("Next Waypoint"));
				m_lstWaypoint.SetItemText(nItem,6,_T(""));
				m_lstWaypoint.SetItemText(nItem,7,_T(""));
				m_lstWaypoint.SetItemText(nItem,8,_T(""));
			}
			else
			{
				m_lstWaypoint.SetItemText(nItem,5,_T("Heading"));

				strText.Format(_T("%d"),pARWayPoint->getDegrees());
				m_lstWaypoint.SetItemText(nItem,6,strText);

				if (pARWayPoint->getHeadingType() == ARWaypoint::Direct)
					m_lstWaypoint.SetItemText(nItem,7,_T("Direct"));
				else
					m_lstWaypoint.SetItemText(nItem,7,_T("Aligned"));

				double lVisDis = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pARWayPoint->getVisDistance());
				strText.Format(_T("%.2f"),lVisDis);
				m_lstWaypoint.SetItemText(nItem,8,strText);

			}
		}
	}

}
void AirsideGUI::CDlgAirRoute::OnDelARWaypoint()
{
	POSITION pos = m_lstWaypoint.GetFirstSelectedItemPosition();
	if (pos)
	{
		int nItem = m_lstWaypoint.GetNextSelectedItem(pos);

		std::vector<ARWaypoint*>& vARWayPoint = m_pCurAirRoute->getARWaypoint();
		ARWaypoint *pARWayPoint = vARWayPoint[nItem];
		if (pARWayPoint->getID() != -1)
		{
			m_vDelARWaypoint.push_back(pARWayPoint);
		}

		vARWayPoint.erase(vARWayPoint.begin()+ nItem);
		m_lstWaypoint.DeleteItem(nItem);
	}

	AdjustARWayPointToolBarState();
}

void AirsideGUI::CDlgAirRoute::OnInsertARWaypoint()
{
	m_cmbType.EnableWindow(FALSE);

	bool bStar = false;

	int nType = m_cmbType.GetCurSel();
	if (nType == CAirRoute::STAR)
		bStar = true;

	ARWaypoint *pARWayPoint = new ARWaypoint(-1);
	CDlgAirRouteWayPointDefine dlg(pARWayPoint,m_vWayPoint, bStar,this);
	if (dlg.DoModal() == IDOK)
	{
		int nRowNum = m_lstWaypoint.GetNextItem(-1, LVNI_SELECTED);
		int nRowCount = m_lstWaypoint.GetItemCount();

		//==0
		if (nRowCount == 0)
		{
			nRowNum = 0;
		}
		//>0
		else
		{
			if (nRowNum < 0)
			{
				nRowNum = nRowCount;
			}
		}

		m_pCurAirRoute->getARWaypoint().insert(m_pCurAirRoute->getARWaypoint().begin()+nRowNum, pARWayPoint);
		int nItem = InsertARWaypointIntoList(pARWayPoint, nRowNum);
		m_lstWaypoint.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	}
	else
	{
		delete pARWayPoint;
	}

	AdjustARWayPointToolBarState();
}

void AirsideGUI::CDlgAirRoute::OnCbnSelchangeComboType()
{
	int nCurSel = m_cmbType.GetCurSel();
	if (nCurSel == LB_ERR)
		return;

	if (nCurSel == CAirRoute::EN_ROUTE)
	{
		//m_cmbRunway.EnableWindow(FALSE);
		//m_cmbRunwayMark.EnableWindow(FALSE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,FALSE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, FALSE);
	}
	else
	{
		//m_cmbRunway.EnableWindow(TRUE);
		//m_cmbRunwayMark.EnableWindow(TRUE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,TRUE);
		m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, FALSE);
	}
}

void AirsideGUI::CDlgAirRoute::AdjustARWayPointToolBarState()
{
 	POSITION pos = m_lstWaypoint.GetFirstSelectedItemPosition();
 	if (pos) //select one
 	{
 		int nItem = m_lstWaypoint.GetNextSelectedItem(pos);
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_EDIT_ARWAYPOINT);
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_DEL_ARWAYPOINT);
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_INSERT_ARWAYPOINT);
 	}
 	else
 	{
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_EDIT_ARWAYPOINT,FALSE);
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_DEL_ARWAYPOINT,FALSE);
 		m_toolBaWayPoint.GetToolBarCtrl().EnableButton(ID_INSERT_ARWAYPOINT,FALSE);
 	}
}
void AirsideGUI::CDlgAirRoute::OnLvnItemchangedARWayPoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	AdjustARWayPointToolBarState();
	*pResult = 0;
}
void AirsideGUI::CDlgAirRoute::OnNMDblclkARWayPointList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnEditARWaypoint();
	*pResult = 0;
}
//void AirsideGUI::CDlgAirRoute::OnCbnSelchangeComboRunway()
//{
//	int nCurSel = m_cmbRunway.GetCurSel();
//
//	if (nCurSel == LB_ERR)
//		return;
//
//	int nRunwayID = m_cmbRunway.GetItemData(nCurSel);
//
//	Runway runway;
//	try
//	{
//		runway.ReadObject(nRunwayID);
//	}
//	catch (CADOException &e)
//	{
//		e.ErrorMessage();
//		MessageBox("Load Runway Mark Error.");
//		return;
//	}
//
//	m_cmbRunwayMark.ResetContent();
//	m_cmbRunwayMark.AddString(runway.GetMarking1().c_str());
//	m_cmbRunwayMark.AddString(runway.GetMarking2().c_str());
//	m_cmbRunwayMark.SetCurSel(0);
//}



bool AirsideGUI::CDlgAirRoute::ConvertUnitFromDBtoGUI(void)
{
	return (false);
}

bool AirsideGUI::CDlgAirRoute::RefreshGUI(void)
{
	m_lstWaypoint.DeleteAllItems();
	for (int i = 0; i < m_lstRunway.GetCount(); i++)
	{
		m_lstRunway.DeleteString(0);
	}

	InitProperties(m_pCurAirRoute);
	return (true);
}

bool AirsideGUI::CDlgAirRoute::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void AirsideGUI::CDlgAirRoute::OnKillEidtFocus()
{
	UpdateData(TRUE);
	m_pCurAirRoute->setName(m_strAirRouteName);
}

void AirsideGUI::CDlgAirRoute::OnLbnSetfocusListRunway()
{
	// TODO: Add your control notification handler code here
	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,TRUE);
	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, TRUE);
}

void AirsideGUI::CDlgAirRoute::OnLbnKillfocusListRunway()
{
	// TODO: Add your control notification handler code here
	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAYPORT,TRUE);
	m_toolBarRunway.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAYPORT, FALSE);
}

BOOL AirsideGUI::CDlgAirRoute::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
//	if (GetRoutingFrame() != NULL) return FALSE;

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
