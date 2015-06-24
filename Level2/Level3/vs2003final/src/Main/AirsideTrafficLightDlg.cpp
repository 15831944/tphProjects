#include "StdAfx.h"
#include "resource.h"
#include "TermPlanDoc.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "AirsideTrafficLightDlg.h"
#include "../common\WinMsg.h" 
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/Stretch.h"
#include "../InputAirside/IntersectItem.h"

IMPLEMENT_DYNAMIC(CAirsideTrafficLightDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideTrafficLightDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_INTERSECTEDSTRETCH_EDITPOINT,OnSelectStretchIntersectNode)
END_MESSAGE_MAP()
CAirsideTrafficLightDlg::CAirsideTrafficLightDlg(int nTrafficLightID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nTrafficLightID,nAirportID, nProjID,pParent),m_vLevelList(nAirportID)
{
	m_pObject = new TrafficLight();
	m_pObject->setID(nTrafficLightID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
	m_nAirportID = nAirportID;
	m_strName = _T("");
	m_nInterNode = -1;
}

//CAirsideTrafficLightDlg::CAirsideTrafficLightDlg(TrafficLight* pTrafficLight,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pTrafficLight, nProjID,pParent),m_vLevelList(pTrafficLight?pTrafficLight->getAptID():-1)
//{
//	if(pTrafficLight)
//		m_path = pTrafficLight->GetPath();
//	m_bPathModified = false;
//}

CAirsideTrafficLightDlg::~CAirsideTrafficLightDlg(void)
{
	m_path.clear();
}
void CAirsideTrafficLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL CAirsideTrafficLightDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/0 );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify TrafficLight"));
	}
	else
	{
		SetWindowText(_T("Define TrafficLight"));
	}
	
	if(GetObject()->getID() != -1)
	{
		((TrafficLight *)GetObject())->ReadObject(GetObject()->getID());
		m_path.clear();
		m_path = ((TrafficLight *)GetObject())->GetPath();
        m_nInterNode = ((TrafficLight *)GetObject())->GetIntersecID();
		IntersectionNode intersectnode;
		intersectnode.ReadData(m_nInterNode);
		m_strName = intersectnode.GetName();
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_8,this);
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
void CAirsideTrafficLightDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	if (m_hRClickItem == m_hIntersection)
	{
		pMenu = menuPopup.GetSubMenu(84);
	}
}


LRESULT CAirsideTrafficLightDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideTrafficLightDlg::GetFallBackReason(void)
{	
	return PICK_ONEPOINT;
}
void CAirsideTrafficLightDlg::SetObjectPath(CPath2008& path)
{
	((TrafficLight *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
}

void CAirsideTrafficLightDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
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
		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ), \
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

void CAirsideTrafficLightDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	//m_treeProp.SetRedraw(0);

	CString strLabel = _T("");
	
	// control points.
//	strLabel = CString( "Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	
//	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
//	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
//	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	//vertical profile.
	strLabel = _T("Vertical Profile");
	m_hVerticalProfile = 0;//m_treeProp.InsertItem(strLabel);


	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;
	HTREEITEM hItemTemp2 = 0;

	//marking
	m_hIntersection = m_treeProp.InsertItem(_T("Select intersection node"));
       if (m_nInterNode>0)
       {
		   aoidDataEx.lSize = sizeof(aoidDataEx);
		   aoidDataEx.dwptrItemData = 0;
		   aoidDataEx.vrItemStringSectionColorShow.clear();
		   isscStringColor.colorSection = RGB(0,0,255);
		   strTemp = m_strName;
		   isscStringColor.strSection = m_strName;
		   aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		   hItemTemp = m_treeProp.InsertItem(m_strName,m_hIntersection);
		   m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
       }


		HTREEITEM hItemTemp3 = 0;

		int iPathCount = m_path.getCount();
		for(int i = 0;i < iPathCount; i++)
		{
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,0);
			char str = static_cast<char>(i+65);
			strTemp = _T("Position");
			strTemp = strTemp + '('+str+')';
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
            hItemTemp2 = m_treeProp.InsertItem(strTemp,m_hIntersection);
			m_treeProp.SetItemDataEx(hItemTemp2,aoidDataEx);

			CString csPoint;
			double FloorAlt = m_path.getPoint(i).getZ()/SCALE_FACTOR;//floors.getVisibleFloorAltitude(  ) ;
			csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), m_path.getPoint(i).getX() ), \
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), m_path.getPoint(i).getY() ), FloorAlt);

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), m_path.getPoint(i).getX() ));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), m_path.getPoint(i).getY() ));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),FloorAlt);		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			hItemTemp3 = m_treeProp.InsertItem( csPoint, hItemTemp2  );
			m_treeProp.SetItemDataEx(hItemTemp3,aoidDataEx);

			m_treeProp.Expand(hItemTemp2, TVE_EXPAND);
		}	 

	m_treeProp.Expand(m_hIntersection,TVE_EXPAND);
	
    //m_treeProp.SetRedraw(1);
}


void CAirsideTrafficLightDlg::OnOK(void)
{
	if(GetObject())
	{
		CPath2008 path = ((TrafficLight *)GetObject())->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the stretch path from the map."));
			return ;
		}
		((TrafficLight *)GetObject())->SetPath(m_path);
        ((TrafficLight*)GetObject())->SetIntersectID(m_nInterNode);
		/*if(GetObject()->getID() != -1)
			((TrafficLight *)GetObject())->UpdateObject(GetObject()->getID());
		else
			((TrafficLight *)GetObject())->SaveObject(m_nAirportID);*/
	}

	CAirsideObjectBaseDlg::OnOK();
	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	//pDoc->UpdateAllViews(NULL,VM_MODIFY_HOLDSHORTLINE,NULL);
}

bool CAirsideTrafficLightDlg::UpdateOtherData(void)
{
	CPath2008 path = ((TrafficLight *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
			((TrafficLight *)GetObject())->UpdateObject(m_nObjID);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void CAirsideTrafficLightDlg::OnSelectStretchIntersectNode(void)
{
	CDlgSelectStretchIntersectionNode  dlg(m_nAirportID,this);
	if (dlg.DoModal() == IDOK)
	{
		m_strName = dlg.m_strName;
		m_nInterNode = dlg.m_nSelectID;
	}
	SetTrafficLightPoints();
	//((TrafficLight*)GetObject())->SetIntersectID(m_nInterNode);
	LoadTree();
}


void CAirsideTrafficLightDlg::SetSubTrafficLightPoints(const DistanceUnit& dist1, const DistanceUnit& dist2,StretchIntersectItem& vStretch1, StretchIntersectItem& vStretch2)
{
	if (trafficLight.first.size()<1)
	{
		trafficLight.first.push_back(vStretch1);
		trafficLight.second.push_back(dist1);
		trafficLight.first.push_back(vStretch2);
		trafficLight.second.push_back(dist2);
	}
	else
	{
		BOOL bSelect = TRUE;
		for (int index=0; index<(int)trafficLight.first.size(); index++)
		{
			if (trafficLight.first[index].GetUID() == vStretch1.GetUID())
			{
				if ((dist1<0&&trafficLight.second[index]<0)||(dist1>0&&trafficLight.second[index]>0))
				{
					if (fabs(dist1)>fabs(trafficLight.second[index]))
					{
						trafficLight.second[index] = dist1;
					}
					bSelect = FALSE;
					break;
				}	
			}
		}
		if (bSelect == TRUE)
		{
			trafficLight.first.push_back(vStretch1);
			trafficLight.second.push_back(dist1);
		}

		bSelect = TRUE;
		for (index=0; index<(int)trafficLight.first.size(); index++)
		{
			if (trafficLight.first[index].GetUID() == vStretch2.GetUID())
			{
				if ((dist2<0&&trafficLight.second[index]<0)||(dist2>0&&trafficLight.second[index]>0))
				{
					if (fabs(dist2)>fabs(trafficLight.second[index]))
					{
						trafficLight.second[index] = dist2;
					}
					bSelect = FALSE;
					break;
				}	
			}
		}
		if (bSelect == TRUE)
		{
			trafficLight.first.push_back(vStretch2);
			trafficLight.second.push_back(dist2);
		}
	}
}

void CAirsideTrafficLightDlg::SetTrafficLightPoints(void)
{
	IntersectionNode pNode;
	pNode.ReadData(m_nInterNode);
	std::vector<StretchIntersectItem*> vStretchItem = pNode.GetStretchIntersectItem();
	trafficLight.first.clear();
	trafficLight.second.clear();
	for(int i=0; i<(int)vStretchItem.size()-1; i++)
	{

		for (int j=1; j<(int)vStretchItem.size(); j++)
		{

			CPath2008 path1 = vStretchItem[i]->GetPath();
			CPath2008 path2 = vStretchItem[j]->GetPath();

			DistanceUnit disLength1 = path1.GetTotalLength();
			DistanceUnit disLength2 = path2.GetTotalLength();
			  
			DistanceUnit distInItem1 = vStretchItem[i]->GetDistInItem();
			DistanceUnit distInItem2 = vStretchItem[j]->GetDistInItem();

			DistanceUnit width1 = vStretchItem[i]->GetStretch()->GetLaneWidth()* vStretchItem[i]->GetStretch()->GetLaneNumber();
			DistanceUnit width2 = vStretchItem[j]->GetStretch()->GetLaneWidth()* vStretchItem[j]->GetStretch()->GetLaneNumber();

			DistanceUnit m_dSegma = (width1+width2)*0.5;

			CPoint2008 point1 = path1.GetDistPoint(distInItem1);
			CPoint2008 point2 = path2.GetDistPoint(distInItem2);

			if( distInItem1 > 1 )
			{
				CPoint2008 vDir1 = CPoint2008(path1.GetDistPoint(distInItem1 -1 ) - point1);
				if( distInItem2 > 1)
				{
					CPoint2008 vDir2 = CPoint2008(path2.GetDistPoint(distInItem2-1) - point2);
					double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
					double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
					DistanceUnit absDistToIntersect = m_dSegma * ctanhalfZeda;

					if( distInItem1 - absDistToIntersect > 0 && distInItem2 - absDistToIntersect > 0)
					{
						DistanceUnit dist1 = distInItem1;
						DistanceUnit dist2 = distInItem2;
						DistanceUnit disInter1 = -absDistToIntersect;
						DistanceUnit disInter2 = -absDistToIntersect;
						SetSubTrafficLightPoints(disInter1,disInter2,*vStretchItem[i],*vStretchItem[j]);

					}

				}

				if( distInItem2 < disLength2 -1)
				{
					CPoint2008 vDir2 = CPoint2008(path2.GetDistPoint(distInItem2+1) - point2);
					double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
					double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
					DistanceUnit absDistToIntersect = m_dSegma * ctanhalfZeda;

					if( distInItem1-absDistToIntersect > 0 && distInItem2 + absDistToIntersect < disLength2 )
					{
						DistanceUnit dist1 = distInItem1;
						DistanceUnit dist2 = distInItem2;
						DistanceUnit disInter1 = -absDistToIntersect;
						DistanceUnit disInter2 = absDistToIntersect;
						SetSubTrafficLightPoints(disInter1,disInter2,*vStretchItem[i],*vStretchItem[j]);
					}
				}
			}
			if( distInItem1 < disLength1 -1 )
			{
				CPoint2008 vDir1 =CPoint2008(path1.GetDistPoint(distInItem1+1) - point1);
				if( distInItem2 > 1)
				{
					CPoint2008 vDir2 = CPoint2008(path2.GetDistPoint(distInItem2-1) - point2);
					double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
					double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
					DistanceUnit absDistToIntersect = m_dSegma * ctanhalfZeda;

					if(distInItem1+absDistToIntersect< disLength1 && distInItem2 - absDistToIntersect > 0)
					{
						DistanceUnit dist1 = distInItem1;
						DistanceUnit dist2 = distInItem2;
						DistanceUnit disInter1 = absDistToIntersect;
						DistanceUnit disInter2 = -absDistToIntersect;
						SetSubTrafficLightPoints(disInter1,disInter2,*vStretchItem[i],*vStretchItem[j]);
					}
				}

				if( distInItem2 < disLength2 -1)
				{
					CPoint2008 vDir2 = CPoint2008(path2.GetDistPoint(distInItem2+1) - point2);
					double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
					double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
					DistanceUnit absDistToIntersect = m_dSegma * ctanhalfZeda;

					if(distInItem1+absDistToIntersect<disLength1 && distInItem2+absDistToIntersect<disLength2)
					{
						DistanceUnit dist1 = distInItem1;
						DistanceUnit dist2 = distInItem2;
						DistanceUnit disInter1 = absDistToIntersect;
						DistanceUnit disInter2 = absDistToIntersect;
						SetSubTrafficLightPoints(disInter1,disInter2,*vStretchItem[i],*vStretchItem[j]);
					}
				}
			}
		}
	}

    vpoints.clear();
	for (int k=0; k<(int)trafficLight.first.size(); k++)
	{
		DistanceUnit disTance = trafficLight.second[k]+trafficLight.first[k].GetDistInItem();
		CPath2008 path = trafficLight.first[k].GetStretch()->GetPath();
		ARCPoint3 location = path.GetDistPoint(disTance);
		double relatlen = path.GetDistIndex(disTance);

		int npt = (int)relatlen;
		int nptnext;
		if(npt >= path.getCount() -1 ){
			nptnext = path.getCount() -2;
			npt = nptnext +1;
		}else {
			nptnext = npt +1;
		}

		CPoint2008 dir = CPoint2008(path.getPoint(npt) - path.getPoint(nptnext));
		dir.setZ(0);
		dir.Normalize();
		dir.rotate(90);
		dir.length(trafficLight.first[k].GetStretch()->GetLaneWidth()*trafficLight.first[k].GetStretch()->GetLaneNumber()*0.5);
		if (trafficLight.second[k]>0)
			vpoints.push_back(location-dir); 
		else
			vpoints.push_back(location+dir);
	}

	int elementCount = trafficLight.first.size();
	CPoint2008 * pointList = new CPoint2008[elementCount];
	for (int m=0; m<elementCount; m++)
	{
		ARCVector3 v3 = vpoints[m]; 
		pointList[m].setPoint(v3[VX],v3[VY],v3[VZ]);
	}
	m_path.init(elementCount,pointList);
	((TrafficLight *)GetObject())->SetPath(m_path);
	m_bPathModified = true;
	delete [] pointList;
}

void CAirsideTrafficLightDlg::OnEditVerticalProfile(void)
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

bool CAirsideTrafficLightDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideTrafficLightDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideTrafficLightDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
