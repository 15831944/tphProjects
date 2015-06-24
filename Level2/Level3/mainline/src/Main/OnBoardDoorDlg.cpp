#include "StdAfx.h"
#include ".\onboarddoordlg.h"

#include "../InputOnboard/Door.h"
#include "../InputOnboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(COnBoardDoorDlg, COnBoardObjectBaseDlg)

COnBoardDoorDlg::COnBoardDoorDlg(CDoor *pDoor,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/)
:COnBoardObjectBaseDlg(pDoor,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardDoorDlg::~COnBoardDoorDlg(void)
{
}
void COnBoardDoorDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardDoorDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardDoorDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	/*if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Door"));
	}
	else
	{
		SetWindowText(_T("Define Door"));
	}*/

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


void COnBoardDoorDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hItemLength || 
		hTreeItem == m_hItemHeight ||
		hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;
		if(hTreeItem == m_hItemLength )
			dValue = ((CDoor *)m_pObject)->GetLength();
		else if(hTreeItem == m_hItemHeight)
			dValue = ((CDoor *)m_pObject)->GetHeight();
		else if(hTreeItem == m_hItemWidth)
			dValue = ((CDoor *)m_pObject)->GetWidth();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hItemRotation)
	{	
		m_hRClickItem = hTreeItem;

		double dValue = 0.0;
		dValue = ((CDoor *)m_pObject)->GetRotation();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum((int) dValue);
		m_treeProp.SetSpinRange( 0,360 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
		return;

	}

}
void COnBoardDoorDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
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

LRESULT COnBoardDoorDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemLength)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CDoor*)m_pObject)->SetLength(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemHeight)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CDoor*)m_pObject)->SetHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CDoor*)m_pObject)->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemRotation)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CDoor*)m_pObject)->SetRotation(pst->iPercent );
			m_bPropModified = true;
			LoadTree();
		}


		if(m_bPropModified)
			UpdateObjectViews();
	}


	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);

}
FallbackReason COnBoardDoorDlg::GetFallBackReason()
{

	return PICK_ONEPOINT;

}
void COnBoardDoorDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			CPoint2008 ptPos = path.getPoint(0);
			CDoor *pDoor = (CDoor *)m_pObject;
			if(pDoor)
			{
				ARCVector3 doorPos = pDoor->GetPosition();
				ptPos.setZ(doorPos[VZ]);
			}
			else
			{
				ptPos.setZ(0.00);
			}

			pDoor->SetPosition(ptPos);
		}

		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardDoorDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	CDoor *pDoor = (CDoor *)m_pObject;


	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	ARCVector3 doorPos = pDoor->GetPosition();

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
	if(pDoor->GetDeck())
		strDeckName = pDoor->GetDeck()->GetName();
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
			double dLenth = pDoor->GetLength();
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
			double dWidth = pDoor->GetWidth();
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
			double dHeight = pDoor->GetHeight();
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
	double dRotation= pDoor->GetRotation();
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

void COnBoardDoorDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	try
	{
		((CDoor *)m_pObject)->SetLayOut(m_pLayout);
		((CDoor *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardDoorDlg::UpdateOtherData()
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

bool COnBoardDoorDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardDoorDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardDoorDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}