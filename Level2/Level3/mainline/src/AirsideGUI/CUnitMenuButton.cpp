// CUnitMenuButton.cpp : implementation file
//

#include "stdafx.h"
#include "CUnitMenuButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCoolBtn
// ========
// 
// To Use:
// 1. Create a Bitmap resource 16x15 Pixels normal default size for a toolbar
//    bitmap.
//
// 2. Call CCoolBtns on Create function.
//
// 3. Call SetButtonImage specificing the Transparency color reference.
//    (Usally RGB(255, 0, 255) magenta)
// 
// 4. Add menu items with AddMenuItem using nMenuFlags to add disabled and
///   seperator menu items
//
// 5. Add the appropiate ON_COMMAND handlers in the parent windows message map
//
// 6. Enjoy...
//
//
// FIX LIST:
// Name					Version		Notes
// Rick H				1.1			BN_CLICK message and paint arrow fix.
// Purna				1.1			Unwanted message removed
// Ernest Laurentin		1.1			Force BS_OWNERDRAW during subclassing
// amitbamzai			1.1			Helper functions to Enable/Remove Menu items
// Julia Larson			1.1			New Add Image Function
// Stephane Rodriguez	1.1			New ChangeMenuFlags function
//							
//
////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  CCoolBtn
//
// DESCRIPTION:	
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
CUnitMenuButton::CUnitMenuButton()
{
	m_bPushed		= FALSE;
	m_bLoaded		= FALSE;
	m_bMenuPushed	= FALSE;
	m_bMenuLoaded	= FALSE;
	m_bDefaultBtn	= FALSE;
	m_nDropBtnWidth =  10;
	m_bAlwaysShowMenu = FALSE;

	ZeroMemory(&m_bm, sizeof m_bm);
	m_menu.CreatePopupMenu();
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  ~CCoolBtn
//
// DESCRIPTION:	
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
CUnitMenuButton::~CUnitMenuButton()
{
}


BEGIN_MESSAGE_MAP(CUnitMenuButton, CButton)
	//{{AFX_MSG_MAP(CCoolBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolBtn message handlers

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  Create
//
// DESCRIPTION:	Create the button
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	dwStyle |= BS_OWNERDRAW; // Enforce
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID );	;
}



////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  DrawItem
//
// DESCRIPTION:	Called in response to draw the button
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::DrawItem(DRAWITEMSTRUCT* lpDIS) 
{
	if (lpDIS->CtlType != ODT_BUTTON)
		return;

	CFont *pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));

	CDC dcMem;
	CBitmap bmp;

	CRect btnRect(lpDIS->rcItem);
	CRect trueRect(btnRect);

	CDC *pDC = CDC::FromHandle(lpDIS->hDC);

	////////////////////////////////////////
	// Button Background                  //
	////////////////////////////////////////
	pDC->FillRect(trueRect,&CBrush(GetSysColor(COLOR_BTNFACE)));
	
	BOOL bDisabled = ODS_DISABLED & lpDIS->itemState;

	if (m_bDefaultBtn)
		btnRect.DeflateRect(1,1);

	CRect rectFocus(btnRect);

	rectFocus.DeflateRect(4,4);

	if (!m_bMenuPushed)
		rectFocus.OffsetRect(m_bPushed,m_bPushed);

	rectFocus.right -= m_nDropBtnWidth;


	////////////////////////////////////////
	// Button in a normal state           //
	////////////////////////////////////////
	if (!m_bPushed || m_bMenuPushed)
		pDC->DrawFrameControl(&btnRect,DFC_BUTTON,DFCS_BUTTONPUSH);


	////////////////////////////////////////
	// Default Button State               //
	////////////////////////////////////////
	if ((m_bDefaultBtn || m_bPushed) && !bDisabled)
	{
		pDC->FrameRect(&lpDIS->rcItem,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	
		if (m_bPushed && !m_bMenuPushed)
			pDC->FrameRect(&btnRect,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH))); 
	}


	////////////////////////////////////////
	// State Focus                        //
	////////////////////////////////////////
	if (lpDIS->itemState & ODS_FOCUS || m_bPushed) 
		if (!m_bMenuPushed)
			pDC->DrawFocusRect(&rectFocus);


	////////////////////////////////////////
	// Action Focus                       //
	////////////////////////////////////////
	if ((lpDIS->itemAction & ODA_FOCUS))
		if (!m_bMenuPushed)
			pDC->DrawFocusRect(&rectFocus);


	////////////////////////////////////////
	// Draw out bitmap                    //
	////////////////////////////////////////

	// Draw out bitmap
	if (m_bLoaded)
	{
		if (!bDisabled)
		{
			m_IL.DrawIndirect(pDC,0,CPoint(6+m_bPushed,6+m_bPushed), CSize(m_bm.bmWidth, m_bm.bmHeight), CPoint(0,0),ILD_NORMAL);
		}
		else
		{
			pDC->DrawState(CPoint(6+m_bPushed,6+m_bPushed), CSize(m_bm.bmWidth, m_bm.bmHeight), m_hbmpDisabled, DST_BITMAP | DSS_DISABLED);
		}
	}


	////////////////////////////////////////
	// Draw out text                      //
	////////////////////////////////////////
	pDC->SelectObject(pFont);
	CRect rectText(rectFocus);
	rectFocus.left += m_bm.bmWidth + 2;

	CString strCaption;
	GetWindowText(strCaption);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));

	if (ODS_DISABLED & lpDIS->itemState)
	{
		rectFocus.OffsetRect(1,1);
		pDC->SetTextColor(GetSysColor(COLOR_WINDOW));
		pDC->DrawText(strCaption,rectFocus,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		rectFocus.OffsetRect(-1,-1);
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		pDC->DrawText(strCaption,rectFocus,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
	{
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->DrawText(strCaption,rectFocus,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}


	CRect rectSplit(btnRect);
	rectSplit.DeflateRect(2,2);
	rectSplit.right -= m_nDropBtnWidth;
	if ((lpDIS->itemAction & ODA_FOCUS) || lpDIS->itemState & ODS_FOCUS || m_bPushed) 
		if (!m_bMenuPushed)
		{
			rectSplit.right = rectFocus.right + 1;
		}		


	////////////////////////////////////////
	// Drop down split                    //
	////////////////////////////////////////
	CPen brFace(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
	pDC->SelectObject(&brFace);
	pDC->MoveTo(rectSplit.right, rectSplit.top);
	pDC->LineTo(rectSplit.right, rectSplit.bottom);


	CPen brLite(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
	pDC->SelectObject(&brLite);
	pDC->MoveTo(rectSplit.right+1 , rectSplit.top);
	pDC->LineTo(rectSplit.right+1, rectSplit.bottom);


	rectSplit.left = rectSplit.right;
	rectSplit.right += m_nDropBtnWidth;

	CPoint pt(rectSplit.CenterPoint());
	pt += CPoint(m_bPushed,m_bPushed);

	CPen penBlack(PS_SOLID, 1, bDisabled ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_WINDOWTEXT));
	pDC->SelectObject(&penBlack);
	DrawArrow(pDC,pt);

	////////////////////////////////////////
	// Drop down state                    //
	////////////////////////////////////////
	if (m_bMenuPushed && !bDisabled)
	{    
		rectSplit.InflateRect(1,1);
		pDC->DrawEdge(rectSplit,BDR_SUNKENOUTER, BF_RECT);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  DrawArrow
//
// DESCRIPTION:	Draws drop down arrow, we could use DrawFrameControl - a bit too 
//              messy
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::DrawArrow(CDC* pDC,CPoint ArrowTip)
{

	CPoint ptDest;

	CPen* pPen = pDC->GetCurrentPen();
	LOGPEN logPen;
	pPen->GetLogPen(&logPen);
	pDC->SetPixel(ArrowTip, logPen.lopnColor);


	ArrowTip -= CPoint(1,1);
	pDC->MoveTo(ArrowTip);

	ptDest = ArrowTip;
	ptDest += CPoint(3,0);
	pDC->LineTo(ptDest);

	ArrowTip -= CPoint(1,1);
	pDC->MoveTo(ArrowTip);

	ptDest = ArrowTip;
	ptDest += CPoint(5,0);
	pDC->LineTo(ptDest);

	ArrowTip -= CPoint(1,1);
	pDC->MoveTo(ArrowTip);

	ptDest = ArrowTip;
	ptDest += CPoint(7,0);
	pDC->LineTo(ptDest);
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnLButtonDown
//
// DESCRIPTION:	handles button pressed state, including drop down menu
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
// NT ALMOND	020703	1.1			  Fix unwanted message
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::OnLButtonDown(UINT nFlags, CPoint point) 
{

	if (m_bMenuPushed)
	{
		m_bMenuPushed = FALSE;
		Invalidate();
		return;
	}

	BOOL bHitMenuBtn = HitMenuBtn(point);
	if (bHitMenuBtn || m_bAlwaysShowMenu)
	{
		m_bMenuPushed = TRUE;
		SetFocus();
		Invalidate();

		CRect rc;
		GetWindowRect(rc);

		int x = rc.left;
		int y = rc.bottom;

		m_menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,x,y,this);

		m_bMenuPushed = FALSE;		
	}
	else
	{
		m_bPushed = TRUE;
	}

	Invalidate();

	if (m_bPushed)
		CButton::OnLButtonDown(nFlags, point);
}

void CUnitMenuButton::SetAlwaysShowMenu(BOOL bShow /*= FALSE*/)
{
	m_bAlwaysShowMenu = bShow;
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnLButtonUp
//
// DESCRIPTION:	Redraws button in normal state
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
// NT ALMOND	020703	1.1			  Generate BN_CLICK EVENT
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::OnLButtonUp(UINT nFlags, CPoint point) 
{

	if (m_bPushed)
	{
		m_bPushed = FALSE;
		ReleaseCapture();
		Invalidate();
	}

	CButton::OnLButtonUp(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnMouseMove
//
// DESCRIPTION:	Tracks mouse whilst pressed
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bPushed)
	{
		ClientToScreen(&point);
	
		if (WindowFromPoint(point) != this)
		{
			m_bPushed = FALSE;
			ReleaseCapture();
			Invalidate();
		}
	}

	CButton::OnMouseMove(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  SetButtonImage
//
// DESCRIPTION:	Sets the button image, COLORREF crMask specifics the transparency
//              color              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::SetButtonImage(UINT nResourceId, COLORREF crMask)
{
	// The ID must exist also as a bitmap resource!!!
	m_btnImage.LoadBitmap(nResourceId);  
	m_btnImage.GetObject(sizeof m_bm, &m_bm);
	m_IL.Create( nResourceId, m_bm.bmWidth, 1, crMask );
	m_bLoaded = TRUE;
	m_crMask = crMask;

	HBITMAP bmTemp;
	COLORMAP mapColor;
	mapColor.from = crMask;
	mapColor.to  = RGB(255,255,255);

	bmTemp = (HBITMAP)::CreateMappedBitmap(AfxGetApp()->m_hInstance, nResourceId, IMAGE_BITMAP, &mapColor, 1);
	m_hbmpDisabled = (HBITMAP)::CopyImage(bmTemp, IMAGE_BITMAP, m_bm.bmWidth, m_bm.bmHeight, LR_COPYDELETEORG);

	return m_bLoaded;
}

void CUnitMenuButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	Invalidate();	
}

void CUnitMenuButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);	
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnSysColorChange
//
// DESCRIPTION:	Called when system colors change, force a button redraw
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	Invalidate();	
}
 
////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  HitMenuBtn
//
// DESCRIPTION:	Helper function to test for menu button hit...
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::HitMenuBtn(CPoint point)
{
	if (!m_bMenuLoaded)
		return FALSE; // Don't allow menu button drop down if no menu items are loaded

	ClientToScreen(&point);

	CRect rect;
	GetWindowRect(rect);
	rect.left = rect.right - m_nDropBtnWidth;

	return rect.PtInRect(point);    
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  AddMenuItem
//
// DESCRIPTION:	Adds a menu item and id to our menu.
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::AddMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags)
{
	BOOL bRet = m_menu.AppendMenu(nMenuFlags | MF_STRING, nMenuId, (LPCTSTR)strMenu);

	m_bMenuLoaded |= bRet;

	return bRet;
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  PreSubclassWindow
//
// DESCRIPTION:	Force BS_OWNERDRAW during subclassing
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::PreSubclassWindow() 
{
	ModifyStyle(0, BS_OWNERDRAW); // Enforce
	CButton::PreSubclassWindow();
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  RemoveMenuItem
//
// DESCRIPTION:	Remove a menu item 
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::RemoveMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags)
{
	BOOL bRet = m_menu.RemoveMenu( nMenuId,nMenuFlags | MF_STRING);
	m_bMenuLoaded |= bRet;

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  EnableMenuItem
//
// DESCRIPTION:	Enable/Disable a menu item 
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CUnitMenuButton::EnableMenuItem(UINT nMenuId,const CString strMenu, BOOL nEnable)
{
	BOOL bRet = m_menu.EnableMenuItem(nMenuId , nEnable);
	m_bMenuLoaded |= bRet;
	return bRet;
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  AddImage
//
// DESCRIPTION:	Adds Image to menu
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::AddImage( int nIndex, CBitmap& bmpEnabled, CBitmap& bmpDisabled )
{
	m_menu.SetMenuItemBitmaps( nIndex, MF_BYPOSITION, &bmpEnabled, &bmpDisabled ) ;
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  ChangeMenuFlags
//
// DESCRIPTION:	  Allow menu flags to be changed dynamically
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::ChangeMenuFlags(UINT nMenuId, UINT nMenuFlags)
{ 
	m_menu.CheckMenuItem(nMenuId, nMenuFlags | MF_BYCOMMAND);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  SetDefaultButton
//
// DESCRIPTION:	  Call this to allow the button to a act as a default button
//
// NOTES:			
//
// MAINTENANCE:
// Name:			Date:		Version:		Notes:
// NT ALMOND		020703		1.1				Origin
//
////////////////////////////////////////////////////////////////////////////////
void CUnitMenuButton::SetDefaultButton(BOOL bDefault)
{
	m_bDefaultBtn = bDefault;
}