// 3DObjModelTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "3DobjModelEditTreeView.h"
#include <Renderer/RenderEngine/3DObjModelEditContext.h>
#include <Renderer/RenderEngine/3DObjModelEditScene.h>
#include <common/FileInDB.h>


// C3DObjModelEditTreeView

IMPLEMENT_DYNCREATE(C3DObjModelEditTreeView, CFormView)

C3DObjModelEditTreeView::C3DObjModelEditTreeView()
	: CFormView(C3DObjModelEditTreeView::IDD)
	, m_selItemType(C3DObjModelEditTree::InvalidItem)
{
}

C3DObjModelEditTreeView::~C3DObjModelEditTreeView()
{
}


BEGIN_MESSAGE_MAP(C3DObjModelEditTreeView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD, OnUpdateBtnAdd )
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateBtnDel )
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONEDIT,OnUpdateBtnEdit)
	ON_MESSAGE(C3DObjModelEditTree::UPDATE_SELECTED_ITEM_TYPE, OnUpdateToolbar)
	ON_MESSAGE(C3DObjModelEditTree::DELETE_NODE_ITEM, OnDeleteNodeItem)
	ON_MESSAGE(C3DObjModelEditTree::SELECT_NODE_ITEM, OnSelectNodeItem)
	ON_MESSAGE(C3DObjModelEditTree::QUERY_IS_LOCKED, OnQueryLocked)
	ON_COMMAND(IDC_BUTTON_SAVEAS, OnSaveAs)
	ON_COMMAND(IDC_BUTTON_SAVE, OnSave)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()


// C3DObjModelEditTreeView diagnostics

#ifdef _DEBUG
void C3DObjModelEditTreeView::AssertValid() const
{
	CFormView::AssertValid();
}

void C3DObjModelEditTreeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void C3DObjModelEditTreeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_btnSaveAs);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TREE_SECTION, m_3DObjTree);
	DDX_Control(pDX, IDC_STATIC_SECTION, m_rectFrame);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_Toolbar);
}

// C3DObjModelEditTreeView message handlers

void C3DObjModelEditTreeView::OnSize(UINT nType, int cx, int cy)
{
 	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (IsWindowVisible())
	{
		SetScrollSizes(MM_TEXT, CSize(cx, cy));

		//------------------------------------------------------------
		// Please configure following parameter to adapt better looking

		// minimum width and height of this view
		// if (cx, cy) is smaller, place controls according the parameters
		const int nViewMinWidth = 100;
		const int nViewMinHeight = 300;

		// the button width and height
		const int nBtnWidth = 60;
		const int nBtnHeight = 23;

		// horizon and vertical slot between between controls
		const int nHorInternal = 5;
		const int nVerInternal = 5;

		// distance that controls keep away from boundary
		const int nHorBoundary = 2;
		const int nVerBoundary = 2;

		// top margin the static frame keeps away from the view
		const int nFrameTopMargin = 23;
		// top margin the tool bar keeps away from the static frame top
		const int nToolbarTopMargin = 6;
		// tool bar's height
		const int nToolbarHeight = 22;
		//------------------------------------------------------------

		if (cx < nViewMinWidth)
		{
			cx = nViewMinWidth;
		}
		if (cy < nViewMinHeight)
		{
			cy = nViewMinHeight;
		}

		CRect rc;

		int nHighLimit = nHorBoundary*2 + nHorInternal*3 + nBtnWidth*4;
        if (cx > nHighLimit)
        {
			int nUseBtnWidth = nBtnWidth*cx/nHighLimit;

			rc.bottom = cy - nHorBoundary;
			rc.top = rc.bottom - nBtnHeight;

			rc.right = cx - nHorBoundary;
			rc.left = rc.right - nUseBtnWidth;
			m_btnCancel.MoveWindow(rc);

			rc.right -= (nHorInternal + nUseBtnWidth);
			rc.left -= (nHorInternal + nUseBtnWidth);
			m_btnOK.MoveWindow(rc);

			rc.right -= (nHorInternal + nUseBtnWidth);
			rc.left -= (nHorInternal + nUseBtnWidth);
			m_btnSave.MoveWindow(rc);

			rc.right -= (nHorInternal + nUseBtnWidth);
			rc.left -= (nHorInternal + nUseBtnWidth);
			m_btnSaveAs.MoveWindow(rc);
        }
		else
		{
			int nLowLimit = nHorBoundary*2 + nHorInternal + nBtnWidth*2;
			int nUseBtnWidth = nBtnWidth*cx/nLowLimit;

			rc.bottom = cy - nHorBoundary;
			rc.top = rc.bottom - nBtnHeight;

			rc.right = cx - nHorBoundary;
			rc.left = rc.right - nUseBtnWidth;
			m_btnCancel.MoveWindow(rc);

			rc.right -= (nHorInternal + nUseBtnWidth);
			rc.left -= (nHorInternal + nUseBtnWidth);
			m_btnOK.MoveWindow(rc);

			rc.bottom -= (nVerInternal + nBtnHeight);
			rc.top -= (nVerInternal + nBtnHeight);
			m_btnSaveAs.MoveWindow(rc);

			rc.left += (nHorInternal + nUseBtnWidth);
			rc.right += (nHorInternal + nUseBtnWidth);
			m_btnSave.MoveWindow(rc);
		}

		rc.bottom = rc.top - nVerInternal;
		rc.top = nFrameTopMargin;
		rc.left = 0;
		rc.right = cx;
		m_rectFrame.MoveWindow(rc);

		int nStaticFrameBottom = rc.bottom;
		rc.left += nHorBoundary;
		rc.right -= nHorBoundary;
		rc.top += nToolbarTopMargin;
		rc.bottom = rc.top + nToolbarHeight;
		m_Toolbar.MoveWindow(rc);

		rc.top = rc.bottom + nVerInternal;
		rc.bottom = nStaticFrameBottom - nVerBoundary;
		m_3DObjTree.MoveWindow(rc);
	}
}

int C3DObjModelEditTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_Toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_Toolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_treeImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2))
		return -1;

	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BITMAP_FOLDER_FILE))
		return -1;

	m_treeImageList.Add( &bmp, RGB(255,0,255));
	bmp.DeleteObject();

	return 0;
}

void C3DObjModelEditTreeView::UpdateView( C3DObjModelEditFrame::UpdateViewType updateType /*= C3DObjModelEditFrame::Type_ReloadData*/, DWORD dwData /*= 0*/ )
{
	switch (updateType)
	{
	case C3DObjModelEditFrame::Type_ReloadData:
		{
			ReloadData();
		}
		break;
	case C3DObjModelEditFrame::Type_AddNewItem:
		{
			C3DNodeObject nodeObj = (Ogre::SceneNode*)dwData;
			if (nodeObj)
			{
				m_3DObjTree.AddNewItemToRoot(nodeObj); // just add the new item to the tree root
			}
		}
		break;
	case C3DObjModelEditFrame::Type_DeleteItem:
		{
			if (!GetEditContext()->IsViewLocked())
			{
				C3DNodeObject nodeObj = (Ogre::SceneNode*)dwData;
				if (nodeObj)
				{
					m_3DObjTree.DeleteDataItem(nodeObj); // just add the new item to the tree root
				}
			}
		}
		break;
	case C3DObjModelEditFrame::Type_SelectItem:
		{
			C3DNodeObject nodeObj = (Ogre::SceneNode*)dwData;
			if (nodeObj)
			{
				m_3DObjTree.SelectNodeObj(nodeObj); // just add the new item to the tree root
			}
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}

void C3DObjModelEditTreeView::ReloadData()
{
	m_selItemType = C3DObjModelEditTree::InvalidItem;
	if (m_3DObjTree.GetSafeHwnd())
	{
		C3DNodeObject nodeObj = GetParentFrame()->GetEditModel();
		if (nodeObj)
		{
			m_3DObjTree.ReloadData(nodeObj);
			m_selItemType = C3DObjModelEditTree::RootItem;
		}
	}
}

C3DObjModelEditFrame* C3DObjModelEditTreeView::GetParentFrame() const
{
	return (C3DObjModelEditFrame*)CFormView::GetParentFrame();
}


void C3DObjModelEditTreeView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	m_3DObjTree.SetImageList(&m_treeImageList,TVSIL_NORMAL); //TVSIL_STATE

	ReloadData();

	m_3DObjTree.UpdateSelcetedItemType();

	m_Toolbar.GetToolBarCtrl().ShowWindow( SW_SHOW );

	if (GetParentFrame()->IsModelExternal())
	{
		m_3DObjTree.EnableWindow(FALSE);
		m_Toolbar.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
}


void C3DObjModelEditTreeView::OnAdd()
{
	m_3DObjTree.DoAdd();
}

void C3DObjModelEditTreeView::OnDel()
{
	m_3DObjTree.DoDel();
}

void C3DObjModelEditTreeView::OnEdit()
{
	m_3DObjTree.DoEdit();
}

void C3DObjModelEditTreeView::OnUpdateBtnAdd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetParentFrame()->IsModelExternal() &&
		(C3DObjModelEditTree::RootItem == m_selItemType || C3DObjModelEditTree::GroupItem == m_selItemType));
}

void C3DObjModelEditTreeView::OnUpdateBtnDel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetParentFrame()->IsModelExternal() &&
		(C3DObjModelEditTree::GroupItem == m_selItemType || C3DObjModelEditTree::ComponentItem == m_selItemType));
}

void C3DObjModelEditTreeView::OnUpdateBtnEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetParentFrame()->IsModelExternal() &&
		(C3DObjModelEditTree::GroupItem == m_selItemType || C3DObjModelEditTree::ComponentItem == m_selItemType));
}

LRESULT C3DObjModelEditTreeView::OnUpdateToolbar( WPARAM wParam, LPARAM )
{
	m_selItemType = (C3DObjModelEditTree::SelectedItemType)wParam;
	m_Toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, C3DObjModelEditTree::RootItem == m_selItemType || C3DObjModelEditTree::GroupItem == m_selItemType);
	m_Toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, C3DObjModelEditTree::GroupItem == m_selItemType || C3DObjModelEditTree::ComponentItem == m_selItemType);
	m_Toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, C3DObjModelEditTree::GroupItem == m_selItemType || C3DObjModelEditTree::ComponentItem == m_selItemType);

	return 1;
}

void C3DObjModelEditTreeView::OnSaveAs()
{
	CFileDialog fileDlg(FALSE, _T("aam"), _T("*.aam"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Arcport Aircraft Model (*.aam)|*.aam|All Files (*.*)|*.*||"), NULL, 0, FALSE);
	if(fileDlg.DoModal() == IDOK) 
	{
		//GetParentFrame()->GetModelEditScene()->SaveModelToFile(fileDlg.GetPathName());
	}
}

void C3DObjModelEditTreeView::OnSave()
{
	GetEditContext()->SetSelNodeObj(C3DNodeObject());
	GetParentFrame()->GetModelEditScene()->SaveModel();
	GetEditContext()->GetEditModel()->SaveData();
}

void C3DObjModelEditTreeView::OnOK()
{
	OnSave();
	GetParentFrame()->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
}

void C3DObjModelEditTreeView::OnCancel()
{
	GetParentFrame()->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
}
BOOL C3DObjModelEditTreeView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if ((WM_KEYDOWN == pMsg->message)
		&& (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		return FALSE;
	}

	return CFormView::PreTranslateMessage(pMsg);
}

LRESULT C3DObjModelEditTreeView::OnDeleteNodeItem( WPARAM wParam, LPARAM )
{
	GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_DeleteItem, wParam);
	return 1;
}

LRESULT C3DObjModelEditTreeView::OnSelectNodeItem( WPARAM wParam, LPARAM )
{
	GetParentFrame()->UpdateViews(this, C3DObjModelEditFrame::Type_SelectItem, wParam);
	return 1;
}

LRESULT C3DObjModelEditTreeView::OnQueryLocked( WPARAM wParam, LPARAM )
{
	return GetEditContext()->IsViewLocked();
}

C3DObjModelEditContext* C3DObjModelEditTreeView::GetEditContext() const
{
	C3DObjModelEditFrame* pParentFrame = GetParentFrame();
	if (pParentFrame)
	{
		return pParentFrame->GetModelEditContext();
	}
	return NULL;
}

