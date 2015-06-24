#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include ".\StartPositionsDlg.h"

#include "../Common/WinMsg.h"
#include "../InputAirside/StartPosition.h"


IMPLEMENT_DYNAMIC(CStartPositionDlg, CAirsideObjectBaseDlg)
CStartPositionDlg::CStartPositionDlg(int nStartPositionID,int nAirportID,int nProjID,CWnd *pParent)
: CAirsideObjectBaseDlg(nStartPositionID,nAirportID,nProjID,pParent),
m_hServiceLocationItem(NULL), m_hServiceLocationData(NULL),
m_hRadiusItem(NULL), m_hRadiusData(NULL),
m_hMarkingItem(NULL), m_hMarkingData(NULL)
{
	m_pObject = new CStartPosition();
	m_pObject->setAptID(nAirportID);
}


CStartPositionDlg::~CStartPositionDlg(void)
{

}
void CStartPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStartPositionDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
END_MESSAGE_MAP()

BOOL CStartPositionDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();

	// To do: add your own dialog-init code here
	HBITMAP hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STARTPOSITION));
	ASSERT(hBitmap);
	m_ImageStatic.SetBitmap(hBitmap);

	if (-1 != m_nObjID)
	{
		SetWindowText(_T("Modify StartPosition"));
	}
	else
	{
		SetWindowText(_T("Define StartPosition"));
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

	// load data from one INSTANCE of StartPosition
	LoadTree();
	return TRUE;
}



void CStartPositionDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
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

void CStartPositionDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if (m_hRClickItem == m_hServiceLocationItem)
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


LRESULT CStartPositionDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	// 

	// To deal with the change of TreeCtrl
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hRadiusData)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStartPosition*)GetObject())->SetRadius(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
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
			((CStartPosition*)GetObject())->SetMarking((const char *)strValue);
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}



FallbackReason CStartPositionDlg::GetFallBackReason()
{
	// We want pick a taxi-way (an OBJECT)
	return PICK_ALTOBJECT;
}

bool CStartPositionDlg::DoTempFallBackFinished(WPARAM wParam, LPARAM lParam)
{
	// You should display the window manually
	ShowWindow(SW_SHOW);	
	EnableWindow();

	//if(m_pSelHoldShortLineNode == NULL)
	//	return (false);

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

	CStartPosition* pStartPos = (CStartPosition*)GetObject();
	pStartPos->SetTaxiwayID(pTaxiway->getID());
	pStartPos->SetTaxiwayDivision(distancePP.m_fPathParameter);
	pStartPos->SetRadius(pTaxiway->GetWidth()/2.0);

	m_bPropModified = true;
	UpdateObjectViews();
	LoadTree();

	return true;

}

void CStartPositionDlg::SetObjectPath(CPath2008& path)
{
	// Do nothing.
	// CAirsideObjectBaseDlg::SetObjectPath(path);
}


void CStartPositionDlg::OnOK()
{
	if (-1 == ((CStartPosition*)GetObject())->GetTaxiwayID())
	{
		MessageBox(_T("Please select the service location in a \"Taxiway\" before saving!"));
		return;
	}
	CAirsideObjectBaseDlg::OnOK();
}

bool CStartPositionDlg::UpdateOtherData()
{
	return true;//CAirsideObjectBaseDlg::UpdateOtherData();
}


bool CStartPositionDlg::ConvertUnitFromDBtoGUI(void)
{
	//return CUnitPiece::ConvertUnitFromDBtoGUI();
	return true;
}

bool CStartPositionDlg::RefreshGUI(void)
{
	LoadTree();
	return true;//CUnitPiece::RefreshGUI();
}

bool CStartPositionDlg::ConvertUnitFromGUItoDB(void)
{
	//return CUnitPiece::ConvertUnitFromGUItoDB();
	return true;
}

// to refresh the tree from data
void CStartPositionDlg::LoadTree(void)
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

	// insert subs
	InsertColoredSubItem();

	m_treeProp.Expand(m_hServiceLocationItem, TVE_EXPAND);
	m_treeProp.Expand(m_hRadiusItem, TVE_EXPAND);
	m_treeProp.Expand(m_hMarkingItem, TVE_EXPAND);
}

void CStartPositionDlg::OnMarkingModify(void)
{
	CString strValue= m_treeProp.GetItemText(m_hRClickItem);
	m_treeProp.StringEditLabel(m_hRClickItem, strValue);
	m_bPropModified = true;
}

// Insert the service point item (" x = *.*,  y = *.*,  z = *.* ")
void CStartPositionDlg::InsertColoredSubItem()
{
	CPoint2008 pt;
	CStartPosition* pStartPosition = (CStartPosition*)GetObject();
	ASSERT(pStartPosition && ALT_STARTPOSITION == pStartPosition->GetType());

	// here GetServicePoint should work OK
//	ASSERT(pStartPosition->GetServicePoint(pt));

	if (pStartPosition->GetServicePoint(pt))
	{
		CString str;

		//set data ex
		ItemStringSectionColor isscStringColor;
		isscStringColor.colorSection = RGB(0,0,255);

		AirsideObjectTreeCtrlItemDataEx aoidDataEx;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;

		// related taxiway's name
		ALTObject* pObj = ALTObject::ReadObjectByID(pStartPosition->GetTaxiwayID());
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
			pStartPosition->GetRadius()));
		m_hRadiusData = m_treeProp.InsertItem(str, m_hRadiusItem);

		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_treeProp.SetItemDataEx(m_hRadiusData, aoidDataEx);

		// marking data
		str = pStartPosition->GetMarking().c_str();
		m_hMarkingData = m_treeProp.InsertItem(str, m_hMarkingItem);

		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.strSection = str;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_treeProp.SetItemDataEx(m_hMarkingData, aoidDataEx);
	}

}

void CStartPositionDlg::OnRadiusModify(void)
{
	double radius = ((CStartPosition*)GetObject())->GetRadius();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),radius )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );

	m_bPropModified = true;
}

void CStartPositionDlg::OnProcpropDelete(void)
{
	CStartPosition* pStartPos = (CStartPosition*)GetObject();
	ASSERT(pStartPos);
	pStartPos->SetTaxiwayID(-1);

	LoadTree();
}
