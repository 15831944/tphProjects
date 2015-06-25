#include "StdAfx.h"
#include "Resource.h"
#include ".\airsidepaxbusparkingposdlg.h"
#include "..\InputAirside\AirsidePaxBusParkSpot.h"

IMPLEMENT_DYNAMIC(CAirsideParkingPosDlg, CAirsideObjectBaseDlg)
CAirsideParkingPosDlg::CAirsideParkingPosDlg(int nGateID,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nGateID,nAirportID,nProjID,pParent)
{	
	//m_bOnRelease = false;
	m_airportInfo.ReadAirport(nAirportID);
}
BEGIN_MESSAGE_MAP(CAirsideParkingPosDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_PROCPROP_TOGGLEDBACKUP, OnProcpropToggledBackup)
	ON_COMMAND(ID_AIRSIDELATLOG_SETLAT,OnEditLatLong)
END_MESSAGE_MAP()

FallbackReason CAirsideParkingPosDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_ONEPOINT;
	}


	if(m_hLeadInLineItem == m_hRClickItem || m_hLeadOutLineItem==m_hRClickItem )
	{
		return PICK_MANYPOINTS;
	}
	return PICK_ONEPOINT;
}

void CAirsideParkingPosDlg::SetObjectPath( CPath2008& path )
{
	AirsideParkSpot * pStand = getParkSpot();
	if (m_hServiceLocation == m_hRClickItem)
	{
		pStand->SetServicePoint(path.getPoint(0));
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}	
	else if( m_hLeadInLineItem==m_hRClickItem) 
	{
	
		pStand->SetLeadInLine(path);
		
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	else if(m_hLeadOutLineItem == m_hRClickItem) 
	{	
		pStand->SetLeadOutLine(path);	
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}

}

bool CAirsideParkingPosDlg::UpdateOtherData()
{
	return true;
}

BOOL CAirsideParkingPosDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STAND));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	CString sTitle = GetTitile(m_nObjID!=-1);
	SetWindowText(sTitle);
	

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_3,this);
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

	//init tree properties
	LoadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideParkingPosDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();
	m_vPositionItems.clear();
	m_vLatLongItems.clear();


	// Service Location
	CString csLabel = CString("Parking Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );
	AirsideParkSpot* pSpot = getParkSpot();

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CPoint2008 point =  pSpot->GetServicePoint();
	CLatitude fixlatitude;
	CLongitude fixlongitude;
	ConvertPosToLatLong(point,fixlatitude, fixlongitude);

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

	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ() + m_airportInfo.getElevation()));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	CString strPoint = _T("");
	strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ() + m_airportInfo.getElevation()));

	hItemTemp = m_treeProp.InsertItem(strPoint,m_hServiceLocation);
	m_vPositionItems.push_back(hItemTemp);
	m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);

	//lat long
	CString strLL;
	CString strLat,strLong;
	fixlatitude.GetValue(strLat);
	fixlongitude.GetValue(strLong);

	strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 		
	isscStringColor.strSection = strLat;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	isscStringColor.strSection = strLong;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	HTREEITEM hFixLL= m_treeProp.InsertItem(strLL,m_hServiceLocation);
	m_treeProp.SetItemDataEx(hFixLL,aoidDataEx);
	m_vLatLongItems.push_back(hFixLL);

	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);



	//Lead in Lines
	
	csLabel.Format("Entry Line From Stretch(%s)", CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	m_hLeadInLineItem = m_treeProp.InsertItem( csLabel );
	//for(size_t i=0;i< pStand->m_vLeadInLines.GetItemCount();i++ )
	{		
		LoadPathItem(m_treeProp,m_hLeadInLineItem,pSpot->getLeadInLine(), aoidDataEx,isscStringColor,CUnitPiece::GetCurSelLengthUnit());		
	}
	m_treeProp.Expand(m_hLeadInLineItem,TVE_EXPAND);

	//Lead out Lines;
	csLabel.Format("Exit Line to Stretch(%s)", CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	m_hLeadOutLineItem = m_treeProp.InsertItem( csLabel );
	//for(size_t i=0;i< pStand->m_vLeadOutLines.GetItemCount();i++ )
	{	
		m_hBackUpItem = m_treeProp.InsertItem( pSpot->IsPushBack()?_T("Backup"):_T("Drive out"),m_hLeadOutLineItem);
		LoadPathItem(m_treeProp,m_hLeadOutLineItem,pSpot->getLeadOutLine(), aoidDataEx,isscStringColor,CUnitPiece::GetCurSelLengthUnit());		
	}
	m_treeProp.Expand(m_hLeadOutLineItem,TVE_EXPAND);
}

void CAirsideParkingPosDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_airportInfo.getLatitude());
	cLong.SetValue(m_airportInfo.getLongtitude());	
	CPoint2008 refPoint = m_airportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,0,0);

	LatLong_Structure convented(&latitude,&longitude);

	float deltaX = (float)(pos.getX() - refPoint.getX());
	float deltaY = (float)(pos.getY() - refPoint.getY());
	deltaX = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaX);
	deltaY = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaY);

	convent.Point_LatLong(deltaX,deltaY,&convented);
}

void CAirsideParkingPosDlg::LoadPathItem( CAirsideObjectTreeCtrl& treeprop,HTREEITEM rootItem, const CPath2008& path,AirsideObjectTreeCtrlItemDataEx& aoidDataEx,ItemStringSectionColor&isscStringColor, ARCUnit_Length lengthUnit )
{
	HTREEITEM hItemTemp = 0;

	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);
		CLatitude fixlatitude;
		CLongitude fixlongitude;
		ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

		CString strTemp;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getZ() + m_airportInfo.getElevation()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,lengthUnit,pt.getZ() + m_airportInfo.getElevation()));

		hItemTemp = treeprop.InsertItem(strPoint,rootItem);
		m_vPositionItems.push_back(hItemTemp);
		treeprop.SetItemDataEx(hItemTemp,aoidDataEx);
		m_treeProp.SetItemData(hItemTemp,(DWORD)i);

		//lat long
		/*CString strLL;
		CString strLat,strLong;
		fixlatitude.GetValue(strLat);
		fixlongitude.GetValue(strLong);

		strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strLat;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		isscStringColor.strSection = strLong;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hFixLL= treeprop.InsertItem(strLL,rootItem);
		treeprop.SetItemDataEx(hFixLL,aoidDataEx);
		m_vLatLongItems.push_back(hFixLL);
		m_treeProp.SetItemData(hFixLL,(DWORD)i);*/
	}
	treeprop.Expand(rootItem,TVE_EXPAND);
}

bool CAirsideParkingPosDlg::ConvertUnitFromDBtoGUI( void )
{
	//return CUnitPiece::ConvertUnitFromDBtoGUI();
	return true;
}

bool CAirsideParkingPosDlg::RefreshGUI( void )
{
	LoadTree();
	return true;//CUnitPiece::RefreshGUI();
}

bool CAirsideParkingPosDlg::ConvertUnitFromGUItoDB( void )
{
	//return CUnitPiece::ConvertUnitFromGUItoDB();
	return true;
}

void CAirsideParkingPosDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	AirsideParkSpot* parkPos = getParkSpot();
	HTREEITEM hItem = m_treeProp.GetParentItem(hTreeItem);


	if (m_vLatLongItems.end() != std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),hTreeItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = parkPos->GetServicePoint();
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				parkPos->SetServicePoint(pt);
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}		
	}
	else if (m_vPositionItems.end() != std::find(m_vPositionItems.begin(),m_vPositionItems.end(),hTreeItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = parkPos->GetServicePoint();
			pt.setZ(pt.getZ() + m_airportInfo.getElevation());
			if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()) )
			{
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				parkPos->SetServicePoint(pt);
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}		
	}

}

void CAirsideParkingPosDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_airportInfo.getLatitude());
	cLong.SetValue(m_airportInfo.getLongtitude());

	CPoint2008 refPoint = m_airportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,refPoint.getX(),refPoint.getY());

	LatLong_Structure Origin(&clat, &cLong);
	LatLong_Structure Convented((CLatitude*)&latitude,(CLongitude*)&longtitude);

	float xdist,ydist,dist;
	convent.Distance_LatLong(&Convented,xdist,ydist,dist);
	xdist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,xdist);
	ydist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,ydist);

	pos.setPoint(refPoint.getX() + xdist,refPoint.getY()+ydist,pos.getZ());
}

void CAirsideParkingPosDlg::OnContextMenu( CMenu& menuPopup, CMenu *& pMenu )
{	

	if(m_hRClickItem  == m_hServiceLocation || m_hRClickItem == m_hLeadOutLineItem )
	{
		pMenu = menuPopup.GetSubMenu(104);
		pMenu->DeleteMenu(0,MF_BYPOSITION);
		pMenu->AppendMenu(MF_STRING, ID_PROCPROP_PICKFROMMAP, _T("Pick From Map"));
		pMenu->EnableMenuItem(ID_PROCPROP_PICKFROMMAP ,MF_BYCOMMAND);	
	}
	if(m_hRClickItem== m_hLeadInLineItem)
	{
		pMenu = menuPopup.GetSubMenu(104);
		pMenu->DeleteMenu(0,MF_BYPOSITION);
		pMenu->AppendMenu(MF_STRING, ID_PROCPROP_PICKFROMMAP, _T("Pick From Map"));
		pMenu->EnableMenuItem(ID_PROCPROP_PICKFROMMAP ,MF_BYCOMMAND);	
		pMenu->AppendMenu(MF_STRING, ID_PROCPROP_TOGGLEDBACKUP, _T("Use as backup Exit line") );
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDBACKUP, MF_BYCOMMAND);
	}
	if(m_hRClickItem == m_hBackUpItem)
	{
		pMenu = menuPopup.GetSubMenu(104);
		pMenu->DeleteMenu(0,MF_BYPOSITION);
		pMenu->AppendMenu(MF_STRING, ID_PROCPROP_TOGGLEDBACKUP, _T("Toggle Backup/DriveOut") );
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDBACKUP, MF_BYCOMMAND);
	}
	
	if (std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem) != m_vLatLongItems.end())
	{
		pMenu = menuPopup.GetSubMenu(97);
	}
}


void CAirsideParkingPosDlg::OnEditLatLong()
{
	AirsideParkSpot* parkPos = getParkSpot();
	HTREEITEM hItem = m_treeProp.GetParentItem(m_hRClickItem);
	if (m_vLatLongItems.end() != std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = parkPos->GetServicePoint();
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				parkPos->SetServicePoint(pt);
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}	
		}
	}
}

void CAirsideParkingPosDlg::OnProcpropToggledBackup()
{
	if (m_hLeadInLineItem == m_hRClickItem)
	{
		AirsideParkSpot* spot = getParkSpot();	
		spot->SetLeadOutLine(spot->getLeadInLine());
		spot->SetPushBack(TRUE);

		UpdateObjectViews();
		m_bPropModified = true;
		LoadTree();
		return;
	}
	if(m_hRClickItem == m_hBackUpItem)
	{
		AirsideParkSpot* spot = getParkSpot();	
		spot->SetPushBack(!spot->IsPushBack());

		UpdateObjectViews();
		m_bPropModified = true;
		LoadTree();
		return;
	}

}
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAirsidePaxBusParkingPosDlg, CAirsideParkingPosDlg)

CAirsidePaxBusParkingPosDlg::CAirsidePaxBusParkingPosDlg( int nObjID,int nAirportID,int nProjID,CWnd* pParent /*= NULL*/ )
:CAirsideParkingPosDlg(nObjID, nAirportID, nProjID, pParent)
{
	m_pObject = new AirsidePaxBusParkSpot();
	m_pObject->setAptID(nAirportID);
}

CString CAirsidePaxBusParkingPosDlg::GetTitile( BOOL bEdit ) const
{
	if (bEdit)
	{
		return (_T("Modify PaxBus Parking Spot"));
	}
	else
	{
		return (_T("Define PaxBus Parking Spot"));
	}
}
