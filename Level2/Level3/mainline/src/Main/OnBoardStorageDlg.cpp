#include "StdAfx.h"
#include ".\onboardstoragedlg.h"

#include "../InputOnboard/Storage.h"
#include "../InputOnboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(COnBoardStorageDlg, COnBoardObjectBaseDlg)
COnBoardStorageDlg::COnBoardStorageDlg( CStorage *pStorage,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/ )
:COnBoardObjectBaseDlg(pStorage,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}
COnBoardStorageDlg::~COnBoardStorageDlg(void)
{
}
void COnBoardStorageDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardStorageDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardStorageDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STORAGE));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Storage"));
	}
	else
	{
		SetWindowText(_T("Define Storage"));
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


void COnBoardStorageDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if (hTreeItem == m_hItemHeightOnDeck)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;

		dValue = ((CStorage *)m_pObject)->GetDeckHeight();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hItemLength || 
		hTreeItem == m_hItemHeight ||
		hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;
		if(hTreeItem == m_hItemLength )
			dValue = ((CStorage *)m_pObject)->GetLength();
		else if(hTreeItem == m_hItemHeight)
			dValue = ((CStorage *)m_pObject)->GetHeight();
		else if(hTreeItem == m_hItemWidth)
			dValue = ((CStorage *)m_pObject)->GetWidth();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hItemRotation)
	{	
		m_hRClickItem = hTreeItem;

		double dValue = 0.0;
		dValue = ((CStorage *)m_pObject)->GetRotation();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum((int) dValue);
		m_treeProp.SetSpinRange( 0,360 );
		m_treeProp.SpinEditLabel( m_hRClickItem );

	}
	else if(hTreeItem == m_hCapacity)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;

		dValue = ((CStorage *)m_pObject)->getCapcity();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );

	}
}
void COnBoardStorageDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
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
	//else if (m_hRClickItem == m_hItemLength )
	//{
	//	//	pMenu = menuPopup.GetSubMenu( 42 );
	//}


}

LRESULT COnBoardStorageDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemHeightOnDeck)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage*)m_pObject)->SetDeckHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemLength)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage*)m_pObject)->SetLength(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemHeight)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage*)m_pObject)->SetHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage*)m_pObject)->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemRotation)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage*)m_pObject)->SetRotation(pst->iPercent );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hCapacity)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CStorage *)m_pObject)->setCapcity(pst->iPercent);
			m_bPropModified = true;
			LoadTree();
		}

		if(m_bPropModified)
			UpdateObjectViews();
	}



	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardStorageDlg::GetFallBackReason()
{

	return PICK_ONEPOINT;

}
void COnBoardStorageDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			CPoint2008 ptPos = path.getPoint(0);
			CStorage *pStorage = (CStorage *)m_pObject;
			if(pStorage)
			{
				ARCVector3 storPos = pStorage->GetPosition();
				ptPos.setZ(storPos[VZ]);
			}
			else
			{
				ptPos.setZ(0.00);
			}

			pStorage->SetPosition(ptPos);
		}


		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardStorageDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	CStorage *pStorage = (CStorage *)m_pObject;

	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	ARCVector3 seatPos = pStorage->GetPosition();

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VX]));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VY]));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ]));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	CString strDeckName = _T("");
	if(pStorage->GetDeck())
		strDeckName = pStorage->GetDeck()->GetName();

	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ]));		
	isscStringColor.strSection = strDeckName;//strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);



	CString strPoint = _T("");
	strPoint.Format( "x = %.2f; y = %.2f; %s",
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VX]),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VY]),
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
			double dLenth = pStorage->GetLength();
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
			double dWidth = pStorage->GetWidth();
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
			double dHeight = pStorage->GetHeight();
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

	//height
	{
		double dHeightOnDeck = pStorage->GetDeckHeight();
		CString csLabel;

		csLabel.Empty();
		csLabel.Format(_T("Height(%.0f)"), dHeightOnDeck);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"), dHeightOnDeck);
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hItemHeightOnDeck =  m_treeProp.InsertItem(csLabel);
		m_treeProp.SetItemDataEx(m_hItemHeightOnDeck,aoidDataEx);
	}

	//rotation
	{
		double dRotation= pStorage->GetRotation();
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

	//capacity
	{	
		double dCapacity = pStorage->getCapcity();
		CString csLabel;
		CString strTemp = _T("");

		csLabel.Empty();
		csLabel.Format(_T("Capacity(%.0f)"),dCapacity);
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.0f"),dCapacity);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		m_hCapacity =  m_treeProp.InsertItem(csLabel);
		m_treeProp.SetItemDataEx(m_hCapacity,aoidDataEx);
	}
}

void COnBoardStorageDlg::OnOK()
{
	if(SetObjectName() == false)
		return;

	CADODatabase::BeginTransaction();
	//
	try
	{
		((CStorage *)m_pObject)->SetLayOut(m_pLayout);
		((CStorage *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardStorageDlg::UpdateOtherData()
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

bool COnBoardStorageDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardStorageDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardStorageDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}