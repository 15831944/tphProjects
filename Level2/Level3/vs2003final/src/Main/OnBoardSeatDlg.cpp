#include "StdAfx.h"
#include ".\onboardseatdlg.h"

#include "InputOnBoard/Seat.h"
#include "../Common/WinMsg.h"
#include "common/ARCUnit.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "Common\ARCStringConvert.h"
#include "InputOnBoard\AircraftPlacements.h"
#include "Common\ARCMathCommon.h"


IMPLEMENT_DYNAMIC(COnBoardSeatDlg, COnBoardObjectBaseDlg)
COnBoardSeatDlg::COnBoardSeatDlg(CSeat *pSeat,CAircaftLayOut *pLayout,int nProjID, CWnd *pParent)
:COnBoardObjectBaseDlg(pSeat,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardSeatDlg::~COnBoardSeatDlg(void)
{
}

void COnBoardSeatDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardSeatDlg,COnBoardObjectBaseDlg)
// 	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnTreeDoubleClick)
END_MESSAGE_MAP()

BOOL COnBoardSeatDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SEAT));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	//show row and column static
	GetDlgItem(IDC_STATIC_ROW)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_COLUMN)->ShowWindow(SW_SHOW);
	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Seat"));
	}
	else
	{
		SetWindowText(_T("Define Seat"));
		m_cmbName3.SetWindowText(_T("1"));
		m_cmbName4.SetWindowText(_T("A"));
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


void COnBoardSeatDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hItemLength || 
		hTreeItem == m_hItemHeight ||
		hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;
		if(hTreeItem == m_hItemLength )
			dValue = ((CSeat *)m_pObject)->GetLength();
		else if(hTreeItem == m_hItemHeight)
			dValue = ((CSeat *)m_pObject)->GetHeight();
		else if(hTreeItem == m_hItemWidth)
			dValue = ((CSeat *)m_pObject)->GetWidth();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hItemRotation)
	{	
		m_hRClickItem = hTreeItem;

		double dValue = 0.0;
		dValue = ((CSeat *)m_pObject)->GetRotation();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum((int) dValue);
		m_treeProp.SetSpinRange( 0,360 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
		return;

	}else if (hTreeItem == m_hItemStoreMark)
	{
		std::vector<CString> vStr;
		vStr.clear();
		CString str = _T("YES");
		vStr.push_back(str);
		str = _T("NO");
		vStr.push_back(str);
		m_treeProp.SetComboWidth(60);
		m_treeProp.SetComboString(m_hItemStoreMark,vStr);	
	}else if (hTreeItem == m_hItemCapacity)
	{

		if (((CSeat*)m_pObject)->GetStoreMark())
		{
			m_hRClickItem = hTreeItem;

			double dValue = 0.0;
			dValue = ((CSeat *)m_pObject)->GetCapacity();
			m_treeProp.SetDisplayType( 2 );
			m_treeProp.SetDisplayNum((int) dValue);
			m_treeProp.SetSpinRange( 0,10000 );
			m_treeProp.SpinEditLabel( m_hRClickItem );
			return;
		}
	}

}
void COnBoardSeatDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
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
	else if (m_hRClickItem == m_hItemLength || m_hRClickItem == m_hItemHeight || m_hRClickItem == m_hItemWidth)
	{
	//	pMenu = menuPopup.GetSubMenu( 42 );
	}


}

LRESULT COnBoardSeatDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	switch( message )
	{
	case WM_INPLACE_SPIN:
		{
			if (m_hRClickItem == m_hItemLength)
			{	
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				((CSeat*)m_pObject)->SetLength(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
				m_bPropModified = true;
				LoadTree();
			}
			else if (m_hRClickItem == m_hItemHeight)
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				((CSeat*)m_pObject)->SetHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
				m_bPropModified = true;
				LoadTree();
			}
			else if (m_hRClickItem == m_hItemWidth)
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				((CSeat*)m_pObject)->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
				m_bPropModified = true;
				LoadTree();
			}
			else if (m_hRClickItem == m_hItemRotation)
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				((CSeat*)m_pObject)->SetRotation(pst->iPercent );
				m_bPropModified = true;
				LoadTree();
			}
			else if (m_hRClickItem == m_hItemCapacity)
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				((CSeat*)m_pObject)->SetCapacity(pst->iPercent );
				m_bPropModified = true;
				LoadTree();

			}


			if(m_bPropModified)
				UpdateObjectViews();
			break;
		}
	case WM_INPLACE_COMBO2:
		{

			HTREEITEM hSelItem = m_treeProp.GetSelectedItem();

			if (hSelItem!=m_hItemStoreMark)
			{
				break;
			}

			CSeat *pSeat = (CSeat *)m_pObject;

			AirsideObjectTreeCtrlItemDataEx *aoidDataEx=m_treeProp.GetItemDataEx(hSelItem);
			ItemStringSectionColor isscStringColor;

			CString csLabel,strTemp;
			int nSelIndex = m_treeProp.m_comboBox.GetCurSel();
			if(nSelIndex == 0)
			{
				csLabel.Format(_T("Could store carryon:YES"));
				strTemp.Format(_T("YES"));
				m_treeProp.SetItemText(hSelItem,csLabel);
				pSeat->SetStoreMark(true);
			}
			else
			{
				csLabel.Format(_T("Could store carryon:NO"));
				strTemp.Format(_T("NO"));
				m_treeProp.SetItemText(hSelItem,csLabel);
				pSeat->SetStoreMark(false);
			}

			isscStringColor.strSection = strTemp;
			isscStringColor.colorSection = RGB(0,0,255);

			aoidDataEx->vrItemStringSectionColorShow.clear();
			aoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
			m_treeProp.SetItemDataEx(hSelItem,*aoidDataEx);

			aoidDataEx=m_treeProp.GetItemDataEx(m_hItemCapacity);
			
			if(nSelIndex == 0)
			{				
				strTemp.Format(_T("%d"),pSeat->GetCapacity());
				isscStringColor.colorSection = RGB(0,0,255);
			}
			else
			{
				strTemp=m_treeProp.GetItemText(m_hItemCapacity);
				isscStringColor.colorSection = RGB(192,192,192);
			}

			isscStringColor.strSection = strTemp;
			aoidDataEx->vrItemStringSectionColorShow.clear();
			aoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
			m_treeProp.SetItemDataEx(m_hItemCapacity,*aoidDataEx);

			m_bPropModified = true;
			LoadTree();


/*
						CWaveCrossingSpecificationItem *pItem = GetItemWaveCross(hSelItem);
						if(pItem == NULL)
							return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
			
						if(itemType == itemType_TakeOffMinState)
						{
							if(pItem->GetTakeoffSettings().GetMinimum() != bMinimum)
							{
								pItem->GetTakeoffSettings().SetMinimum(bMinimum);
			
			
								HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
								DeleteItem(hSelItem);
			
								LoadTakeOffItems(hParentItem,pItem);
			
							}
						}
						if (itemType == itemType_LandingMinState)
						{
							if(pItem->GetLandingSettings().GetMinimum() != bMinimum)
							{
								pItem->GetLandingSettings().SetMinimum(bMinimum);
								HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hSelItem);
								DeleteItem(hSelItem);
								LoadLandingItems(hParentItem,pItem);
							}
						}*/
			
			break;
		}

	}





	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardSeatDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hItemPosition)
	{
		return PICK_ONEPOINT;
	}
	return PICK_ONEPOINT;

}
void COnBoardSeatDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			CPoint2008 ptPos = path.getPoint(0);
			ptPos.setZ(0.00);
			((CSeat *)m_pObject)->SetPosition(ptPos);
		}
		
		m_bPathModified = true;

		///UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardSeatDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	 CSeat *pSeat = (CSeat *)m_pObject;
 

	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	ARCVector3 seatPos = pSeat->GetPosition();

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

	CString strDeckName = _T("");
	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ]));		
	ASSERT(pSeat->GetDeck());
	if(pSeat->GetDeck())
	{
		strDeckName = pSeat->GetDeck()->GetName();
		isscStringColor.strSection = strDeckName;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	}



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
			double dLenth = pSeat->GetLength();
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
			double dWidth = pSeat->GetWidth();
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
			double dHeight = pSeat->GetHeight();
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
	double dRotation= pSeat->GetRotation();
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

	//store mark
	bool bStoreMark=pSeat->GetStoreMark();
	csLabel.Empty();
	if (bStoreMark)
	{
		csLabel.Format(_T("Could store carryon:YES"));
		strTemp.Format(_T("YES"));

	}else
	{
		csLabel.Format(_T("Could store carryon:NO"));
		strTemp.Format(_T("NO"));
	}

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hItemStoreMark =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hItemStoreMark,aoidDataEx);

	//capacity
	int nCapacity=pSeat->GetCapacity();
	csLabel.Empty();
	csLabel.Format(_T("Capacity:%d"),nCapacity);

	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;

	if (bStoreMark)
	{
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%d"),nCapacity);	
	}else
	{
		isscStringColor.colorSection = RGB(192,192,192);
		strTemp=csLabel;	
	}

	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hItemCapacity =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hItemCapacity,aoidDataEx);

}

void COnBoardSeatDlg::OnOK()
{
	//if(SetObjectName() == false)
	//	return;

	//check the seat name is valid or not
	CString strName;
	m_cmbName1.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("please input object name.");
		return;
	}
	m_bNameModified = true;
	m_objName.m_val[0] = strName.MakeUpper();

	m_cmbName2.GetWindowText(strName);// name 2
	strName.Trim();
	if(strName.IsEmpty())
	{
		MessageBox("The seat name is not valid.\r\nThe field can not be empty, seat name format: SEAT-ECONS-1-A");
		m_cmbName2.SetFocus();
		return;
	}
	m_objName.m_val[1] = strName.MakeUpper();
	m_cmbName3.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("The seat name is not valid. \r\nThe field can not be empty, seat name format: SEAT-ECONS-1-A");
		m_cmbName3.SetFocus();
		return;
	}
	if(ARCStringConvert::isAllNumeric(strName) == false)
	{
		MessageBox("The seat name is not valid. Seat name format: SEAT-ECONS-1-A");
		m_cmbName3.SetFocus();
		return;
	}
	m_objName.m_val[2] = strName.MakeUpper();

	m_cmbName4.GetWindowText(strName);
	strName.MakeUpper();
	strName.Trim();
	if(strName.IsEmpty())
	{
		MessageBox("The seat name is not valid. \r\nThe field can not be empty, seat name format: SEAT-ECONS-1-A");
		m_cmbName3.SetFocus();
		return;
	}
	if(strName.GetLength() > 1)
	{
		MessageBox("The seat name is not valid. \r\nThe field can not be more than 1 letter, seat name format: SEAT-ECONS-1-A");
		return;
	}

	if(strName.GetAt(0) > 'Z' || strName.GetAt(0) < 'A')
	{
		MessageBox("The seat name is not valid.\r\n The field can not be between 'A' and 'Z' , seat name format: SEAT-ECONS-1-A");
		return;
	}
	m_objName.m_val[3] = strName.MakeUpper();
	if( CSeat* pSeat = m_pLayout->GetPlacements()->GetSeatData()->GetNameConflict(m_objName) )
	{
		if(pSeat!=m_pObject)
		{
			MessageBox("The seat name input has been exist.");
			return;
		}
	}

	m_pObject->SetIDName(m_objName);

	if(m_pObject->GetIDName().m_val == m_objName.m_val)
	{
		m_bNameModified = false;
	}
	
	CADODatabase::BeginTransaction();
	try
	{
		((CSeat *)m_pObject)->SetLayOut(m_pLayout);
		((CSeat *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardSeatDlg::UpdateOtherData()
{

	return true;
}

bool COnBoardSeatDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardSeatDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardSeatDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}




