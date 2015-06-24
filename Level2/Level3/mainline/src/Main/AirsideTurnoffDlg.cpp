// AirsideRoundaboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "../InputAirside/Stretch.h"
#include "AirsideTurnoffDlg.h"
#include "../common\WinMsg.h"
#include "../common/UnitsManager.h"

// CAirsideRoundaboutDlg

IMPLEMENT_DYNAMIC(CAirsideTurnoffDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideTurnoffDlg, CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_ALTOBJECTWIDTH_DEFINEWIDTH,OnDefineWidth)
END_MESSAGE_MAP()

CAirsideTurnoffDlg::CAirsideTurnoffDlg(int nTurnoffID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nTurnoffID,nAirportID, nProjID,pParent)
{
	m_pObject = new Turnoff();
	m_pObject->setID(nTurnoffID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

//CAirsideTurnoffDlg::CAirsideTurnoffDlg(Turnoff * pTurnoff,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pTurnoff, nProjID,pParent)
//{
//	if(pTurnoff)
//	{
//		for(InsecObjectPartVector::const_iterator ctitrOBJPart = pTurnoff->GetIntersectionsPart().begin();\
//			ctitrOBJPart != pTurnoff->GetIntersectionsPart().end();++ctitrOBJPart)
//		{
//			m_vrLinkStretches.push_back(*ctitrOBJPart);
//		} 
//		m_path = pTurnoff->GetPath();
//	}	
//
//	m_bPathModified = false;
//}
CAirsideTurnoffDlg::~CAirsideTurnoffDlg(void)
{
	m_vrLinkStretches.clear();
	m_path.clear();
}

void CAirsideTurnoffDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BOOL CAirsideTurnoffDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap(/* bitmap */0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify TurnOff"));
	}
	else
	{
		SetWindowText(_T("Define TurnOff"));
	}
	
	m_dLaneWidth = 8;
	if(GetObject()->getID() != -1)
	{
		((Turnoff *)GetObject())->ReadObject(GetObject()->getID());
		m_dLaneWidth = ((Turnoff *)GetObject())->GetLaneWidth();
		m_path = ((Turnoff *)GetObject())->GetPath();
		for(InsecObjectPartVector::const_iterator ctitrOBJPart = ((Turnoff *)GetObject())->GetIntersectionsPart().begin();\
			ctitrOBJPart != ((Turnoff *)GetObject())->GetIntersectionsPart().end();++ctitrOBJPart)
		{
			m_vrLinkStretches.push_back(*ctitrOBJPart);
		}
	}

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideTurnoffDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hLaneWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineWidth();
	}
}


void CAirsideTurnoffDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hLinkStretches || m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hLaneWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
}


LRESULT CAirsideTurnoffDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			m_dLaneWidth = (UNITSMANAGER->UnConvertLength(pst->iPercent));
		}

		LoadTree();
		return TRUE;
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideTurnoffDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideTurnoffDlg::SetObjectPath(CPath2008& path)
{
	((Turnoff *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
}
void CAirsideTurnoffDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
{	
}
void CAirsideTurnoffDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");	

	// control points.
	strLabel = CString( "Control Point(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
	
	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	//vertical profile.
	strLabel = _T("Vertical Profile");
	m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	//lane width.
	strLabel.Format(_T("Lane Width: %.2f %s "),UNITSMANAGER->ConvertLength(m_dLaneWidth) ,UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) );
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);

	//link control points.
	strLabel = _T( "Linking Stretches ");
	m_hLinkStretches  = m_treeProp.InsertItem(strLabel);

	Stretch stretch;
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = m_vrLinkStretches.begin();ctitrOBJPart != m_vrLinkStretches.end();++ctitrOBJPart)
	{
		stretch.setID((*ctitrOBJPart).nALTObjectID);
		strLabel = stretch.ReadObjectByID((*ctitrOBJPart).nALTObjectID)->GetObjectName().GetIDString();
		strLabel.Format("%s:%d",strLabel,(*ctitrOBJPart).part);
		m_treeProp.InsertItem(strLabel , m_hLinkStretches);
	}

    m_treeProp.SetRedraw(1);
}


void CAirsideTurnoffDlg::OnOK(void)
{
	if(GetObject())
	{
		CPath2008 path = ((Turnoff *)GetObject())->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the TurnOff path from the map."));
			return ;
		}
		m_dLaneWidth = _tstof(m_treeProp.GetItemText(m_hLaneWidth));
		((Turnoff *)GetObject())->SetPath(m_path);
		((Turnoff *)GetObject())->SetLaneWidth(m_dLaneWidth); 
		((Turnoff *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
		if(-1 != GetObject()->getID())
		{
			((Turnoff *)GetObject())->UpdateObject(GetObject()->getID());
		}
		else
		{
			((Turnoff *)GetObject())->SaveObject(m_nAirportID);
		}
	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideTurnoffDlg::UpdateOtherData(void)
{
	CPath2008 path = ((Stretch *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	
	((Turnoff *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
	((Turnoff *)GetObject())->SetLaneWidth(m_dLaneWidth); 
	((Turnoff *)GetObject())->SetPath(m_path);

	return (true);
}

void CAirsideTurnoffDlg::OnEditVerticalProfile(void)
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

void CAirsideTurnoffDlg::OnDefineWidth(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_dLaneWidth)) );
	m_treeProp.SetSpinRange( 0,360);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
