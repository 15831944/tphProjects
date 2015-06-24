// MeshEditTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "MeshEditTreeView.h"
#include "../dlgaircraftmodelitemdefine.h"
#include "inputonboard/CInputOnboard.h"
#include "inputonboard/ComponentEditContext.h"
#include "../TVNode.h"
#include "InputOnboard/ComponentModel.h"
#include "OnboardViewMsg.h"
#include ".\meshedittreeview.h"
#include ".\MeshEditFuncs.h"
#include "common/UnitsManager.h"
#include "Common/ARCUnit.h"
#include "../ACComponentShapesManager.h"
//#include "InputAirside/ARCUnitManager.h"

#define DEFAULT_DATABASE_LENGTH_UNIT  UM_LEN_CENTIMETERS

#ifdef TEST_CODE
#include "../../Renderer/Test/mainDoc.h"
CComponentEditContext* CMeshEditTreeView::GetEditContext()
{
	return &((CmainDoc*)GetDocument())->mEditMeshContext;
}
#else
#include "..\TermPlanDoc.h"
#include "..\..\InputOnBoard\CInputOnboard.h"
CComponentEditContext* CMeshEditTreeView::GetEditContext()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	return pDoc->GetInputOnboard()->GetComponentEditContext();
}
#endif


//#define IDC_SECTION_TREE 11
// CMeshEditTreeView
#define MINLOD 3
#define MAXLOD 100

IMPLEMENT_DYNCREATE(CMeshEditTreeView, CFormView)

CMeshEditTreeView::CMeshEditTreeView()
	: CFormView(CMeshEditTreeView::IDD)
	, m_nLOD(0)
	, m_dHeight(0)	
{
}

CMeshEditTreeView::~CMeshEditTreeView()
{
}

void CMeshEditTreeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_btnSaveAs);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TREE_SECTION, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_SLIDER_LOD, m_ctlLOD);
	DDX_Text(pDX, IDC_EDIT_LOD, m_nLOD);
	DDV_MinMaxInt(pDX, m_nLOD, MINLOD, MAXLOD);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
}

BEGIN_MESSAGE_MAP(CMeshEditTreeView, CFormView)
	//ON_MESSAGE(ITEMEXPANDING,OnItemExpanding)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)

	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonsave)

	ON_COMMAND(ID_ADDSECTION_NEWSECTION, OnAddsectionNewsection)
	ON_COMMAND(ID_EDITSECTION_DELETE, OnEditsectionDelete)
	ON_COMMAND(ID_EDITSECTION_PROP, OnEditsectionEdit)
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnAddsectionNewsection)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnEditsectionDelete)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD, OnUpdateAddsectionNewsection )
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateEditsectionDelete )
	ON_EN_KILLFOCUS(IDC_EDIT_LOD, OnEnKillfocusEditLod)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CMeshEditTreeView diagnostics

#ifdef _DEBUG
void CMeshEditTreeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMeshEditTreeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG



// CMeshEditTreeView message handlers
void CMeshEditTreeView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CRect rect;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
	m_wndToolbar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT,TRUE);

	if (GetEditContext()->GetCurrentComponent()->mbExternalMesh)
	{
		m_btnOK.EnableWindow(FALSE);
		m_btnSave.EnableWindow(FALSE);
		m_btnSaveAs.EnableWindow(FALSE);
		m_btnCancel.EnableWindow(FALSE);
		m_wndTreeCtrl.EnableWindow(FALSE);
		m_ctlLOD.EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_LOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(FALSE);
	} 
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);


		InitCtrls();
		InitTree();
	}
	// TODO: Add your specialized code here and/or call the base class
	//if (m_imgList.m_hImageList == NULL)
	//{
	//	m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
	//	CBitmap bmp;
	//	bmp.LoadBitmap(IDB_COOLTREE);
	//	m_imgList.Add(&bmp,RGB(255,0,255));
	//}
	//CBitmap bmL;

	//bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	//m_imgList.Add(&bmL, RGB(255,0,255));
	//bmL.DeleteObject();
	//bmL.LoadBitmap(IDB_NODESMALLIMG);
	//m_imgList.Add(&bmL, RGB(255,0,255));
	//bmL.DeleteObject();
	//bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	//m_imgList.Add(&bmL, RGB(255,0,255));
	//bmL.DeleteObject();
	//bmL.LoadBitmap(IDB_AREASSMALLIMG);
	//m_imgList.Add(&bmL, RGB(255,0,255));
	//bmL.DeleteObject();
	//bmL.LoadBitmap(IDB_PORTALSSMALLIMG);
	//m_imgList.Add(&bmL, RGB(255,0,255));

	//GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
	//GetTreeCtrl().SetImageList(&m_imgList,TVSIL_STATE);


	//SetScrollPos(SB_VERT,0);
}

int CMeshEditTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR)||
		!m_wndToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//// Create the style
	//DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |WS_CLIPCHILDREN |TVS_NOHSCROLL/*|TVS_EDITLABELS*/;

	//BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
	//	this, IDC_SECTION_TREE);

	//m_font.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));

	//m_btnSaveAs.Create(_T("Save As"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
	//	CRect(10,10,90,30), this, IDC_BUTTON_SAVEAS);
	//m_btnSaveAs.SetFont(&m_font);

	//m_btnSave.Create(_T("Save"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
	//	CRect(10,10,90,30), this, IDC_BUTTON_SAVE);
	//m_btnSave.SetFont(&m_font);

	//m_btnOK.Create(_T("OK"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
	//	CRect(10,10,90,30), this, IDOK);
	//m_btnOK.SetFont(&m_font);

	//m_btnCancel.Create(_T("Cancel"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
	//	CRect(10,10,90,30), this, IDCANCEL);
	//m_btnCancel.SetFont(&m_font);

	//return (bResult ? 0 : -1);
	return 0;

}

void CMeshEditTreeView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
	{
		CRect rectWnd, rectWnd2;

		int iWidth = cx - 18;

		GetDlgItem(IDC_STATIC_LOD)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		GetDlgItem(IDC_EDIT_LOD)->MoveWindow( rectWnd.right, rectWnd.top, iWidth/2 -rectWnd.Width(), rectWnd.Height(), TRUE);
		m_ctlLOD.MoveWindow( rectWnd.left + iWidth/2 + 3, rectWnd.top, iWidth/2 -3, rectWnd.Height(), TRUE);

		GetDlgItem(IDC_STATIC_HEIGHT)->GetWindowRect(rectWnd);
		GetDlgItem(IDC_BUTTON_APPLY)->GetWindowRect(rectWnd2);
		ScreenToClient(rectWnd);
		ScreenToClient(rectWnd2);
		GetDlgItem(IDC_EDIT_HEIGHT)->MoveWindow(rectWnd.right, rectWnd.top, iWidth/2-rectWnd.Width(), rectWnd2.Height(), TRUE);
		GetDlgItem(IDC_STATIC_UNIT)->MoveWindow(rectWnd.left + iWidth/2 + 1, rectWnd.top, 16, rectWnd2.Height(),TRUE);
		GetDlgItem(IDC_BUTTON_APPLY)->MoveWindow(rectWnd.left + iWidth - rectWnd2.Width(), rectWnd.top, rectWnd2.Width(), rectWnd2.Height(), TRUE);

		GetDlgItem(IDC_STATIC_SECTION)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		GetDlgItem(IDC_STATIC_SECTION)->MoveWindow(rectWnd.left, rectWnd.top,rectWnd.left+ iWidth, cy -rectWnd.top - 50,TRUE);
		m_wndTreeCtrl.GetWindowRect(rectWnd2);
		ScreenToClient(rectWnd2);
		m_wndTreeCtrl.MoveWindow( rectWnd2.left, rectWnd2.top,rectWnd.left+ iWidth, cy -rectWnd2.top - 50, TRUE);
		m_wndTreeCtrl.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		CRect btnrc;
		m_btnSave.GetWindowRect( btnrc );
		m_btnSaveAs.MoveWindow(rectWnd.left, rectWnd.bottom + 20, cx/3 -4, btnrc.Height(),TRUE);
		//m_btnSave.GetWindowRect( btnrc );
		//ScreenToClient(btnrc);

		int nbWidth = (iWidth - cx/3 -20)/3;
		m_btnCancel.MoveWindow(rectWnd.right - nbWidth, rectWnd.bottom + 20, nbWidth, btnrc.Height(), TRUE);
		m_btnOK.MoveWindow( rectWnd.right - 2*nbWidth - 3, rectWnd.bottom + 20, nbWidth, btnrc.Height(), TRUE);		
		m_btnSave.MoveWindow(rectWnd.right - 3*nbWidth - 6, rectWnd.bottom + 20, nbWidth, btnrc.Height(),TRUE);
	
		SetScaleToFitSize(CSize(cx,cy));
	}

	//if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		//m_wndTreeCtrl.MoveWindow(0, 0, cx, cy-75, TRUE);




	//m_btnCancel.Invalidate();
	//m_btnCancel.UpdateWindow();

	//m_btnOK.Invalidate();
	//m_btnOK.UpdateWindow();

	//m_btnSave.Invalidate();
	//m_btnSave.UpdateWindow();

	//m_btnSaveAs.Invalidate();
	//m_btnSaveAs.UpdateWindow();
}

void CMeshEditTreeView::InitTree()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	HTREEITEM hItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	HTREEITEM hRoot = m_wndTreeCtrl.InsertItem("Sections:",cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);

	CComponentMeshModel* pMeshModel = GetEditContext()->GetCurrentComponent();
	if(pMeshModel)
	{
		int nCount = pMeshModel->GetSectionCount();
		for (int i =0; i < nCount; i++)
		{
			CComponentMeshSection* pSection = pMeshModel->GetSection(i);

			CString strName = pSection->getName();
			hItem = m_wndTreeCtrl.InsertItem(strName,cni,FALSE,FALSE,hRoot,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD)pSection);

		}
	}
	m_wndTreeCtrl.Expand(hRoot,TVE_EXPAND);
}

//BOOL CMeshEditTreeView::PreCreateWindow(CREATESTRUCT& cs) 
//{
//	if(!CFormView::PreCreateWindow(cs))
//		return FALSE;
//
//	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS|WS_CLIPCHILDREN;
//
//	return TRUE;
//}

void CMeshEditTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	
	{
		if(lHint == VM_ONBOARD_COMPONENT_EDIT_OPERATION)
		{
			OnUpdateOperation((CComponentEditOperation*)pHint);
		}
		if(lHint == VM_ONBOARD_COMPONENT_NEWSTATION)
		{
			CComponentMeshSection* pSection = (CComponentMeshSection*) pHint;
			if(pSection != NULL )
			{
				HTREEITEM hRoot = m_wndTreeCtrl.GetRootItem();

				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				cni.nt = NT_NORMAL;
				cni.net = NET_NORMAL;

				CString strName = pSection->getName();
				HTREEITEM hItem = m_wndTreeCtrl.InsertItem(strName,cni,FALSE,FALSE,hRoot,TVI_LAST);
				m_wndTreeCtrl.SetItemData(hItem,(DWORD)pSection);
			}
			CFormView::OnUpdate(pSender, lHint, pHint);
		}
		else if(lHint == VM_ONBOARD_COMPONENT_DELSTATION)
		{
			CComponentMeshSection* pSection = (CComponentMeshSection*) pHint;
			if(pSection != NULL)
			{
				HTREEITEM hRoot = m_wndTreeCtrl.GetRootItem();
				if (m_wndTreeCtrl.ItemHasChildren(hRoot))
				{
					HTREEITEM hItem = m_wndTreeCtrl.GetChildItem(hRoot);

					while (hItem != NULL)
					{
						CComponentMeshSection* pData = (CComponentMeshSection*)m_wndTreeCtrl.GetItemData(hItem);
						if (pData == pSection)
						{
							m_wndTreeCtrl.DeleteItem(hItem);
							break;
						}
						hItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
					}
				}

			}
			CFormView::OnUpdate(pSender, lHint, pHint);
		}
	}

	/////////////////////////////////////////////////
	//send message to tree ctrl to make h_scroolbar scroll to end_left 	
	m_wndTreeCtrl.SendMessage( WM_HSCROLL, MAKEWPARAM(SB_LEFT,0),(LPARAM)NULL );
}

//LRESULT CMeshEditTreeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
//{
//
//	return CFormView::WindowProc(message, wParam, lParam);	
//}

void CMeshEditTreeView::PumpMessages()
{
	ASSERT(m_hWnd!=NULL);
	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
	}
}

void CMeshEditTreeView::DeleteAllChildren( HTREEITEM hItem )
{
	HTREEITEM hChild;
	if((hChild = m_wndTreeCtrl.GetChildItem(hItem)) == NULL)
		return;

#ifndef TEST_CODE
	((CTermPlanDoc*)GetDocument())->m_bCanResposeEvent = false;	
#endif

	do
	{
		HTREEITEM hNext = m_wndTreeCtrl.GetNextSiblingItem(hChild);
		m_wndTreeCtrl.DeleteItem(hChild);
		hChild = hNext;
	} while(hChild != NULL);

#ifndef TEST_CODE
	((CTermPlanDoc*)GetDocument())->m_bCanResposeEvent = true;
#endif

}


//LRESULT CMeshEditTreeView::OnItemExpanding(WPARAM wParam, LPARAM lParam) 
//{
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
//	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
//	CTVNode* pNode = (CTVNode*)GetTreeCtrl().GetItemData( hItem );
//	//	*pResult = FALSE;
//
//	if(pNMTreeView->action == TVE_EXPAND)
//	{
//		DeleteAllChildren(pNode->m_hItem);
//		// add the real children (only if they themselves have children
//		for(int i=0; i<pNode->GetChildCount(); i++)
//		{
//			CTVNode* pChild = (CTVNode*) pNode->GetChildByIdx(i);
//			if(!pChild->IsLeaf())
//				AddItem(pChild);
//		}
//		pNode->m_eState = CTVNode::expanded;
//	}
//	else // pNMTreeView->action = TVE_COLLAPSE
//	{
//		DeleteAllChildren(pNode->m_hItem);
//		// set the node's childrens' m_hItem to NULL
//		for(int i=0; i<pNode->GetChildCount(); i++)
//		{
//			((CTVNode*) pNode->GetChildByIdx(i))->m_hItem = NULL;
//		}
//		pNode->m_eState = CTVNode::collapsed;
//		// add a dummy child if the node has children
//		COOLTREE_NODE_INFO cni;
//		CARCBaseTree::InitNodeInfo(this,cni);
//		cni.nImage = 0+ID_IMAGE_COUNT;
//		cni.nImageSeled = 0+ID_IMAGE_COUNT;
//
//		if(pNode->GetChildCount() != 0)
//			GetTreeCtrl().InsertItem("error",cni,FALSE,FALSE,pNode->m_hItem,TVI_LAST);
//	}
//	return 0;
//}

void CMeshEditTreeView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	// Mark message as handled and suppress default handling
	*pResult = 1;
}

//void CMeshEditTreeView::OnPaint() 
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Create a memory DC compatible with the paint DC
//	CDC memDC;
//	memDC.CreateCompatibleDC( &dc );
//
//	CRect rcClip, rcClient;
//	dc.GetClipBox( &rcClip );
//	GetClientRect(&rcClient);
//
//	// Select a compatible bitmap into the memory DC
//	CBitmap bitmap;
//	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
//	memDC.SelectObject( &bitmap );
//
//	// Set clip region to be same as that in paint DC
//	CRgn rgn;
//	rgn.CreateRectRgnIndirect( &rcClip );
//	memDC.SelectClipRgn(&rgn);
//	rgn.DeleteObject();
//
//	// First let the control do its default drawing.
//	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );
//
//	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
//		rcClip.left, rcClip.top, SRCCOPY );
//}

void CMeshEditTreeView::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect rectTree;
	m_wndTreeCtrl.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeCtrl.SetFocus();
	CPoint pt = point;
	m_wndTreeCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hItem = NULL;
		return;
	}

	m_wndTreeCtrl.SelectItem(hItem);


	//CPoint cliPt = point;
	//ScreenToClient(&cliPt);

	//UINT uFlags;
	//HTREEITEM hItem = m_wndTreeCtrl.HitTest(cliPt, &uFlags);
	CMenu menu, *pCtxMenu = NULL;
	if(hItem != NULL)
	{	
		menu.LoadMenu(IDR_MENU_ONBOARD_SECTION);
		if (hItem == m_wndTreeCtrl.GetRootItem())
		{
			pCtxMenu = menu.GetSubMenu(0);
		}
		else
			pCtxMenu = menu.GetSubMenu(1);
	}

	if(pCtxMenu != NULL)
	{
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN , point.x, point.y, this);
	}
}

void CMeshEditTreeView::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonsave();

	GetParentFrame()->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
}

void CMeshEditTreeView::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//hide the view or close	
	GetParentFrame()->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
}

void CMeshEditTreeView::OnBnClickedButtonSaveas()
{
	// TODO: Add your control notification handler code here
	if (GetEditContext())
	{
		CComponentMeshModel* pMeshModel = GetEditContext()->GetCurrentComponent();
		if (pMeshModel)
		{
			double dHeight = UNITSMANAGER->ConvertLength(UNITSMANAGER->GetLengthUnits(),DEFAULT_DATABASE_LENGTH_UNIT,m_dHeight);

			pMeshModel->SetHeight(dHeight);
			pMeshModel->SetLOD(m_nLOD);
			//pMeshModel->EnableThickness(m_bThickness>0?true:false);
		}
	}

}

void CMeshEditTreeView::OnBnClickedButtonsave()
{
	// TODO: Add your control notification handler code here
	if( GetEditContext() )
	{		
		CComponentMeshModel* pMeshModel = GetEditContext()->GetCurrentComponent();
		if (pMeshModel)
		{
			double dHeight = UNITSMANAGER->ConvertLength(UNITSMANAGER->GetLengthUnits(),DEFAULT_DATABASE_LENGTH_UNIT,m_dHeight);

			pMeshModel->SetHeight(dHeight);
			pMeshModel->SetLOD(m_nLOD);

			GetDocument()->UpdateAllViews(this,VM_ONBOARD_COMPONENT_SAVE_MODEL);
			GetEditContext()->SaveEditModel();
			ACCOMPONENTSHAPESMANAGER.Reload(pMeshModel->m_pACCompDB);
		}
	
		if( GetEditContext()->IsEditWithOutSave() )
		{
			m_btnSave.EnableWindow(TRUE);
		}
		else
		{
			m_btnSave.EnableWindow(FALSE);
		}
	}

}

void CMeshEditTreeView::OnAddsectionNewsection()
{	
	if( AddSectionCommand*pCmd = MeshEditFuncs::OnAddSection(GetEditContext(),0) )
	{
		HTREEITEM hRoot = m_wndTreeCtrl.GetRootItem();
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;

		CString strName = pCmd->GetSection()->getName();
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(strName,cni,FALSE,FALSE,hRoot,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD)pCmd->GetSection());

		GetDocument()->UpdateAllViews(this,VM_ONBOARD_COMPONENT_EDIT_OPERATION,(CObject*)pCmd);
	}
}

void CMeshEditTreeView::OnEditsectionDelete()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (m_wndTreeCtrl.GetParentItem(hSelItem) == NULL)
		return;

	CComponentMeshSection* pDelSection = (CComponentMeshSection*)m_wndTreeCtrl.GetItemData(hSelItem);	
	if (DelSectionCommand* pCmd = MeshEditFuncs::OnDelSection(GetEditContext(),pDelSection) )
	{
		m_wndTreeCtrl.DeleteItem(hSelItem);
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONENT_EDIT_OPERATION, (CObject*)pCmd);
	}
}

void CMeshEditTreeView::OnEditsectionEdit()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	CComponentMeshSection* pEditSection = (CComponentMeshSection*)m_wndTreeCtrl.GetItemData(hSelItem);
	if(EditSectionPropCommand* pCmd = MeshEditFuncs::OnEditSection(GetEditContext(),pEditSection))
	{
		m_wndTreeCtrl.SetItemText(hSelItem,pEditSection->getName());
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONENT_EDIT_OPERATION, (CObject*)pCmd);
	}
}

void CMeshEditTreeView::OnUpdateAddsectionNewsection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetEditContext()->GetCurrentComponent()->mbExternalMesh);
}

void CMeshEditTreeView::OnUpdateEditsectionDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		!GetEditContext()->GetCurrentComponent()->mbExternalMesh
		&& m_wndTreeCtrl.GetSelectedItem()
		);
}


void CMeshEditTreeView::OnEnKillfocusEditLod()
{
	UpdateData(TRUE);
	m_ctlLOD.SetPos(m_nLOD);
}

void CMeshEditTreeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*) pScrollBar;
		nPos = pSlider->GetPos();
		switch(nSBCode)
		{
			case TB_PAGEDOWN:
			case TB_PAGEUP:
			case TB_ENDTRACK:
			case TB_THUMBPOSITION:
			case TB_THUMBTRACK:
				m_nLOD = nPos;
				UpdateData(FALSE);
				break;
		}
	}


	//CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMeshEditTreeView::OnUpdateOperation( CComponentEditOperation* pCmd )
{
	if(!pCmd)
		return;
	if(pCmd->getEditOp() == OP_addSection
		|| pCmd->getEditOp()==OP_delSection)
	{
		InitTree();
	}

	if(pCmd->getEditOp()==OP_moveModelHeight)
	{
		m_dHeight = GetEditContext()->GetCurrentComponent()->getHeight();
		UpdateUnit();
	}
}
void CMeshEditTreeView::OnBnClickedButtonApply()
{
	
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
	EditModelLODHeightCommand* pCmd = new EditModelLODHeightCommand(pModel);
	pCmd->BeginOp();
	pModel->SetLOD(m_nLOD);
	double dHeight = UNITSMANAGER->ConvertLength(UNITSMANAGER->GetLengthUnits(),DEFAULT_DATABASE_LENGTH_UNIT,m_dHeight);

	pModel->SetHeight(dHeight);
	if(GetEditContext()->AddCommand(pCmd))
	{
		GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONENT_EDIT_OPERATION, (CObject*)pCmd);
	}else{	delete pCmd; }

}

void CMeshEditTreeView::InitCtrls()
{
	if(GetEditContext()&& GetEditContext()->GetCurrentComponent())
	{
		CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
		m_nLOD = pModel->GetLOD();
		m_dHeight = pModel->getHeight();
	}

	m_ctlLOD.SetRange(MINLOD,MAXLOD,TRUE);
	m_ctlLOD.SetPos(m_nLOD);

	UpdateUnit();
	
	UpdateData(FALSE);
}

void CMeshEditTreeView::UpdateUnit()
{
	CString strUnit;
	strUnit.Format("%s", UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
	GetDlgItem(IDC_STATIC_UNIT)->SetWindowText(strUnit);
	CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
	if (pModel)
		m_dHeight = UNITSMANAGER->ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,UNITSMANAGER->GetLengthUnits(),pModel->getHeight());
	CString strHeight;
	strHeight.Format("%.2f", m_dHeight);
	GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(strHeight);
}


