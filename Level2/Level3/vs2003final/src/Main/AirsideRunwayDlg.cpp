#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsiderunwaydlg.h"
#include "../InputAirside/Runway.h" 
#include "../Common/WinMsg.h"
#include "../Common/ARCVector.h"
#include "../Common/Path2008.h"
#include "../Common/LatLongConvert.h"
#include "../Common/UnitsManager.h"
#include "DlgInitArp.h"
#include "AirsideStretchVerticalProfileEditor.h"

#define  DEFAULT_COUNT 2
IMPLEMENT_DYNAMIC(CAirsideRunwayDlg, CAirsideObjectBaseDlg)
CAirsideRunwayDlg::CAirsideRunwayDlg(int nRunwayID,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nRunwayID,nAirportID,nProjID,pParent)
{
	m_pObject = new Runway();
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
	m_airportInfo.ReadAirport(nAirportID);
}

// CAirsideRunwayDlg::CAirsideRunwayDlg( Runway* pRunway,int nProjID,CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pRunway,nProjID,pParent)
// {
// 	m_bPathModified = false;
// 	m_airportInfo.ReadAirport(m_nAirportID);
// }

CAirsideRunwayDlg::~CAirsideRunwayDlg(void)
{
	//delete m_pObject;
}
void CAirsideRunwayDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAirsideRunwayDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_RUNWAYMARK_MODIFY, OnRunwaymarkModify)
	ON_COMMAND(ID_MENUE_CONVEY_WIDTH, OnModifyWidth)
	ON_COMMAND(ID_RUNWAYTHRESHOLD_MODIFYTHRESHOLD, OnModifyThreshold)
	ON_COMMAND(ID_PROCPROP_DEFINE_Z_POS,OnEditVerticalProfile)
	ON_COMMAND(ID_AIRSIDELATLOG_SETLAT,OnEditLatLong)
	ON_COMMAND(ID_AIRSIDELATLOG_DELETE,OnDeleteLatLog)
	ON_COMMAND(ID_AIRSIDEPOSITION_SETXYZ,OnEditPosition)
	ON_COMMAND(ID_AIRSIDEPOSITION_DELETE,OnDeletePosition)
END_MESSAGE_MAP()
BOOL CAirsideRunwayDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Runway"));
	}
	else
	{
		SetWindowText(_T("Define Runway"));
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


void CAirsideRunwayDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hWidth)
	{
		m_hRClickItem = hTreeItem;
		OnModifyWidth();
	}

	if(hTreeItem == m_hMarking1 || hTreeItem == m_hMarking2)
	{
		m_hRClickItem = hTreeItem;
		OnRunwaymarkModify();
	}	
	else if (hTreeItem == m_hMarking1LandingThreshold || 
		hTreeItem == m_hMarking2LandingThreshold ||
		hTreeItem == m_hMarking1TakeOffThreshold ||		
		hTreeItem == m_hMarking2TakeOffThreshold)
	{
		m_hRClickItem = hTreeItem;
		OnModifyThreshold();
	}

	CPath2008& path = ((Runway *)GetObject())->getPath();
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

void CAirsideRunwayDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
	//	pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
		pMenu->ModifyMenu(1,MF_BYPOSITION|MF_STRING,ID_PROCPROP_DEFINE_Z_POS, _T("Vertical profile"));
	}
	else if (m_hRClickItem == m_hWidth)
	{
		pMenu = menuPopup.GetSubMenu( 42 );
	}
	else if (m_hRClickItem == m_hMarking1 || m_hRClickItem == m_hMarking2)
	{
		pMenu = menuPopup.GetSubMenu(63);
	}
	else if (m_hRClickItem == m_hMarking1LandingThreshold || 
		m_hRClickItem == m_hMarking2LandingThreshold ||
		m_hRClickItem == m_hMarking1TakeOffThreshold ||		
		m_hRClickItem == m_hMarking2TakeOffThreshold)
	{
		pMenu = menuPopup.GetSubMenu( 80 );
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


LRESULT CAirsideRunwayDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hWidth)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Runway*)GetObject())->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hMarking1LandingThreshold)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Runway*)GetObject())->SetMark1LandingThreshold(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hMarking2LandingThreshold)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Runway*)GetObject())->SetMark2LandingThreshold(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hMarking1TakeOffThreshold)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Runway*)GetObject())->SetMark1TakeOffThreshold(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hMarking2TakeOffThreshold)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Runway*)GetObject())->SetMark2TakeOffThreshold(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		if(m_bPropModified)
			UpdateObjectViews();
	}

	if (message == WM_INPLACE_EDIT)
	{
		if (m_hRClickItem == m_hMarking1)
		{	
			CString strValue = *(CString*)lParam;
			((Runway*)GetObject())->SetMarking1((const char *)strValue);
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hMarking2)
		{		
			CString strValue = *(CString*)lParam;
			((Runway*)GetObject())->SetMarking2((const char *)strValue);
			m_bPropModified = true;
			LoadTree();
		}
		if(m_bPropModified)
			UpdateObjectViews();
	}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
void CAirsideRunwayDlg::OnRunwaymarkModify()
{
	CString strValue= m_treeProp.GetItemText(m_hRClickItem);
	m_treeProp.StringEditLabel(m_hRClickItem, strValue);
}

void CAirsideRunwayDlg::OnModifyWidth()
{

	double dWidth = ((Runway*)GetObject())->GetWidth();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dWidth )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideRunwayDlg::OnModifyThreshold()
{

	double dThreshold = 0.0;
	if (m_hRClickItem == m_hMarking1LandingThreshold)
	{
		dThreshold = ((Runway*)GetObject())->GetMark1LandingThreshold();
	}
	else if (m_hRClickItem == m_hMarking2LandingThreshold)
	{
		dThreshold = ((Runway*)GetObject())->GetMark2LandingThreshold();
	}
	else if (m_hRClickItem == m_hMarking1TakeOffThreshold)
	{
		dThreshold = ((Runway*)GetObject())->GetMark1TakeOffThreshold();
	}
	else if (m_hRClickItem == m_hMarking2TakeOffThreshold)
	{
		dThreshold = ((Runway*)GetObject())->GetMark2TakeOffThreshold();
	}
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dThreshold )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

FallbackReason CAirsideRunwayDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_TWOPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideRunwayDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Runway *)GetObject())->SetPath(path);
		m_bPathModified = true;
		autoCalRunwayMark(path);
		UpdateObjectViews();
		LoadTree();
	}
}

void CAirsideRunwayDlg::LoadTree()
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

	CPath2008 path = ((Runway *)GetObject())->GetPath();	
	
	//if (path.getCount() == 0)
	//{
	//	for (int n =0; n < DEFAULT_COUNT; n++)
	//	{
	//		CPoint2008 pt(0,0,0);
	//		CLatitude fixlatitude;
	//		CLongitude fixlongitude;
	//		ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

	//		aoidDataEx.lSize = sizeof(aoidDataEx);
	//		aoidDataEx.dwptrItemData = 0;
	//		aoidDataEx.vrItemStringSectionColorShow.clear();
	//		isscStringColor.colorSection = RGB(0,0,255);
	//		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
	//		isscStringColor.strSection = strTemp;
	//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
	//		isscStringColor.strSection = strTemp;
	//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
	//		isscStringColor.strSection = strTemp;
	//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//		CString strPoint = _T("");
	//		strPoint.Format(_T("x = %.2f; y = %.2f; z = %.2f"),
	//			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
	//			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
	//			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

	//		hItemTemp = m_treeProp.InsertItem(strPoint,m_hServiceLocation);
	//		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);

	//		//lat long
	//		CString strLL;
	//		CString strLat,strLong;
	//		fixlatitude.GetValue(strLat);
	//		fixlongitude.GetValue(strLong);

	//		strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

	//		aoidDataEx.lSize = sizeof(aoidDataEx);
	//		aoidDataEx.dwptrItemData = 0;
	//		aoidDataEx.vrItemStringSectionColorShow.clear();
	//		isscStringColor.colorSection = RGB(0,0,255); 		
	//		isscStringColor.strSection = strLat;
	//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//		isscStringColor.strSection = strLong;
	//		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//		HTREEITEM hFixLL= m_treeProp.InsertItem(strLL,m_hServiceLocation);
	//		m_treeProp.SetItemDataEx(hFixLL,aoidDataEx);
	//		m_vLatLongItems.push_back(hFixLL);
	//	}
	//}
	//else
	
	{//load path tree
		if(path.getCount()!=2)
		{
			path.init(2);
			path[0] = CPoint2008(0,0,0);
			path[1] = CPoint2008(0,0,0);
		}
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
	}

	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);

	//width
	csLabel.Empty();
	csLabel.Format(_T("Width ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetWidth()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetWidth()));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hWidth =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hWidth,aoidDataEx);

	//marking
	HTREEITEM hMarking = m_treeProp.InsertItem(_T("Marking"));

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	isscStringColor.strSection = ((Runway*)GetObject())->GetMarking1().c_str();
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hMarking1 = m_treeProp.InsertItem(((Runway*)GetObject())->GetMarking1().c_str(),hMarking);
	m_treeProp.SetItemDataEx(m_hMarking1,aoidDataEx);
	
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	isscStringColor.strSection = ((Runway*)GetObject())->GetMarking2().c_str();
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hMarking2 = m_treeProp.InsertItem(((Runway*)GetObject())->GetMarking2().c_str(),hMarking);
	m_treeProp.SetItemDataEx(m_hMarking2,aoidDataEx);

	//mark threshold
	CString strMark1(((Runway*)GetObject())->GetMarking1().c_str());
	CString strMark2(((Runway*)GetObject())->GetMarking2().c_str());
	//Landing threshold
	
	{
		CString strLandingText;
		strLandingText.Format(_T("Landing Threshold(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
		HTREEITEM hLandingThreshold = m_treeProp.InsertItem(strLandingText);
	
		csLabel.Empty();
		csLabel.Format(_T("%s(%.0f)"), 
			strMark1,CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark1LandingThreshold()));
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark1LandingThreshold()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hMarking1LandingThreshold = m_treeProp.InsertItem(csLabel,hLandingThreshold);
		m_treeProp.SetItemDataEx(m_hMarking1LandingThreshold,aoidDataEx);

		csLabel.Empty();
		csLabel.Format(_T("%s(%.0f)"), 
			strMark2,CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark2LandingThreshold()));
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark2LandingThreshold()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hMarking2LandingThreshold = m_treeProp.InsertItem(csLabel,hLandingThreshold);
		m_treeProp.SetItemDataEx(m_hMarking2LandingThreshold,aoidDataEx);
		m_treeProp.Expand(hLandingThreshold,TVE_EXPAND);
	}
	//TakeOff threshold

	{
		CString strTakeOffText;
		strTakeOffText.Format(_T("Take Off Threshold(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
		HTREEITEM hLTakeOffThreshold = m_treeProp.InsertItem(strTakeOffText);

		csLabel.Empty();
		csLabel.Format(_T("%s(%.0f)"), 
			strMark1,CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark1TakeOffThreshold()));
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark1TakeOffThreshold()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hMarking1TakeOffThreshold = m_treeProp.InsertItem(csLabel,hLTakeOffThreshold);
		m_treeProp.SetItemDataEx(m_hMarking1TakeOffThreshold,aoidDataEx);

		csLabel.Empty();
		csLabel.Format(_T("%s(%.0f)"), 
			strMark2,CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark2TakeOffThreshold()));
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Runway*)GetObject())->GetMark2TakeOffThreshold()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hMarking2TakeOffThreshold = m_treeProp.InsertItem(csLabel,hLTakeOffThreshold);
		m_treeProp.SetItemDataEx(m_hMarking2TakeOffThreshold,aoidDataEx);
		m_treeProp.Expand(hLTakeOffThreshold,TVE_EXPAND);
	}
	m_treeProp.Expand(hMarking,TVE_EXPAND);
}


void CAirsideRunwayDlg::OnOK()
{
	CPath2008 path = ((Runway *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the runway path from the map."));
		return;
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
		{
			((Runway *)GetObject())->UpdatePath();
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
bool CAirsideRunwayDlg::UpdateOtherData()
{
	CPath2008 path = ((Runway *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the runway path from the map."));
		return false;
	}
	//try
	//{
		if (m_bPathModified && m_nObjID != -1)
		{
			((Runway *)GetObject())->UpdatePath();
		}
	//}
	//catch (CADOException &e)
	//{
	//	e.ErrorMessage();
	//	MessageBox(_T("Cann't update the Object path."));
	//	return false;
	//}
	return true;
}


void CAirsideRunwayDlg::autoCalRunwayMark( CPath2008& path )
{
	if(path.getCount()<2)return ;
	ARCVector2 vnorth (0,1);
	ARCVector3 vrunway3 = path.getPoint(1)-path.getPoint(0);
	ARCVector2 vrunway2(vrunway3[VX],vrunway3[VY]);
	double dangle =  (vrunway2.GetAngleOfTwoVector(vnorth));
	//ARCVector3 vP = vnorth^vrunway2;
	/*if(vP[VZ]<0){
		dangle = -dangle;
	}*/	

	
	dangle -= m_airportInfo.GetMagnectVariation().GetRealVariation();	

	if(dangle <= 0 )dangle += 360;

	double dAngle1 = dangle;
	double dAngle2 = ( dangle + 180 );
	
	
		
	CString strMark1,strMark2;
	int iAngle1 = ( (int)(dAngle1/10.0 + 0.5) ) % 36;
	int iAngle2 = ((int)(dAngle2/10.0 + 0.5) )% 36;	

	strMark1.Format("%d",iAngle1 );
	strMark2.Format("%d" , iAngle2);

	Runway * pRunway = (Runway*)GetObject();
	
	pRunway->SetMarking1((strMark1).GetBuffer());
	pRunway->SetMarking2((strMark2).GetBuffer());
	m_bPropModified = true;
}

bool CAirsideRunwayDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideRunwayDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideRunwayDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideRunwayDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
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
void CAirsideRunwayDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
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

void CAirsideRunwayDlg::OnEditVerticalProfile(void)
{
	std::vector<double>vXYPos;
	std::vector<double>vZPos;

	CPath2008& path = ((Runway*)GetObject())->getPath();
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

void CAirsideRunwayDlg::OnEditLatLong()
{
	CPath2008& path = ((Runway *)GetObject())->getPath();
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
				break;
			}
		}
	}
}

void CAirsideRunwayDlg::OnEditPosition()
{
	CPath2008& path = ((Runway *)GetObject())->getPath();
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

void CAirsideRunwayDlg::OnDeleteLatLog()
{
	CPath2008& path = ((Runway *)GetObject())->getPath();
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

void CAirsideRunwayDlg::OnDeletePosition()
{
	CPath2008& path = ((Runway *)GetObject())->getPath();
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