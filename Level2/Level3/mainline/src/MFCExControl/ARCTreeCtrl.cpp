// ARCTreeCtrl.cpp : implementation file
//

#include "stdafx.h" 
#include "ARCTreeCtrl.h"


// CARCTreeCtrl

IMPLEMENT_DYNAMIC(CARCTreeCtrl, CCoolTree)
CARCTreeCtrl::CARCTreeCtrl()
{
	m_nWidth = 260;
}

CARCTreeCtrl::~CARCTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CARCTreeCtrl, CCoolTree)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CARCTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();



	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	HTREEITEM hItem = GetFirstVisibleItem();
	int n = GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;

		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem)	;

		if ( !(GetItemState( hItem, selflag ) & selflag )&&pCNI)
		{

			CFont *pFontDC;
			CFont fontDC;//,fontBoldDC;
			LOGFONT logfont;

			if( pCNI->lfontItem.lfFaceName[0] != '\0' ) 
			{
				logfont = pCNI->lfontItem;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}

			fontDC.CreateFontIndirect( &logfont );
			//	logfont.lfWeight=FW_BOLD;
			//	fontBoldDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC);


			memDC.SetTextColor( pCNI->clrItem);

			CString sItem = GetItemText( hItem );

			GetItemRect( hItem, &rect, TRUE );
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
			//		if(pCNI->net==	NET_EDITSPIN_WITH_VALUE||pCNI->net==NET_EDIT_WITH_VALUE)
			//		{
			int nFlag = -1;
			int nPosFind = sItem.Find(":",0);
			if(nPosFind == -1)
			{
				nPosFind = sItem.Find("(",0);
				if(nPosFind == -1)
				{
					nPosFind = sItem.Find("[",0);
					if(nPosFind != -1)
						nFlag = 2;
				}
				else
					nFlag = 0;
			}
			else
				nFlag = 1;

			if(nPosFind != -1)
			{
				if(nFlag == 0)
				{
					CString strLeft,strRightTemp,strMid,strRight;
					strLeft=sItem.Left(nPosFind+1);
					strRightTemp=sItem.Right(sItem.GetLength()-nPosFind-1);
					int nPosFind2 = strRightTemp.Find(")",0);
					strMid = strRightTemp.Left(nPosFind2);
					strRight = strRightTemp.Right(strRightTemp.GetLength()-nPosFind2);
					memDC.TextOut(rect.left+2,rect.top+1,strLeft);
					int nWidth=memDC.GetTextExtent(strLeft).cx;
					//	memDC.SelectObject(&fontBoldDC);
					if (!pCNI->bInvalidData)
					{
						memDC.SetTextColor( RGB(0,0,255));
					}
					else
					{
						memDC.SetTextColor(RGB(210,0,0));
					}
					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strMid);

					//	memDC.SelectObject( &fontDC);
					nWidth=memDC.GetTextExtent(strLeft).cx+memDC.GetTextExtent(strMid).cx;
					memDC.SetTextColor( pCNI->clrItem);
					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strRight);
				}
				else if(nFlag == 1)
				{
					CString strLeft,strRight;
					strLeft=sItem.Left(nPosFind+1);
					strRight=sItem.Right(sItem.GetLength()-nPosFind-1);
					memDC.TextOut(rect.left+2,rect.top+1,strLeft);
					int nWidth=memDC.GetTextExtent(strLeft).cx;
					//	memDC.SelectObject(&fontBoldDC);

					if (!pCNI->bInvalidData)
					{
						memDC.SetTextColor( RGB(0,0,255));
					}
					else
					{
						memDC.SetTextColor(RGB(210,220,227));
					}

					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strRight);
				}
				else
				{
					CString strLeft,strRightTemp,strMid,strRight;
					strLeft=sItem.Left(nPosFind+1);
					strRightTemp=sItem.Right(sItem.GetLength()-nPosFind-1);
					int nPosFind2 = strRightTemp.Find("]",0);
					strMid = strRightTemp.Left(nPosFind2);
					strRight = strRightTemp.Right(strRightTemp.GetLength()-nPosFind2);
					memDC.TextOut(rect.left+2,rect.top+1,strLeft);
					int nWidth=memDC.GetTextExtent(strLeft).cx;
					//	memDC.SelectObject(&fontBoldDC);
					memDC.SetTextColor( RGB(0,0,255));
					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strMid);

					//	memDC.SelectObject( &fontDC);
					nWidth=memDC.GetTextExtent(strLeft).cx+memDC.GetTextExtent(strMid).cx;
					memDC.SetTextColor( pCNI->clrItem);
					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strRight);
				}
			}
			else
			{
				memDC.TextOut(rect.left+2,rect.top+1,sItem);
			}

			//		}
			//		else
			//			memDC.TextOut(rect.left+2,rect.top+1,sItem);




			memDC.SelectObject( pFontDC );
		}
		hItem = GetNextVisibleItem( hItem );
	}


	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

}