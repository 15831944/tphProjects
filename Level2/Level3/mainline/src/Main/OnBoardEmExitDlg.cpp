#include "StdAfx.h"
#include ".\onboardemexitdlg.h"
#include "inputonboard/AircaftLayOut.h"
#include "InputOnBoard/EmergencyExit.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(COnBoardEmExitDlg, COnBoardObjectBaseDlg)

COnBoardEmExitDlg::COnBoardEmExitDlg(CEmergencyExit *pEmExit,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/)
:COnBoardObjectBaseDlg(pEmExit,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardEmExitDlg::~COnBoardEmExitDlg( void )
{

}
void COnBoardEmExitDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardEmExitDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardEmExitDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
	SetWindowText(_T("Modify Emergency Exit"));
	}
	else
	{
	SetWindowText(_T("Define Emergency Exit"));
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


void COnBoardEmExitDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hItemLength || 
		hTreeItem == m_hItemHeight ||
		hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;
		if(hTreeItem == m_hItemLength )
			dValue = ((CEmergencyExit *)m_pObject)->GetLength();
		else if(hTreeItem == m_hItemHeight)
			dValue = ((CEmergencyExit *)m_pObject)->GetHeight();
		else if(hTreeItem == m_hItemWidth)
			dValue = ((CEmergencyExit *)m_pObject)->GetWidth();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hItemRotation)
	{	
		m_hRClickItem = hTreeItem;

		double dValue = 0.0;
		dValue = ((CEmergencyExit *)m_pObject)->GetRotation();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum((int) dValue);
		m_treeProp.SetSpinRange( 0,360 );
		m_treeProp.SpinEditLabel( m_hRClickItem );

	}
}
void COnBoardEmExitDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
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

LRESULT COnBoardEmExitDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemLength)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CEmergencyExit*)m_pObject)->SetLength(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemHeight)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CEmergencyExit*)m_pObject)->SetHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CEmergencyExit*)m_pObject)->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemRotation)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CEmergencyExit*)m_pObject)->SetRotation(pst->iPercent );
			m_bPropModified = true;
			LoadTree();
		}

		if(m_bPropModified)
			UpdateObjectViews();
	}
	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason COnBoardEmExitDlg::GetFallBackReason()
{
	return PICK_ONEPOINT;
}

void COnBoardEmExitDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			CPoint2008 ptPos = path.getPoint(0);
			ptPos.setZ(0.00);
			((CEmergencyExit *)m_pObject)->SetPosition(ptPos);
		}
		m_bPathModified = true;
		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardEmExitDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	CEmergencyExit *pEmExit = (CEmergencyExit *)m_pObject;


	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	ARCVector3 doorPos = pEmExit->GetPosition();

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

	CString strDeckName = _T("");
	if(pEmExit->GetDeck())
		strDeckName = pEmExit->GetDeck()->GetName();
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

	//dimension
	CString strDimensionLabel = _T("Dimension(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");

	HTREEITEM hItemDimension = m_treeProp.InsertItem(strDimensionLabel);

	{
		{
			double dLenth = pEmExit->GetLength();
			CString csLabel;

			csLabel.Empty();
			csLabel.Format(_T("Length(%.0f)"), CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dLenth));
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dLenth));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemLength =  m_treeProp.InsertItem(csLabel,hItemDimension);
			m_treeProp.SetItemDataEx(m_hItemLength,aoidDataEx);
		}
		//width
		{
			double dWidth = pEmExit->GetWidth();
			CString csLabel;

			csLabel.Empty();
			csLabel.Format(_T("Width(%.0f)"), CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dWidth));
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dWidth));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemWidth =  m_treeProp.InsertItem(csLabel,hItemDimension);
			m_treeProp.SetItemDataEx(m_hItemWidth,aoidDataEx);
		}
		//height
		{
			double dHeight = pEmExit->GetHeight();
			CString csLabel;

			csLabel.Empty();
			csLabel.Format(_T("Height(%.0f)"), CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dHeight));
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dHeight));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemHeight =  m_treeProp.InsertItem(csLabel,hItemDimension);
			m_treeProp.SetItemDataEx(m_hItemHeight,aoidDataEx);
		}

		m_treeProp.Expand(hItemDimension,TVE_EXPAND);


	}

	//rotation
	double dRotation= pEmExit->GetRotation();
	CString csLabel;

	csLabel.Empty();
	csLabel.Format(_T("Rotation(%.0f)degree"), dRotation);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.0f"), dRotation);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hItemRotation =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hItemRotation,aoidDataEx);
}

void COnBoardEmExitDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	try
	{
		((CEmergencyExit *)m_pObject)->SetLayOut(m_pLayout);
		((CEmergencyExit *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardEmExitDlg::UpdateOtherData()
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

bool COnBoardEmExitDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardEmExitDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardEmExitDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}