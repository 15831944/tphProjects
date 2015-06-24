// ThumbnailsList.cpp : implementation file
//

#include "stdafx.h"

#include "ThumbnailsList.h"

#include "Dib.h"
#include "INI.h"
#include "ListWndOld.h"
#include "PaxShapeDefs.h"
#include "ShapesManager.h"

#include "common\ProjectManager.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CThumbnailsList

CThumbnailsList::CThumbnailsList(BOOL bPreview)
{
	m_strImageDir=PROJMANAGER->GetAppPath()+"\\Databases\\MobImages";
	m_bPreview=bPreview;
	m_szShapeLabel = NULL;//(char**)szShapeLabel;
}

CThumbnailsList::~CThumbnailsList()
{
}


BEGIN_MESSAGE_MAP(CThumbnailsList, CListCtrl)
	//{{AFX_MSG_MAP(CThumbnailsList)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CThumbnailsList::CreateThumnailImageList()
{
	m_ImageListThumb.Create(LW_THUMBNAIL_WIDTH, LW_THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
}


/////////////////////////////////////////////////////////////////////////////
// CThumbnailsList message handlers





void CThumbnailsList::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	// TODO: Add your message handler code here and/or call default
	if(m_bPreview&&(nFlags&MK_LBUTTON))
	{
		int nSeledIndex=HitTest(point);
		if(nSeledIndex!=-1)
		{
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
			((CListWndOld*)m_pParent)->DisplayPreviewCtrl(nSeledIndex); 
		}
	}
	CListCtrl::OnMouseMove(nFlags, point);
}

void CThumbnailsList::DrawThumbnails()
{
	m_nPrevSeledIndex=-1;
	CBitmap*    pImage = NULL;	
	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;

	CFile		ImgFile;
	CDib		dib;
	CString		strPath;
	int			nWidth, nHeight;

	// no images
	if(m_VectorImageNames.empty())
		return;

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 6;

	// hold the window update to avoid flicking
	SetRedraw(FALSE);

	int i;
	// reset our image list
	for(i=0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);	

	// remove all items from list view
	if(GetItemCount() != 0)
		DeleteAllItems();

	// set the size of the image list
	m_ImageListThumb.SetImageCount(m_VectorImageNames.size());
	i = 0;

	// draw the thumbnails
	std::vector<CString>::iterator	iter;
	for(iter = m_VectorImageNames.begin(); iter != m_VectorImageNames.end(); iter++)
	{		
		// load the bitmap
		strPath = *iter;
		ImgFile.Open(strPath, CFile::modeRead);
		dib.Read(ImgFile);
		ImgFile.Close();

		// borrow our dib header to create our thumbnail bitmap
		nWidth = dib.m_pBMI->bmiHeader.biWidth;
		nHeight = dib.m_pBMI->bmiHeader.biHeight;
		dib.m_pBMI->bmiHeader.biWidth = LW_THUMBNAIL_WIDTH;
		dib.m_pBMI->bmiHeader.biHeight = LW_THUMBNAIL_HEIGHT;

		// create thumbnail bitmap section
		hBitmap = ::CreateDIBSection(NULL, 
									 dib.m_pBMI, 
								     DIB_RGB_COLORS, 
								     NULL, 
								     NULL, 
								     0); 

		// restore dib header
		dib.m_pBMI->bmiHeader.biWidth = nWidth;
		dib.m_pBMI->bmiHeader.biHeight = nHeight;

		// select thumbnail bitmap into screen dc
		hMemDC = ::CreateCompatibleDC(NULL);	
		hOldObj = ::SelectObject(hMemDC, hBitmap);
	  
		// grayscale image, need palette
		if(dib.m_pPalette != NULL)
		{
			hPal = ::SelectPalette(hMemDC, (HPALETTE)dib.m_pPalette->GetSafeHandle(), FALSE);
			::RealizePalette(hMemDC);
		}
	  
		// set stretch mode
		::SetStretchBltMode(hMemDC, COLORONCOLOR);

		// populate the thumbnail bitmap bits
	
		::StretchDIBits(hMemDC, 0, 0, 
						LW_THUMBNAIL_WIDTH, LW_THUMBNAIL_HEIGHT, 
						0, 0, 
						dib.m_pBMI->bmiHeader.biWidth, 
						dib.m_pBMI->bmiHeader.biHeight, 
						dib.m_pBits, 
						dib.m_pBMI, 
						DIB_RGB_COLORS, 
						SRCCOPY);

		// restore DC object
		::SelectObject(hMemDC, hOldObj);
	  
		// restore DC palette
		if(dib.m_pPalette != NULL)
			::SelectPalette(hMemDC, (HPALETTE)hPal, FALSE);
	  
		// clean up
		::DeleteObject(hMemDC);

		//hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), strPath, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
		
		// attach the thumbnail bitmap handle to an CBitmap object
		pImage = new CBitmap();		 
		pImage->Attach(hBitmap);

		// add bitmap to our image list
		m_ImageListThumb.Replace(i, pImage, NULL);
		//m_ImageListThumb.Add(pImage, RGB(255, 0, 255));

		// put item to display
		// set the image file name as item text
		CString strFName;
		int nData;
		if(m_bPreview)
		{
			strFName=m_VectorItemDataRight[i]; 
			nData=atoi(strFName.GetBuffer(strFName.GetLength() ) );
			
			//strFName= szShapeLabel[nData];
			strFName= m_szShapeLabel[nData];
		}
		else
			strFName=m_VectorItemDataLeft[i]; 
		InsertItem(i, strFName, i);
		if(m_bPreview)
			SetItemData(i,nData);

		i++;
			
		delete pImage;
	}

	// let's show the new thumbnails
	SetRedraw(TRUE); 
}

BOOL CThumbnailsList::GetImageFileNames(CString strSection)
{		
	CWaitCursor wc;
	m_VectorImageNames.erase(m_VectorImageNames.begin(), m_VectorImageNames.end());
	m_VectorItemDataLeft.erase (m_VectorItemDataLeft.begin(), m_VectorItemDataLeft.end());
	m_VectorItemDataRight.erase (m_VectorItemDataRight.begin(), m_VectorItemDataRight.end());
	
	CIniReader IniReader(m_strImageDir+"\\Shape.INI");
	
	CStringList* pSL = IniReader.getSectionData(strSection);

	POSITION position;
	CString strLeft,strRight,str;
	for(position = pSL->GetHeadPosition(); position != NULL; ) 
	{
		str=pSL->GetNext(position);
		strLeft=IniReader.GetDataLeft(str) ;
		strRight=IniReader.GetDataRight(str) ;
		
		if(m_bPreview)
		{
			str=strLeft;
		}
		else
		{
			str=strRight ;
		}
		m_VectorItemDataLeft.push_back(strLeft) ;
		m_VectorItemDataRight.push_back(strRight)  ;
		m_VectorImageNames.push_back(m_strImageDir+"\\"+str+".bmp");
	}
	return TRUE;

}





void CThumbnailsList::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	GetParent()->SendMessage(UM_LISTCTRL_KILLFOCUS) ;
	// TODO: Add your message handler code here
	
}

void CThumbnailsList::SetImageDir(CString str)
{
	m_strImageDir=PROJMANAGER->GetAppPath()+str;
}

void CThumbnailsList::SetLabelString(const char* szLabel[])
{
	m_szShapeLabel = (char**)szLabel; 
}




int CThumbnailsList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}


void CThumbnailsList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your control notification handler code here
	if(m_bPreview)
	{
		OnMouseMove(nFlags,point);
	}
	
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CThumbnailsList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	((CListWndOld*)GetParent())->CompleteSelect(); 
	m_nPrevSeledIndex=-1;
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

