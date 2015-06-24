// DlgStructureProp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgStructureProp.h"
#include <Common/WinMsg.h>
#include <common/UnitsManager.h>
#include <CommonData/Fallback.h>
#include <Inputs/Structure.h>
#include <Inputs/StructureList.h>
#include <CommonData/Textures.h>
#include "Floor2.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "CommonData/SurfaceMaterialLib.h"
#include <gdiplus.h>
using Gdiplus::Bitmap;
// CDlgStructureProp dialog

IMPLEMENT_DYNAMIC(CDlgStructureProp, CDialog)

CDlgStructureProp::CDlgStructureProp(CStructure* _pStructure/* =NULL */, CWnd* pParent /* = NULL */)
: CDialog(CDlgStructureProp::IDD, pParent)
{

	m_csLevel1 = _T("");
	m_csLevel2 = _T("");
	m_csLevel3 = _T("");
	m_csLevel4 = _T("");
	m_strTexture = _T("");
	m_pStructure = NULL;
	if (_pStructure)
	{
		m_pStructure = _pStructure ;
	}

}

CDlgStructureProp::~CDlgStructureProp()
{
	 Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void CDlgStructureProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NAME_1, m_wndName1Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_2, m_wndName2Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_3, m_wndName3Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_4, m_wndName4Combo);
    DDX_Control(pDX, IDC_TREE_PROPERTIES, m_wndPropTree);
	DDX_Control(pDX, IDC_LIST_TEXTURE, m_wndTextureList);

	DDX_CBString(pDX, IDC_COMBO_NAME_1, m_csLevel1);
	DDX_CBString(pDX, IDC_COMBO_NAME_2, m_csLevel2);
	DDX_CBString(pDX, IDC_COMBO_NAME_3, m_csLevel3);
	DDX_CBString(pDX, IDC_COMBO_NAME_4, m_csLevel4);
}

BEGIN_MESSAGE_MAP(CDlgStructureProp, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PROPERTIES, OnNMRclickTreeProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgStructureProp message handlers

BOOL CDlgStructureProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
/*
	pDoc->m_logFile.OpenLog("STRUCTURE");
	pDoc->m_logFile.Log("STRUCTURE","CString");
	pDoc->m_logFile.Log("STRUCTURE",123);
	
	pDoc->m_logFile.OpenLog("pDoc m_logFile");
	pDoc->m_logFile.Log("pDoc m_logFile","pDoc->m_logFile");

	pDoc->m_logFile.CloseAllLog();
	// set the properties tree contents
/*	m_hServiceLocation = m_wndPropTree.InsertItem(_T("Service Location(m)"));

	for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
	{
		ARCVector3 v3D = m_vServiceLocation[i];	
		CString csPoint;
		csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );
		m_wndPropTree.InsertItem(csPoint ,m_hServiceLocation );
	}
*/

	// set the texture list
	m_wndTextureList.ModifyStyle(0, LVS_ICON|LVS_ALIGNTOP|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	m_TextureImgList.Create(32, 32, ILC_COLOR24, 0, 1);
	m_wndTextureList.SetImageList(&m_TextureImgList, LVSIL_NORMAL);
	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	int nTxetureItemCount = pDoc->GetSurfaceMaterialLib().GetTextureItemCount();
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	CString		strPath;
	for (int i = 0; i < nTxetureItemCount; i++)
	{		
		CSurfaceMaterialLib::TextureItem item = pDoc->GetSurfaceMaterialLib().GetTextureItem(i);
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
	// load the name of the structure
	m_hServiceLocation = m_wndPropTree.InsertItem(_T("Service Location(m)"));
	

	if(m_pStructure)
	{

		const CStructureID pID = m_pStructure->getID();
		if (!pID.isBlank())
		{	
			char szname[128];
			pID.getNameAtLevel( szname, 0 );
			setcsLevel1(szname);
			szname[0] = 0;
			pID.getNameAtLevel( szname, 1 );
			setcsLevel2(szname);
			szname[0] = 0;
			pID.getNameAtLevel( szname, 2 );
			setcsLevel3(szname);
			szname[0] = 0;
			pID.getNameAtLevel( szname, 3 );
			setcsLevel4(szname);
		}
		// service location 
		for(int i=0;i<m_pStructure->getPointNum();i++)
		{
			Point & p=m_pStructure->getPointAt(i);
			m_vServiceLocation.push_back(ARCVector3(p.getX(),p.getY(),0.0));
		}
		UpdateData(FALSE);
		LoadTree();
		// texture ID
		setText(m_pStructure->getTexture());

		m_wndTextureList.SetFocus();
		int nItemCount = m_wndTextureList.GetItemCount();
		int nItem = 0;
		for (int i = 0; i < nItemCount; ++i)
		{
			CString strItemText = m_wndTextureList.GetItemText(i,0);
			if (strItemText.CompareNoCase(m_strTexture) == 0)
			{
				nItem = i;
				break;
			}
		}
		if (nItem >= 0 && nItem < m_wndTextureList.GetItemCount())
		{
			m_wndTextureList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			m_wndTextureList.SetItemState(nItem, LVIS_FOCUSED , LVIS_FOCUSED);
		}

	}
//	else
//	{
//		m_hServiceLocation = m_wndPropTree.InsertItem(_T("Service Location(m)"));
//	}


	return TRUE;
}

void CDlgStructureProp::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here

	CRect rectTree;
	m_wndPropTree.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_wndPropTree.SetFocus();
	CPoint pt = point;
	m_wndPropTree.ScreenToClient( &pt );
//	UINT iRet;
//	m_hRClickItem = m_treeProp.HitTest( pt, &iRet );
//	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
//	{
//		m_hRClickItem = NULL;
//	}
//
//	if (m_hRClickItem==NULL)
//		return;

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;
	pMenu = menuPopup.GetSubMenu( 5 );

//	if( m_treeProp.GetParentItem( m_hRClickItem ) != NULL )
//	{
//		m_treeProp.SelectItem( NULL );
//		return;
//	}

	pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS,MF_BYCOMMAND );
//	if( m_hRClickItem != m_hQueue || nProcType == LineProc || nProcType == BaggageProc )
//		pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDQUEUEFIXED, MF_GRAYED );
//	m_treeProp.SelectItem( m_hRClickItem );
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}

void CDlgStructureProp::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgStructureProp::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

int CDlgStructureProp::DoFakeModal()
{
	if( CDialog::Create(CDlgStructureProp::IDD, m_pParentWnd) ) 
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}


void CDlgStructureProp::OnProcpropPickfrommap() 
{

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}

}

void CDlgStructureProp::OnNMRclickTreeProperties(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_wndPropTree.GetSelectedItem();
	if (m_wndPropTree.GetParentItem(hItem) == NULL 
		&& m_wndPropTree.GetPrevSiblingItem(hItem) == NULL)
	{
		
	}

	*pResult = 0;
}


LRESULT CDlgStructureProp::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	
	m_vServiceLocation = *pData;			
    //pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);

	LoadTree();

	return TRUE;
}


CFloor2* CDlgStructureProp::GetPointFloor(int nLevel)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;

	for (iter = vFloors.begin(); iter != vFloors.end(); iter++)
	{
		if ((*iter)->Level() == nLevel)
			return (*iter);
	}

	return NULL;
}

void CDlgStructureProp::LoadTree()
{
//	m_wndPropTree.DeleteAllItems();
//	m_hServiceLocation = m_wndPropTree.InsertItem(_T("Service Location(m)"));
	if(m_wndPropTree.ItemHasChildren(m_hServiceLocation))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_wndPropTree.GetChildItem(m_hServiceLocation);

		while (hChildItem != NULL)
		{
			hNextItem = m_wndPropTree.GetNextItem(hChildItem, TVGN_NEXT);
			m_wndPropTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}


	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
//	CString csLabel = CString("Service Location (") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
//	m_hServiceLocation = m_wndPropTree.InsertItem( csLabel );
	if( m_vServiceLocation.size() > 0 )
	{
		for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
		{
			ARCVector3 v3D = m_vServiceLocation[i];
			CString csPoint;

			csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );

			CFloor2* pFloor = NULL;
			pFloor = GetPointFloor( pDoc->m_nActiveFloor );
			if (pFloor)
				csPoint += _T("; Floor:") + pFloor->FloorName();

			m_wndPropTree.InsertItem( csPoint, m_hServiceLocation  );
		}
		m_wndPropTree.Expand( m_hServiceLocation, TVE_EXPAND );
	}
}

void CDlgStructureProp::OnBnClickedOk()
{
	OnOK();
}
bool CDlgStructureProp::IsNameValid(CString& strName)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc->GetCurStructurelist().IsNameValid(strName);

}

void CDlgStructureProp::OnOK() 
{
	UpdateData();
	
	if(static_cast<int>(m_vServiceLocation.size()) <3)
	{
		AfxMessageBox("The point number of the server location must larger than 2 !");
		return;	
	}

	PreProcessName();
//	if( !CheckProcessorName() )
//		return;

	CString csName = m_csLevel1;
	if( !m_csLevel2.IsEmpty() )
		csName += "-" + m_csLevel2;
	if( !m_csLevel3.IsEmpty() )
		csName += "-" + m_csLevel3;
	if( !m_csLevel4.IsEmpty() )
		csName += "-" + m_csLevel4;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CStructureID tempID;
	tempID.SetStrDict(pDoc->GetTerminal().inStrDict);
	tempID.setID(csName);
	if (tempID.isBlank())
	{
		AfxMessageBox("Please input the name  !");
		return;	
	}
	if (tempID.compare(m_pStructure->getID() ) != 0)
	{
		if(IsNameValid(csName) == false)
		{
			AfxMessageBox("The name inputted is not valid !");
			return;
		}
		m_pStructure->setID(csName);
	}

	POSITION pos = m_wndTextureList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndTextureList.GetNextSelectedItem(pos);
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		C3DView* p3DView = pDoc->Get3DView();
		if (p3DView == NULL)
		{
			pDoc->GetMainFrame()->CreateOrActive3DView();
			p3DView = pDoc->Get3DView();
		}
		if (!p3DView)
		{
			ASSERT(FALSE);
			return;
		}
		m_strTexture = m_wndTextureList.GetItemText(nItem,0);
	}
	CDialog::OnOK();
}

void CDlgStructureProp::PreProcessName()
{
	m_csLevel1.TrimLeft();
	m_csLevel1.TrimRight();
	m_csLevel1.Remove( _T(',') );
	m_csLevel1.Replace(_T(' '), _T('_'));
	m_csLevel1.MakeUpper();

	m_csLevel2.TrimLeft();
	m_csLevel2.TrimRight();
	m_csLevel2.Remove( _T(',') );
	m_csLevel2.Replace(_T(' '), _T('_'));
	m_csLevel2.MakeUpper();

	m_csLevel3.TrimLeft();
	m_csLevel3.TrimRight();
	m_csLevel3.Remove( _T(',') );
	m_csLevel3.Replace(_T(' '), _T('_'));
	m_csLevel3.MakeUpper();

	m_csLevel4.TrimLeft();
	m_csLevel4.TrimRight();
	m_csLevel4.Remove( _T(',') );
	m_csLevel4.Replace(_T(' '), _T('_'));
	m_csLevel4.MakeUpper();
}
void CDlgStructureProp::setServiceLocation(std::vector<ARCVector3> & AreaPath)
{
	m_vServiceLocation=AreaPath;
}

void CDlgStructureProp::OnSize(UINT nType , int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(!GetDlgItem(IDC_STATIC2)->GetSafeHwnd())
		return;
	CRect rectSTATIC2;
	GetDlgItem(IDC_STATIC2)->GetWindowRect(&rectSTATIC2);
	ScreenToClient( &rectSTATIC2 );
	GetDlgItem(IDC_STATIC2)->MoveWindow(rectSTATIC2.left,rectSTATIC2.top,cx- rectSTATIC2.left*2,rectSTATIC2.Height());

	CRect rect;
	m_wndName1Combo.GetWindowRect(&rect);
	ScreenToClient( &rect );

	const int nIconWidth = 40;
	int iLevelWidth = (cx - 3*9 - nIconWidth)/4;
	m_wndName1Combo.MoveWindow(rect.left,rect.top,iLevelWidth,12);
	m_wndName2Combo.MoveWindow(rect.left+iLevelWidth+9,rect.top,iLevelWidth,12 );
	m_wndName3Combo.MoveWindow(rect.left+2*iLevelWidth+18, rect.top, iLevelWidth,12 );
	m_wndName4Combo.MoveWindow(rect.left+3*iLevelWidth+27, rect.top, iLevelWidth,12 );

	CRect rectSTATIC3;
	GetDlgItem(IDC_STATIC3)->GetWindowRect(&rectSTATIC3);
	ScreenToClient( &rectSTATIC3 );
	GetDlgItem(IDC_STATIC3)->MoveWindow(rectSTATIC3.left,rectSTATIC3.top,cx/2- 20,cy-rectSTATIC3.top - 80);
	m_wndTextureList.MoveWindow(rectSTATIC3.left+10,rectSTATIC3.top+20,cx/2-40,cy-rectSTATIC3.top - 110);

	GetDlgItem(IDC_STATIC4)->MoveWindow(cx/2+rectSTATIC3.left,rectSTATIC3.top,cx/2- 20,cy-rectSTATIC3.top - 80);
	m_wndPropTree.MoveWindow(cx/2+rectSTATIC3.left+10,rectSTATIC3.top+20,cx/2- 40,cy-rectSTATIC3.top - 110);
	
	GetDlgItem(IDC_STATIC1)->MoveWindow(11,cy-65,cx-20,2);

	GetDlgItem(IDOK)->GetWindowRect(rect);
	ScreenToClient(&rect);
	GetDlgItem(IDOK)->MoveWindow(cx-rect.Width()*2-30,cy-15-rect.Height(),rect.Width(),rect.Height());
	GetDlgItem(IDCANCEL)->MoveWindow(cx-rect.Width()-15,cy-15-rect.Height(),rect.Width(),rect.Height());
	ReloadTextureList();
}

void CDlgStructureProp::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	lpMMI->ptMinTrackSize.x = 518;
	lpMMI->ptMinTrackSize.y = 346;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgStructureProp::ReloadTextureList()
{
 	m_wndTextureList.DeleteAllItems();
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int nTxetureItemCount = pDoc->GetSurfaceMaterialLib().GetTextureItemCount();
	for (int i = 0; i < nTxetureItemCount; i++)
	{		
		CSurfaceMaterialLib::TextureItem item = pDoc->GetSurfaceMaterialLib().GetTextureItem(i);	
		m_wndTextureList.InsertItem(i,item.m_key,i);				
	}
}


