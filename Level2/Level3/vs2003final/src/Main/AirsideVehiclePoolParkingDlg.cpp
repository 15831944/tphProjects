#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\AirsideVehiclePoolParkingDlg.h"
#include "../common\WinMsg.h"
#include "LayoutObjectPropDlgImpl.h"

IMPLEMENT_DYNAMIC(CAirsideVehiclePoolParkingDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideVehiclePoolParkingDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
END_MESSAGE_MAP()
CAirsideVehiclePoolParkingDlg::CAirsideVehiclePoolParkingDlg(int nVehiclePoolParkingID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nVehiclePoolParkingID,nAirportID, nProjID,pParent),m_vLevelList(nAirportID)
{
	m_pObject = new VehiclePoolParking();
	m_pObject->setID(nVehiclePoolParkingID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

//CAirsideVehiclePoolParkingDlg::CAirsideVehiclePoolParkingDlg(VehiclePoolParking* pVehiclePoolParking,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pVehiclePoolParking, nProjID,pParent),m_vLevelList(pVehiclePoolParking?pVehiclePoolParking->getAptID():-1)
//{
//	if(pVehiclePoolParking)
//		m_path = pVehiclePoolParking->GetPath();
//	m_bPathModified = false;
//}

CAirsideVehiclePoolParkingDlg::~CAirsideVehiclePoolParkingDlg(void)
{
	m_path.clear();
}

bool CAirsideVehiclePoolParkingDlg::DoTempFallBackFinished(WPARAM wParam, LPARAM lParam)
{
	if(m_hRClickItem == m_hCtrlPoints)
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
	}
	if (m_hRClickItem == m_hParkSpaces)
	{
		std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
		size_t nSize = pData->size();
		ParkingSpace space;
		space.m_ctrlPath.init(nSize);	
		for (size_t i =0; i < nSize; ++i)
		{
			ARCVector3 v3 = pData->at(i);
			space.m_ctrlPath[i].setPoint(v3[VX] ,v3[VY],0.0);
		}
		VehiclePoolParking *poolParking = ((VehiclePoolParking *)GetObject());
		poolParking->m_parkspaces.AddSpace(space);
		LoadTree();
		UpdateObjectViews();
	}
	if (m_hRClickItem == m_hDriveLanes)
	{
		std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
		size_t nSize = pData->size();
		ParkingDrivePipe pipe;
		pipe.m_ctrlPath.init(nSize);	
		for (size_t i =0; i < nSize; ++i)
		{
			ARCVector3 v3 = pData->at(i);
			pipe.m_ctrlPath[i].setPoint(v3[VX] ,v3[VY],0.0);
		}
		VehiclePoolParking *poolParking = ((VehiclePoolParking *)GetObject());
		poolParking->m_dirvepipes.AddPipe(pipe);
		LoadTree();
		UpdateObjectViews();
	}
	
	return true;
}

void CAirsideVehiclePoolParkingDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;
	AirsideObjectTreeCtrlItemDataEx* pParentData = m_treeProp.GetItemDataEx(hTreeItem);
	if (pParentData->nSubType == _PSType)
	{
		OnSpotTypeCombo(hTreeItem);
	}
	if (pParentData->nSubType == _PSWidth)
	{
		OnSpotWidthEdit(hTreeItem);
	}
	if (pParentData->nSubType == _PSLength)
	{
		OnSpotLengthEdit(hTreeItem);
	}
	if (pParentData->nSubType == _PSAngle)
	{
		OnSpotAngleEdit(hTreeItem);
	}
	if (pParentData->nSubType == _PSOperation)
	{
		OnSpotOpTypeCombo(hTreeItem);
	}
	if (pParentData->nSubType == _PPWidth)
	{
		OnDrivePathWidth(hTreeItem);
	}
	if (pParentData->nSubType == _PPLaneNum)
	{
		OnDrivePathLaneNum(hTreeItem);
	}
	if (pParentData->nSubType == _PPDirection)
	{
		OnDrivePathDir(hTreeItem);
	}
}

void CAirsideVehiclePoolParkingDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL CAirsideVehiclePoolParkingDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/ 0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify VehiclePoolParking"));
	}
	else
	{
		SetWindowText(_T("Define VehiclePoolParking"));
	}

	if(GetObject()->getID() != -1)
	{
		((VehiclePoolParking *)GetObject())->ReadObject(GetObject()->getID());
		m_path = ((VehiclePoolParking *)GetObject())->GetPath();
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_7,this);
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
void CAirsideVehiclePoolParkingDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints|| m_hRClickItem == m_hParkSpaces||
		m_hRClickItem == m_hDriveLanes)
	{
		pMenu=menuPopup.GetSubMenu(73);
		//pMenu=menuPopup.GetSubMenu(100);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else
	{	
		AirsideObjectTreeCtrlItemDataEx* pParentData = m_treeProp.GetItemDataEx(m_hRClickItem);
		if( pParentData->nSubType == _ParkSpace||pParentData->nSubType == _ParkDrivePipe )
		{
			pMenu=menuPopup.GetSubMenu(100);
		}
	}
}


LRESULT CAirsideVehiclePoolParkingDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	if (message == WM_INPLACE_COMBO2)
	{
		VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
		AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(m_hRClickItem);
		if(!pItemData)
			return false;
		int selitem = m_treeProp.m_comboBox.GetCurSel();	
		if(pItemData->nSubType ==  _PSType)
		{			
			ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
			ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
			selitem = MAX(ParkingSpace::_perpendicular,selitem);
			selitem = MIN(ParkingSpace::_angle,selitem);
			space.m_type = (ParkingSpace::ParkType)selitem;
			CString strText;
			strText.Format("Spot Type: %s",space.GetTypeString());
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		if (pItemData->nSubType == _PSOperation)
		{
			ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
			selitem = MAX(ParkingSpace::_backup,selitem);
			selitem = MIN(ParkingSpace::_through,selitem);
			ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
			space.m_opType = (ParkingSpace::OperationType)selitem;
			CString strText;
			strText.Format("Operation: %s",space.GetOpTypeString());
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		
		if (pItemData->nSubType == _PPDirection)
		{
			selitem += 1;
			ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
			selitem = MAX(ParkingDrivePipe::_directional,selitem);
			selitem = MIN(ParkingDrivePipe::_bidirectional,selitem);
			ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
			pipe.m_nType = (ParkingDrivePipe::dirtype)(selitem);
			CString strText;
			strText.Format("Direction: %s",pipe.getDirectionTypeStr());
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
	}
	if (message == WM_INPLACE_SPIN)
	{
		VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
		AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(m_hRClickItem);
		if(!pItemData)
			return false;
		if (pItemData->nSubType == _PSWidth)
		{
			ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
			ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			space.m_dWidth = dValue;
			ARCUnit_Length  curLengthUnit = CUnitPiece::GetCurSelLengthUnit();
			CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
			CString strText;
			strText.Format("Spot Width(%s): %.2lf",curLengthUnitString,ConvertLength(dValue));
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		if (pItemData->nSubType == _PSLength)
		{	
			ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
			ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			space.m_dLength = dValue;
			ARCUnit_Length  curLengthUnit = CUnitPiece::GetCurSelLengthUnit();
			CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
			CString strText;
			strText.Format("Spot Length(%s): %.2lf",curLengthUnitString,ConvertLength(dValue));
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		if (pItemData->nSubType == _PSAngle)
		{
			ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
			ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = pst->iPercent;
			space.m_dAngle = dValue;
			CString strText;
			strText.Format("Spot Angle: %.2lf(degree)",dValue);
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		if (pItemData->nSubType == _PPWidth)
		{		
			ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
			ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			pipe.m_width = dValue;
			ARCUnit_Length  curLengthUnit = CUnitPiece::GetCurSelLengthUnit();
			CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
			CString strText;
			strText.Format("Pipe Width(%s): %.2lf",curLengthUnitString,ConvertLength(dValue));
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
		if (pItemData->nSubType == _PPLaneNum)
		{
			ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
			ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int dValue = pst->iPercent;
			pipe.m_nLaneNum = dValue;
			CString strText;
			strText.Format("Lane Num: %d",dValue);
			m_treeProp.SetItemText(m_hRClickItem,strText);
		}
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideVehiclePoolParkingDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideVehiclePoolParkingDlg::SetObjectPath(CPath2008& path)
{
	((VehiclePoolParking *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
	UpdateObjectViews();
}

void CAirsideVehiclePoolParkingDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
{
	m_vLevelList.ReadFullLevelList();
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		CString csPoint;
		double FloorAlt = path.getPoint(i).getZ()/SCALE_FACTOR;//floors.getVisibleFloorAltitude(  ) ;
		csPoint.Format("x = %.2f; y = %.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ), \
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getY() ), FloorAlt);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getX() ));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getY() ));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}	

	///////////////////////////////
}

void CAirsideVehiclePoolParkingDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	// control points.
	strLabel = CString( "Area Vertices(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

    ////////////////////////////////////////////////////
	ARCUnit_Length  curLengthUnit = CUnitPiece::GetCurSelLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;
	VehiclePoolParking *poolParking = (VehiclePoolParking *)GetObject();
	//parking spaces
	m_hParkSpaces = m_treeProp.InsertItem("Parking Space");
	for (size_t i=0;i<poolParking->m_parkspaces.m_spaces.size();i++)
	{
		//Parking Spcae
		const ParkingSpace& parkspace = poolParking->m_parkspaces.m_spaces[i];
		CString sPark;sPark.Format("Parking Space ( %d )",i+1);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _ParkSpace;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		HTREEITEM hSpaceItem = m_treeProp.InsertItem(sPark,m_hParkSpaces);
		m_treeProp.SetItemDataEx(hSpaceItem,aoidDataEx);
		//Path
		CString sPath = "Path (" + curLengthUnitString + ")";
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSPath;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		HTREEITEM hPathItem = m_treeProp.InsertItem(sPath,hSpaceItem);
		m_treeProp.SetItemDataEx(hPathItem,aoidDataEx);
		const CPath2008& path = parkspace.m_ctrlPath;
		int iPathCount = path.getCount();
		for(int j = 0;j < iPathCount; j++)
		{
			CString csPoint;
			double FloorAlt = path.getPoint(j).getZ();//m_pt.getZ();//floors.getVisibleFloorAltitude() ;

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(j).getX()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(j).getY()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),FloorAlt);		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(j).getX() ), \
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(j).getY() ), FloorAlt);
			hItemTemp = m_treeProp.InsertItem( csPoint, hPathItem  );
			m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
		}
		//m_treeProp.Expand(hPathItem,TVE_EXPAND);

		//Spot Type
		CString sType;sType = "Spot Type:"+parkspace.GetTypeString();
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSType;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = parkspace.GetTypeString();
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hTypeItem = m_treeProp.InsertItem(sType,hSpaceItem);
		m_treeProp.SetItemDataEx(hTypeItem,aoidDataEx);

		//Spot Width
		strTemp.Format("%.2lf",ConvertLength(parkspace.m_dWidth));
		CString sWidth;sWidth ="Spot Width (" + curLengthUnitString + "):"+strTemp;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSWidth;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hWidthItem = m_treeProp.InsertItem(sWidth,hSpaceItem);
		m_treeProp.SetItemDataEx(hWidthItem,aoidDataEx);

		//Spot Length
		strTemp.Format("%.2lf",ConvertLength(parkspace.m_dLength));
		CString sLength;sLength = "Spot Length (" + curLengthUnitString + "):"+strTemp;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSLength;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hLengthItem = m_treeProp.InsertItem(sLength,hSpaceItem);
		m_treeProp.SetItemDataEx(hLengthItem,aoidDataEx);

		//Spot Angle
		strTemp.Format("%.2lf",parkspace.m_dAngle);
		CString sAngle;sAngle = "Spot Angle:" + strTemp + "(degree)";
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSAngle;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hAngleItem = m_treeProp.InsertItem(sAngle,hSpaceItem);
		m_treeProp.SetItemDataEx(hAngleItem,aoidDataEx);

		//Operation
		CString sOperation;sOperation = "Operation:"+parkspace.GetOpTypeString();
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PSOperation;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = parkspace.GetOpTypeString();
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hOperationItem = m_treeProp.InsertItem(sOperation,hSpaceItem);
		m_treeProp.SetItemDataEx(hOperationItem,aoidDataEx);
	}
	m_treeProp.Expand(m_hParkSpaces,TVE_EXPAND);

	//drive pipes
	m_hDriveLanes = m_treeProp.InsertItem("Parking DriveLanes");
	for (size_t i=0;i<poolParking->m_dirvepipes.m_pipes.size();i++)
	{
		//Parking DrivePipe
		const ParkingDrivePipe& parkpipe = poolParking->m_dirvepipes.m_pipes[i];
		CString sPipe;sPipe.Format("Parking DrivePipe( %d ):",i+1);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _ParkDrivePipe;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		HTREEITEM hPipeItem = m_treeProp.InsertItem(sPipe,m_hDriveLanes);
		m_treeProp.SetItemDataEx(hPipeItem,aoidDataEx);

		//Path
		CString sPath = "Control Point (" + curLengthUnitString + ")";
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PPPath;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		HTREEITEM hPathItem = m_treeProp.InsertItem(sPath,hPipeItem);
		m_treeProp.SetItemDataEx(hPathItem,aoidDataEx);
		const CPath2008& path = parkpipe.m_ctrlPath;
		int iPathCount = path.getCount();
		for(int j = 0;j < iPathCount; j++)
		{
			CString csPoint;
			double FloorAlt = path.getPoint(j).getZ();//m_pt.getZ();//floors.getVisibleFloorAltitude() ;

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(j).getX()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(j).getY()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),FloorAlt);		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(j).getX() ), \
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(j).getY() ), FloorAlt);
			hItemTemp = m_treeProp.InsertItem( csPoint, hPathItem  );
			m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
		}
		//Width
		strTemp.Format("%.2lf",ConvertLength(parkpipe.m_width));
		CString sWidth;sWidth = "Pipe Width (" + curLengthUnitString + "):" + strTemp;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PPWidth;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hWidthItem = m_treeProp.InsertItem(sWidth,hPipeItem);
		m_treeProp.SetItemDataEx(hWidthItem,aoidDataEx);

		//Number
		strTemp.Format("%d",parkpipe.m_nLaneNum);
		CString sNum;sNum = "Lane Num:"+strTemp;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PPLaneNum;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hNumItem = m_treeProp.InsertItem(sNum,hPipeItem);
		m_treeProp.SetItemDataEx(hNumItem,aoidDataEx);

		//Direction
		CString sDir;sDir = "Direction:" + parkpipe.getDirectionTypeStr();
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.nSubType = _PPDirection;
		aoidDataEx.dwptrItemData = i;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		isscStringColor.strSection = parkpipe.getDirectionTypeStr();
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		HTREEITEM hDirItem = m_treeProp.InsertItem(sDir,hPipeItem);
		m_treeProp.SetItemDataEx(hDirItem,aoidDataEx);

	}
	m_treeProp.Expand(m_hDriveLanes,TVE_EXPAND);
	////vertical profile.
	//strLabel = _T("Vertical Profile");
	//m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	m_treeProp.SetRedraw(1);
}


void CAirsideVehiclePoolParkingDlg::OnOK(void)
{
	if(GetObject()&&m_nObjID!=-1)
	{
		VehiclePoolParking *poolParking = (VehiclePoolParking *)GetObject();
		CPath2008 path = poolParking->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the VehiclePoolParking path from the map."));
			return ;
		}		
		poolParking->SetPath(m_path);
		poolParking->UpdateObject(m_nObjID);
	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideVehiclePoolParkingDlg::UpdateOtherData(void)
{
	CPath2008 path = ((VehiclePoolParking *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the VehiclePoolParking path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
			((VehiclePoolParking *)GetObject())->UpdateObject(m_nObjID);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void CAirsideVehiclePoolParkingDlg::OnEditVerticalProfile(void)
{
	// get every floor's real height
	//CTermPlanDoc* pDoc	= GetDocument();
	//CFloors & floors = pDoc->GetLandsideDoc()->GetLevels();
	//int nFloorCount = pDoc->GetLandsideDoc()->GetLevels().m_vFloors.size();
	//std::vector<double>vFloorAltitudes;
	//
	//for(int i=0; i<nFloorCount; i++) 
	//{
	//	vFloorAltitudes.push_back( i );				
	//}

	////get x (distance from point 0) and z
	//std::vector<double>vXYPos;
	//std::vector<double>vZPos;
	//	
	//CPath2008 tmpPath =m_vCtrlPoints.getPath();	
	//if( tmpPath.getCount() < 2)
	//{
	//	return;
	//}
	////adjust path
	////AdjustZVal(tmpPath);

	//vXYPos.reserve(tmpPath.getCount());
	//vZPos.reserve(tmpPath.getCount());		
	//vXYPos.push_back(0.0); 
	//vZPos.push_back( tmpPath.getPoint(0).getZ() /SCALE_FACTOR);
	//
	//double dLensoFar = 0.0;
	//
	//for(int i=1;i<tmpPath.getCount();i++)
	//{
	//	ARCVector3 v3D(tmpPath[i].getX()-tmpPath[i-1].getX(), tmpPath[i].getY()-tmpPath[i-1].getY(), 0.0);
	//	dLensoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
	//	vXYPos.push_back(dLensoFar);				
	//	vZPos.push_back( tmpPath[i].getZ() /SCALE_FACTOR );
	//}

	//CStringArray strArrFloorName;
	//strArrFloorName.SetSize(nFloorCount);
	//for(int k=0;k<nFloorCount;k++)
	//{
	//	strArrFloorName[k] = pDoc->GetLandsideDoc()->GetLevels().m_vFloors[k]->FloorName();
	//}
	//CChangeVerticalProfileDlg dlg(vXYPos,vZPos,vFloorAltitudes,tmpPath,&strArrFloorName);
	//

	//if( dlg.DoModal() == IDOK )
	//{	
	//	ASSERT(tmpPath.getCount() == (int) vZPos.size());
	//	vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
	//	for(size_t i=0;i<m_vCtrlPoints.size();i++)
	//	{
	//		m_vCtrlPoints[i][VZ] = vZPos[i] * SCALE_FACTOR ;//pDoc->GetLandsideDoc()->GetLevels().getVisibleAltitude( vZPos[i] * SCALE_FACTOR ) ;
	//	}
	//	
	//	LoadTree();
	//}
}

bool CAirsideVehiclePoolParkingDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideVehiclePoolParkingDlg::RefreshGUI(void)
{
	LoadTree();
	UpdateObjectViews();
	return (true);
}

bool CAirsideVehiclePoolParkingDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideVehiclePoolParkingDlg::OnProcpropDelete()
{
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(m_hRClickItem);
	if(!pItemData)
		return;	
	if(pItemData->nSubType == _ParkSpace)
	{			
		ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
		spaceList.RemoveSpace(pItemData->dwptrItemData);
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE * FROM VEHICLEPARKINGSPACE WHERE parentid = %d"),m_nObjID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	if (pItemData->nSubType == _ParkDrivePipe)
	{
		ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
		pipeList.RemovePipe(pItemData->dwptrItemData);
	//	ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE * FROM VEHICLEDRIVEPIPE WHERE parentid = %d"),m_nObjID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	LoadTree();
	UpdateObjectViews();
}

void CAirsideVehiclePoolParkingDlg::OnSpotTypeCombo(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);

	std::vector<CString> vString;
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_perpendicular));
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_parallel));
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_angle));

	m_treeProp.SetComboWidth(300);
	m_treeProp.SetComboString(hItem,vString);
	m_treeProp.m_comboBox.SetCurSel(space.m_type);
}

void CAirsideVehiclePoolParkingDlg::OnSpotWidthEdit(HTREEITEM hItem)
{
	
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);

	double dUnitNum = ConvertLength(space.m_dWidth);
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,10000);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hItem );
}

void CAirsideVehiclePoolParkingDlg::OnSpotLengthEdit(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
	
	double dUnitNum = ConvertLength(space.m_dLength);	
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,10000);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hItem );
}

void CAirsideVehiclePoolParkingDlg::OnSpotAngleEdit(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
	
	double dUnitNum = space.m_dAngle;
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(-90,90);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hItem );
}

void CAirsideVehiclePoolParkingDlg::OnSpotOpTypeCombo(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingLotParkingSpaceList& spaceList = pLot->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->dwptrItemData);
	
	std::vector<CString> vString;
	vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_backup));
	vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_through));

	m_treeProp.SetComboWidth(300);
	m_treeProp.SetComboString(hItem,vString);
	m_treeProp.m_comboBox.SetCurSel(space.m_type);

}

void CAirsideVehiclePoolParkingDlg::OnDrivePathWidth(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
	ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
	
	double dUnitNum = ConvertLength(pipe.m_width);	
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(1,10000);
	m_treeProp.SetDisplayNum(int(dUnitNum+.5));	
	m_treeProp.SpinEditLabel( hItem );
}
void CAirsideVehiclePoolParkingDlg::OnDrivePathLaneNum(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
	ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);
	
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(1,10000);
	m_treeProp.SetDisplayNum(pipe.m_nLaneNum);	
	m_treeProp.SpinEditLabel( hItem );
}

void CAirsideVehiclePoolParkingDlg::OnDrivePathDir(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx* pItemData = m_treeProp.GetItemDataEx(hItem);
	if(!pItemData)
		return;
	VehiclePoolParking* pLot = (VehiclePoolParking *)GetObject();
	ParkingDrivePipeList& pipeList = pLot->m_dirvepipes;
	ParkingDrivePipe& pipe = pipeList.getPipe(pItemData->dwptrItemData);

	std::vector<CString> vString;
	vString.push_back(ParkingDrivePipe::getDirectionTypeStr(ParkingDrivePipe::_directional));
	vString.push_back(ParkingDrivePipe::getDirectionTypeStr(ParkingDrivePipe::_bidirectional));
	
	m_treeProp.SetComboWidth(300);
	m_treeProp.SetComboString(hItem,vString);
	m_treeProp.m_comboBox.SetCurSel(pipe.m_nType);

}