// AirsideDriveRoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "AirsideDriveRoadDlg.h"
#include "TermPlanDoc.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "../common\WinMsg.h"
#include "../Common/ARCUnit.h"
#include "../Common/UnitsManager.h"
#include "AirsideStretchVerticalProfileEditor.h"

// AirsideDriveRoadDlg dialog

IMPLEMENT_DYNAMIC(AirsideDriveRoadDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(AirsideDriveRoadDlg, CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_ALTOBJECTNUMBER_DEFINENUMBER,OnDefineNumber)
	ON_COMMAND(ID_ALTOBJECTWIDTH_DEFINEWIDTH,OnDefineWidth)
END_MESSAGE_MAP()
AirsideDriveRoadDlg::AirsideDriveRoadDlg(int nRoadID, int nParentID, int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nRoadID,nAirportID,nProjID,pParent),m_vLevelList(nAirportID)
{
	if (nRoadID >=0 )
	{
		m_pObject = ALTObject::ReadObjectByID(nRoadID);
	}
	else
	{
		m_pObject =  new DriveRoad(nParentID);
		m_pObject->setID(nRoadID);
		m_pObject->setAptID(nAirportID);
	}

	m_nRoadID = nRoadID;
	m_nAirportID = nAirportID;
	m_bPathModified = false;

	DriveRoad* pRoad = (DriveRoad*)GetObject();

	m_nLaneNum = pRoad->GetLaneNumber() ;
	m_dLaneWidth = ARCUnit::ConvertLength( pRoad->GetLaneWidth(), ARCUnit::M, ARCUnit::CM) ;

}

AirsideDriveRoadDlg::~AirsideDriveRoadDlg()
{
	m_path.clear();
}

void AirsideDriveRoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL AirsideDriveRoadDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap */0);

	if (m_nRoadID >=0)
	{
		SetWindowText(_T("Modify Drive Road"));
	}
	else
	{
		SetWindowText(_T("Define Drive Road"));
	}

	if(m_nRoadID >=0)
	{
		DriveRoad* pRoad = (DriveRoad *)GetObject();
		pRoad->ReadObject(GetObject()->getID());
		m_path = pRoad->GetPath();
		m_dLaneWidth = pRoad->GetLaneWidth();
		m_nLaneNum = pRoad->GetLaneNumber();
		m_enumDir = pRoad->GetRoadDir();
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

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AirsideDriveRoadDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hLaneNumber)
	{
		m_hRClickItem = hTreeItem;
		OnDefineNumber();
	}

	if(hTreeItem == m_hLaneWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineWidth();
	}

}

void AirsideDriveRoadDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else if (m_hRClickItem == m_hLaneNumber)
	{
		pMenu = menuPopup.GetSubMenu(75);
	}
	else if (m_hRClickItem == m_hLaneWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
}


LRESULT AirsideDriveRoadDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	DriveRoad* pRoad = (DriveRoad*)GetObject();
	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			m_dLaneWidth = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			if(m_dLaneWidth <= 0)
				m_dLaneWidth = 1.0;
			m_bPropModified = true;
			pRoad->SetLaneWidth(m_dLaneWidth);

		}
		else if (m_hRClickItem == m_hLaneNumber)
		{
			m_nLaneNum = (pst->iPercent);		
			if(m_nLaneNum <= 0)
				m_nLaneNum = 1;
			m_bPropModified = true;
			pRoad->SetLaneNumber(m_nLaneNum);
		}
		UpdateObjectViews();
		LoadTree();
	}

	if (message == WM_INPLACE_COMBO2)
	{
		if (m_hRClickItem == m_hDirect)
		{
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			if (nSel >= 0)
			{
				if (nSel == 0)
				{
					pRoad->SetRoadDir(IN_Dir);
				}
				else
				{
					pRoad->SetRoadDir(OUT_Dir);
				}

				m_bPropModified = true;
				UpdateObjectViews();
				LoadTree();
			}

		}
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason AirsideDriveRoadDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void AirsideDriveRoadDlg::SetObjectPath(CPath2008& path)
{

	((DriveRoad *)GetObject())->SetPath(path);
	((DriveRoad *)GetObject())->SetLaneWidth(m_dLaneWidth);
	((DriveRoad *)GetObject())->SetLaneNumber(m_nLaneNum);
	((DriveRoad *)GetObject())->SetRoadDir(m_enumDir);

	m_path = path;
	m_bPathModified = true;
	UpdateObjectViews();
	LoadTree();
}

void AirsideDriveRoadDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
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

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ), \
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getY() ), FloorAlt);
		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}	
}

void AirsideDriveRoadDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	// control points.
	strLabel = CString( "Control Point(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	//vertical profile.
	strLabel = _T("Vertical Profile");
	m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	//number of lanes.
	strLabel.Format(_T("Number of Lanes : %d"), m_nLaneNum );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),m_nLaneNum);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneNumber = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneNumber,aoidDataEx);

	//lane width.
	strLabel.Format(_T("Lane Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth) , CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneWidth,aoidDataEx);

	//Direction
	CString strSelect = _T("");
	DriveRoad* pRoad = (DriveRoad*)GetObject();

	if (pRoad->GetRoadDir() == IN_Dir)
		strSelect.Format(_T("In"));
	else 
		strSelect.Format(_T("Out"));

	strLabel.Format(_T("Road Direction: %s"),strSelect);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),strSelect);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hDirect = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hDirect,aoidDataEx);

	m_treeProp.SetRedraw(1);
}


void AirsideDriveRoadDlg::OnOK(void)
{
	if(GetObject())
	{
		((DriveRoad *)GetObject())->SetPath(m_path);
		((DriveRoad *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((DriveRoad *)GetObject())->SetLaneNumber(m_nLaneNum);
		((DriveRoad*)GetObject())->SetRoadDir(m_enumDir);
	}

	CAirsideObjectBaseDlg::OnOK();
	UpdateAddandRemoveObject();
}

bool AirsideDriveRoadDlg::UpdateOtherData(void)
{
	//	CPath2008 path = ((Stretch *)GetObject())->GetPath();
	if (m_path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nRoadID >=0)
		{
			((DriveRoad*)GetObject())->UpdateObject(m_nRoadID);
		}

	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void AirsideDriveRoadDlg::OnEditVerticalProfile(void)
{
	std::vector<double>vXYPos;
	std::vector<double>vZPos;

	if( m_path.getCount() < 2)
	{
		return;
	}

	vXYPos.reserve(m_path.getCount());
	vZPos.reserve(m_path.getCount());		
	vXYPos.push_back(0.0); 
	vZPos.push_back( m_path.getPoint(0).getZ() /SCALE_FACTOR);

	double dLensoFar = 0.0;

	for(int i=1;i<m_path.getCount();i++)
	{
		ARCVector3 v3D(m_path[i].getX()-m_path[i-1].getX(), m_path[i].getY()-m_path[i-1].getY(), 0.0);
		dLensoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
		vXYPos.push_back(dLensoFar);				
		vZPos.push_back( m_path[i].getZ() /SCALE_FACTOR );
	}

	CAirsideStretchVerticalProfileEditor dlg(vXYPos,vZPos, m_path);

	if( dlg.DoModal() == IDOK )
	{	
		ASSERT(m_path.getCount() == (int) vZPos.size());
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;

		CPoint2008* pPointList = m_path.getPointList();
		ASSERT(NULL != pPointList);
		for(i=0; i<m_path.getCount(); i++)
		{
			pPointList[i].setZ(vZPos[i] * SCALE_FACTOR);
			m_bPropModified = TRUE;
		}

		LoadTree();
	}
}

void AirsideDriveRoadDlg::OnDefineNumber(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum(m_nLaneNum);
	m_treeProp.SetSpinRange( 1,40);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void AirsideDriveRoadDlg::OnDefineWidth(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth)) );
	m_treeProp.SetSpinRange( 1,360);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void AirsideDriveRoadDlg::ModifyDirection(void)
{
	std::vector<CString> vStr;
	vStr.clear();
	CString str = _T("In");
	vStr.push_back(str);
	str = _T("Out");
	vStr.push_back(str);
	m_treeProp.SetComboWidth(60);
	m_treeProp.SetComboString(m_hDirect,vStr);	

}

bool AirsideDriveRoadDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool AirsideDriveRoadDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool AirsideDriveRoadDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

