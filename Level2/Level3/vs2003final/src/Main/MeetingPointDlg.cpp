#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include ".\MeetingPointDlg.h"

#include "../Common/WinMsg.h"
#include "../InputAirside/MeetingPoint.h"


IMPLEMENT_DYNAMIC(CMeetingPointDlg, CAirsideObjectBaseDlg)
CMeetingPointDlg::CMeetingPointDlg(int nMeetingPointID,int nAirportID,int nProjID,CWnd *pParent)
	: CAirsideObjectBaseDlg(nMeetingPointID,nAirportID,nProjID,pParent)
	, m_hServiceLocationItem(NULL)
	, m_hServiceLocationData(NULL)
	, m_hRadiusItem(NULL)
	, m_hRadiusData(NULL)
	, m_hMarkingItem(NULL)
	, m_hMarkingData(NULL)
	,m_hWaitingHoldItem(NULL)
{
	m_pObject = new CMeetingPoint();
	m_pObject->setAptID(nAirportID);
}


CMeetingPointDlg::~CMeetingPointDlg(void)
{

}
void CMeetingPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMeetingPointDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_PROCPROP_DELETE, CMeetingPointDlg::OnProcpropDelete)
END_MESSAGE_MAP()

BOOL CMeetingPointDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();

	// To do: add your own dialog-init code here
	HBITMAP hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MEETINGPOINT));
	ASSERT(hBitmap);
	m_ImageStatic.SetBitmap(hBitmap);

	if (-1 != m_nObjID)
	{
		SetWindowText(_T("Modify MeetingPoint"));
	}
	else
	{
		SetWindowText(_T("Define MeetingPoint"));
	}
	
	// Init CUnitPiece
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

	// load data from one INSTANCE of MeetingPoint
	LoadTree();
	return TRUE;
}



void CMeetingPointDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;

	// To do: add your code dealing with Property Tree double-click
	if (m_hRadiusData == hTreeItem)
	{
		OnRadiusModify();
	}
	if (m_hMarkingData == hTreeItem)
	{
		OnMarkingModify();
	}
	
	
	CAirsideObjectBaseDlg::OnDoubleClickPropTree(hTreeItem);
}

void CMeetingPointDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if (m_hRClickItem == m_hServiceLocationItem || m_hRClickItem == m_hWaitingHoldItem)
	{
		pMenu = menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu(1, MF_BYPOSITION);
		pMenu->DeleteMenu(2, MF_BYPOSITION);
		pMenu->DeleteMenu(2, MF_BYPOSITION);
		pMenu->DeleteMenu(2, MF_BYPOSITION);
	}
	//if (m_hRClickItem == m_hMarkingItem)
	//{
	//	pMenu = menuPopup.GetSubMenu(5);
	//}
	
	//CAirsideObjectBaseDlg::OnContextMenu(menuPopup, pMenu);
}


LRESULT CMeetingPointDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	// 

	// To deal with the change of TreeCtrl
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hRadiusData)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CMeetingPoint*)GetObject())->SetRadius(
				CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent)
				);
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}
	if (message == WM_INPLACE_EDIT)
	{
		if (m_hRClickItem == m_hMarkingData)
		{	
			CString strValue = *(CString*)lParam;
			((CMeetingPoint*)GetObject())->SetMarking((const char *)strValue);
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}



FallbackReason CMeetingPointDlg::GetFallBackReason()
{
	// We want pick a taxi-way (an OBJECT)
	if(m_hRClickItem == m_hServiceLocationItem)
		return PICK_ALTOBJECT;
	else
		return PICK_MANYPOINTS ;
}

bool CMeetingPointDlg::DoTempFallBackFinished(WPARAM wParam, LPARAM lParam)
{
	// You should display the window manually
	ShowWindow(SW_SHOW);	
	EnableWindow();

	//if(m_pSelHoldShortLineNode == NULL)
	//	return (false);
	if(m_hRClickItem == NULL)
		return FALSE ;
		CMeetingPoint* 	pMeetingPoint = (CMeetingPoint*)GetObject();
	if(m_hRClickItem == m_hWaitingHoldItem)
	{
		CAirsideObjectBaseDlg::DoTempFallBackFinished(wParam,lParam);
	}
	else
	{
		ALTObject* pObj = reinterpret_cast<ALTObject*>(wParam);
		if (pObj->GetType() != ALT_TAXIWAY)
		{
			MessageBox("Please select a point on Taxiway!");
			return false;
		}
		std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>*>(lParam);
		if(!pData)		// get a vector of points
			return (false);
		size_t nSize = pData->size();
		if (nSize != 1)   //actually there is only one point if nothing wrong
			return (false);

		CPoint2008 ptSelect;   // translate from ARCVector3 to CPoint2008
		ARCVector3 v3 = pData->at(0);
		ptSelect.setPoint(v3[VX] ,v3[VY],v3[VZ]);


		Taxiway* pTaxiway = (Taxiway*)pObj;  // fetch the Taxiway object
		DistancePointPath3D distancePP(ptSelect, pTaxiway->GetPath());
		distancePP.GetSquared();

		pMeetingPoint->SetTaxiwayID(pTaxiway->getID());
		pMeetingPoint->SetTaxiwayDivision(distancePP.m_fPathParameter);
		pMeetingPoint->SetRadius(pTaxiway->GetWidth()/2.0);
	}
	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();
	return true;

}

void CMeetingPointDlg::SetObjectPath(CPath2008& path)
{
	// Do nothing.
	CMeetingPoint* 	pMeetingPoint = (CMeetingPoint*)GetObject();
	pMeetingPoint->SetPath(path);
	pMeetingPoint->GetPath()->init(path.getCount(),path.getPointList());
}


void CMeetingPointDlg::OnOK()
{
	if (-1 == ((CMeetingPoint*)GetObject())->GetTaxiwayID())
	{
		MessageBox(_T("Please select the service location in a \"Taxiway\" before saving!"));
		return;
	}
	CAirsideObjectBaseDlg::OnOK();
}

bool CMeetingPointDlg::UpdateOtherData()
{
	return true;//CAirsideObjectBaseDlg::UpdateOtherData();
}


bool CMeetingPointDlg::ConvertUnitFromDBtoGUI(void)
{
	//return CUnitPiece::ConvertUnitFromDBtoGUI();
	return true;
}

bool CMeetingPointDlg::RefreshGUI(void)
{
	LoadTree();
	return true;//CUnitPiece::RefreshGUI();
}

bool CMeetingPointDlg::ConvertUnitFromGUItoDB(void)
{
	//return CUnitPiece::ConvertUnitFromGUItoDB();
	return true;
}

// to refresh the tree from data
void CMeetingPointDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	HTREEITEM hTmpItem = NULL;
	CString   str;

	// To do: add our nodes in iteration
	// service location
	str = CString("Service Location (")
		+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	m_hServiceLocationItem = m_treeProp.InsertItem(str);
	// radius
	str = CString("Radius for visualization(")
		+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	m_hRadiusItem = m_treeProp.InsertItem(str);
	// marking
	m_hMarkingItem = m_treeProp.InsertItem(_T("Marking"));

	//waiting area
	m_hWaitingHoldItem = m_treeProp.InsertItem(_T("Waiting Area"));
	// insert subs
	InsertColoredSubItem();

	m_treeProp.Expand(m_hServiceLocationItem, TVE_EXPAND);
	m_treeProp.Expand(m_hRadiusItem, TVE_EXPAND);
	m_treeProp.Expand(m_hMarkingItem, TVE_EXPAND);
	m_treeProp.Expand(m_hWaitingHoldItem, TVE_EXPAND);
}

void CMeetingPointDlg::OnMarkingModify(void)
{
	CString strValue= m_treeProp.GetItemText(m_hRClickItem);
	m_treeProp.StringEditLabel(m_hRClickItem, strValue);
	m_bPropModified = true;
}

// Insert the service point item (" x = *.*,  y = *.*,  z = *.* ")
void CMeetingPointDlg::InsertColoredSubItem()
{
	CPoint2008 pt;
	CMeetingPoint* pMeetingPoint = (CMeetingPoint*)GetObject();
	ASSERT(pMeetingPoint && ALT_MEETINGPOINT == pMeetingPoint->GetType());

	// here GetServicePoint should work OK
//	ASSERT(pMeetingPoint->GetServicePoint(pt));

	if (pMeetingPoint->GetServicePoint(pt))
	{
		CString str;

		//set data ex
		ItemStringSectionColor isscStringColor;
		isscStringColor.colorSection = RGB(0,0,255);

		AirsideObjectTreeCtrlItemDataEx aoidDataEx;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;

		// related taxiway's name
		ALTObject* pObj = ALTObject::ReadObjectByID(pMeetingPoint->GetTaxiwayID());
		if (pObj)
		{
			str = pObj->GetObjNameString();
			HTREEITEM hItem = m_treeProp.InsertItem(str, m_hServiceLocationItem);

			aoidDataEx.vrItemStringSectionColorShow.clear();
			ALTObjectID name = pObj->GetObjectName();
			for (int i=0;i<4;i++)
			{
				str = name.at(i).c_str();
				if (_T("") != str)
				{
					isscStringColor.strSection = str;
					aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
				}
			}
			m_treeProp.SetItemDataEx(hItem, aoidDataEx);

			delete pObj;
		}

		// service location data
		str.Format(_T("x = %.2f; y = %.2f; z = %.2f"),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));
		m_hServiceLocationData = m_treeProp.InsertItem(str, m_hServiceLocationItem);

		
		aoidDataEx.vrItemStringSectionColorShow.clear();

		str.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		str.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		str.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		m_treeProp.SetItemDataEx(m_hServiceLocationData, aoidDataEx);


		// radius data
		str.Format(_T("%.2f"),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),
			pMeetingPoint->GetRadius()));
		m_hRadiusData = m_treeProp.InsertItem(str, m_hRadiusItem);

		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_treeProp.SetItemDataEx(m_hRadiusData, aoidDataEx);

		// marking data
		str = pMeetingPoint->GetMarking().c_str();
		m_hMarkingData = m_treeProp.InsertItem(str, m_hMarkingItem);

		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_treeProp.SetItemDataEx(m_hMarkingData, aoidDataEx);
	}
	//waiting area
	InitWaitingAreaSubItem(pMeetingPoint) ;
}
CString CMeetingPointDlg::FormatStringForPoint(const CPoint2008& _point)
{
	CString str ;
	str.Format(_T("x = %.2f; y = %.2f; z = %.2f"),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),_point.getX()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),_point.getY()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),_point.getZ()));
	return str ;
}
void CMeetingPointDlg::InitWaitingAreaSubItem(CMeetingPoint* pMeetingPoint)
{
	//delete all child item 
	for (int i = 0 ; i < pMeetingPoint->GetPath()->getCount() ; i++)
	{
		CPoint2008 point = pMeetingPoint->GetPath()->getPoint(i) ;
		m_treeProp.InsertItem(FormatStringForPoint(point),m_hWaitingHoldItem) ;
	}
}
void CMeetingPointDlg::OnRadiusModify(void)
{
	double radius = ((CMeetingPoint*)GetObject())->GetRadius();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,
		CUnitPiece::GetCurSelLengthUnit(),radius ))
		);
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );

	m_bPropModified = true;
}

void CMeetingPointDlg::OnProcpropDelete(void)
{
	CMeetingPoint* pMeetingPoint = (CMeetingPoint*)GetObject();
	ASSERT(pMeetingPoint);
	if(m_hRClickItem == m_hServiceLocationItem)
		pMeetingPoint->SetTaxiwayID(-1);
	if(m_hRClickItem == m_hWaitingHoldItem)
		pMeetingPoint->GetPath()->clear() ;
	m_bPropModified = TRUE ;
	LoadTree();
}
