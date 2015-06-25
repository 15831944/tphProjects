#include "StdAfx.h"
#include ".\thumbnailslistex.h"
#include "ThumbnailsList2.h"

#include "Dib.h"
#include "ListWndEX.h"
#include "PaxShapeDefs.h"
#include "ShapesManager.h"

#include "common\ProjectManager.h"
#include "common\WinMsg.h"
#include "../Landside/ShapeList.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	THUMBNAIL_WIDTH			32
#define	THUMBNAIL_HEIGHT		32
CThumbnailsListEx::CThumbnailsListEx(void)
{
}

CThumbnailsListEx::~CThumbnailsListEx(void)
{
}

BEGIN_MESSAGE_MAP(CThumbnailsListEx, CListCtrl)
	//{{AFX_MSG_MAP(CThumbnailsListEx)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CThumbnailsListEx::CreateThumnailImageList()
{
	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// CThumbnailsListEx message handlers





void CThumbnailsListEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	//if(m_bPreview&&(nFlags&MK_LBUTTON))
	//{
		int nSeledIndex=HitTest(point);
		if(nSeledIndex!=-1)
		{
			m_nMouseMoveItem = nSeledIndex;
			SetItemState( nSeledIndex, LVIS_SELECTED, LVIS_SELECTED );
			CRect rect(0,0,0,0);
			CClientDC dc(this);
			dc.SetROP2(R2_NOTXORPEN) ;
			dc.SelectStockObject(NULL_BRUSH) ;
			if(m_nPrevSeledIndex!=-1)
			{
				GetItemRect(m_nPrevSeledIndex,rect,LVIR_BOUNDS);
				rect.DeflateRect(-1,-1);
				dc.Rectangle(rect) 	;
			}
			GetItemRect(nSeledIndex,rect,LVIR_BOUNDS);
			rect.DeflateRect(-1,-1);
			dc.Rectangle(rect) ;
			m_nPrevSeledIndex=nSeledIndex;

			((CListWndEX*)m_pParent)->DisplayPreviewCtrl(this);
		}
	//}
	CListCtrl::OnMouseMove(nFlags, point);
}

void CThumbnailsListEx::DrawThumbnails(int _nSet,int nShapeRelationFlag /*= -1*/)
{
	/*
	CBitmap*    pImage = NULL;	
	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;

	CDib		dib;
	int			nWidth, nHeight;
	*/

	m_nPrevSeledIndex=-1;

	int nImgCount = 0;

	if(-1 == _nSet)
		nImgCount = CShapeList::getInstance()->getShapeCount();  // we want the shape set thumbnails
	else
	{
		if(nShapeRelationFlag == -1)
			nImgCount = CShapeList::getInstance()->getShapeCount(_nSet);
		else 
			nImgCount = 1;
	}


	// no images
	if(0 >= nImgCount)
		return;

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 6;

	// hold the window update to avoid flicking
	SetRedraw(FALSE);

	// reset our image list
	for(int i=0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);	

	// remove all items from list view
	if(GetItemCount() != 0)
		DeleteAllItems();

	// set the size of the image list
	m_ImageListThumb.SetImageCount(nImgCount);

	BYTE* pBuffer = NULL;

	// draw the thumbnails
	for( int _i=0; _i<nImgCount; _i++) {

		/*

		HGLOBAL handle = NULL;

		if(-1 == _nSet)
			handle = SHAPESMANAGER->GetShapeImgResource(_i, 0);
		else
			handle = SHAPESMANAGER->GetShapeImgResource(_nSet, _i);

		ASSERT(handle);

		dib.ReadFromHandle( handle );
	
		dib.ReadFromHandle(hBmp);

		nWidth = dib.m_pBMI->bmiHeader.biWidth;
		nHeight = dib.m_pBMI->bmiHeader.biHeight;
		dib.m_pBMI->bmiHeader.biWidth = THUMBNAIL_WIDTH;
		dib.m_pBMI->bmiHeader.biHeight = THUMBNAIL_HEIGHT;


		*/
		/*
		// create thumbnail bitmap section
		hBitmap = ::CreateDIBSection(
							NULL, 
							dib.m_pBMI, 
							DIB_RGB_COLORS, 
							NULL, 
							NULL, 
							0);

		// restore dib header
		dib.m_pBMI->bmiHeader.biWidth = nWidth;
		dib.m_pBMI->bmiHeader.biHeight = nHeight;
		*/

		

		HBITMAP hBitmapOrig;
		if(-1 == _nSet)
			hBitmapOrig = CShapeList::getInstance()->GetShapeImgBitmap(_i, -1);
		else
		{
			if(nShapeRelationFlag == -1)
				hBitmapOrig = CShapeList::getInstance()->GetShapeImgBitmap(_nSet, _i);
			else
				hBitmapOrig = CShapeList::getInstance()->GetShapeImgBitmap(_nSet, -1);
		}

		ASSERT(hBitmapOrig);

		BITMAP bm;
		::GetObject( hBitmapOrig, sizeof(bm), &bm );

		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       = bm.bmWidth;
		bmi.bmiHeader.biHeight      = bm.bmHeight;
		bmi.bmiHeader.biPlanes      = bm.bmPlanes;
		bmi.bmiHeader.biBitCount    = bm.bmBitsPixel;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage   = 0;

		UINT cbBufferSize = bm.bmWidthBytes*bm.bmHeight;

	//	if(!pBuffer)
			pBuffer = new BYTE[cbBufferSize];

		::GetBitmapBits( hBitmapOrig, cbBufferSize, pBuffer );

		/*
		DIBSECTION bm;
		::GetObject( hBitmapOrig, sizeof(bm), &bm );
		*/

		HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

		HDC hMemDC = CreateCompatibleDC ( hScreen );

		HBITMAP hBitmap = CreateCompatibleBitmap( hScreen, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT );
		
		HGDIOBJ hOldObj = ::SelectObject(hMemDC, hBitmap);
	  
		// set stretch mode
		::SetStretchBltMode(hMemDC, COLORONCOLOR);

		// populate the thumbnail bitmap bits	
		::StretchDIBits(hMemDC, 0, 0, 
						THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, 
						0, bm.bmHeight+1,
						bm.bmWidth,
						-bm.bmHeight,
						pBuffer,
						&bmi, 
						DIB_RGB_COLORS, 
						SRCCOPY);

		// restore DC object
		::SelectObject(hMemDC, hOldObj);
	  
		// clean up
		::DeleteDC(hMemDC);

		// attach the thumbnail bitmap handle to an CBitmap object
		CBitmap* pImage = new CBitmap();		 
		pImage->Attach(hBitmap);

		// add bitmap to our image list
		m_ImageListThumb.Replace(_i, pImage, NULL);

		// put item to display
		// set the image file name as item text
		if(-1 == _nSet)
			InsertItem(_i, CShapeList::getInstance()->GetShapeName(_i,-1), _i);
		else
		{
			if(nShapeRelationFlag == -1)
				InsertItem(_i, CShapeList::getInstance()->GetShapeName(_nSet, _i), _i);
			else
				InsertItem(_i, CShapeList::getInstance()->GetShapeName(_nSet,-1), _i);
		}

		::DeleteObject( hBitmap );

		pImage->Detach();
					
		delete pImage;
		delete [] pBuffer;
	}

//	delete [] pBuffer;

	// let's show the new thumbnails
	SetRedraw(TRUE); 
}



void CThumbnailsListEx::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	GetParent()->SendMessage(UM_LISTCTRL_KILLFOCUS) ;
}



int CThumbnailsListEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nSelectedItem = 0;
	
	return 0;
}


void CThumbnailsListEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nSelectedItem=HitTest(point);
	if(nSelectedItem!=-1)
	{
		m_nSelectedItem = nSelectedItem;
		((CListWndEX*)GetParent())->CompleteSelect(this); 
		m_nPrevSeledIndex=-1;
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

int CThumbnailsListEx::GetSelectedItem()
{
	return m_nSelectedItem;
}

int CThumbnailsListEx::SetSelectedItem(int _n)
{
	int ret = m_nSelectedItem;
	m_nSelectedItem = _n;
	return ret;
}