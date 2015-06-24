#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidewaypointdlg.h"
#include "../InputAirside/AirWayPoint.h"
#include "../Common/WinMsg.h"
#include "DlgInitArp.h"
#include "../Common/LatLongConvert.h"
#include "../InputAirside/InputAirside.h"
#include "../Common/Path2008.h"

IMPLEMENT_DYNAMIC(CAirsideWaypointDlg, CAirsideObjectBaseDlg)
CAirsideWaypointDlg::CAirsideWaypointDlg(int nWayPointID,int nAirspaceID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nWayPointID,nAirspaceID,nProjID,pParent)
{
	m_pObject = new AirWayPoint;
	
	std::vector<int> vAirportsID;
	InputAirside::GetAirportList(nAirspaceID,vAirportsID);
	

	if(vAirportsID.size())
		m_AirportInfo.ReadAirport(vAirportsID.at(0));
	

}
//
//CAirsideWaypointDlg::CAirsideWaypointDlg( AirWayPoint * pWayPoint,int nProjID, CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pWayPoint, nProjID,pParent)
//{
//	std::vector<int> vAirportsID;
//	InputAirside::GetAirportList(m_nAirportID,vAirportsID);
//	if(vAirportsID.size())
//		m_AirportInfo.ReadAirport(vAirportsID.at(0));
//
//}
CAirsideWaypointDlg::~CAirsideWaypointDlg(void)
{
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideWaypointDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_WAYPOINT_MODIFYLOWLIMIT, OnPropModifyLowLimitValue)
	ON_COMMAND(ID_WAYPOINT_MODIFYHIGHLIMIT, OnPropModifyHighLimitValue)
	ON_COMMAND(ID_PROCPROP_CHANGLATLONG, OnPropModifyLatLong)	
	ON_COMMAND(ID_POSITIONMETHOD_POSITION, OnWayPointPositon)
	ON_COMMAND(ID_POSITIONMETHOD_WAYPOINTBEARINGDISTANCE, OnWayPointBearingDistance)
	ON_COMMAND(ID_POSITIONMETHOD_WAYPOINTBEARINGWAYPOINTBEARING, OnWayPointBearingWayPointBearing)
	ON_COMMAND(ID_WAYPOINTBEARINGDISTANCE_BEARING, OnWayPointBearing)
	ON_COMMAND(ID_WAYPOINTBEARINGDISTANCE_DISTANCE, OnWayPointDistance)
	ON_COMMAND(ID_WAYPOINTBEARINGDISTANCE_OTHERWAYPOINT, OnOtherWayPoint)
	ON_COMMAND(ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT1, OnOtherWayPoint1)
	ON_COMMAND(ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING1, OnWayPointBearing1)
	ON_COMMAND(ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT2, OnOtherWayPoint2)
	ON_COMMAND(ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING2, OnWayPointBearing2)
END_MESSAGE_MAP()

BOOL CAirsideWaypointDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_WAYPOINT));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Waypoint"));
	}
	else
	{
		SetWindowText(_T("Define Waypoint"));
	}
	
	
	try
	{
		std::vector<ALTObject> vWaypoint;
		ALTObject::GetObjectList(ALT_WAYPOINT,m_nAirportID,vWaypoint);

		for (int i = 0; i < static_cast<int>(vWaypoint.size()); ++ i)
		{
			m_vWaypoint.push_back(std::make_pair(vWaypoint[i].GetObjNameString(),vWaypoint[i].getID()));
		}
	}
	catch (CADOException &e)
	{
		CString strError = e.ErrorMessage();
		MessageBox(_T("Cann't load way point."));

	}
	AirWayPoint::GetWayPointList(m_nAirportID, m_vWaypointlist);

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_12,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_treeProp.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
	/////

	LoadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideWaypointDlg::hItemClear()
{
	m_hServiceLocation = NULL;
	m_hxyz = NULL;
	m_hFixLL = NULL;
	m_hWayPointBearingDistance = NULL;
	m_hOtherWayPoint = NULL;
	m_hBearing = NULL;
	m_hDistance = NULL;
	m_hWayPointBearingAndWayPointBearing = NULL;
	m_hOtherWayPoint1 = NULL;
	m_hBearing1 = NULL;
	m_hOtherWayPoint2 = NULL;
	m_hBearing2 = NULL;
	m_hHighLimit = NULL;
	m_hLowLimit = NULL;

	m_fixlatitude = NULL;
	m_fixlongitude = NULL;
}

void CAirsideWaypointDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	//nFlag = 0,1,2
	if(hTreeItem == m_hFixLL)
	{
		m_hRClickItem = hTreeItem;
		OnPropModifyLatLong();
	}
	else if(hTreeItem == m_hLowLimit)
	{
		m_hRClickItem = hTreeItem;
		OnPropModifyLowLimitValue();
	}
	else if(hTreeItem == m_hHighLimit)
	{
		m_hRClickItem = hTreeItem;
		OnPropModifyHighLimitValue();
	}



	else if(hTreeItem == m_hOtherWayPoint)
	{
		m_hRClickItem = hTreeItem;
		OnOtherWayPoint();
	}
	else if(hTreeItem == m_hBearing)
	{
		m_hRClickItem = hTreeItem;
		OnWayPointBearing();
	}
	else if(hTreeItem == m_hDistance)
	{
		m_hRClickItem = hTreeItem;
		OnWayPointDistance();
	}

    
	else if(hTreeItem == m_hOtherWayPoint1)
	{
		m_hRClickItem = hTreeItem;
		OnOtherWayPoint1();
	}
	else if(hTreeItem == m_hBearing1)
	{
		m_hRClickItem = hTreeItem;
		OnWayPointBearing1();
	}
	else if(hTreeItem == m_hOtherWayPoint2)
	{
		m_hRClickItem = hTreeItem;
		OnOtherWayPoint2();
	}
	else if(hTreeItem == m_hBearing2)
	{
		m_hRClickItem = hTreeItem;
		OnWayPointBearing2();
	}
	
}

void CAirsideWaypointDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem  == m_hServiceLocation )
	{
		pMenu=menuPopup.GetSubMenu(70);
		pMenu->DeleteMenu( ID_POSITIONMETHOD_POSITION ,MF_BYCOMMAND);	
	}
	else if(m_hRClickItem  == m_hWayPointBearingDistance )
	{
		pMenu=menuPopup.GetSubMenu(70);
		pMenu->DeleteMenu( ID_POSITIONMETHOD_WAYPOINTBEARINGDISTANCE ,MF_BYCOMMAND);	
	}
	else if(m_hRClickItem  == m_hWayPointBearingAndWayPointBearing )
	{
		pMenu=menuPopup.GetSubMenu(70);
		pMenu->DeleteMenu( ID_POSITIONMETHOD_WAYPOINTBEARINGWAYPOINTBEARING ,MF_BYCOMMAND);	
	}
	else if(m_hRClickItem  == m_hxyz )
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu(3,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DELETE ,MF_BYCOMMAND);			
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);			
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED, MF_BYCOMMAND );
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP, MF_BYCOMMAND );
	}
	else if (m_hLowLimit == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(66);
		pMenu->DeleteMenu( ID_WAYPOINT_MODIFYHIGHLIMIT ,MF_BYCOMMAND);	

	}
	else if (m_hHighLimit == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(66);
		pMenu->DeleteMenu( ID_WAYPOINT_MODIFYLOWLIMIT ,MF_BYCOMMAND);	

	}
	else if( m_hFixLL == m_hRClickItem ) 
	{
		pMenu = menuPopup.GetSubMenu( 48 );
	}
	else if (m_hBearing == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(71);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_DISTANCE ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_OTHERWAYPOINT ,MF_BYCOMMAND);
	}
	else if (m_hDistance == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(71);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_BEARING ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_OTHERWAYPOINT ,MF_BYCOMMAND);
	}
	else if (m_hOtherWayPoint == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(71);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_BEARING ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGDISTANCE_DISTANCE ,MF_BYCOMMAND);	
	}
	else if (m_hOtherWayPoint1 == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(72);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT2 ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING1 ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING2 ,MF_BYCOMMAND);	
	}
	else if (m_hBearing1 == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(72);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT1 ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT2 ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING2 ,MF_BYCOMMAND);	
	}
	else if (m_hOtherWayPoint2 == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(72);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT1 ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING1 ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING2 ,MF_BYCOMMAND);	
	}
	else if (m_hBearing2 == m_hRClickItem)
	{
		pMenu=menuPopup.GetSubMenu(72);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT1 ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_WAYPOINT2 ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_WAYPOINTBEARINGANDWAYPOINTBEARING_BEARING1 ,MF_BYCOMMAND);	
	}
}
LRESULT CAirsideWaypointDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hLowLimit)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirWayPoint*)GetObject())->SetLowLimit( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
		else if (m_hRClickItem == m_hHighLimit)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirWayPoint*)GetObject())->SetHighLimit( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
		else if (m_hRClickItem == m_hBearing)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dBearing = pst->iPercent;
			if(dBearing > 360)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 360;
			}
			else if(dBearing < 0)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 0;
			}
			((AirWayPoint*)GetObject())->SetBearing(dBearing);
			UpdateObjectViews();
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hDistance)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirWayPoint*)GetObject())->SetDistance( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			UpdateObjectViews();
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hBearing1)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dBearing = pst->iPercent;
			if(dBearing > 360)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 360;
			}
			else if(dBearing < 0)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 0;
			}
			((AirWayPoint*)GetObject())->SetBearing1(dBearing);
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
		else if (m_hRClickItem == m_hBearing2)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dBearing = pst->iPercent;
			if(dBearing > 360)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 360;
			}
			else if(dBearing < 0)
			{
				MessageBox(_T("The Bearing should between 0 and 360."));
				dBearing = 0;
			}
			((AirWayPoint*)GetObject())->SetBearing2(dBearing);
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}
	if (message == WM_INPLACE_COMBO2)
	{	
		if(m_hRClickItem == m_hOtherWayPoint)
		{
			int selitem = m_treeProp.m_comboBox.GetCurSel();	
			if(selitem >=0)
			{			
				if(!IsInWaypointDependencies(((AirWayPoint *)GetObject())->getID(),m_vOtherWaypoint[selitem].second))
				{
					((AirWayPoint *)GetObject())->SetOtherWayPointID(m_vOtherWaypoint[selitem].second);	
					m_bPropModified = true;
					UpdateObjectViews();
					LoadTree();
				}
				else	
					AfxMessageBox("Sorry,this waypoint cannot depend on "+m_vOtherWaypoint[selitem].first+", Because "+m_vOtherWaypoint[selitem].first+" has created depending on this waypoint.");			
			}
		}
		else if(m_hRClickItem == m_hOtherWayPoint1)
		{
			int selitem = m_treeProp.m_comboBox.GetCurSel();	
			if(selitem >=0)
			{		
				if(!IsInWaypointDependencies(((AirWayPoint *)GetObject())->getID(),m_vOtherWaypoint1[selitem].second))
				{
					((AirWayPoint *)GetObject())->SetOtherWayPoint1ID(m_vOtherWaypoint1[selitem].second);	
					m_bPropModified = true;
					UpdateObjectViews();
					LoadTree();
				}
				else
					AfxMessageBox("Sorry,this waypoint cannot depend on "+m_vOtherWaypoint1[selitem].first+", Because "+m_vOtherWaypoint1[selitem].first+" has created depending on this waypoint.");
			}
		}
		else if(m_hRClickItem == m_hOtherWayPoint2)
		{
			int selitem = m_treeProp.m_comboBox.GetCurSel();	
			if(selitem >=0)
			{	
				if(!IsInWaypointDependencies(((AirWayPoint *)GetObject())->getID(),m_vOtherWaypoint2[selitem].second))
				{
					((AirWayPoint *)GetObject())->SetOtherWayPoint2ID(m_vOtherWaypoint2[selitem].second);	
					m_bPropModified = true;
					UpdateObjectViews();
					LoadTree();
				}
				else
					AfxMessageBox("Sorry,this waypoint cannot depend on "+m_vOtherWaypoint2[selitem].first+", Because "+m_vOtherWaypoint2[selitem].first+" has created depending on this waypoint.");
			}
		}
	}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
bool CAirsideWaypointDlg::IsInWaypointDependencies(int WaypointID,int OtherWaypointID)
{
	for (int i = 0; i < static_cast<int>(m_vWaypointlist.size()); ++ i)
	{
		if(m_vWaypointlist[i].GetFlag() == 1)
		{
			if(m_vWaypointlist[i].getID() == OtherWaypointID && m_vWaypointlist[i].GetOtherWayPointID() == WaypointID)
				return true;
		}
		else if(m_vWaypointlist[i].GetFlag() == 2)
		{
			if(m_vWaypointlist[i].getID() == OtherWaypointID && 
				(m_vWaypointlist[i].GetOtherWayPoint1ID() == WaypointID || m_vWaypointlist[i].GetOtherWayPoint2ID() == WaypointID))
				return true;
		}
	}
	return false;
}
FallbackReason CAirsideWaypointDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hxyz)
	{
		return PICK_ONEPOINT;
	}

	return PICK_MANYPOINTS;

}

void CAirsideWaypointDlg::SetObjectPath(CPath2008& path)
{
	if (m_hxyz == m_hRClickItem)
	{
		((AirWayPoint *)GetObject())->SetServicePoint(path.getPoint(0));
		UpdateObjectViews();
		m_bPropModified = true;
		LoadTree();
	}

}

void CAirsideWaypointDlg::LoadTree()
{

	m_treeProp.DeleteAllItems();
	hItemClear(); 
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;
	int nFlag = ((AirWayPoint *)GetObject())->GetFlag();
	if(nFlag == 0 ||nFlag == -1)
	{
		// Service Location
		CString csLabel = CString("Position (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
		m_hServiceLocation = m_treeProp.InsertItem( csLabel );
		CPoint2008 point = ((AirWayPoint *)GetObject())->GetServicePoint();
		ConvertPosToLatLong(point,m_fixlatitude, m_fixlongitude);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z=%.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_hxyz = m_treeProp.InsertItem(strPoint,m_hServiceLocation);	
		m_treeProp.SetItemDataEx(m_hxyz,aoidDataEx);

		//lat long
		CString strLL;
		CString strLat,strLong;
		m_fixlatitude.GetValue(strLat);
		m_fixlongitude.GetValue(strLong);

		strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strLat;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		isscStringColor.strSection = strLong;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
        
		m_hFixLL= m_treeProp.InsertItem(strLL,m_hServiceLocation);
		m_treeProp.SetItemDataEx(m_hFixLL,aoidDataEx);

		m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
	}
	else if(nFlag == 1)
	{
		m_hWayPointBearingDistance = m_treeProp.InsertItem(_T("WayPoint Bearing Distance"));
		CString strLabel;
		//-------
		int nOtherWayPointID = ((AirWayPoint *)GetObject())->GetOtherWayPointID();

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 

		if(nOtherWayPointID != 0)
		{		
			std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
			CString strLabelName;
			for (; iter != m_vWaypoint.end(); ++iter)
			{
				if(iter->second == nOtherWayPointID)
				{
					strLabelName = iter->first;
					break;
				}
			}
			strLabel.Format("Waypoint (%s)",strLabelName); 
			strTemp = strLabelName;
		}
		else
		{
			strLabel.Format("Waypoint (%s)",_T("Plese Select"));
			strTemp = _T("Plese Select");
		}
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hOtherWayPoint = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);	
		m_treeProp.SetItemDataEx(m_hOtherWayPoint,aoidDataEx);
		//-----------
		strLabel.Empty();
		strLabel.Format(_T("Bearing (Degree)(%.0f)"),
			((AirWayPoint *)GetObject())->GetBearing());

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		strTemp.Format(_T("%.0f"),((AirWayPoint *)GetObject())->GetBearing());
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_hBearing = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);
		m_treeProp.SetItemDataEx(m_hBearing,aoidDataEx);

		strLabel.Empty();
		strLabel.Format(_T("Distance (%s)(%.0f)"),
			 CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) , 
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetDistance()));

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetDistance()));
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_hDistance = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);
		m_treeProp.SetItemDataEx(m_hDistance,aoidDataEx);
		m_treeProp.Expand( m_hWayPointBearingDistance, TVE_EXPAND );
	}
	else if(nFlag == 2)
	{
		m_hWayPointBearingAndWayPointBearing = m_treeProp.InsertItem(_T("WayPoint Bearing And WayPoint Bearing"),TVI_ROOT,TVI_FIRST);
		CString strLabel;
		//-------
		int nOtherWayPoint1ID = ((AirWayPoint *)GetObject())->GetOtherWayPoint1ID();

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 

		if(nOtherWayPoint1ID != 0)
		{		
			std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
			CString strLabelName;
			for (; iter != m_vWaypoint.end(); ++iter)
			{
				if(iter->second == nOtherWayPoint1ID)
				{
					strLabelName = iter->first;
					break;
				}
			}
			strLabel.Format("Waypoint (%s)",strLabelName);
			strTemp = strLabelName;
		}
		else
		{
			strLabel.Format("Waypoint (%s)",_T("Plese Select"));
			strTemp = _T("Plese Select");
		}
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hOtherWayPoint1 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);
		m_treeProp.SetItemDataEx(m_hOtherWayPoint1,aoidDataEx);
		//-----------
		strLabel.Empty();
		strLabel.Format(_T("Bearing (Degree)(%.0f)"),
			((AirWayPoint *)GetObject())->GetBearing1());

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		strTemp.Format(_T("%.0f"),((AirWayPoint *)GetObject())->GetBearing1());
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_hBearing1 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);
		m_treeProp.SetItemDataEx(m_hBearing1,aoidDataEx);

		int nOtherWayPoint2ID = ((AirWayPoint *)GetObject())->GetOtherWayPoint2ID();

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 

		if(nOtherWayPoint2ID != 0)
		{		
			std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
			CString strLableName;
			for (; iter != m_vWaypoint.end(); ++iter)
			{
				if(iter->second == nOtherWayPoint2ID)
				{
					strLableName = iter->first;
					break;
				}
			}
			strLabel.Empty();
			strLabel.Format("Waypoint (%s)",strLableName);
			strTemp = strLableName;
		}
		else
		{
			strLabel.Empty();
			strLabel.Format("Waypoint (%s)",_T("Plese Select"));
			strTemp = _T("Plese Select");
		}
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hOtherWayPoint2 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);	
		m_treeProp.SetItemDataEx(m_hOtherWayPoint2,aoidDataEx);
		//-----------
		strLabel.Empty();
		strLabel.Format(_T("Bearing (Degree)(%.0f)"),
			((AirWayPoint *)GetObject())->GetBearing2());

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		strTemp.Format(_T("%.0f"),((AirWayPoint *)GetObject())->GetBearing2());
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_hBearing2 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);
		m_treeProp.SetItemDataEx(m_hBearing2,aoidDataEx);

		m_treeProp.Expand( m_hWayPointBearingAndWayPointBearing, TVE_EXPAND );
	}


	CString strLabel;
	strLabel.Format(_T("Lower Limit (%s)(%.0f)"),\
		 CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) , \
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetLowLimit()));

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetLowLimit()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	m_hLowLimit = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLowLimit,aoidDataEx);

	strLabel.Empty();
	strLabel.Format(_T("High Limit (%s)(%.0f)"),\
		 CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) , \
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetHighLimit()));

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirWayPoint *)GetObject())->GetHighLimit()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	m_hHighLimit = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hHighLimit,aoidDataEx);
	//m_treeProp.SetRedraw(TRUE);
}

void CAirsideWaypointDlg::OnPropModifyLowLimitValue()
{
	double dLowLimit =((AirWayPoint *)GetObject())->GetLowLimit();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dLowLimit )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideWaypointDlg::OnPropModifyHighLimitValue()
{
	double dHighLimit =((AirWayPoint *)GetObject())->GetHighLimit();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dHighLimit )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}





void CAirsideWaypointDlg::OnOK()
{

	CAirsideObjectBaseDlg::OnOK();
}
bool CAirsideWaypointDlg::UpdateOtherData()
{
	int nFlag = ((AirWayPoint *)GetObject())->GetFlag();
	if(nFlag == 0||nFlag == -1)
	{
		CPoint2008 point = ((AirWayPoint *)GetObject())->GetServicePoint();
		CPoint2008 ptDefault;
		if (point == ptDefault)
		{
			MessageBox(_T("Please pick waypoint service location."));
			return false;
		}	
	}
	if(nFlag == 1)
	{
		AirWayPoint* airwaypoint = (AirWayPoint *)GetObject();
		if(airwaypoint->GetOtherWayPointID() == 0)
		{
			MessageBox(_T("Please Select WayPoint."));
			return false;
		}		
	}
	if(nFlag == 2)
	{
		AirWayPoint* airwaypoint = (AirWayPoint *)GetObject();
		if(airwaypoint->GetOtherWayPoint1ID() == 0 || airwaypoint->GetOtherWayPoint2ID() == 0)
		{
			MessageBox(_T("Please Select WayPoint."));
			return false;
		}		
	}
	return true;
}

void CAirsideWaypointDlg::OnPropModifyLatLong()
{
	CDlgInitArp initArp;
	m_fixlatitude.GetValue(initArp.m_strLatitude);
	m_fixlongitude.GetValue(initArp.m_strLongitude);

	if (initArp.DoModal()==IDOK)
	{
		
		m_fixlatitude.SetValue(initArp.m_strLatitude);
		m_fixlongitude.SetValue(initArp.m_strLongitude);

		CPoint2008 pt = ((AirWayPoint*)GetObject())->GetServicePoint();
		ConvertLatLongToPos(m_fixlatitude,m_fixlongitude,pt);
		((AirWayPoint*)GetObject())->SetServicePoint(pt);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
}
void CAirsideWaypointDlg::OnWayPointPositon()
{
	//-------------
	int nFlag = ((AirWayPoint *)GetObject())->GetFlag();
	if(nFlag == 0||nFlag == -1)
		return;
	else if(nFlag == 1)
	{
		m_treeProp.DeleteItem(m_hWayPointBearingDistance);
		((AirWayPoint *)GetObject())->WayPointBearingDistanceClear();
	}
	else if(nFlag == 2)
	{
		m_treeProp.DeleteItem(m_hWayPointBearingAndWayPointBearing);
		((AirWayPoint *)GetObject())->WayPointBearingAndWayPointBearingClear();
	}
	//---------------
	
	((AirWayPoint*)GetObject())->SetFlag(0);
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
	////m_treeProp.SetRedraw(FALSE);
	//// Service Location
	//CString csLabel = CString("Position (") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
	//m_hServiceLocation = m_treeProp.InsertItem( csLabel );
	//CPoint2008 point = ((AirWayPoint *)GetObject())->GetServicePoint();
	//ConvertPosToLatLong(point,m_fixlatitude, m_fixlongitude);

	//CString strPoint = _T("");
	//strPoint.Format( "x = %.2f; y = %.2f; z=%.2f",
	//	UNITSMANAGER->ConvertLength(point.getX()),
	//	UNITSMANAGER->ConvertLength(point.getY()),
	//	UNITSMANAGER->ConvertLength(point.getZ()));

	//m_hxyz = m_treeProp.InsertItem(strPoint,m_hServiceLocation);		

	////lat long
	//CString strLL;
	//CString strLat,strLong;
	//m_fixlatitude.GetValue(strLat);
	//m_fixlongitude.GetValue(strLong);

	//strLL.Format("Longitude = %s ; Latitude = %s",strLat,strLong);
	//m_hFixLL= m_treeProp.InsertItem(strLL,m_hServiceLocation);
	////m_treeProp.SetRedraw(TRUE);
	//m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
}
void CAirsideWaypointDlg::OnWayPointBearingDistance()
{
	//-------------
	int nFlag = ((AirWayPoint *)GetObject())->GetFlag();
	if(nFlag == 1)
		return;
	else if(nFlag == 0||nFlag == -1)
	{
//		m_treeProp.DeleteItem(m_hServiceLocation);
		((AirWayPoint *)GetObject())->WayPointPositionClear();
	}
	else if(nFlag == 2)
	{
//		m_treeProp.DeleteItem(m_hWayPointBearingAndWayPointBearing);
		((AirWayPoint *)GetObject())->WayPointBearingAndWayPointBearingClear();
	}
	//--------------
	((AirWayPoint*)GetObject())->SetFlag(1);
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
	////m_treeProp.SetRedraw(FALSE);
	
	//m_hWayPointBearingDistance = m_treeProp.InsertItem(_T("WayPoint Bearing Distance"),TVI_ROOT,TVI_FIRST);
	//CString strLabel;
	////-------
	//int nOtherWayPointID = ((AirWayPoint *)GetObject())->GetOtherWayPointID();
	//if(nOtherWayPointID != 0)
	//{		
	//	std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
	//	for (; iter != m_vWaypoint.end(); ++iter)
	//	{
	//		if(iter->second == nOtherWayPointID)
	//		{
	//			strLabel = iter->first;
	//			break;
	//		}
	//	}
	//	strLabel.Format("Waypoint:%s",strLabel);
	//}
	//else
	//	strLabel.Format("Waypoint:%s",_T("Plese Select"));
	//m_hOtherWayPoint = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);	
	////-----------
	//
	//strLabel.Format(_T("Bearing (%s)(%.0f)"),
	//	UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), 
	//	UNITSMANAGER->ConvertLength(((AirWayPoint *)GetObject())->GetBearing()));

	//m_hBearing = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);

	//strLabel.Empty();
	//strLabel.Format(_T("Distance (%s)(%.0f)"),
	//	UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), 
	//	UNITSMANAGER->ConvertLength(((AirWayPoint *)GetObject())->GetDistance()));

	//m_hDistance = m_treeProp.InsertItem(strLabel,m_hWayPointBearingDistance);
	//m_treeProp.Invalidate();
	//m_treeProp.UpdateWindow();
	////m_treeProp.SetRedraw(TRUE);
	//m_treeProp.Expand( m_hWayPointBearingDistance, TVE_EXPAND );
}
void CAirsideWaypointDlg::OnWayPointBearingWayPointBearing()
{
	//-------------
	int nFlag = ((AirWayPoint *)GetObject())->GetFlag();
	if(nFlag == 2)
		return;
	else if(nFlag == 0||nFlag == -1)
	{
//		m_treeProp.DeleteItem(m_hServiceLocation);
		((AirWayPoint *)GetObject())->WayPointPositionClear();
	}
	else if(nFlag == 1)
	{
//		m_treeProp.DeleteItem(m_hWayPointBearingDistance);
		((AirWayPoint *)GetObject())->WayPointBearingDistanceClear();
	}
	//---------------
	((AirWayPoint*)GetObject())->SetFlag(2);
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
//	//m_treeProp.SetRedraw(FALSE);
//	m_hWayPointBearingAndWayPointBearing = m_treeProp.InsertItem(_T("WayPoint Bearing And WayPoint Bearing"),TVI_ROOT,TVI_FIRST);
//	CString strLabel;
//	//-------
//	int nOtherWayPoint1ID = ((AirWayPoint *)GetObject())->GetOtherWayPoint1ID();
//	if(nOtherWayPoint1ID != 0)
//	{		
//		std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
//		for (; iter != m_vWaypoint.end(); ++iter)
//		{
//			if(iter->second == nOtherWayPoint1ID)
//			{
//				strLabel = iter->first;
//				break;
//			}
//		}
//		strLabel.Format("Waypoint:%s",strLabel);
//	}
//	else
//		strLabel.Format("Waypoint:%s",_T("Plese Select"));
//	m_hOtherWayPoint1 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);	
//	//-----------
//	
//	strLabel.Format(_T("Bearing (%s)(%.0f)"),
//		UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), 
//		UNITSMANAGER->ConvertLength(((AirWayPoint *)GetObject())->GetBearing1()));
//
//	m_hBearing1 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);
//
//	int nOtherWayPoint2ID = ((AirWayPoint *)GetObject())->GetOtherWayPoint2ID();
//	if(nOtherWayPoint2ID != 0)
//	{		
//		std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
//		for (; iter != m_vWaypoint.end(); ++iter)
//		{
//			if(iter->second == nOtherWayPoint2ID)
//			{
//				strLabel = iter->first;
//				break;
//			}
//		}
//		strLabel.Format("Waypoint:%s",strLabel);
//	}
//	else
//		strLabel.Format("Waypoint:%s",_T("Plese Select"));
//	m_hOtherWayPoint2 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);	
//	//-----------
//
//	strLabel.Format(_T("Bearing (%s)(%.0f)"),
//		UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), 
//		UNITSMANAGER->ConvertLength(((AirWayPoint *)GetObject())->GetBearing2()));
//
//	m_hBearing2 = m_treeProp.InsertItem(strLabel,m_hWayPointBearingAndWayPointBearing);
////	m_treeProp.SetRedraw(TRUE); 
//	m_treeProp.Invalidate();
//	m_treeProp.UpdateWindow();
//	m_treeProp.Expand( m_hWayPointBearingAndWayPointBearing, TVE_EXPAND );
}
void CAirsideWaypointDlg::OnWayPointBearing()
{
	double dBearing =((AirWayPoint *)GetObject())->GetBearing();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(dBearing ) );
	m_treeProp.SetSpinRange( 0,360 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideWaypointDlg::OnWayPointBearing1()
{
	double dBearing =((AirWayPoint *)GetObject())->GetBearing1();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(dBearing ) );
	m_treeProp.SetSpinRange( 0,360 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideWaypointDlg::OnWayPointBearing2()
{
	double dBearing =((AirWayPoint *)GetObject())->GetBearing2();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(dBearing ) );
	m_treeProp.SetSpinRange( 0,360 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideWaypointDlg::OnWayPointDistance()
{
	double dDistance =((AirWayPoint *)GetObject())->GetDistance();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dDistance )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideWaypointDlg::OnOtherWayPoint()
{
	std::vector<CString> vWayPointName;
	vWayPointName.clear();
	m_vOtherWaypoint.clear();
	std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
	for (; iter != m_vWaypoint.end(); ++iter)
	{
		if(iter->second != ((AirWayPoint *)GetObject())->getID())
		{
			vWayPointName.push_back(iter->first);
			m_vOtherWaypoint.push_back(*iter);
		}
	}
	m_treeProp.SetComboWidth(200);
	m_treeProp.SetComboString(m_hOtherWayPoint,vWayPointName);
}
void CAirsideWaypointDlg::OnOtherWayPoint1()
{
	std::vector<CString> vWayPointName;
	vWayPointName.clear();
	m_vOtherWaypoint1.clear();
	std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
	for (; iter != m_vWaypoint.end(); ++iter)
	{
		if(iter->second != ((AirWayPoint *)GetObject())->getID())
		{
			vWayPointName.push_back(iter->first);
			m_vOtherWaypoint1.push_back(*iter);
		}
	}
	m_treeProp.SetComboWidth(200);
	m_treeProp.SetComboString(m_hOtherWayPoint1,vWayPointName);
}
void CAirsideWaypointDlg::OnOtherWayPoint2()
{
	std::vector<CString> vWayPointName;
	vWayPointName.clear();
	m_vOtherWaypoint2.clear();
	std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
	for (; iter != m_vWaypoint.end(); ++iter)
	{
		if(iter->second != ((AirWayPoint *)GetObject())->getID() && iter->second != ((AirWayPoint *)GetObject())->GetOtherWayPoint1ID())
		{
			vWayPointName.push_back(iter->first);
			m_vOtherWaypoint2.push_back(*iter);
		}
	}
	m_treeProp.SetComboWidth(200);
	m_treeProp.SetComboString(m_hOtherWayPoint2,vWayPointName);
}
void CAirsideWaypointDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_AirportInfo.getLatitude());
	cLong.SetValue(m_AirportInfo.getLongtitude());	
	CPoint2008 refPoint = m_AirportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,0,0);

	LatLong_Structure convented(&latitude,&longitude);

	float deltaX = (float)(pos.getX() - refPoint.getX());
	float deltaY = (float)(pos.getY() - refPoint.getY());
	deltaX = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaX);
	deltaY = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaY);

	convent.Point_LatLong(deltaX,deltaY,&convented);
}
void CAirsideWaypointDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_AirportInfo.getLatitude());
	cLong.SetValue(m_AirportInfo.getLongtitude());

	CPoint2008 refPoint = m_AirportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,refPoint.getX(),refPoint.getY());

	LatLong_Structure Origin(&clat, &cLong);
	LatLong_Structure Convented((CLatitude*)&latitude,(CLongitude*)&longtitude);

	float xdist,ydist,dist;
	convent.Distance_LatLong(&Convented,xdist,ydist,dist);
	xdist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,xdist);
	ydist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,ydist);

	pos.setPoint(refPoint.getX() + xdist,refPoint.getY()+ydist,pos.getZ());

}

bool CAirsideWaypointDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideWaypointDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideWaypointDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
