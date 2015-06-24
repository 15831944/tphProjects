#include "StdAfx.h"
#include "resource.h"

#include ".\airsideparkingplacedlg.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "../Common/WinMsg.h"
#include "../Common/ARCVector.h"

IMPLEMENT_DYNAMIC(AirsideParkingPlaceDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(AirsideParkingPlaceDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_HOLDARGUMENT_EDIT, OnEditParameter)
END_MESSAGE_MAP()

AirsideParkingPlaceDlg::AirsideParkingPlaceDlg(int nObjID,int nParentID, int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nObjID,nAirportID,nProjID,pParent)
{
	if (nObjID >=0 )
	{
		m_pObject = ALTObject::ReadObjectByID(nObjID);
	}
	else
	{
		m_pObject =  new ParkingPlace(nParentID);
		m_pObject->setID(nObjID);
		m_pObject->setAptID(nAirportID);
	}

	m_nObjID = nObjID;
	m_nAirportID = nAirportID;
	m_bPathModified = false;
}

AirsideParkingPlaceDlg::~AirsideParkingPlaceDlg(void)
{
}


void AirsideParkingPlaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL AirsideParkingPlaceDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	if (m_nObjID >=0)
	{
		SetWindowText(_T("Modify Parking Place"));
	}
	else
	{
		SetWindowText(_T("Define Parking Place"));
	}

	if(m_nObjID >=0)
	{
		ParkingPlace* pParking = (ParkingPlace *)GetObject();
		pParking->ReadObject(GetObject()->getID());
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

void AirsideParkingPlaceDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hSpotLength)
	{
		m_hRClickItem = hTreeItem;
		ModifySpotLength();
	}

	if(hTreeItem == m_hSpotWidth)
	{
		m_hRClickItem = hTreeItem;
		ModifySpotWidth();
	}

	if (hTreeItem == m_hParkingType)
	{
		m_hRClickItem = hTreeItem;
		ModifyParkingType();
	}

	if (hTreeItem == m_hSpotAngle)
	{
		m_hRClickItem = hTreeItem;
		ModifySpotAngle();
	}

}

void AirsideParkingPlaceDlg::OnEditParameter()
{
	m_hRClickItem = m_treeProp.GetSelectedItem();
	if(m_hRClickItem == m_hSpotLength)
	{
		ModifySpotLength();
	}

	if(m_hRClickItem == m_hSpotWidth)
	{
		ModifySpotWidth();
	}

	if (m_hRClickItem == m_hParkingType)
	{

		ModifyParkingType();
	}

	if (m_hRClickItem == m_hSpotAngle)
	{
		ModifySpotAngle();
	}
}

void AirsideParkingPlaceDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else 
	{
		pMenu = menuPopup.GetSubMenu(60);
	}
}


LRESULT AirsideParkingPlaceDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	ParkingPlace* pParking = (ParkingPlace*)GetObject();
	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hSpotLength)
		{			
			double dLength = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			if(dLength <= 0)
				dLength = 1.0;
			pParking->SetSpotLength(dLength);

		}

		if (m_hRClickItem == m_hSpotWidth)
		{
			double dWidth = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );		
			if(dWidth <= 0)
				dWidth = 1;
			pParking->SetSpotWidth(dWidth);
		}

		if (m_hRClickItem == m_hSpotAngle)
		{
			int nAngle = pst->iPercent;		
			pParking->SetSpotAngle(nAngle);
		}
		UpdateObjectViews();
		LoadTree();
	}

	if (message == WM_INPLACE_COMBO2)
	{
		if (m_hRClickItem == m_hParkingType)
		{
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			if (nSel >= 0)
			{
				if (nSel == 0)
				{
					pParking->SetParkingType(NOSEIN);
					pParking->SetSpotAngle(90);		//default angle is 90 degree
				}
				else
				{
					pParking->SetParkingType(LINE);
				}

				m_bPropModified = true;
				UpdateObjectViews();
				LoadTree();
			}

		}
	}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason AirsideParkingPlaceDlg::GetFallBackReason(void)
{	
	return PICK_TWOPOINTS;
}
void AirsideParkingPlaceDlg::SetObjectPath(CPath2008& path)
{

	((ParkingPlace *)GetObject())->SetSpotLine(path);

	m_bPathModified = true;
	UpdateObjectViews();
	LoadTree();
}

void AirsideParkingPlaceDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
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

void AirsideParkingPlaceDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	ParkingPlace* pParking = (ParkingPlace*)GetObject();
	// control points.
	strLabel = CString( "Spots line(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	m_hServiceLocation = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hServiceLocation,pParking->GetSpotLine());	
	m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );

	//spot length.
	strLabel.Format(_T("Spot Length: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotLength()) , CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),pParking->GetSpotLength());
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hSpotLength = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hSpotLength,aoidDataEx);

	//spot width.
	strLabel.Format(_T("Spot Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotWidth()) , CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotWidth()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hSpotWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hSpotWidth,aoidDataEx);

	//Parking type
	CString strSelect = _T("");


	if ((int)pParking->GetParingType() == 0)
		strSelect.Format(_T("Nose In"));
	else 
		strSelect.Format(_T("Line"));

	strLabel.Format(_T("Parking Type: %s"),strSelect);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),strSelect);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hParkingType = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hParkingType,aoidDataEx);

	//spot angle
	if (pParking->GetParingType() == NOSEIN)
	{
		strLabel.Format(_T("Spot Angle: %d (Degree)"),pParking->GetSpotAngle());
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		strTemp.Format(_T("%d"),pParking->GetSpotAngle());
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hSpotAngle = m_treeProp.InsertItem(strLabel,m_hParkingType);
		m_treeProp.SetItemDataEx(m_hParkingType,aoidDataEx);
	}

	m_treeProp.SetRedraw(1);
}


void AirsideParkingPlaceDlg::OnOK(void)
{
	CAirsideObjectBaseDlg::OnOK();
	UpdateAddandRemoveObject();
}

bool AirsideParkingPlaceDlg::UpdateOtherData(void)
{
	CPath2008 path = ((ParkingPlace *)GetObject())->GetSpotLine();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the spot line from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID >=0)
		{
			((ParkingPlace*)GetObject())->UpdateObject(m_nObjID);
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

void AirsideParkingPlaceDlg::ModifySpotLength()
{
	ParkingPlace* pParking = (ParkingPlace *)GetObject();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum(static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotLength())) );
	m_treeProp.SetSpinRange( 1,100);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void AirsideParkingPlaceDlg::ModifySpotWidth()
{
	ParkingPlace* pParking = (ParkingPlace *)GetObject();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotWidth())) );
	m_treeProp.SetSpinRange( 1,100);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void AirsideParkingPlaceDlg::ModifyParkingType()
{
	std::vector<CString> vStr;
	vStr.clear();
	CString str = _T("Nose In");
	vStr.push_back(str);
	str = _T("Line");
	vStr.push_back(str);
	m_treeProp.SetComboWidth(60);
	m_treeProp.SetComboString(m_hParkingType,vStr);

}

void AirsideParkingPlaceDlg::ModifySpotAngle()
{
	ParkingPlace* pParking = (ParkingPlace *)GetObject();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pParking->GetSpotAngle())) );
	m_treeProp.SetSpinRange( 1,100);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

bool AirsideParkingPlaceDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool AirsideParkingPlaceDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool AirsideParkingPlaceDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}