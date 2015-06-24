// AirsideRoundaboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "../InputAirside/Stretch.h"
#include "AirsideLineParkingDlg.h"
#include "../common\WinMsg.h"
#include "../common/UnitsManager.h"

// CAirsideLineParkingDlg

IMPLEMENT_DYNAMIC(CAirsideLineParkingDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideLineParkingDlg, CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_ALTOBJECTNUMBER_DEFINENUMBER,OnDefineNumber)
	ON_COMMAND(ID_ALTOBJECTWIDTH_DEFINEWIDTH,OnDefineWidth)
END_MESSAGE_MAP()

CAirsideLineParkingDlg::CAirsideLineParkingDlg(int nLineParkingID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nLineParkingID,nAirportID, nProjID,pParent)
{
	m_pObject = new LineParking();
	m_pObject->setID(nLineParkingID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

// CAirsideLineParkingDlg::CAirsideLineParkingDlg(LineParking * pLineParking,int nProjID,CWnd * pParent /*= NULL*/ )
// : CAirsideObjectBaseDlg(pLineParking, nProjID,pParent)
// {
// 	if(pLineParking)
// 	{
// 		for(InsecObjectPartVector::const_iterator ctitrOBJPart = pLineParking->GetIntersectionsPart().begin();\
// 			ctitrOBJPart != pLineParking->GetIntersectionsPart().end();++ctitrOBJPart)
// 		{
// 			m_vrLinkStretches.push_back(*ctitrOBJPart);
// 		} 
// 		m_path = pLineParking->GetPath();
// 	}	
// 	CAirsideLineParkingDlg(LineParking * pLineParking,int nProjID,CWnd * pParent = NULL);
CAirsideLineParkingDlg::~CAirsideLineParkingDlg(void)
{
	m_vrLinkStretches.clear();
	m_path.clear();
}

void CAirsideLineParkingDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BOOL CAirsideLineParkingDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify LineParking"));
	}
	else
	{
		SetWindowText(_T("Define LineParking"));
	}
	
	m_dSpotWidth = 8;
	m_iSpotNum = 1;
	if(GetObject()->getID() != -1)
	{
		((LineParking *)GetObject())->ReadObject(GetObject()->getID());
		m_dSpotWidth = ((LineParking *)GetObject())->GetSpotWidth();
		m_iSpotNum = ((LineParking *)GetObject())->GetSpotNumber();
		m_path = ((LineParking *)GetObject())->GetPath();
		for(InsecObjectPartVector::const_iterator ctitrOBJPart = ((LineParking *)GetObject())->GetIntersectionsPart().begin();\
			ctitrOBJPart != ((LineParking *)GetObject())->GetIntersectionsPart().end();++ctitrOBJPart)
		{
			m_vrLinkStretches.push_back(*ctitrOBJPart);
		}
	}

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideLineParkingDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hLinkStretches || m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hSpotWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else if (m_hRClickItem == m_hSpotNumber)
	{
		pMenu = menuPopup.GetSubMenu(75);
	}
}


LRESULT CAirsideLineParkingDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hSpotWidth)
		{			
			m_dSpotWidth = (UNITSMANAGER->UnConvertLength(pst->iPercent));
		}
		else if (m_hRClickItem == m_hSpotNumber)
		{
			m_iSpotNum = (pst->iPercent);			
		}

		LoadTree();
		return TRUE;
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideLineParkingDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideLineParkingDlg::SetObjectPath(CPath2008& path)
{
	((LineParking *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
}
void CAirsideLineParkingDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
{	
}
void CAirsideLineParkingDlg::LoadTree(void)
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

	//number of spots.
	strLabel.Format(_T("Number of Spots : %d"), m_iSpotNum );
	m_hSpotNumber = m_treeProp.InsertItem(strLabel);

	//spot width.
	strLabel.Format(_T("Spot Width: %.2f %s "),UNITSMANAGER->ConvertLength(m_dSpotWidth) ,UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) );
	m_hSpotWidth = m_treeProp.InsertItem(strLabel);

    m_treeProp.SetRedraw(1);
}


void CAirsideLineParkingDlg::OnOK(void)
{
	if(GetObject())
	{
		CPath2008 path = ((LineParking *)GetObject())->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the TurnOff path from the map."));
			return ;
		}
		m_iSpotNum   = _tstoi(m_treeProp.GetItemText(m_hSpotNumber));
		m_dSpotWidth = _tstof(m_treeProp.GetItemText(m_hSpotWidth));
		((LineParking *)GetObject())->SetPath(m_path);
		((LineParking *)GetObject())->SetSpotWidth(m_dSpotWidth); 
		((LineParking *)GetObject())->SetSpotNumber(m_iSpotNum);
		((LineParking *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
		try
		{
			CADODatabase::BeginTransaction() ;
			if(-1 != GetObject()->getID())
			{
				((LineParking *)GetObject())->UpdateObject(GetObject()->getID());
			}
			else
			{
				((LineParking *)GetObject())->SaveObject(m_nAirportID);
			}
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException e)
		{
			CADODatabase::RollBackTransation() ;
		}

	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideLineParkingDlg::UpdateOtherData(void)
{
	CPath2008 path = ((LineParking *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	m_iSpotNum   = _tstoi(m_treeProp.GetItemText(m_hSpotNumber));
	m_dSpotWidth = _tstof(m_treeProp.GetItemText(m_hSpotWidth));
	((LineParking *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
	((LineParking *)GetObject())->SetSpotWidth(m_dSpotWidth); 
	((LineParking *)GetObject())->SetSpotNumber(m_iSpotNum);
	((LineParking *)GetObject())->SetPath(m_path);

	return (true);
}

void CAirsideLineParkingDlg::OnEditVerticalProfile(void)
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

void CAirsideLineParkingDlg::OnDefineNumber(void)
{
	m_treeProp.SetDisplayType( 2 ); 
	m_treeProp.SetDisplayNum(m_iSpotNum); 
	m_treeProp.SetSpinRange( 0,40);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CAirsideLineParkingDlg::OnDefineWidth(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_dSpotWidth)) );
	m_treeProp.SetSpinRange( 0,360);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
