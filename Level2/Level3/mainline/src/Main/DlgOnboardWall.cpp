#include "StdAfx.h"
#include ".\dlgonboardwall.h"
#include "InputOnBoard\OnBoardWall.h"
#include "InputOnBoard\Deck.h"
#include "inputonboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CDlgOnboardWall, COnBoardObjectBaseDlg)

CDlgOnboardWall::CDlgOnboardWall( COnBoardWall *pWall,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/ )
:COnBoardObjectBaseDlg(pWall,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;

	//m_dWidth = 0;
	//m_dheight = 0;
}
CDlgOnboardWall::~CDlgOnboardWall(void)
{
}


void CDlgOnboardWall::DoDataExchange( CDataExchange* pDX )
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgOnboardWall,COnBoardObjectBaseDlg)
END_MESSAGE_MAP()


BOOL CDlgOnboardWall::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Wall"));
	}
	else
	{
		SetWindowText(_T("Define Wall"));
	}


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

	return TRUE;
}


FallbackReason CDlgOnboardWall::GetFallBackReason()
{	
	if (m_hRClickItem == m_hItemPosition)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_ONEPOINT;
}
bool CDlgOnboardWall::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CDlgOnboardWall::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CDlgOnboardWall::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CDlgOnboardWall::LoadTree()
{
	m_treeProp.SetRedraw(FALSE);

	m_treeProp.DeleteAllItems();

	COnBoardWall *pWall = (COnBoardWall *)m_pObject;


	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);
	
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	//path
	{
		CPath2008 wallPath = pWall->getPath();

		int nPointCount = wallPath.getCount();
		for (int nPoint = 0; nPoint < nPointCount; ++nPoint)
		{
			CPoint2008 ptPath = wallPath.getPoint(nPoint);


			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),ptPath.getX()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),ptPath.getY()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			CString strDeckName = _T("");
			//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ]));		
			ASSERT(pWall->GetDeck());
			if(pWall->GetDeck())
			{
				strDeckName = pWall->GetDeck()->GetName();
				isscStringColor.strSection = strDeckName;
				aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			}



			CString strPoint = _T("");
			strPoint.Format( "x = %.2f; y = %.2f; %s",
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),ptPath.getX()),
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),ptPath.getY()),
				strDeckName
				/*CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),seatPos[VZ])*/);

				hItemTemp = m_treeProp.InsertItem(strPoint,m_hItemPosition);
			m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);


		}
	}
	m_treeProp.Expand(m_hItemPosition,TVE_EXPAND);
	//dimension


	CString strDimensionLabel = _T("Dimension(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");

	HTREEITEM hItemDimension = m_treeProp.InsertItem(strDimensionLabel);

	{
	//width
		{
			double dWidth = pWall->getWidth();
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
			double dHeight = pWall->getHeight();
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
	m_treeProp.SetRedraw(TRUE);
	
}

void CDlgOnboardWall::SetObjectPath( CPath2008& path )
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			((COnBoardWall *)m_pObject)->setPath(path);
		}

		m_bPathModified = true;

		///UpdateObjectViews();
		LoadTree();
	}
}

bool CDlgOnboardWall::UpdateOtherData()
{
	return true;
}

LRESULT CDlgOnboardWall::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemHeight)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((COnBoardWall*)m_pObject)->setHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}
		else if (m_hRClickItem == m_hItemWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((COnBoardWall*)m_pObject)->setWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}

		if(m_bPropModified)
			UpdateObjectViews();
	}

	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);

}

void CDlgOnboardWall::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	if(((CDeckPathElement*)m_pObject)->getPath().getCount() == 0)
	{
		MessageBox(_T("Please define the Position!"));
		return;
	}
	try
	{
		((COnBoardWall *)m_pObject)->SetLayOut(m_pLayout);
		((COnBoardWall *)m_pObject)->SaveData(m_pLayout->GetID());
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

void CDlgOnboardWall::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	if(hTreeItem == m_hItemHeight || hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;

		if(hTreeItem == m_hItemHeight)
			dValue = ((COnBoardWall *)m_pObject)->getHeight();
		else if(hTreeItem == m_hItemWidth)
			dValue = ((COnBoardWall *)m_pObject)->getWidth();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );


	}
}

void CDlgOnboardWall::OnContextMenu( CMenu& menuPopup, CMenu *& pMenu )
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
	else if (m_hRClickItem == m_hItemHeight || m_hRClickItem == m_hItemWidth)
	{
		//	pMenu = menuPopup.GetSubMenu( 42 );
	}
}