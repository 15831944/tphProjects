
#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidegatedlg.h"
#include "../InputAirside/stand.h"
#include "../Common/WinMsg.h"
#include "../Common/DistanceLineLine.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"
#include <algorithm>
#include "DlgInitArp.h"
#include "../Common/LatLongConvert.h"
#include "AirsideStretchVerticalProfileEditor.h"
IMPLEMENT_DYNAMIC(CAirsideGateDlg, CAirsideObjectBaseDlg)
CAirsideGateDlg::CAirsideGateDlg(int nGateID,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nGateID,nAirportID,nProjID,pParent)
{
	m_pObject = new Stand();
	m_pObject->setAptID(nAirportID);
	m_bOnRelease = false;
	m_airportInfo.ReadAirport(nAirportID);
}


CAirsideGateDlg::~CAirsideGateDlg(void)
{

}
BEGIN_MESSAGE_MAP(CAirsideGateDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_PROCPROP_TOGGLEDBACKUP, OnProcpropToggledBackup)
	ON_COMMAND(ID_STAND_ADDLEADINLINE, OnAddNewLeadInLine)
	ON_COMMAND(ID_STAND_ADDLEADOUTLINE, OnAddNewLeadOutLine)
	ON_COMMAND(ID_PROCPROP_DELETE, OnDeleteLeadLine)
	ON_COMMAND(ID_PROCPROP_TOGGLEDQUEUEFIXED,OnAddReleasePoint)
	ON_COMMAND(ID_PROCPROP_DEFINE_Z_POS,RemoveReleasePoint)
	ON_COMMAND(ID_STANDMENU_VERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_AIRSIDELATLOG_SETLAT,OnEditLatLong)
	ON_COMMAND(ID_AIRSIDELATLOG_DELETE,OnDeleteLatLog)
	ON_COMMAND(ID_AIRSIDEPOSITION_SETXYZ,OnEditPosition)
	ON_COMMAND(ID_AIRSIDEPOSITION_DELETE,OnDeletePosition)
END_MESSAGE_MAP()

BOOL CAirsideGateDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STAND));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Gate"));
	}
	else
	{
		SetWindowText(_T("Define Gate"));
	}

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
void CAirsideGateDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem  == m_hServiceLocation )
	{
		pMenu=menuPopup.GetSubMenu(98);
	}
	
	if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), m_hRClickItem) )
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
		pMenu->AppendMenu(MF_STRING, ID_PROCPROP_TOGGLEDBACKUP, _T("Use as a push back leadoutline"));;
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
	}
	if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem) )
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDBACKUP,MF_BYCOMMAND | MF_ENABLED);
		pMenu->AppendMenu(MF_STRING,ID_PROCPROP_TOGGLEDQUEUEFIXED,_T("Release point (pick)"));
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);
		pMenu->AppendMenu(MF_STRING,ID_PROCPROP_DEFINE_Z_POS,_T("Delete Release Point"));
		pMenu->EnableMenuItem(ID_PROCPROP_DEFINE_Z_POS,MF_BYCOMMAND);
		Stand* pStand = (Stand*) GetObject();
		if (m_vhLeadOutLines.end() != std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem))
		{
			int idx = m_treeProp.GetItemData(m_hRClickItem);
			StandLeadOutLine& pLeadOutLine = pStand->m_vLeadOutLines.ItemAt(idx);
			if (pLeadOutLine.getPath().getCount() > 0 && pLeadOutLine.IsPushBack())
			{
				pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDQUEUEFIXED,MF_BYCOMMAND | MF_ENABLED);
				pMenu->EnableMenuItem(ID_PROCPROP_DEFINE_Z_POS,MF_BYCOMMAND | MF_ENABLED);
				return;
			}
		}
		pMenu->EnableMenuItem(ID_PROCPROP_TOGGLEDQUEUEFIXED,MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_PROCPROP_DEFINE_Z_POS,MF_BYCOMMAND | MF_GRAYED);
	}
	if( m_hRClickItem == m_hLeadInLineItem )
	{
		pMenu=menuPopup.GetSubMenu(81);
	}
	if( m_hRClickItem == m_hLeadOutLineItem )
	{
		pMenu = menuPopup.GetSubMenu(82);
	}
	
	if (std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem) != m_vLatLongItems.end())
	{
		pMenu = menuPopup.GetSubMenu(97);
	}

	if (std::find(m_vPositionItems.begin(),m_vPositionItems.end(),m_hRClickItem) != m_vPositionItems.end())
	{
		pMenu = menuPopup.GetSubMenu(96);
	}
}

LRESULT CAirsideGateDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideGateDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_ONEPOINT;
	}

	
	if( m_vhLeadInLines.end() != std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(),m_hRClickItem) 
		|| m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem) )
	{
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideGateDlg::SetObjectPath(CPath2008& path)
{
	
	Stand * pStand = (Stand*) GetObject();
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Stand *)GetObject())->SetServicePoint(path.getPoint(0));
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}	
	else if( m_vhLeadInLines.end() != std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(),m_hRClickItem) )
	{
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		StandLeadInLine& pLeadInLine = pStand->m_vLeadInLines.ItemAt(idx);
		pLeadInLine.SetPath(path);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem) )
	{
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		StandLeadOutLine& pLeadOutLine = pStand->m_vLeadOutLines.ItemAt(idx);
		if (m_bOnRelease)
		{
			DistancePointPath3D path3D(path.getPoint(0),pLeadOutLine.getPath());
			if (path3D.GetSquared() > 1000.0*1000.0)
			{
				::AfxMessageBox(_T("The distance from release point to lead out line cannot exceed 10m "));
			}
			else
			{
				pLeadOutLine.SetReleasePoint(path3D.m_clostPoint);
				pLeadOutLine.toSetReleasePoint(TRUE);
			}
		}
		else
		{
			pLeadOutLine.SetPath(path);
		}
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	
}

void CAirsideGateDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	Stand * pStand = (Stand*) GetObject();
	HTREEITEM hItem = m_treeProp.GetParentItem(hTreeItem);
	if (m_vLatLongItems.end() != std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),hTreeItem))
	{
		if (hItem == m_hServiceLocation)
		{
			
			CPoint2008 pt = pStand->GetServicePoint();
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				pStand->SetServicePoint(pt);
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();
				
				int nPosIdx = m_treeProp.GetItemData(hTreeItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					CLatitude fixlatitude;
					CLongitude fixlongitude;
					ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

					if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
					{
						ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();
				
				int nPosIdx = m_treeProp.GetItemData(hTreeItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					CLatitude fixlatitude;
					CLongitude fixlongitude;
					ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

					if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
					{
						ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
		}
	}
	else if (m_vPositionItems.end() != std::find(m_vPositionItems.begin(),m_vPositionItems.end(),hTreeItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = pStand->GetServicePoint();
			pt.setZ(pt.getZ() + m_airportInfo.getElevation());
			if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()) )
			{
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				pStand->SetServicePoint(pt);
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(hTreeItem);
				if (nPosIdx < path.getCount())
				{	
					CPoint2008 pt = path.getPoint(nPosIdx);
					pt.setZ(pt.getZ() + m_airportInfo.getElevation());
					if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()))
					{
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(hTreeItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					pt.setZ(pt.getZ() + m_airportInfo.getElevation());
					if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()))
					{
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
		}
	}

}

void CAirsideGateDlg::LoadPathItem(CAirsideObjectTreeCtrl& treeprop,HTREEITEM rootItem, const CPath2008& path,AirsideObjectTreeCtrlItemDataEx& aoidDataEx,ItemStringSectionColor&isscStringColor, ARCUnit_Length lengthUnit )
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

		HTREEITEM hFixLL= treeprop.InsertItem(strLL,rootItem);
		treeprop.SetItemDataEx(hFixLL,aoidDataEx);
		m_vLatLongItems.push_back(hFixLL);
		m_treeProp.SetItemData(hFixLL,(DWORD)i);
	}
	treeprop.Expand(rootItem,TVE_EXPAND);
}


void CAirsideGateDlg::LoadTree()
{

	m_treeProp.DeleteAllItems();
	m_vhLeadInLines.clear();
	m_vhLeadOutLines.clear();
	m_vLatLongItems.clear();
	m_vPositionItems.clear();

	// Service Location
	CString csLabel = CString("Parking Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CPoint2008 point = ((Stand *)GetObject())->GetServicePoint();
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
	Stand * pStand = (Stand*)GetObject();
	csLabel.Format("Lead in Lines(%s)", CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	m_hLeadInLineItem = m_treeProp.InsertItem( csLabel );
	for(size_t i=0;i< pStand->m_vLeadInLines.GetItemCount();i++ )
	{	
		StandLeadInLine * pStandLeadIn = &pStand->m_vLeadInLines.ItemAt(i);
		CString strLineLable = pStandLeadIn->GetName();
		if(strLineLable.IsEmpty())
		{
			strLineLable.Format("LINE %d", i+1);
			pStandLeadIn->SetName(strLineLable);
		}
		HTREEITEM lineItem = m_treeProp.InsertItem(strLineLable, m_hLeadInLineItem);
		m_treeProp.SetItemData(lineItem,(DWORD)i);
		LoadPathItem(m_treeProp,lineItem,pStandLeadIn->getPath(), aoidDataEx,isscStringColor,CUnitPiece::GetCurSelLengthUnit());
		m_vhLeadInLines.push_back(lineItem);
	}
	m_treeProp.Expand(m_hLeadInLineItem,TVE_EXPAND);

	//Lead out Lines;
	csLabel.Format("Lead out Lines(%s)", CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	m_hLeadOutLineItem = m_treeProp.InsertItem( csLabel );
	for(size_t i=0;i< pStand->m_vLeadOutLines.GetItemCount();i++ )
	{	
		StandLeadOutLine * pStandLeadOut = &pStand->m_vLeadOutLines.ItemAt(i);
		CString strLineLable = pStandLeadOut->GetName();
		if(strLineLable.IsEmpty())
		{
			strLineLable.Format("LINE %d", i+1);
			pStandLeadOut->SetName(strLineLable);
		}
		HTREEITEM lineItem = m_treeProp.InsertItem(strLineLable, m_hLeadOutLineItem);
		m_treeProp.SetItemData(lineItem, (DWORD)i);
		if(pStandLeadOut->IsPushBack())
			m_treeProp.InsertItem( _T("Back up"), lineItem);

		LoadPathItem(m_treeProp,lineItem,pStandLeadOut->getPath(), aoidDataEx,isscStringColor,CUnitPiece::GetCurSelLengthUnit());
		m_vhLeadOutLines.push_back(lineItem);

		if (pStandLeadOut->IsSetReleasePoint())
		{
			HTREEITEM hReleasePointItem = m_treeProp.InsertItem(_T("Release Point"),lineItem);
			CPath2008 path;
			path.init(1);
			path[0] = pStandLeadOut->GetReleasePoint();
			LoadPathItem(m_treeProp,hReleasePointItem,path,aoidDataEx,isscStringColor,CUnitPiece::GetCurSelLengthUnit());
		}
	}
	m_treeProp.Expand(m_hLeadOutLineItem,TVE_EXPAND);
	

}

void CAirsideGateDlg::OnProcpropToggledBackup()
{
	if (m_vhLeadInLines.end() != std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(),m_hRClickItem))
	{
		Stand * pStand = (Stand*)GetObject();
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
		
		StandLeadOutLine pNewLead;
		CString newLeadName;
		newLeadName.Format("Line %d", pStand->m_vLeadOutLines.GetItemCount()+1 );
		pNewLead.SetName(newLeadName);
		CPath2008 path(pLeadInLine->GetPath());
		path.invertList();
		pNewLead.SetPath(path);
		pNewLead.SetPushBack(TRUE);
		pStand->m_vLeadOutLines.AddNewItem(pNewLead);
	
		m_bPropModified = true;
	
		UpdateObjectViews();
		LoadTree();
		return;
	}
	if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem) )
	{
		Stand * pStand = (Stand*)GetObject();
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
		pLeadOutLine->SetPushBack(!pLeadOutLine->IsPushBack());
		pLeadOutLine->toSetReleasePoint(FALSE);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
		return;
	}

	
	m_bPropModified = true;
	LoadTree();
}
void CAirsideGateDlg::OnOK()
{
	if (m_nObjID != -1) // 
	{
		
	}

	Stand * pStand = (Stand*)GetObject();
	if (FALSE == pStand->IsAllLinesValid())
	{
		MessageBox(_T("Please assign all lead-in lines and lead-out lines."));
		return;
	}
	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideGateDlg::UpdateOtherData()
{

	return true;
}

bool CAirsideGateDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideGateDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideGateDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideGateDlg::OnAddNewLeadInLine()
{
	Stand * pStand = (Stand*) GetObject();
	StandLeadInLine pNewLead;
	CString newLeadName;
	newLeadName.Format("Line %d", pStand->m_vLeadInLines.GetItemCount()+1 );
	pNewLead.SetName(newLeadName);
	pStand->m_vLeadInLines.AddNewItem(pNewLead);
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
}

void CAirsideGateDlg::OnAddNewLeadOutLine()
{
	Stand * pStand = (Stand*) GetObject();
	StandLeadOutLine pNewLead;
	CString newLeadName;
	newLeadName.Format("Line %d", pStand->m_vLeadOutLines.GetItemCount()+1 );
	pNewLead.SetName(newLeadName);
	pStand->m_vLeadOutLines.AddNewItem(pNewLead);	
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
}

void CAirsideGateDlg::OnDeleteLeadLine()
{
	Stand * pStand = (Stand*) GetObject();
	if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), m_hRClickItem) )
	{
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		pStand->m_vLeadInLines.DeleteItem(idx);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), m_hRClickItem) )
	{
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		pStand->m_vLeadOutLines.DeleteItem(idx);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
}


void CAirsideGateDlg::OnAddReleasePoint()
{
	m_bOnRelease = true;
	CTermPlanDoc* pDoc	= GetDocument();

	IRender3DView* p3DView = pDoc->Get3DView();
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

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CAirsideGateDlg::RemoveReleasePoint()
{
	if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(),m_hRClickItem) )
	{
		Stand * pStand = (Stand*)GetObject();
		int idx = m_treeProp.GetItemData(m_hRClickItem);
		StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
		pLeadOutLine->toSetReleasePoint(FALSE);
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
}

void CAirsideGateDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
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

void CAirsideGateDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
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

void CAirsideGateDlg::OnEditVerticalProfile(void)
{
 	std::vector<double>vXYPos;
 	std::vector<double>vZPos;
 
	CPoint2008 serverLocation = ((Stand*)GetObject())->GetServicePoint();
	CPath2008 path;
	path.init(1,&serverLocation);
 	vXYPos.reserve(1);
 	vZPos.reserve(1);		
 	vXYPos.push_back(0.0); 
 	vZPos.push_back( serverLocation.getZ() /SCALE_FACTOR);
 
 	double dLensoFar = 0.0;

 	CAirsideStretchVerticalProfileEditor dlg(vXYPos,vZPos, path);
 
 	if( dlg.DoModal() == IDOK )
 	{	
 		ASSERT(path.getCount() == (int) vZPos.size());
 		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
 
 		CPoint2008* pPointList = path.getPointList();
 		ASSERT(NULL != pPointList);
 		for(int i=0; i<path.getCount(); i++)
 		{
 			pPointList[i].setZ(vZPos[i] * SCALE_FACTOR);
			((Stand*)GetObject())->SetServicePoint(pPointList[i]);
 			m_bPropModified = TRUE;
 		}
 
 		LoadTree();
 	}
}

void CAirsideGateDlg::OnEditLatLong()
{
	Stand * pStand = (Stand*) GetObject();
	HTREEITEM hItem = m_treeProp.GetParentItem(m_hRClickItem);
	if (m_vLatLongItems.end() != std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = pStand->GetServicePoint();
			CLatitude fixlatitude;
			CLongitude fixlongitude;
			ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

			if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
			{
				ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
				pStand->SetServicePoint(pt);
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					CLatitude fixlatitude;
					CLongitude fixlongitude;
					ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

					if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
					{
						ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					CLatitude fixlatitude;
					CLongitude fixlongitude;
					ConvertPosToLatLong(pt,fixlatitude, fixlongitude);

					if( OnPropModifyLatLong(fixlatitude, fixlongitude) )
					{
						ConvertLatLongToPos(fixlatitude,fixlongitude, pt);
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
		}
	}
}

void CAirsideGateDlg::OnEditPosition()
{
	Stand * pStand = (Stand*) GetObject();
	HTREEITEM hItem = m_treeProp.GetParentItem(m_hRClickItem);
	if (m_vPositionItems.end() != std::find(m_vPositionItems.begin(),m_vPositionItems.end(),m_hRClickItem))
	{
		if (hItem == m_hServiceLocation)
		{

			CPoint2008 pt = pStand->GetServicePoint();
			pt.setZ(pt.getZ() + m_airportInfo.getElevation());
			if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()) )
			{
				pt.setZ(pt.getZ() - m_airportInfo.getElevation());
				pStand->SetServicePoint(pt);
				m_bPathModified = true;
				UpdateObjectViews();
				LoadTree();
			}
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					pt.setZ(pt.getZ() + m_airportInfo.getElevation());
					if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()))
					{
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = path.getPoint(nPosIdx);
					pt.setZ(pt.getZ() + m_airportInfo.getElevation());
					if( OnPropModifyPosition(pt,CUnitPiece::GetCurSelLengthUnit()))
					{
						pt.setZ(pt.getZ() - m_airportInfo.getElevation());
						path[nPosIdx] = pt;
						m_bPathModified = true;
						UpdateObjectViews();
						LoadTree();
					}
				}
			}
		}
	}
}

void CAirsideGateDlg::OnDeleteLatLog()
{
	Stand * pStand = (Stand*) GetObject();
	HTREEITEM hItem = m_treeProp.GetParentItem(m_hRClickItem);
	if (m_vLatLongItems.end() != std::find(m_vLatLongItems.begin(),m_vLatLongItems.end(),m_hRClickItem))
	{
		if (hItem == m_hServiceLocation)
		{
			CPoint2008 pt;
			pt = CPoint2008(0.0,0.0,0.0);
			pStand->SetServicePoint(pt);
			m_bPathModified = true;
			UpdateObjectViews();
			LoadTree();
			
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = CPoint2008(0.0,0.0,0.0);
					path[nPosIdx] = pt;
					m_bPathModified = true;
					UpdateObjectViews();
					LoadTree();
					
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = CPoint2008(0.0,0.0,CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_airportInfo.getElevation()));
					path[nPosIdx] = pt;
					m_bPathModified = true;
					UpdateObjectViews();
					LoadTree();
				}
			}
		}
	}
}

void CAirsideGateDlg::OnDeletePosition()
{
	Stand * pStand = (Stand*) GetObject();
	HTREEITEM hItem = m_treeProp.GetParentItem(m_hRClickItem);
	if (m_vPositionItems.end() != std::find(m_vPositionItems.begin(),m_vPositionItems.end(),m_hRClickItem))
	{
		if (hItem == m_hServiceLocation)
		{
			CPoint2008 pt = CPoint2008(0.0,0.0,0.0);
			pStand->SetServicePoint(pt);
			m_bPathModified = true;
			UpdateObjectViews();
			LoadTree();
		}
		else
		{
			if( m_vhLeadInLines.end()!= std::find(m_vhLeadInLines.begin(),m_vhLeadInLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadInLine * pLeadInLine = &pStand->m_vLeadInLines.ItemAt(idx);
				CPath2008& path = pLeadInLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = CPoint2008(0.0,0.0,0.0);
					path[nPosIdx] = pt;
					m_bPathModified = true;
					UpdateObjectViews();
					LoadTree();
				}
			}
			else if( m_vhLeadOutLines.end()!= std::find(m_vhLeadOutLines.begin(),m_vhLeadOutLines.end(), hItem) )
			{
				int idx = m_treeProp.GetItemData(hItem);
				StandLeadOutLine * pLeadOutLine = &pStand->m_vLeadOutLines.ItemAt(idx);
				CPath2008& path = pLeadOutLine->getPath();

				int nPosIdx = m_treeProp.GetItemData(m_hRClickItem);
				if (nPosIdx < path.getCount())
				{
					CPoint2008 pt = CPoint2008(0.0,0.0,0.0);
					path[nPosIdx] = pt;
					m_bPathModified = true;
					UpdateObjectViews();
					LoadTree();
				}
			}
		}
	}
}