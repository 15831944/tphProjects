#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidesectordlg.h"
#include "../Common/WinMsg.h"
#include "../InputAirside/AirSector.h"
#include "../Common/LatLongConvert.h"
#include "DlgInitArp.h"

IMPLEMENT_DYNAMIC(CAirsideSectorDlg, CAirsideObjectBaseDlg)
CAirsideSectorDlg::CAirsideSectorDlg(int nSectorID,int nAirportID,int nProjID,CWnd* pParent)
:CAirsideObjectBaseDlg(nSectorID, nAirportID,nProjID,pParent)
{
	m_pObject = new AirSector;
	m_bPathModified = false;
	m_iTransfer = 0;
}

//CAirsideSectorDlg::CAirsideSectorDlg( AirSector* pSector,int nProjID, CWnd* pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pSector, nProjID,pParent)
//{
//	m_bPathModified = false;
//}
CAirsideSectorDlg::~CAirsideSectorDlg(void)
{
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideSectorDlg,CAirsideObjectBaseDlg)
ON_COMMAND(ID_SECTOR_MODIFYLOWALTITUDE, OnModifyLowAltitude)
ON_COMMAND(ID_SECTOR_MODIFYHIGHALTITUDE, OnModifyHighAltitude)
ON_COMMAND(ID_SECTOR_MODIFYBANDINTERVAL, OnModifyBandInterval)
ON_COMMAND(ID_PROCPROP_CHANGLATLONG, OnPropModifyLatLong)	
END_MESSAGE_MAP()

BOOL CAirsideSectorDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SECTOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Sector"));
	}
	else
	{
		SetWindowText(_T("Define Sector"));
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_14,this);
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

void CAirsideSectorDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{

	if(hTreeItem == m_hLowAlt)
	{
		m_hRClickItem = hTreeItem;
		OnModifyLowAltitude();
	}

	if(hTreeItem == m_hHighAlt)
	{
		m_hRClickItem = hTreeItem;
		OnModifyHighAltitude();
	}

	if(hTreeItem == m_hInterval)
	{
		m_hRClickItem = hTreeItem;
		OnModifyBandInterval();
	}
	//if (hTreeItem == m_hFixLL)
	//{
 //       m_hRClickItem = hTreeItem;
	//	OnPropModifyLatLong();
	//}
	for (int i=0; i<static_cast<int>(m_hFixLList.size()); i++)
	{
		if(hTreeItem == m_hFixLList[i])
		{
			m_hRClickItem = hTreeItem;
			m_iTransfer = i;
			OnPropModifyLatLong();
			break;
		}
	}
}

void CAirsideSectorDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
	}
	//else if (m_hFixLL == m_hRClickItem)
	//{
	//	pMenu=menuPopup.GetSubMenu(48);
	//}
	else if (m_hRClickItem == m_hHighAlt)
	{
		pMenu = menuPopup.GetSubMenu( 67 );
		pMenu->DeleteMenu(ID_SECTOR_MODIFYLOWALTITUDE ,MF_BYCOMMAND);	
		pMenu->DeleteMenu(ID_SECTOR_MODIFYBANDINTERVAL ,MF_BYCOMMAND);	

	}
	else if (m_hRClickItem == m_hInterval)
	{
		pMenu = menuPopup.GetSubMenu(67);	
		pMenu->DeleteMenu(ID_SECTOR_MODIFYLOWALTITUDE ,MF_BYCOMMAND);		
		pMenu->DeleteMenu(ID_SECTOR_MODIFYHIGHALTITUDE ,MF_BYCOMMAND);	
	}
	else if (m_hRClickItem == m_hLowAlt)
	{

		pMenu = menuPopup.GetSubMenu(67);
		pMenu->DeleteMenu(ID_SECTOR_MODIFYHIGHALTITUDE ,MF_BYCOMMAND);	
		pMenu->DeleteMenu(ID_SECTOR_MODIFYBANDINTERVAL ,MF_BYCOMMAND);	
	}

	for(int i=0; i<static_cast<int>(m_hFixLList.size()); i++)
	{
		if(m_hRClickItem == m_hFixLList[i])
		{
			m_iTransfer = i;
			pMenu = menuPopup.GetSubMenu(48);
			break;
		}
	}

}
void CAirsideSectorDlg::OnModifyLowAltitude()
{
	double dAlt = ((AirSector *)GetObject())->GetLowAltitude();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dAlt )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideSectorDlg::OnModifyHighAltitude()
{	
	double dAlt = ((AirSector *)GetObject())->GetHighAltitude();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dAlt )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideSectorDlg::OnModifyBandInterval()
{	
	double dAlt = ((AirSector *)GetObject())->GetBandInterval();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dAlt )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
LRESULT CAirsideSectorDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hLowAlt)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirSector*)GetObject())->SetLowAltitude( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;

			LoadTree();
		}
		else if (m_hHighAlt == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirSector*)GetObject())->SetHighAltitude( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hInterval == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((AirSector*)GetObject())->SetBandInterval( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}

	}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideSectorDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideSectorDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((AirSector *)GetObject())->SetPath(path);
		m_bPathModified = true;
		LoadTree();
	}
}

void CAirsideSectorDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	// Service Location
	//CString csLabel = CString("Service Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	//m_hServiceLocation = m_treeProp.InsertItem( csLabel );

	//AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	//ItemStringSectionColor isscStringColor;
	//CString strTemp = _T("");
	//HTREEITEM hItemTemp = 0;

	//CPath2008 path = ((AirSector *)GetObject())->GetPath();
	//for (int i =0; i <path.getCount();++i)
	//{
	//	CPoint2008 pt = path.getPoint(i);

	//	aoidDataEx.lSize = sizeof(aoidDataEx);
	//	aoidDataEx.dwptrItemData = 0;
	//	aoidDataEx.vrItemStringSectionColorShow.clear();
	//	isscStringColor.colorSection = RGB(0,0,255);
	//	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
	//	isscStringColor.strSection = strTemp;
	//	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
	//	isscStringColor.strSection = strTemp;
	//	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
	//	isscStringColor.strSection = strTemp;
	//	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//	CString strPoint = _T("");
	//	strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
	//		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
	//		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
	//		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

	//	hItemTemp =  m_treeProp.InsertItem(strPoint,m_hServiceLocation);
	//	m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	//}

	//m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND); 

	CString strTemp = _T("");
	CString strVertices = _T("");
	HTREEITEM hItemTemp = 0;
	int  numberVertice = 0;
	m_hFixLList.clear();

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;

	CPath2008 path = ((AirSector *)GetObject())->GetPath();
	m_fixlatitude.resize(path.getCount());
	m_fixlongitude.resize(path.getCount());
	if (path.getCount()>0)
	{
		numberVertice = path.getCount();
	}



    aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData =0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection =RGB(0,0,255);
    strTemp.Format(_T("%d"),numberVertice);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	CString csLabel = _T("");
	csLabel.Format(_T("Number of Vertices  %d"),numberVertice);
	//CString csLabel = CString("Number of Vertices"+strTemp);
	m_hServiceLocation = m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hServiceLocation,aoidDataEx);



	for(int i=0; i<numberVertice; i++)
	{
		//Vertice
		strTemp.Empty();
		strVertices.Empty();
		//strTemp.Format(_T("  %d"),i+1);
		//strVertices = CString("Vertex"+strTemp);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
        strVertices.Format(_T("Vertex  %d"),i+1);
		//strTemp.Format(_T("%d"),i+1);
        isscStringColor.strSection = strVertices;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		//strVertices.Format("Vertex  %d",i+1);
        m_hVertices = m_treeProp.InsertItem(strVertices,m_hServiceLocation);
		m_treeProp.SetItemDataEx(m_hVertices,aoidDataEx);

		//CPoint2008
        CPoint2008 pt = path.getPoint(i);
        ConvertPosToLatLong(pt,m_fixlatitude[i],m_fixlongitude[i]);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		//isscStringColor.strSection = strTemp;
		//aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		//isscStringColor.strSection = strTemp;
		//aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		//isscStringColor.strSection = strTemp;
		//aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));
		isscStringColor.strSection = strPoint;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp =  m_treeProp.InsertItem(strPoint,m_hVertices);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);

       //latitude and longitude

       CString strLat,strLong;
	   m_fixlatitude[i].GetValue(strLat);
	   m_fixlongitude[i].GetValue(strLong);
	   strTemp.Empty();

	   aoidDataEx.lSize = sizeof(aoidDataEx);
	   aoidDataEx.dwptrItemData = 0;
	   aoidDataEx.vrItemStringSectionColorShow.clear();
	   isscStringColor.colorSection = RGB(0,0,255);
	   //isscStringColor.strSection = strLat;
	   //aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	   //isscStringColor.strSection = strLong;
	   //aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	   strTemp.Format("Latitude = %s; Longitude = %s",strLat,strLong);
	   isscStringColor.strSection = strTemp;
	   aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	   m_hFixLL = m_treeProp.InsertItem(strTemp,m_hVertices);
	   m_hFixLList.push_back(m_hFixLL);
	   m_treeProp.SetItemDataEx(m_hFixLL,aoidDataEx);  	
	}

	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND); 
    

	//low altitude
	csLabel.Empty();
	csLabel.Format(_T("Low Altitude ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), \
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetLowAltitude()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetLowAltitude()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLowAlt =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hLowAlt,aoidDataEx);

	//High alt
	csLabel.Empty();
	csLabel.Format(_T("High Altitude ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), \
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetHighAltitude()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetHighAltitude()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hHighAlt =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hHighAlt,aoidDataEx);


	//Interval
	csLabel.Empty();
	csLabel.Format(_T("Band Interval ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"),\
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetBandInterval()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((AirSector*)GetObject())->GetBandInterval()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hInterval =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hInterval,aoidDataEx);
}

void CAirsideSectorDlg::OnOK()
{
	CAirsideObjectBaseDlg::OnOK();
}
bool CAirsideSectorDlg::UpdateOtherData()
{	
	CPath2008 path = ((AirSector *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the AirSector path from the map."));
		return false;
	}

	if (m_bPathModified && m_nObjID != -1)
	{
		((AirSector*)GetObject())->UpdatePath();
	}
	return true;
}

bool CAirsideSectorDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideSectorDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideSectorDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideSectorDlg::OnPropModifyLatLong()
{
	CDlgInitArp initArp;	
	CString m_ctrLatitude, m_ctrLongitude;
	CPath2008 path = ((AirSector*)GetObject())->GetPath(); 

	m_fixlatitude[m_iTransfer].GetValue(initArp.m_strLatitude);
	m_fixlongitude[m_iTransfer].GetValue(initArp.m_strLongitude);

	m_fixlatitude[m_iTransfer].GetValue(m_ctrLatitude,FALSE);
	m_fixlongitude[m_iTransfer].GetValue(m_ctrLongitude,FALSE);
		
	if (initArp.DoModal()==IDOK)
	{
		m_fixlatitude[m_iTransfer].SetValue(initArp.m_strLatitude,m_ctrLatitude);
		m_fixlongitude[m_iTransfer].SetValue(initArp.m_strLongitude,m_ctrLongitude);

		ConvertLatLongToPos(m_fixlatitude[m_iTransfer],m_fixlongitude[m_iTransfer],path[m_iTransfer]);    
   

		((AirSector*)GetObject())->SetPath(path);
		m_bPropModified = true;
		m_bPathModified = true;

    	LoadTree();
	}      
}

void CAirsideSectorDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
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
void CAirsideSectorDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
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


