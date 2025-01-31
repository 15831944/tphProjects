// CoolBtn.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessDefineDlg.h"
#include "CoolBtn.h"
#include "Resource.h"
#include "RepControlView.h"
#include "ImportFlightFromFileDlg.h"
#include "AirsideRepControlView.h"
#include "DlgDBAircraftTypes.h"
#include "DlgDBAirline.h"
#include "DlgDBAirports.h"
#include "DlgProbDist.h"
#include "DlgFlightDB.h"
#include "DlgAircraftAlias.h"
#include "CmpReportTreeView.h"
#include "FltPaxDataDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nDropBtnWidth =  16;

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
CCoolBtn::CCoolBtn()
{
  m_bPushed = FALSE;
  m_bLoaded = FALSE;
  m_bMenuPushed = FALSE;
  m_bMenuLoaded = TRUE;
  m_iOperation = -1;
m_Type = TY_TERMINAL ;
  m_bIsOpenButton = FALSE;

  ZeroMemory(&m_bm, sizeof m_bm);
  m_menu.CreatePopupMenu();
  SetButtonImage(IDB_OPERATE, RGB(255,0,255));
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
CCoolBtn::~CCoolBtn()
{
}


BEGIN_MESSAGE_MAP(CCoolBtn, CButton)
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
BOOL CCoolBtn::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
  dwStyle |= BS_OWNERDRAW; // Enforce
  m_pParentWnd = pParentWnd;
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
void CCoolBtn::DrawItem(DRAWITEMSTRUCT* lpDIS) 
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

  BOOL bDefaultBtn = GetWindowLong(m_hWnd,GWL_STYLE) & BS_DEFPUSHBUTTON;
  BOOL bDisabled = ODS_DISABLED & lpDIS->itemState;
 
  if (bDefaultBtn)
    btnRect.DeflateRect(1,1);

  CRect rectFocus(btnRect);

  rectFocus.DeflateRect(4,4);

  if (!m_bMenuPushed)
    rectFocus.OffsetRect(m_bPushed,m_bPushed);

  rectFocus.right -= nDropBtnWidth;


  ////////////////////////////////////////
  // Button in a normal state           //
  ////////////////////////////////////////
  if (!m_bPushed || m_bMenuPushed)
    pDC->DrawFrameControl(&btnRect,DFC_BUTTON,DFCS_BUTTONPUSH);
  

  ////////////////////////////////////////
  // Default Button State               //
  ////////////////////////////////////////
  if ((bDefaultBtn || m_bPushed) && !bDisabled)
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
  rectSplit.right -= nDropBtnWidth;


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
  rectSplit.right += nDropBtnWidth;

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
void CCoolBtn::DrawArrow(CDC* pDC,CPoint ArrowTip)
{

  CPoint ptDest;

  pDC->SetPixel(ArrowTip,RGB(0,0,0));

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
//
////////////////////////////////////////////////////////////////////////////////
void CCoolBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CButton::OnLButtonDown(nFlags, point);

  if (m_bMenuPushed)
  {
    m_bMenuPushed = FALSE;
    Invalidate();
    return;

  }

  if (HitMenuBtn(point))
  {
    m_bMenuPushed = TRUE;
    Invalidate();

    CRect rc;
    GetWindowRect(rc);
    
    int x = rc.left;
    int y = rc.bottom;
   
	CMenu menu,*pMenu;
	menu.LoadMenu(IDR_MENU_POPUP);
	if (!m_bIsOpenButton)
	{
		switch(m_Type)
		{
		case TY_TERMINAL:
		case TY_CMPREPORTTREEVIEW:
			pMenu=menu.GetSubMenu(33);
			break ;
		case TY_AIRSIDE:
			pMenu=menu.GetSubMenu(95);
			break ;
		case TY_AIRLINE:
		case TY_AIRPORT:
		case TY_ACTYPE:
		case TY_DISTRIBUTION:
		case TY_FLIGHTGROUP:
		case TY_AIRCRAFTALIAS:
			pMenu = menu.GetSubMenu(99);
			break;
		case TY_FLTPAX:
			pMenu = menu.GetSubMenu(105);
			break;
		}
		
	}
	else
	{
		pMenu = menu.GetSubMenu(46);
		CImportFlightFromFileDlg* pParent = (CImportFlightFromFileDlg*)GetParent();
		if (pParent->IsFlightScheduleDlg())
			pMenu->RemoveMenu(ID_OPEN_FROM_ARCCAPTURE, MF_BYCOMMAND);
	}
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,x,y,GetParent());
	GetParent()->SendMessage(WM_CLICKDROPDOWN,0,0 );

    m_bMenuPushed = FALSE;
  }
  else
  {
	  m_bPushed = TRUE;
	  //GetParent()->SendMessage(WM_CLICKBUTTON,0,0);
	  if (!m_bIsOpenButton)
	  {
		  switch(m_Type)
		  {
		  case TY_TERMINAL:
			  {
				  CRepControlView* pRCV=(CRepControlView*)GetParent();
				  pRCV->OnClickMultiBtn();
			  }
			  break ;
		  case TY_AIRSIDE:
			  {
				  CAirsideRepControlView* pRCV=(CAirsideRepControlView*)GetParent();
				  pRCV->ClickMuiButton();
			  }
			  break ;
		  case TY_AIRLINE:
			  {
				  CDlgDBAirline* pParent = (CDlgDBAirline*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_AIRPORT:
			  {
				  CDlgDBAirports* pParent = (CDlgDBAirports*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_ACTYPE:
			  {
				  CDlgDBAircraftTypes* pParent = (CDlgDBAircraftTypes*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_DISTRIBUTION:
			  {
				 CDlgProbDist* pParent = (CDlgProbDist*)GetParent();
				 pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_FLIGHTGROUP:
			  {
				  CDlgFlightDB* pParent = (CDlgFlightDB*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_AIRCRAFTALIAS:
			  {
				  CDlgAircraftAlias *pParent=(CDlgAircraftAlias*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_CMPREPORTTREEVIEW:
			  {
				  CCmpReportTreeView* pParent = (CCmpReportTreeView*)GetParent();
				  pParent->OnClickMultiBtn();
			  }
			  break;
		  case TY_FLTPAX:
			  {
					CFltPaxDataDlg* pParent = (CFltPaxDataDlg*)GetParent();
					pParent->OnClickMultiBtn();
			  }
			  break;
		  }
		
	  }
	  else
	  {
		  CImportFlightFromFileDlg* pParent = (CImportFlightFromFileDlg*)GetParent();
		  pParent->Openfile();
	  }
  }

  Invalidate();

	
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
//
////////////////////////////////////////////////////////////////////////////////
void CCoolBtn::OnLButtonUp(UINT nFlags, CPoint point) 
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
void CCoolBtn::OnMouseMove(UINT nFlags, CPoint point) 
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
BOOL CCoolBtn::SetButtonImage(UINT nResourceId, COLORREF crMask)
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

void CCoolBtn::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
  Invalidate();
	
}

void CCoolBtn::OnKillFocus(CWnd* pNewWnd) 
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
void CCoolBtn::OnSysColorChange() 
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
BOOL CCoolBtn::HitMenuBtn(CPoint point)
{
  if (!m_bMenuLoaded)
    return FALSE; // Don't allow menu button drop down if no menu items are loaded
  
  ClientToScreen(&point);

  CRect rect;
  GetWindowRect(rect);
  rect.left = rect.right - nDropBtnWidth;

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
BOOL CCoolBtn::AddMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags)
{
  BOOL bRet = m_menu.AppendMenu(nMenuFlags | MF_STRING, nMenuId, (LPCTSTR)strMenu);
  
  m_bMenuLoaded |= bRet;
  
  return bRet;
}



void CCoolBtn::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	UINT unStyle=GetButtonStyle();
	SetButtonStyle(unStyle|BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}
