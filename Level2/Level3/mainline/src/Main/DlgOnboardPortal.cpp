#include "StdAfx.h"
#include ".\dlgonboardportal.h"
#include "InputOnBoard\OnboardPortal.h"
#include "InputOnBoard\Deck.h"
#include "inputonboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"


IMPLEMENT_DYNAMIC(CDlgOnboardPortal, COnBoardObjectBaseDlg)

CDlgOnboardPortal::CDlgOnboardPortal( COnboardPortal *pPortal,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/ )
:COnBoardObjectBaseDlg(pPortal,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;

}
CDlgOnboardPortal::~CDlgOnboardPortal(void)
{
}

BEGIN_MESSAGE_MAP(CDlgOnboardPortal,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL CDlgOnboardPortal::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Portal"));
	}
	else
	{
		SetWindowText(_T("Define Portal"));
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

FallbackReason CDlgOnboardPortal::GetFallBackReason()
{
	if (m_hRClickItem == m_hItemPosition)
	{
		return PICK_TWOPOINTS;
	}
	return PICK_ONEPOINT;
}

bool CDlgOnboardPortal::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CDlgOnboardPortal::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CDlgOnboardPortal::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CDlgOnboardPortal::LoadTree()
{
	m_treeProp.SetRedraw(FALSE);

	m_treeProp.DeleteAllItems();

	COnboardPortal *pPortal = (COnboardPortal *)m_pObject;


	//position
	CString strPositionLabel =_T("Position(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemPosition = m_treeProp.InsertItem(strPositionLabel);

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;

	//path
	{
		CPath2008 wallPath = pPortal->getPath();

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
			ASSERT(pPortal->GetDeck());
			if(pPortal->GetDeck())
			{
				strDeckName = pPortal->GetDeck()->GetName();
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

	m_treeProp.SetRedraw(TRUE);
}

void CDlgOnboardPortal::SetObjectPath( CPath2008& path )
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			((COnboardPortal *)m_pObject)->setPath(path);
		}

		m_bPathModified = true;

		///UpdateObjectViews();
		LoadTree();
	}
}

bool CDlgOnboardPortal::UpdateOtherData()
{
	return true;
}
LRESULT CDlgOnboardPortal::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{
	}

	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);

}

void CDlgOnboardPortal::OnOK()
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
	//
	try
	{
		((COnboardPortal *)m_pObject)->SetLayOut(m_pLayout);
		((COnboardPortal *)m_pObject)->SaveData(m_pLayout->GetID());
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

void CDlgOnboardPortal::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{

}

void CDlgOnboardPortal::OnContextMenu( CMenu& menuPopup, CMenu *& pMenu )
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


























