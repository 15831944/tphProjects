#include "StdAfx.h"
#include "resource.h"
#include "../AirsideGUI/UnitPiece.h"
#include ".\dlgreportingarea.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "../common\WinMsg.h"

IMPLEMENT_DYNAMIC(DlgReportingArea, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(DlgReportingArea,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
END_MESSAGE_MAP()
DlgReportingArea::DlgReportingArea(int nAreaID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nAreaID,nAirportID, nProjID,pParent)
,m_vLevelList(nAirportID)
{
	m_pObject = new CReportingArea();
	m_pObject->setID(nAreaID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}


DlgReportingArea::~DlgReportingArea(void)
{
	m_path.clear();
}

void DlgReportingArea::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL DlgReportingArea::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/ 0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify ReportingArea"));
	}
	else
	{
		SetWindowText(_T("Define ReportingArea"));
	}

	if(GetObject()->getID() != -1)
	{
		((CReportingArea *)GetObject())->ReadObject(GetObject()->getID());
		m_path = ((CReportingArea *)GetObject())->GetPath();
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
void DlgReportingArea::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
}


LRESULT DlgReportingArea::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason DlgReportingArea::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void DlgReportingArea::SetObjectPath(CPath2008& path)
{
	((CReportingArea *)GetObject())->SetPath(path);
	m_path = path;
	m_bPathModified = true;
	LoadTree();
}

void DlgReportingArea::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
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
}

void DlgReportingArea::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	// control points.
	strLabel = CString( "Area Vertices(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	////vertical profile.
	//strLabel = _T("Vertical Profile");
	//m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	m_treeProp.SetRedraw(1);
}


void DlgReportingArea::OnOK(void)
{
	if(GetObject())
	{
		CPath2008 path = ((CReportingArea *)GetObject())->GetPath();
		if (path.getCount() ==0)
		{
			MessageBox(_T("Please pick the Reporting Area path from the map."));
			return ;
		}		
		((CReportingArea *)GetObject())->SetPath(m_path);
	}

	CAirsideObjectBaseDlg::OnOK();
}

bool DlgReportingArea::UpdateOtherData(void)
{
	CPath2008 path = ((CReportingArea *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the Reporting Area path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
			((CReportingArea *)GetObject())->UpdateObject(m_nObjID);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void DlgReportingArea::OnEditVerticalProfile(void)
{

}

bool DlgReportingArea::ConvertUnitFromDBtoGUI(void)
{
	return (false);
}

bool DlgReportingArea::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool DlgReportingArea::ConvertUnitFromGUItoDB(void)
{

	return (false);
}