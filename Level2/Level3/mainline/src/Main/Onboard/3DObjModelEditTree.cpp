// C3DObjModelEditTree.cpp : implementation file
//

#include "stdafx.h"
#include "3DObjModelEditTree.h"

#include ".\3dobjmodeledittree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DObjModelEditTree

C3DObjModelEditTree::C3DObjModelEditTree()
	: m_hTrackPopupMenuItem(NULL)
{
	memDC_bgColor_bitmap = RGB(255,255,255);

	m_pDragImage = NULL;
	m_bLDragging = FALSE; 

	isImageTiled = true ;

	SetDefaultCursor() ;
}

C3DObjModelEditTree::~C3DObjModelEditTree()
{
}


BEGIN_MESSAGE_MAP(C3DObjModelEditTree, CTreeCtrl)
	//{{AFX_MSG_MAP(C3DObjModelEditTree)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)

	ON_COMMAND(ID_ADD_GROUP, OnAddGroup)
	ON_COMMAND(ID_EDIT_GROUP, OnEditGroup)
	ON_COMMAND(ID_DEL_GROUP, OnDelGroup)
	ON_COMMAND(ID_EDIT_COMPONENT_NAME, OnEditComp)
	ON_COMMAND(ID_DEL_COMPONENT, OnDelComp)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DObjModelEditTree message handlers

void C3DObjModelEditTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
	
	HTREEITEM hSelItem = GetSelectedItem();
	if (NULL == hSelItem)
		return;
}

void C3DObjModelEditTree::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	
	// Item user started dragging ...
	m_hitemDrag = pNMTreeView->itemNew.hItem;

	m_pDragImage = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}

void C3DObjModelEditTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLDragging)
	{
		CPoint pointScreen = point;
		ClientToScreen( &pointScreen );
		CImageList::DragMove(pointScreen);

		UINT flags;
		HTREEITEM hDropItem = HitTest(point, &flags);
		CImageList::DragShowNolock(FALSE);

		if (IsDragAllowed(m_hitemDrag, hDropItem))
		{
			if (cursor_arr != ::GetCursor())
				::SetCursor(cursor_arr); 
			SelectDropTarget(hDropItem);
		}
		else
		{
			if (cursor_no != ::GetCursor())
			{
				::SetCursor(cursor_no);
				SelectDropTarget(NULL);
			}
		}

		CImageList::DragShowNolock(TRUE);
	}
	else 
	{
		// Set cursor to arrow if not dragged
		// Otherwise, cursor will stay hand or arrow depen. on prev setting
		::SetCursor(cursor_arr);
	}

	CTreeCtrl::OnMouseMove(nFlags, point);

}

void C3DObjModelEditTree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		if(m_pDragImage != NULL) 
		{ 
		delete m_pDragImage; 
		m_pDragImage = NULL; 
		} 

		// Remove drop target highlighting
		SelectDropTarget(NULL);


		HTREEITEM hDropItem = HitTest(point, &nFlags);
		if (IsDragAllowed(m_hitemDrag, hDropItem))
		{
			HTREEITEM htiNew = MoveChildItem( m_hitemDrag, hDropItem, TVI_LAST );
			if (htiNew)
			{
				SelectItem( htiNew );
				Expand( hDropItem, TVE_EXPAND ) ;
			}
		}
	}
}

HTREEITEM C3DObjModelEditTree::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
	ASSERT(m_nodeObj);
	ASSERT(hItem);
	HTREEITEM hItemSrc = GetParentItem(hItem);

	Ogre::SceneNode* pNodeItem = (Ogre::SceneNode*)GetItemData(hItem);
	Ogre::SceneNode* pNodeSrc = hItemSrc ? (Ogre::SceneNode*)GetItemData(hItemSrc) : m_nodeObj.GetSceneNode();
	Ogre::SceneNode* pNodeDst = htiNewParent ? (Ogre::SceneNode*)GetItemData(htiNewParent) : m_nodeObj.GetSceneNode();
	if (pNodeDst == pNodeSrc)
		return NULL;

	C3DNodeObject nodeItem(pNodeItem);
// 	C3DNodeObject nodeSrc(pNodeSrc);
	C3DNodeObject nodeDst(pNodeDst);

	nodeItem.Detach();
	DeleteItem(hItem);

	nodeItem.AttachTo(nodeDst);
	int nImageIndex = nodeItem.IsGroup() ? 0 : 1;
	HTREEITEM hNewItem = InsertItem(nodeItem.GetName(), nImageIndex, nImageIndex, htiNewParent);
	SetItemData(hNewItem, (DWORD)nodeItem.GetSceneNode());
	SetCheck(hNewItem, nodeItem.GetVisible());
	LoadData(nodeItem, hNewItem);

    return hNewItem;
}

void C3DObjModelEditTree::OnPaint() 
{

		// Remove comments from next five lines if you don't need any 
	// specialization beyond adding a background image
//	if( m_bitmap.m_hObject == NULL )
//	{
//		CTreeCtrl::OnPaint();
//		return;
//	}

	CPaintDC dc(this);

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Create a compatible memory DC 
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap, bmpImage;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// Draw bitmap in the background if one has been set
	if( m_bitmap.m_hObject != NULL )
	{
		// Now create a mask
		CDC maskDC;
		maskDC.CreateCompatibleDC(&dc);
		CBitmap maskBitmap;

		// Create monochrome bitmap for the mask
		maskBitmap.CreateBitmap( rcClient.Width(), rcClient.Height(), 
						1, 1, NULL );
		maskDC.SelectObject( &maskBitmap );
		memDC.SetBkColor(RGB(255,255,255) /*::GetSysColor( COLOR_WINDOW )*/ );

		// Create the mask from the memory DC
		maskDC.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &memDC, 
					rcClient.left, rcClient.top, SRCCOPY );

		
		CDC tempDC;
		tempDC.CreateCompatibleDC(&dc);
		tempDC.SelectObject( &m_bitmap );

		CDC imageDC;
		CBitmap bmpImage;
		imageDC.CreateCompatibleDC( &dc );
		bmpImage.CreateCompatibleBitmap( &dc, rcClient.Width(), 
						rcClient.Height() );
		imageDC.SelectObject( &bmpImage );

		if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_pal.m_hObject != NULL )
		{
			dc.SelectPalette( &m_pal, FALSE );
			dc.RealizePalette();

			imageDC.SelectPalette( &m_pal, FALSE );
		}

		// Get x and y offset
		CRect rcRoot;
		GetItemRect( GetRootItem(), rcRoot, FALSE );
		rcRoot.left = -GetScrollPos( SB_HORZ );

	
		if (isImageTiled)
		{
			// Draw bitmap in tiled manner to imageDC
			for( int i = rcRoot.left; i < rcClient.right; i += m_cxBitmap )
			for( int j = rcRoot.top; j < rcClient.bottom; j += m_cyBitmap )
				imageDC.BitBlt( i, j, m_cxBitmap, m_cyBitmap, &tempDC, 
							0, 0, SRCCOPY );
		}else
		{
			int x=0,y=0 ;
			(m_cxBitmap > rcClient.right) ? x=0:x=(rcClient.right - m_cxBitmap);
			(m_cyBitmap > rcClient.bottom)? y=0:y=(rcClient.bottom - m_cyBitmap);
			imageDC.BitBlt( x, y, m_cxBitmap, m_cyBitmap, &tempDC, 
							0, 0, SRCCOPY );
		}

		// Set the background in memDC to black. Using SRCPAINT with black and any other
		// color results in the other color, thus making black the transparent color
		memDC.SetBkColor( RGB(0,0,0)/*memDC_bgColor_bitmap*/);        
		memDC.SetTextColor(RGB(255,255,255));
		memDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC, 
				rcClip.left, rcClip.top, SRCAND);

		// Set the foreground to black. See comment above.
		imageDC.SetBkColor(RGB(255,255,255));
		imageDC.SetTextColor(RGB(0,0,0));
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC, 
				rcClip.left, rcClip.top, SRCAND);

		// Combine the foreground with the background
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), 
					&memDC, rcClip.left, rcClip.top,SRCPAINT);

		//*****************************
			/*	for( int yy = 0; yy < rcClient.Height(); yy++)
				for( int xx = 0; xx < rcClient.Width(); xx++ )
			{
				if (imageDC.GetPixel(CPoint(xx,yy)) == RGB(0,0,0))
					imageDC.SetPixel(CPoint(xx,yy),RGB(255,255,255));
			}
			 Create a compatible memory DC 48068
			CDC whiteDC;
			whiteDC.CreateCompatibleDC( &dc );
	
			// Select a compatible bitmap into the memory DC
				CBitmap cBmp;
			blankBmp.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
			whiteDC.SelectObject( &blankBmp );*/
		//*****************************

		// Draw the final image to the screen		
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), 
					&imageDC, rcClip.left, rcClip.top, SRCCOPY );
	}
	else
	{
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), 
				rcClip.Height(), &memDC, 
				rcClip.left, rcClip.top, SRCCOPY );
	}

}

void C3DObjModelEditTree::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_bitmap.m_hObject != NULL ) InvalidateRect(NULL);

	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

}

void C3DObjModelEditTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_bitmap.m_hObject != NULL )	InvalidateRect(NULL);

	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

}

void C3DObjModelEditTree::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if( m_bitmap.m_hObject != NULL ) InvalidateRect(NULL);
	
	*pResult = 0;

}

BOOL C3DObjModelEditTree::OnEraseBkgnd(CDC* pDC) 
{
	if( m_bitmap.m_hObject != NULL )	return TRUE;

	return CTreeCtrl::OnEraseBkgnd(pDC);

}

BOOL C3DObjModelEditTree::OnQueryNewPalette() 
{

	CClientDC dc(this);
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_pal.m_hObject != NULL )
	{
		dc.SelectPalette( &m_pal, FALSE );
		BOOL result = dc.RealizePalette();
		if( result )
			Invalidate();
		return result;
	}
	
	return CTreeCtrl::OnQueryNewPalette();
}

void C3DObjModelEditTree::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CTreeCtrl::OnPaletteChanged(pFocusWnd);
	
	if( pFocusWnd == this )
		return;

	OnQueryNewPalette();

}

BOOL C3DObjModelEditTree::SetBkImage(UINT nIDResource)
{
	return SetBkImage( (LPCTSTR)nIDResource );
}

BOOL C3DObjModelEditTree::SetBkImage(LPCTSTR lpszResourceName)
{

	// If this is not the first call then Delete GDI objects
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	if( m_pal.m_hObject != NULL )
		m_pal.DeleteObject();
	
	
	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
			lpszResourceName, IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION );

	if( hBmp == NULL ) 
		return FALSE;

	m_bitmap.Attach( hBmp );
	BITMAP bm;
	m_bitmap.GetBitmap( &bm );
	m_cxBitmap = bm.bmWidth;
	m_cyBitmap = bm.bmHeight;


	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	m_bitmap.GetObject( sizeof(ds), &ds );

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << bmInfo.biBitCount;

	// Create a halftone palette if colors > 256. 
	CClientDC dc(NULL);			// Desktop DC
	if( nColors > 256 )
		m_pal.CreateHalftonePalette( &dc );
	else
	{
		// Create the palette

		RGBQUAD *pRGB = new RGBQUAD[nColors];
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		memDC.SelectObject( &m_bitmap );
		::GetDIBColorTable( memDC, 0, nColors, pRGB );

		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		m_pal.CreatePalette( pLP );

		delete[] pLP;
		delete[] pRGB;
	}
	Invalidate();

	return TRUE;
}



void C3DObjModelEditTree::SetDefaultCursor()
{
	   // Get the windows directory
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _T("\\winhlp32.exe");
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
			{
                cursor_hand = CopyCursor(hHandCursor);
			}
			      
        }
        FreeLibrary(hModule);

		cursor_arr	= ::LoadCursor(NULL, IDC_ARROW);
		cursor_no	= ::LoadCursor(NULL, IDC_NO) ;
}
BOOL C3DObjModelEditTree::IsDragAllowed(HTREEITEM hDragItem, HTREEITEM hDropItem)
{
	// the drag item can not be null
	if (NULL == hDragItem)
		return FALSE;

	// target item can not be drag item's parent
	if (GetParentItem(hDragItem) == hDropItem)
		return FALSE;

	// can not drop ancestor item
	HTREEITEM hDropItemIte = hDropItem;
	while (hDropItemIte)
	{
		if (hDropItemIte == hDragItem)
			return FALSE;

		hDropItemIte = GetParentItem(hDropItemIte);
	}

	// hDropItem must be a group
	Ogre::SceneNode* pSceneNode = hDropItem ? (Ogre::SceneNode*)GetItemData(hDropItem) : m_nodeObj.GetSceneNode();
	C3DNodeObject nodeObj(pSceneNode);
	return nodeObj.IsGroup();
}

void C3DObjModelEditTree::ReloadData( C3DNodeObject nodeObj )
{
	DeleteAllItems();

	m_nodeObj = nodeObj;
	if (m_nodeObj)
	{
		LoadData(m_nodeObj, TVI_ROOT);
	}
}

bool C3DObjModelEditTree::DeleteDataItem( C3DNodeObject nodeObj )
{
	HTREEITEM hItem = RecursiveFind(nodeObj);
	if (hItem)
	{
		DeleteItem(hItem);
		return true;
	}
	return false;
}

void C3DObjModelEditTree::LoadData( C3DNodeObject nodeObj, HTREEITEM hParent )
{
	if (NULL == hParent)
		hParent = TVI_ROOT;

	C3DNodeObject::NodeList nodeList = nodeObj.GetChild();
	C3DNodeObject::NodeList::iterator ite = nodeList.begin();
	C3DNodeObject::NodeList::iterator iteEn = nodeList.end();
	for (;ite!=iteEn;ite++)
	{
		C3DNodeObject childObj = *ite;
		int nImageIndex = childObj.IsGroup() ? 0 : 1;
		HTREEITEM hChildItem = InsertItem(childObj.GetName(), nImageIndex, nImageIndex, hParent);
		SetItemData(hChildItem, (DWORD)childObj.GetSceneNode());
		SetCheck(hChildItem, childObj.GetVisible());
		LoadData(childObj, hChildItem);
	}

	Expand(hParent, TVE_EXPAND);
}

void C3DObjModelEditTree::AddNewItemToRoot( C3DNodeObject nodeObj )
{
	if (RecursiveFind(nodeObj))
		return;
	
	int nImageIndex = nodeObj.IsGroup() ? 0 : 1;
	HTREEITEM hChildItem = InsertItem(nodeObj.GetName(), nImageIndex, nImageIndex, TVI_ROOT);
	SetItemData(hChildItem, (DWORD)nodeObj.GetSceneNode());
	SetCheck(hChildItem, nodeObj.GetVisible());
	LoadData(nodeObj, hChildItem);
}

HTREEITEM C3DObjModelEditTree::RecursiveFind(C3DNodeObject nodeObj, HTREEITEM hParent /*= NULL*/)
{
	if (ItemHasChildren(hParent))
	{
		HTREEITEM hChildItem = GetChildItem(hParent);

		while (hChildItem != NULL)
		{
			if (nodeObj.GetSceneNode() == (Ogre::SceneNode*)GetItemData(hChildItem))
				return hChildItem;

			HTREEITEM hFoundItem = RecursiveFind(nodeObj, hChildItem);
			if (hFoundItem)
				return hFoundItem;

			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return NULL;
}


void C3DObjModelEditTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	// verify that we have a mouse click in the check box area 
	DWORD pos = GetMessagePos(); 
	CPoint point(LOWORD(pos), HIWORD(pos)); 
	ScreenToClient(&point); 

	UINT flags; 
	HTREEITEM item = HitTest(point, &flags); 
	SelectItem(item);
	VERIFY(GetParent()->SendMessage(SELECT_NODE_ITEM, item ? (WPARAM)GetItemData(item) : 0));
	if(item && (flags & TVHT_ONITEMSTATEICON)) 
	{
		bool bNewState = !GetCheck(item);
		CheckChildItems(item, bNewState);
		Ogre::SceneNode* pNode = (Ogre::SceneNode*)GetItemData(item);
		ASSERT(pNode);
		C3DNodeObject nodeObj(pNode);
		nodeObj.SetVisible(bNewState);
	} 
	UpdateSelcetedItemType();
	*pResult = 0;
}

void C3DObjModelEditTree::CheckChildItems(HTREEITEM hParentItem, BOOL bVisible)
{
// 	SetCheck(hParentItem, bVisible);
	if (ItemHasChildren(hParentItem))
	{
		HTREEITEM hChildItem = GetChildItem(hParentItem);

		while (hChildItem != NULL)
		{
			SetCheck(hChildItem, bVisible);
			CheckChildItems(hChildItem, bVisible);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}

}

void C3DObjModelEditTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (pTVDispInfo)
	{
		HTREEITEM hItem = pTVDispInfo->item.hItem;
		LPCTSTR lpNewName = pTVDispInfo->item.pszText;
		if (hItem)
		{
			Ogre::SceneNode* pSceneNode = (Ogre::SceneNode*)GetItemData(hItem);
			ASSERT(pSceneNode);
			C3DNodeObject nodeObj(pSceneNode);
			if (lpNewName && '\x00' != *lpNewName && !IsLocked())
			{
				nodeObj.SetName(lpNewName);
			}
			SetItemText(hItem, nodeObj.GetName());
		}
	}
	*pResult = 0;
}

void C3DObjModelEditTree::OnAddGroup()
{
	AddToSel(m_hTrackPopupMenuItem);
}

void C3DObjModelEditTree::OnEditGroup()
{
	EditSel(m_hTrackPopupMenuItem);
}

void C3DObjModelEditTree::OnDelGroup()
{
	DelSel(m_hTrackPopupMenuItem);	
}

void C3DObjModelEditTree::OnEditComp()
{
	EditSel(m_hTrackPopupMenuItem);
}

void C3DObjModelEditTree::OnDelComp()
{
	DelSel(m_hTrackPopupMenuItem);	
}

void C3DObjModelEditTree::UpdateNewComponent(const C3DNodeObject& nodeNew)
{
	if (NULL == m_nodeObj)
		return;

	HTREEITEM hChildItem = InsertItem(nodeNew.GetName(), 1, 1);
	SetItemData(hChildItem, (DWORD)nodeNew.GetSceneNode());
	SetCheck(hChildItem, nodeNew.GetVisible());
}

void C3DObjModelEditTree::OnRButtonDown(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnRButtonDown(nFlags, point);

	// TODO: Add your message handler code here
	UINT uEnableAddGroup = MF_STRING | MF_DISABLED | MF_GRAYED;
	UINT uEnableEditGroup = MF_STRING | MF_DISABLED | MF_GRAYED;
	UINT uEnableDelGroup = MF_STRING | MF_DISABLED | MF_GRAYED;
// 	UINT uEnableAddComp = MF_STRING | MF_DISABLED | MF_GRAYED;
	UINT uEnableEditComp = MF_STRING | MF_DISABLED | MF_GRAYED;
	UINT uEnableDelComp = MF_STRING | MF_DISABLED | MF_GRAYED;

	if (!IsLocked())
	{
		UINT uFlags;
		m_hTrackPopupMenuItem = HitTest(point, &uFlags);
		if((m_hTrackPopupMenuItem != NULL) && (TVHT_ONITEM & uFlags))
		{
			SelectItem(m_hTrackPopupMenuItem);
			Ogre::SceneNode* pSceneNode = (Ogre::SceneNode*)GetItemData(m_hTrackPopupMenuItem);
			C3DNodeObject nodeObj(pSceneNode);
			if (nodeObj.IsGroup())
			{
				uEnableAddGroup = MF_STRING | MF_ENABLED;
				uEnableEditGroup = MF_STRING | MF_ENABLED;
				uEnableDelGroup = MF_STRING | MF_ENABLED;
	
				// 			uEnableAddComp = MF_STRING | MF_ENABLED;
			}
			else
			{
				uEnableEditComp = MF_STRING | MF_ENABLED;
				uEnableDelComp = MF_STRING | MF_ENABLED;
			}
		}
		else
		{
			uEnableAddGroup = MF_STRING | MF_ENABLED;
		}
	}

	CMenu popupMenu;
	popupMenu.CreatePopupMenu();
	popupMenu.AppendMenu(uEnableAddGroup, ID_ADD_GROUP, _T("Add Group"));
	popupMenu.AppendMenu(uEnableEditGroup, ID_EDIT_GROUP, _T("Edit Group Name"));
	popupMenu.AppendMenu(uEnableDelGroup, ID_DEL_GROUP, _T("Delete Group"));
	popupMenu.AppendMenu(MF_SEPARATOR, 0, _T(""));
	// 	popupMenu.AppendMenu(uEnableAddComp, ID_ADD_COMPONENT, _T("Add Component"));
	popupMenu.AppendMenu(uEnableEditComp, ID_EDIT_COMPONENT_NAME, _T("Edit Component Name"));
	popupMenu.AppendMenu(uEnableDelComp, ID_DEL_COMPONENT, _T("Delete Component"));

	CPoint ptScreen = point;
	ClientToScreen(&ptScreen);
	popupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , ptScreen.x, ptScreen.y, this);
}

void C3DObjModelEditTree::DoAdd()
{
	HTREEITEM hSelItem = GetSelectedItem();
	AddToSel(hSelItem);
}

void C3DObjModelEditTree::DoDel()
{
	HTREEITEM hSelItem = GetSelectedItem();
	DelSel(hSelItem);
}

void C3DObjModelEditTree::DoEdit()
{
	HTREEITEM hSelItem = GetSelectedItem();
	EditSel(hSelItem);
}

void C3DObjModelEditTree::AddToSel( HTREEITEM hSelItem )
{
	if (NULL == m_nodeObj || IsLocked())
		return;

	Ogre::SceneNode* pSceneNode = hSelItem ? (Ogre::SceneNode*)GetItemData(hSelItem) : m_nodeObj.GetSceneNode();
	C3DNodeObject nodeObj(pSceneNode);
	C3DNodeObject subGroup = nodeObj.CreateNewChild();
	subGroup.SetName(_T("DEFAULT"));
	HTREEITEM hChildItem = InsertItem(subGroup.GetName(), 0, 0, hSelItem ? hSelItem : TVI_ROOT);
	SetItemData(hChildItem, (DWORD)subGroup.GetSceneNode());
	SetCheck(hChildItem, subGroup.GetVisible());
	SelectItem(hChildItem);
	if (hSelItem)
	{
		Expand(hSelItem, TVE_EXPAND);
	}

	EditLabel(hChildItem);
}

void C3DObjModelEditTree::DelSel( HTREEITEM hSelItem )
{
	if (NULL == m_nodeObj || IsLocked())
		return;

	if (hSelItem)
	{
		VERIFY(GetParent()->SendMessage(DELETE_NODE_ITEM, (WPARAM)GetItemData(hSelItem)));
		DeleteItem(hSelItem);
	}
}

void C3DObjModelEditTree::EditSel( HTREEITEM hSelItem )
{
	if (NULL == m_nodeObj || IsLocked())
		return;

	if (hSelItem)
	{
		EditLabel(hSelItem);
	}
}

void C3DObjModelEditTree::UpdateSelcetedItemType()
{
	SelectedItemType itemType = InvalidItem;
	if (m_nodeObj)
	{
		itemType = RootItem;
		HTREEITEM hSelItem = GetSelectedItem();
		if (hSelItem)
		{
			Ogre::SceneNode* pNode = (Ogre::SceneNode*)GetItemData(hSelItem);
			C3DNodeObject nodeObj(pNode);
			itemType = nodeObj.IsGroup() ? GroupItem : ComponentItem;
		}
	}
	GetParent()->SendMessage(UPDATE_SELECTED_ITEM_TYPE, itemType);
}
void C3DObjModelEditTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_DELETE == nChar)
	{
		DoDel();
	}
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL C3DObjModelEditTree::IsLocked()
{
	return GetParent()->SendMessage(QUERY_IS_LOCKED);
}

void C3DObjModelEditTree::SelectNodeObj( C3DNodeObject nodeObj )
{
	HTREEITEM hItem = RecursiveFind(nodeObj);
	if (hItem)
	{
		SelectItem(hItem);
	}
}