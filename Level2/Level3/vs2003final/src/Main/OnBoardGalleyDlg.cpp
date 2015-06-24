#include "StdAfx.h"
#include ".\onboardgalleydlg.h"
#include "inputonboard/AircaftLayOut.h"
#include "InputOnBoard/OnboardGalley.h"

IMPLEMENT_DYNAMIC(COnBoardGalleyDlg, COnBoardObjectBaseDlg)

COnBoardGalleyDlg::COnBoardGalleyDlg(COnboardGalley *pGalley,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/)
:COnBoardObjectBaseDlg(pGalley,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardGalleyDlg::~COnBoardGalleyDlg( void )
{

}
void COnBoardGalleyDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardGalleyDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardGalleyDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
	SetWindowText(_T("Modify Galley"));
	}
	else
	{
	SetWindowText(_T("Define Galley"));
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

	//init tree properties
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COnBoardGalleyDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	//if(hTreeItem == m_hItemThickness)
	//{
	//	m_hRClickItem = hTreeItem;
	//	double dValue = 0.0;

	//	dValue = ((CCabinDivider *)m_pObject)->getThickness();

	//	m_treeProp.SetDisplayType( 2 );
	//	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
	//	m_treeProp.SetSpinRange( 1,10000 );
	//	m_treeProp.SpinEditLabel( m_hRClickItem );

	//}
	//if(hTreeItem == m_hItemPosition)
	//{
	//	m_hRClickItem = hTreeItem;
	//	double dValue = 0.0;

	//	dValue = ((CCabinDivider *)m_pObject)->GetPos();

	//	m_treeProp.SetDisplayType( 2 );
	//	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
	//	m_treeProp.SetSpinRange( 1,10000 );
	//	m_treeProp.SpinEditLabel( m_hRClickItem );

	//}


}
void COnBoardGalleyDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hItemPosition)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
		pMenu->DeleteMenu( ID_PROCPROP_DELETE ,MF_BYCOMMAND);
	}



}

LRESULT COnBoardGalleyDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	//if( message == WM_INPLACE_SPIN )
	//{
	//	if (m_hRClickItem == m_hItemThickness)
	//	{	
	//		LPSPINTEXT pst = (LPSPINTEXT) lParam;
	//		((CCabinDivider *)m_pObject)->setThickness(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
	//		m_bPropModified = true;
	//		LoadTree();
	//	}
	//	else if (m_hRClickItem == m_hItemPosition)
	//	{
	//		LPSPINTEXT pst = (LPSPINTEXT) lParam;
	//		((CCabinDivider *)m_pObject)->SetPos(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
	//		m_bPropModified = true;
	//		LoadTree();
	//	}


	//	if(m_bPropModified)
	//		UpdateObjectViews();
	//}



	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardGalleyDlg::GetFallBackReason()
{

	return PICK_ONEPOINT;

}
void COnBoardGalleyDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			CPoint2008 ptPos = path.getPoint(0);
			ptPos.setZ(0.00);
			((COnboardGalley *)m_pObject)->SetPosition(ptPos);
		}
		m_bPathModified = true;
		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardGalleyDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	COnboardGalley *pGalley = (COnboardGalley *)m_pObject;

	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	ARCVector3 doorPos = pGalley->GetPosition();

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),doorPos[VX]));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),doorPos[VY]));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),doorPos[VZ]));		
	//isscStringColor.strSection = strTemp;
	//aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);


	CString strDeckName = _T("");
	if(pGalley->GetDeck())
		strDeckName = pGalley->GetDeck()->GetName();
	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ]));		
	isscStringColor.strSection = strDeckName;//strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);



	CString strPoint = _T("");
	strPoint.Format( "x = %.2f; y = %.2f; %s",
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),doorPos[VX]),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),doorPos[VY]),
		strDeckName
		/*CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ])*/);

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hItemPosition);
	m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);

	m_treeProp.Expand(m_hItemPosition,TVE_EXPAND);



}

void COnBoardGalleyDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	try
	{
		((COnboardGalley *)m_pObject)->SetLayOut(m_pLayout);
		((COnboardGalley *)m_pObject)->SaveData(m_pLayout->GetID());
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save data failed"));
		return;
	}


	COnBoardObjectBaseDlg::OnOK();
}

bool COnBoardGalleyDlg::UpdateOtherData()
{
	//CPath2008 path = ((Runway *)GetObject())->GetPath();
	//if (path.getCount() ==0)
	//{
	//	MessageBox(_T("Please pick the runway path from the map."));
	//	return false;
	//}
	////try
	////{
	//if (m_bPathModified && m_nObjID != -1)
	//{
	//	((Runway *)GetObject())->UpdatePath();
	//}
	////}
	////catch (CADOException &e)
	////{
	////	e.ErrorMessage();
	////	MessageBox(_T("Cann't update the Object path."));
	////	return false;
	////}
	return true;
}

bool COnBoardGalleyDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardGalleyDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardGalleyDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}