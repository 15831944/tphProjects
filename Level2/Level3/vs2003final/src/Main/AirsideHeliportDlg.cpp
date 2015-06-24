#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\AirsideHeliportDlg.h"
#include "../InputAirside/Heliport.h" 
#include "../Common/WinMsg.h"
#include "../Common/ARCVector.h"
#include "DlgInitArp.h"
#include "../Common/LatLongConvert.h"
#include "../Common/UnitsManager.h"
#include "AirsideStretchVerticalProfileEditor.h"

IMPLEMENT_DYNAMIC(CAirsideHeliportDlg, CAirsideObjectBaseDlg)
CAirsideHeliportDlg::CAirsideHeliportDlg(int nHeliportID,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nHeliportID,nAirportID,nProjID,pParent)
{
	m_pObject = new Heliport();
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
	m_airportInfo.ReadAirport(nAirportID);
}

//CAirsideHeliportDlg::CAirsideHeliportDlg( Heliport* pHeliport,int nProjID,CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pHeliport,nProjID,pParent)
//{
//	m_bPathModified = false;
//	m_airportInfo.ReadAirport(m_nAirportID);
//}
//
CAirsideHeliportDlg::~CAirsideHeliportDlg(void)
{
	//delete m_pObject;
}
void CAirsideHeliportDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAirsideHeliportDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_MENUE_HELIPORT_RADIUS, OnModifyRadius)
	ON_COMMAND(ID_PROCPROP_DEFINE_Z_POS,OnEditVerticalProfile)
	ON_COMMAND(ID_AIRSIDELATLOG_SETLAT,OnEditLatLong)
	ON_COMMAND(ID_AIRSIDELATLOG_DELETE,OnDeleteLatLog)
	ON_COMMAND(ID_AIRSIDEPOSITION_SETXYZ,OnEditPosition)
	ON_COMMAND(ID_AIRSIDEPOSITION_DELETE,OnDeletePosition)
END_MESSAGE_MAP()
BOOL CAirsideHeliportDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_HELIPORT));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Heliport"));
	}
	else
	{
		SetWindowText(_T("Define Heliport"));
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_1,this);
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

//prop lat long dialog
bool OnPropModifyLatLong(CLatitude& lat, CLongitude& longt)
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

void CAirsideHeliportDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hRadius)
	{
		m_hRClickItem = hTreeItem;
		OnModifyRadius();
	}

	CPath2008& path = ((Heliport *)GetObject())->getPath();
	if (path.getCount() != 2)
	{
		path.init(2);
		path[0] = CPoint2008(0,0,0);
		path[1] = CPoint2008(0,0,0);
	}
	for(int i=0;i<(int)m_vLatLongItems.size();i++)
	{
		if( m_vLatLongItems[i] == hTreeItem) // the lat long
		{
			CPoint2008 pt = path.getPoint(i);
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				path[i] = pt;
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
				return;
			}
		}
	}

	for (i = 0; i < (int)m_vPositionItems.size(); i++)
	{
		if (m_vPositionItems[i] == hTreeItem)
		{
			CPoint2008 pt = path.getPoint(i);
			pt.setZ(pt.getZ() + m_airportInfo.getElevation());
			if( OnPropModifyPosition(pt, CUnitPiece::GetCurSelLengthUnit()) )
			{
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				path[i] = pt;
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
				break;
			}
		}
	}
}

void CAirsideHeliportDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
//		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
		pMenu->ModifyMenu(1,MF_BYPOSITION|MF_STRING,ID_PROCPROP_DEFINE_Z_POS, _T("Vertical profile"));
	}
	else if (m_hRClickItem == m_hRadius)
	{
		pMenu = menuPopup.GetSubMenu(83);
	}
	else if (std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem) != m_vLatLongItems.end())
	{
		pMenu = menuPopup.GetSubMenu(97);
	}
	else if (std::find(m_vPositionItems.begin(),m_vPositionItems.end(),m_hRClickItem) != m_vPositionItems.end())
	{
		pMenu = menuPopup.GetSubMenu(96);
	}
}


LRESULT CAirsideHeliportDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hRadius)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Heliport*)GetObject())->SetRadius(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
//void CAirsideHeliportDlg::OnRunwaymarkModify()
//{
//
//	CString strValue= m_treeProp.GetItemText(m_hRClickItem);
//	m_treeProp.StringEditLabel(m_hRClickItem, strValue);
//
//}

void CAirsideHeliportDlg::OnModifyRadius()
{

	double dRadius = ((Heliport*)GetObject())->GetRadius();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dRadius )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

FallbackReason CAirsideHeliportDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_TWOPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideHeliportDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Heliport *)GetObject())->SetPath(path);
		m_bPathModified = true;
		UpdateObjectViews();
//		autoCalRunwayMark(path);
		LoadTree();
	}
}

void CAirsideHeliportDlg::LoadTree()
{

	m_treeProp.DeleteAllItems();
	m_vLatLongItems.clear();
	m_vPositionItems.clear();

	// Service Location
	CString csLabel = CString("Service Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CPath2008 path = ((Heliport *)GetObject())->GetPath();	

	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);
		CLatitude fixlatitude;
		CLongitude fixlongitude;
		ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ() + m_airportInfo.getElevation()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format(_T("x = %.2f; y = %.2f; z = %.2f"),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ() + m_airportInfo.getElevation()));

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
	}

	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);

	//width
	csLabel.Empty();
	csLabel.Format(_T("Radius ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Heliport*)GetObject())->GetRadius()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Heliport*)GetObject())->GetRadius()));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hRadius =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hRadius,aoidDataEx);
}


void CAirsideHeliportDlg::OnOK()
{
	CPath2008 path = ((Heliport *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the Heliport path from the map."));
		return;
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
		{
			((Heliport *)GetObject())->UpdatePath();
		}
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return;
	}


	CAirsideObjectBaseDlg::OnOK();
}
bool CAirsideHeliportDlg::UpdateOtherData()
{
	CPath2008 path = ((Heliport *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the Heliport path from the map."));
		return false;
	}

	if (m_bPathModified && m_nObjID != -1)
	{
		((Heliport *)GetObject())->UpdatePath();
	}

	return true;
}


bool CAirsideHeliportDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideHeliportDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideHeliportDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideHeliportDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
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
void CAirsideHeliportDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
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

void CAirsideHeliportDlg::OnEditVerticalProfile(void)
{
	std::vector<double>vXYPos;
	std::vector<double>vZPos;

	CPath2008& path = ((Heliport*)GetObject())->getPath();
	if( path.getCount() < 2)
	{
		return;
	}

	vXYPos.reserve(path.getCount());
	vZPos.reserve(path.getCount());		
	vXYPos.push_back(0.0); 
	vZPos.push_back( path.getPoint(0).getZ() /SCALE_FACTOR);

	double dLensoFar = 0.0;

	for(int i=1;i<path.getCount();i++)
	{
		ARCVector3 v3D(path[i].getX()-path[i-1].getX(), path[i].getY()-path[i-1].getY(), 0.0);
		dLensoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
		vXYPos.push_back(dLensoFar);				
		vZPos.push_back( path[i].getZ() /SCALE_FACTOR );
	}

	CAirsideStretchVerticalProfileEditor dlg(vXYPos,vZPos, path);

	if( dlg.DoModal() == IDOK )
	{	
		ASSERT(path.getCount() == (int) vZPos.size());
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;

		CPoint2008* pPointList = path.getPointList();
		ASSERT(NULL != pPointList);
		for(i=0; i<path.getCount(); i++)
		{
			pPointList[i].setZ(vZPos[i] * SCALE_FACTOR);
			//m_vCtrlPoints[i][VZ] = vZPos[i] * SCALE_FACTOR ;//pDoc->GetLandsideDoc()->GetLevels().getVisibleAltitude( vZPos[i] * SCALE_FACTOR ) ;
			m_bPropModified = TRUE;
		}

		LoadTree();
	}
}

void CAirsideHeliportDlg::OnEditLatLong()
{
	CPath2008& path = ((Heliport *)GetObject())->getPath();
	if (path.getCount() != 2)
	{
		path.init(2);
		path[0] = CPoint2008(0,0,0);
		path[1] = CPoint2008(0,0,0);
	}
	for(int i=0;i<(int)m_vLatLongItems.size();i++)
	{
		if( m_vLatLongItems[i] == m_hRClickItem) // the lat long
		{
			CPoint2008 pt = path.getPoint(i);
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				path[i] = pt;
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
				return;
			}
		}
	}

}

void CAirsideHeliportDlg::OnEditPosition()
{
	CPath2008& path = ((Heliport *)GetObject())->getPath();
	if (path.getCount() != 2)
	{
		path.init(2);
		path[0] = CPoint2008(0,0,0);
		path[1] = CPoint2008(0,0,0);
	}
	for (int i = 0; i < (int)m_vPositionItems.size(); i++)
	{
		if (m_vPositionItems[i] == m_hRClickItem)
		{
			CPoint2008 pt = path.getPoint(i);
			pt.setZ(pt.getZ() + m_airportInfo.getElevation());
			if( OnPropModifyPosition(pt, CUnitPiece::GetCurSelLengthUnit()) )
			{
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				path[i] = pt;
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
				break;
			}
		}
	}
}

void CAirsideHeliportDlg::OnDeleteLatLog()
{
	CPath2008& path = ((Heliport *)GetObject())->getPath();
	if (path.getCount() != 2)
	{
		path.init(2);
		path[0] = CPoint2008(0,0,0);
		path[1] = CPoint2008(0,0,0);
	}
	for(int i=0;i<(int)m_vLatLongItems.size();i++)
	{
		if( m_vLatLongItems[i] == m_hRClickItem) // the lat long
		{
			path[i] = CPoint2008(0.0,0.0,0.0);
			m_bPathModified = true;
			UpdateObjectViews();
			LoadTree();
			break;

		}
	}
}

void CAirsideHeliportDlg::OnDeletePosition()
{
	CPath2008& path = ((Heliport *)GetObject())->getPath();
	if (path.getCount() != 2)
	{
		path.init(2);
		path[0] = CPoint2008(0,0,0);
		path[1] = CPoint2008(0,0,0);
	}
	for (int i = 0; i < (int)m_vPositionItems.size(); i++)
	{
		if (m_vPositionItems[i] == m_hRClickItem)
		{
			path[i] = CPoint2008(0.0,0.0,0.0);
			m_bPathModified = true;
			UpdateObjectViews();
			LoadTree();
			break;

		}
	}
}