#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidecontourdlg.h"
#include "../InputAirside/contour.h"
#include "../InputAirside/ALTAirport.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CAirsideContourDlg, CAirsideObjectBaseDlg)
CAirsideContourDlg::CAirsideContourDlg(int nContourID,int nAirportID, int nParentID,int nProjID, CWnd* pParent):
CAirsideObjectBaseDlg(nContourID,nAirportID, nProjID,pParent)
{
	m_pObject = new Contour;

	Contour *pContour = (Contour*)m_pObject.get();
	pContour->SetParentID(nParentID);
	m_pObject->setAptID(nAirportID);

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	m_dAptAltitdude = altAirport.getElevation();

	m_bPathModified = false;
}

//CAirsideContourDlg::CAirsideContourDlg( Contour * pContour,int nProjID, CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pContour, nProjID,pParent)
//{
//	m_bPathModified = false;
//}

CAirsideContourDlg::~CAirsideContourDlg(void)
{
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideContourDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_CONTOUR_MODIFYALTITUDE, OnModifyAltitude)
END_MESSAGE_MAP()
BOOL CAirsideContourDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();

	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));
	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/0 );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Contour"));
	}
	else
	{
		SetWindowText(_T("Define Contour"));
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_16,this);
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

void CAirsideContourDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hAltitude)
	{
		m_hRClickItem = hTreeItem;
		OnModifyAltitude();
	}

}

void CAirsideContourDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
	}
	else if (m_hRClickItem == m_hAltitude)
	{
		pMenu = menuPopup.GetSubMenu( 68);
	}
}
void CAirsideContourDlg::OnModifyAltitude()
{
	double dAlt= ((Contour *)GetObject())->GetAltitude();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dAlt )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

LRESULT CAirsideContourDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hAltitude)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((Contour*)GetObject())->SetAltude( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified= true;
			LoadTree();
		}
	}


	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideContourDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideContourDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Contour *)GetObject())->SetPath(path);
		m_bPathModified = true;
		LoadTree();
	}
}

void CAirsideContourDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	// Service Location
	CString csLabel = CString("Service Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	m_hServiceLocation = m_treeProp.InsertItem( csLabel );


	CPath2008 path = ((Contour *)GetObject())->GetPath();
	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);

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

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hServiceLocation);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);


	//width
	csLabel.Empty();
	csLabel.Format(_T("Altitude ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Contour*)GetObject())->GetAltitude()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((Contour*)GetObject())->GetAltitude()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hAltitude =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hAltitude,aoidDataEx);
}


void CAirsideContourDlg::OnOK()
{	

	CAirsideObjectBaseDlg::OnOK();
}
bool CAirsideContourDlg::UpdateOtherData()
{
	CPath2008 path = ((Contour *)GetObject())->GetPath();
	if (path.getCount() < 3)
	{
		MessageBox(_T("Please pick the Contour path from the map(At Least 3 Points)."));
		return false;
	}
	
	Contour* pContour = ((Contour *)GetObject());
	if( pContour && pContour->GetAltitude() < m_dAptAltitdude )
	{
		if ( AfxMessageBox(_T("The altitude is below the active airport elevation , are you sure?"), MB_YESNO | MB_ICONQUESTION) != IDYES )
			return false;
	}

	if (m_bPathModified && m_nObjID != -1)
	{
		((Contour *)GetObject())->UpdatePath();
	}
	return true;
}

bool CAirsideContourDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideContourDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideContourDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
