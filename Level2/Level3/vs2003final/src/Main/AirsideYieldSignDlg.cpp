#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "AirsideYieldSignDlg.h"
#include "../common\WinMsg.h" 
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/Stretch.h"
#include "../Common/DistanceLineLine.h"

IMPLEMENT_DYNAMIC(CAirsideYieldSignDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideYieldSignDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_YIELDSIGNLINKSTRETCH_INTERSECTION, OnSelectYieldSignIntersection)
END_MESSAGE_MAP()
CAirsideYieldSignDlg::CAirsideYieldSignDlg(int nYieldSignID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nYieldSignID,nAirportID, nProjID,pParent),m_vLevelList(nAirportID)
{
	m_pObject = new YieldSign();
	m_pObject->setID(nYieldSignID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
	m_nAirportID = nAirportID;
	m_nIntersect = -1;
	m_strName = _T("");
	m_nStretchID = -1;
	m_strStretchName = _T("");
	m_degree = 0.0;
}

//CAirsideYieldSignDlg::CAirsideYieldSignDlg(YieldSign* pYieldSign,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pYieldSign, nProjID,pParent),m_vLevelList(pYieldSign?pYieldSign->getAptID():-1)
//{
//	if(pYieldSign)
//		m_path = pYieldSign->GetPath();
//	m_bPathModified = false;
//}

CAirsideYieldSignDlg::~CAirsideYieldSignDlg(void)
{
	m_path.clear();
}
void CAirsideYieldSignDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL CAirsideYieldSignDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/ 0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify YieldSign"));
	}
	else
	{
		SetWindowText(_T("Define YieldSign"));
	}
	
	if(GetObject()->getID() != -1)
	{
		((YieldSign *)GetObject())->ReadObject(GetObject()->getID());
		m_path.clear();
		m_path = ((YieldSign *)GetObject())->GetPath();
		m_degree = ((YieldSign *)GetObject())->GetRotation();

		m_nIntersect = ((YieldSign *)GetObject())->GetIntersectionNodeID();

		IntersectionNode intersection;
		intersection.ReadData(m_nIntersect);
		m_strName = intersection.GetName();

		m_nStretchID = ((YieldSign *)GetObject())->GetStretchID();

		Stretch pStretch;
		pStretch.ReadObject(m_nStretchID);
		m_strStretchName = pStretch.GetObjNameString();	
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_11,this);
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
void CAirsideYieldSignDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else if (m_hRClickItem == m_hIntersection)
	{
		pMenu = menuPopup.GetSubMenu(86);
	}
}


LRESULT CAirsideYieldSignDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideYieldSignDlg::GetFallBackReason(void)
{	
	return PICK_ONEPOINT;
}
void CAirsideYieldSignDlg::SetObjectPath(CPath2008& path)
{
	((YieldSign *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
}

void CAirsideYieldSignDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		CString csPoint;
		double FloorAlt = path.getPoint(i).getZ()/SCALE_FACTOR;//floors.getVisibleFloorAltitude(  ) ;
		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ),\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getY() ), FloorAlt);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getY() ));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
}

void CAirsideYieldSignDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");
	
	// control points.
	strLabel = CString( "Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	
	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	strLabel.Empty();
	strLabel = CString(_T("Select Intersection Node Frome Map"));
	m_hIntersection = m_treeProp.InsertItem(strLabel);
	LoadIntersectionNode(m_hIntersection);
	m_treeProp.Expand(m_hIntersection, TVE_EXPAND);

	//vertical profile.
	strLabel = _T("Vertical Profile");
	m_hVerticalProfile = 0;//m_treeProp.InsertItem(strLabel);
	
    m_treeProp.SetRedraw(1);
}

void CAirsideYieldSignDlg::LoadIntersectionNode(HTREEITEM preItem)
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp1 = 0;
	HTREEITEM hItemTemp2 = 0;
	HTREEITEM hItem1 = 0;
	HTREEITEM hItem2 = 0;
	if (m_nIntersect>0 && m_nStretchID>0)
	{
		//show intersection node information
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,0);
		strTemp = _T("Intersection Node");
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		hItem1 = m_treeProp.InsertItem(strTemp,preItem);
		m_treeProp.SetItemDataEx(hItem1,aoidDataEx);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp = m_strName;
		isscStringColor.strSection = m_strName;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp1 = m_treeProp.InsertItem(strTemp,hItem1);
		m_treeProp.SetItemDataEx(hItemTemp1,aoidDataEx);
		m_treeProp.Expand(hItem1,TVE_EXPAND);

		//show stretch information
		strTemp.Empty();
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,0);
		strTemp = _T("Stretch");
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		hItem2 = m_treeProp.InsertItem(strTemp,preItem);
		m_treeProp.SetItemDataEx(hItem2,aoidDataEx);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp = m_strStretchName;
		isscStringColor.strSection = m_strStretchName;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp2 = m_treeProp.InsertItem(strTemp,hItem2);
		m_treeProp.SetItemDataEx(hItemTemp2,aoidDataEx);
		m_treeProp.Expand(hItem2,TVE_EXPAND);
	}
}


void CAirsideYieldSignDlg::OnOK(void)
{
	if(GetObject())
	{
		CPath2008 path = ((YieldSign *)GetObject())->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the stretch path from the map."));
			return ;
		}
		((YieldSign *)GetObject())->SetPath(m_path);
		((YieldSign *)GetObject())->SetIntersectionNodeID(m_nIntersect);
		((YieldSign *)GetObject())->SetStretchID(m_nStretchID);
		((YieldSign *)GetObject())->SetRotation(m_degree);

		/*if(GetObject()->getID() != -1)
			((YieldSign *)GetObject())->UpdateObject(GetObject()->getID());
		else
			((YieldSign *)GetObject())->SaveObject(m_nAirportID);*/
	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideYieldSignDlg::UpdateOtherData(void)
{
	CPath2008 path = ((YieldSign *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
			((YieldSign *)GetObject())->UpdateObject(m_nObjID);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void CAirsideYieldSignDlg::OnSelectYieldSignIntersection(void)
{
	CDlgSelectStretchIntersectionNode dlg(m_nAirportID,this);
	if (dlg.DoModal() == IDOK)
	{
		m_nIntersect = dlg.m_nSelectID;
		m_strName = dlg.m_strName;
	}
	m_bPathModified = true;
	if (m_path.getCount()>0)
	{
		CalculateStrechID();
		CalculateRotateDegree();
	}
	LoadTree();
}

void CAirsideYieldSignDlg::CalculateStrechID(void)
{
	IntersectionNode intersection;
	intersection.ReadData(m_nIntersect);
	m_vStretchPath.first.clear();
	m_vStretchPath.second.clear();
	std::vector<StretchIntersectItem*> m_vIntersectItem = intersection.GetStretchIntersectItem();
	for (int i=0; i<(int)m_vIntersectItem.size(); i++)
	{
		int id = m_vIntersectItem[i]->GetStretch()->getID();
		/*CPath2008 path = m_vIntersectItem[i].GetStretch()->GetPath();*/
		m_vStretchPath.first.push_back(id);
		m_vStretchPath.second.push_back(*m_vIntersectItem[i]);
	}
	CPoint2008 position = m_path.getPoint(0);

	DistanceUnit minDistance = ARCMath::DISTANCE_INFINITE;

	for (i=0; i<(int)m_vStretchPath.first.size(); i++)
	{
		DistancePointPath3D * pDistance = new DistancePointPath3D(position,m_vStretchPath.second[i].GetStretch()->GetPath());
		DistanceUnit disTance = pDistance->GetSquared();
		if (disTance<minDistance)
		{
			m_nStretchID = m_vStretchPath.first[i];
			stretchItem = m_vStretchPath.second[i];
			minDistance = disTance;
		}
		delete pDistance;
	}
	Stretch pStretch;
	pStretch.ReadObject(m_nStretchID);
	m_strStretchName = pStretch.GetObjNameString();	

}


void CAirsideYieldSignDlg::CalculateRotateDegree()
{
	if (stretchItem.GetUID()==-1)
	{
		return;
	}
	double realDist = stretchItem.GetDistInItem();
	CPath2008 m_temp = stretchItem.GetStretch()->GetPath();

	if (m_temp.getCount()<2)
		return;

	double accdist = 0;
	double thisdist = 0;
	for(int i=0 ;i< m_temp.getCount()-1;i++)
	{
		if(accdist >= realDist )
			break;
		thisdist  = m_temp.getPoint(i).distance3D(m_temp.getPoint(i+1));
		accdist += thisdist;
	}

	if (i-1>=0)
	{
		CPoint2008 pos_head = m_temp.getPoint(i-1);
		CPoint2008 pos_trail = m_temp.getPoint(i);
		CPoint2008 pos_inter = m_temp.GetDistPoint(realDist);
		CLine2008 line(pos_head,pos_trail);
		CPoint2008 pos = m_path.getPoint(0);
		CPoint2008 pos_line = line.getProjectPoint(pos);
		if (pos_inter.distance3D(pos_head)>pos_line.distance3D(pos_head))
		{
			ARCVector3 vHeadDir = ((ARCVector3)pos_head - (ARCVector3)pos_inter).Normalize();
			ARCVector3 vObjectDir(0.0,1.0,0.0);
			double Theta =  ARCMath::RadiansToDegrees(acos(vObjectDir.dot(vHeadDir)));
			ARCVector3 vNormalDir = cross(vObjectDir,vHeadDir).Normalize();
			if (vNormalDir[VZ]>0)
				m_degree = Theta;
			else
				m_degree = -Theta;
		}
		else
		{
			ARCVector3 vTrailDir = ((ARCVector3)pos_trail - (ARCVector3)pos_inter).Normalize();
			ARCVector3 vObjectDir(0.0,1.0,0.0);
			double Theta =  ARCMath::RadiansToDegrees(acos(vObjectDir.dot(vTrailDir)));
			ARCVector3 vNormalDir = cross(vObjectDir,vTrailDir).Normalize();
			if (vNormalDir[VZ]>0)
				m_degree = Theta;
			else
				m_degree = -Theta;

		}
	}
	else
	{
		CPoint2008 pos_head = m_temp.getPoint(i);
		CPoint2008 pos_trail = m_temp.getPoint(i+1);
		CPoint2008 pos_inter = m_temp.GetDistPoint(realDist);
		CLine2008 line(pos_head,pos_trail);
		CPoint2008 pos = m_path.getPoint(0);
		CPoint2008 pos_line = line.getProjectPoint(pos);

		if(pos_inter.distance3D(pos_trail)>pos_line.distance3D(pos_trail))
		{
			ARCVector3 vTrailDir = ((ARCVector3)pos_trail - (ARCVector3)pos_inter).Normalize();
			ARCVector3 vObjectDir(0.0,1.0,0.0);
			double Theta =  ARCMath::RadiansToDegrees(acos(vObjectDir.dot(vTrailDir)));
			ARCVector3 vNormalDir = cross(vObjectDir,vTrailDir).Normalize();
			if (vNormalDir[VZ]>0)
				m_degree = Theta;
			else
				m_degree = -Theta;

		}
		else
			return;
	}


}


void CAirsideYieldSignDlg::OnEditVerticalProfile(void)
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

bool CAirsideYieldSignDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideYieldSignDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideYieldSignDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
