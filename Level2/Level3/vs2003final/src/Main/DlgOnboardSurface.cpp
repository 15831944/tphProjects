#include "StdAfx.h"
#include ".\dlgonboardsurface.h"
#include "InputOnBoard\OnboardSurface.h"
#include "InputOnBoard\Deck.h"
#include "inputonboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CDlgOnboardSurface, COnBoardObjectBaseDlg)

CDlgOnboardSurface::CDlgOnboardSurface( COnboardSurface *pSureface,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/ )
:COnBoardObjectBaseDlg(pSureface,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;

}
CDlgOnboardSurface::~CDlgOnboardSurface(void)
{
}
void CDlgOnboardSurface::DoDataExchange(CDataExchange* pDX)
{

	DDX_Control(pDX, IDC_LIST_PROP, m_wndTextureList);
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgOnboardSurface,COnBoardObjectBaseDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MOVIES, OnLvnItemchangedListProp)
END_MESSAGE_MAP()


BOOL CDlgOnboardSurface::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();

	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Surface"));
	}
	else
	{
		SetWindowText(_T("Define Surface"));
	}

	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_SHOW);
	//init Texture List
	InitTextureList();
	RemoveResize(IDC_TREE_PROPERTY);
	SetResize(IDC_TREE_PROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	RemoveResize(IDC_LIST_PROP);
	SetResize(IDC_LIST_PROP, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	//select the back ground
	{
		m_wndTextureList.SetFocus();
		int nItemCount = m_wndTextureList.GetItemCount();
		int nItem = 0;
		CString strTexture = ((COnboardSurface *)m_pObject)->getTexture();
		for (int i = 0; i < nItemCount; ++i)
		{
			CString strItemText = m_wndTextureList.GetItemText(i,0);
			if (strItemText.CompareNoCase(strTexture) == 0)
			{
				nItem = i;
				break;
			}
		}
		if (nItem >= 0 && nItem < m_wndTextureList.GetItemCount())
		{
			//			m_wndTextureList.SetSelectionMark(nItem);	
			m_wndTextureList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			m_wndTextureList.SetItemState(nItem, LVIS_FOCUSED , LVIS_FOCUSED);
		}
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

void CDlgOnboardSurface::InitTextureList()
{
	CRect rctree;
	m_treeProp.GetWindowRect(&rctree);

	ScreenToClient(&rctree);
	CRect rclist(0,0,0,0);
	rclist.SetRect(rctree.Width()/2 + rctree.left +1,rctree.top,rctree.right,rctree.bottom);

	rctree.right = rctree.Width()/2 + rctree.left -1;
	m_treeProp.MoveWindow(rctree,true);
	m_wndTextureList.MoveWindow(rclist);
	m_wndTextureList.ShowWindow(SW_SHOW);


	m_wndTextureList.ModifyStyle(0, LVS_ICON|LVS_ALIGNTOP|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	m_TextureImgList.Create(32, 32, ILC_COLOR24, 0, 1);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_SURFACEAREASTEXTURE);
	m_TextureImgList.Add(&bmp, RGB(255, 255, 255));
	m_wndTextureList.SetImageList(&m_TextureImgList, LVSIL_NORMAL);
	m_wndTextureList.InsertItem(0, "Lawn-1", 0);
	m_wndTextureList.InsertItem(1, "Lawn-2", 1);
	m_wndTextureList.InsertItem(2, "Lawn-3", 2);
	m_wndTextureList.InsertItem(3, "RoadBlack", 3);
	m_wndTextureList.InsertItem(4, "SeaBlue", 4);
	m_wndTextureList.InsertItem(5, "BlueCarpet", 5);
	m_wndTextureList.InsertItem(6, "CyanCarpet", 6);
	m_wndTextureList.InsertItem(7, "GreyCarpet", 7);
	m_wndTextureList.InsertItem(8, "MagentaCarpet", 8);
	m_wndTextureList.InsertItem(9, "OrangeCarpet", 9);
	m_wndTextureList.InsertItem(10, "PurpleCarpet", 10);
	m_wndTextureList.InsertItem(11, "RedCarpet", 11);


}

FallbackReason CDlgOnboardSurface::GetFallBackReason()
{
	if (m_hRClickItem == m_hItemPosition)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_ONEPOINT;
}
bool CDlgOnboardSurface::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CDlgOnboardSurface::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CDlgOnboardSurface::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CDlgOnboardSurface::LoadTree()
{
	m_treeProp.SetRedraw(FALSE);

	m_treeProp.DeleteAllItems();

	COnboardSurface *pPortal = (COnboardSurface *)m_pObject;


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

void CDlgOnboardSurface::SetObjectPath( CPath2008& path )
{
	if (m_hItemPosition == m_hRClickItem)
	{
		if(path.getCount() > 0)
		{
			((COnboardSurface *)m_pObject)->setPath(path);
		}

		m_bPathModified = true;

		///UpdateObjectViews();
		LoadTree();
	}
}

bool CDlgOnboardSurface::UpdateOtherData()
{
	return true;
}

LRESULT CDlgOnboardSurface::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{

	}

	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);

}

void CDlgOnboardSurface::OnOK()
{
	if(SetObjectName() == false)
		return;
	//save texture
	OnLvnItemchangedListProp(NULL,NULL);
	//
	if(((CDeckPathElement*)m_pObject)->getPath().getCount() == 0)
	{
		MessageBox(_T("Please define the Position!"));
		return;
	}
	CADODatabase::BeginTransaction();
	//
	try
	{
		((COnboardSurface *)m_pObject)->SetLayOut(m_pLayout);
		((COnboardSurface *)m_pObject)->SaveData(m_pLayout->GetID());
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

void CDlgOnboardSurface::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{

}

void CDlgOnboardSurface::OnContextMenu( CMenu& menuPopup, CMenu *& pMenu )
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
void CDlgOnboardSurface::OnLvnItemchangedListProp( NMHDR *pNMHDR, LRESULT *pResult )
{
	POSITION pos = m_wndTextureList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndTextureList.GetNextSelectedItem(pos);
		CString strText = m_wndTextureList.GetItemText(nItem,0);
		if (((COnboardSurface *)m_pObject)->getTexture().CompareNoCase(strText) != 0)
		{
			((COnboardSurface *)m_pObject)->setTexture(strText);
			m_bPropModified = true;
		}
	}
}