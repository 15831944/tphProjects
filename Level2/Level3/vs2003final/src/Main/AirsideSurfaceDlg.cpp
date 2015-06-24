#include "StdAfx.h"
#include "resource.h"
#include "TermPlanDoc.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidesurfacedlg.h"
#include "../InputAirside/Surface.h"
#include "../Common/WinMsg.h"
#include "CommonData/SurfaceMaterialLib.h"
#include <gdiplus.h>
using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CAirsideSurfaceDlg, CAirsideObjectBaseDlg)
CAirsideSurfaceDlg::CAirsideSurfaceDlg(int nSurfaceID,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nSurfaceID,nAirportID,nProjID,pParent)
{
	m_pObject = new Surface();
	m_bPathModified = false;
}

//CAirsideSurfaceDlg::CAirsideSurfaceDlg( Surface* pSurface,int nProjID, CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pSurface, nProjID,pParent)
//{
//	m_bPathModified = false;
//}
CAirsideSurfaceDlg::~CAirsideSurfaceDlg(void)
{
	GdiplusShutdown(m_gdiplusToken);
	//delete m_pObject;
}

BEGIN_MESSAGE_MAP(CAirsideSurfaceDlg,CAirsideObjectBaseDlg) 
	ON_WM_SIZE( )
END_MESSAGE_MAP()

BOOL CAirsideSurfaceDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();

	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));
	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/ 0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Surface"));
	}
	else
	{
		SetWindowText(_T("Define Surface"));
	}

	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_SHOW);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	//init Texture List
	InitTextureList();
	//select the back ground
	{
		m_wndTextureList.SetFocus();
		int nItemCount = m_wndTextureList.GetItemCount();
		int nItem = 0;
		CString strTexture = ((Surface *)GetObject())->GetTexture();
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

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_15,this);
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

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideSurfaceDlg::InitTextureList()
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
 	m_wndTextureList.SetImageList(&m_TextureImgList, LVSIL_NORMAL);

	int nTxetureItemCount = GetDocument()->GetSurfaceMaterialLib().GetTextureItemCount();
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	CString		strPath;
	for (int i = 0; i < nTxetureItemCount; i++)
	{		
		CSurfaceMaterialLib::TextureItem item = GetDocument()->GetSurfaceMaterialLib().GetTextureItem(i);
		USES_CONVERSION;
		Bitmap img( A2W(item.filename) );
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(32, 32, NULL, NULL));
		// attach the thumbnail bitmap handle to an CBitmap object
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		pImage = new CBitmap();		 
		pImage->Attach(hBmp);
		m_TextureImgList.Add(pImage,RGB(255, 255, 255));
		m_wndTextureList.InsertItem(i,item.m_key,i);		
		delete pImage;
		delete pThumbnail;
	}
}
void CAirsideSurfaceDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
	}
	//else if (m_hRClickItem == m_hWidth)
	//{
	//	pMenu = menuPopup.GetSubMenu( 42 );
	//}
	//else if (m_hRClickItem == m_hMarking1 || m_hRClickItem == m_hMarking2)
	//{
	//	pMenu = menuPopup.GetSubMenu(63);
	//}

}
LRESULT CAirsideSurfaceDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideSurfaceDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideSurfaceDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Surface *)GetObject())->SetPath(path);
		m_bPathModified = true;
		LoadTree();
	}
}

void CAirsideSurfaceDlg::LoadTree()
{

	m_treeProp.DeleteAllItems();

	// Service Location
	CString csLabel = CString("Service Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CPath2008 path = ((Surface *)GetObject())->GetPath();

	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hServiceLocation);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}

	m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);


}
void CAirsideSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	
	DDX_Control(pDX, IDC_LIST_PROP, m_wndTextureList);
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

void CAirsideSurfaceDlg::OnOK()
{


	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideSurfaceDlg::UpdateOtherData()
{
	POSITION pos = m_wndTextureList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndTextureList.GetNextSelectedItem(pos);
		CString strText = m_wndTextureList.GetItemText(nItem,0);
		if (((Surface *)GetObject())->GetTexture().CompareNoCase(strText) != 0)
		{
			((Surface *)GetObject())->SetTexture(strText);
			m_bPropModified = true;
		}
	}
	CPath2008 path = ((Surface *)GetObject())->GetPath();
	if (path.getCount() ==0)
	{
		MessageBox(_T("Please pick the surface path from the map."));
		return false;
	}
	if (m_nObjID != -1 && m_bPathModified)
	{
		((Surface *)GetObject())->UpdatePath();
	}

	return true;
}



void CAirsideSurfaceDlg::OnLvnItemchangedListProp( NMHDR *pNMHDR, LRESULT *pResult )
{
	POSITION pos = m_wndTextureList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndTextureList.GetNextSelectedItem(pos);
		CString strText = m_wndTextureList.GetItemText(nItem,0);
		if (((Surface *)GetObject())->GetTexture().CompareNoCase(strText) != 0)
		{
			((Surface *)GetObject())->SetTexture(strText);
			m_bPropModified = true;
		}
	}
}

void CAirsideSurfaceDlg::OnSize( UINT nType , int cx, int cy)
{
	CAirsideObjectBaseDlg::OnSize(nType,cx,cy);
	if(m_treeProp.GetSafeHwnd())
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
	}
}

bool CAirsideSurfaceDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideSurfaceDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideSurfaceDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}