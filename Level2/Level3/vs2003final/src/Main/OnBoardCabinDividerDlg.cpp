#include "StdAfx.h"
#include ".\onboardcabindividerdlg.h"

#include "InputOnBoard/CabinDivider.h"
#include "../Common/WinMsg.h"
#include "common/ARCUnit.h"
#include "InputOnBoard/AircaftLayOut.h"




IMPLEMENT_DYNAMIC(COnBoardCabinDividerDlg, COnBoardObjectBaseDlg)
COnBoardCabinDividerDlg::COnBoardCabinDividerDlg(CCabinDivider *pDivider,CAircaftLayOut *pLayout,int nProjID, CWnd *pParent)
//:COnBoardObjectBaseDlg(pDivider,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}


COnBoardCabinDividerDlg::~COnBoardCabinDividerDlg(void)
{
}

void COnBoardCabinDividerDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardCabinDividerDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardCabinDividerDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	/*if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Cabin Divider"));
	}
	else
	{
		SetWindowText(_T("Define Cabin Divider"));
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


void COnBoardCabinDividerDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hItemThickness)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;

		dValue = ((CCabinDivider *)m_pObject)->getThickness();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );

	}
	if(hTreeItem == m_hItemPosition)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;

		dValue = ((CCabinDivider *)m_pObject)->GetPos();

		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );

	}


}
void COnBoardCabinDividerDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	//if(m_hRClickItem==m_hItemPosition)
	//{
	//	//pMenu=menuPopup.GetSubMenu(5);
	//	//pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
	//	//pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
	//	//pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
	//	//pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
	//	//pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
	//}
	//else if (m_hRClickItem == m_hItemLength )
	//{
	//	//	pMenu = menuPopup.GetSubMenu( 42 );
	//}


}

LRESULT COnBoardCabinDividerDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemThickness)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CCabinDivider *)m_pObject)->setThickness(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemPosition)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((CCabinDivider *)m_pObject)->SetPos(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}


		if(m_bPropModified)
			UpdateObjectViews();
	}



	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardCabinDividerDlg::GetFallBackReason()
{

	return PICK_ONEPOINT;

}
void COnBoardCabinDividerDlg::SetObjectPath(CPath2008& path)
{

}

void COnBoardCabinDividerDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	CCabinDivider *pDivider = (CCabinDivider *)m_pObject;


	//position

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;


	{
		//position
		{
			double dPosition = pDivider->GetPos();
			CString csLabel;
			CString strTemp = _T("");

			csLabel.Empty();
			csLabel.Format(_T("Position("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dPosition));	
			
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dPosition));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemPosition =  m_treeProp.InsertItem(csLabel);
			m_treeProp.SetItemDataEx(m_hItemPosition,aoidDataEx);
		}
		//width
		{
			double dThickness = pDivider->getThickness();
			CString csLabel;
			CString strTemp = _T("");

			csLabel.Empty();
			csLabel.Format(_T("Width("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dThickness));
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dThickness));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemThickness =  m_treeProp.InsertItem(csLabel);
			m_treeProp.SetItemDataEx(m_hItemThickness,aoidDataEx);
		}

	}



}

void COnBoardCabinDividerDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
		
	CADODatabase::BeginTransaction();
	//
	try
	{
		((CCabinDivider *)m_pObject)->SetLayOut(m_pLayout);
		((CCabinDivider *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardCabinDividerDlg::UpdateOtherData()
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

bool COnBoardCabinDividerDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardCabinDividerDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardCabinDividerDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}