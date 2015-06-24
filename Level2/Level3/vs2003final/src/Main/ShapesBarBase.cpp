// ShapesBar.cpp: implementation of the CShapesBarBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TermPlan.h"
#include "ShapesBarBase.h"
#include "common\WinMsg.h"
#include <Common\BitmapSaver.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define LARGE_IMAGE_LIST_SIZE         32
#define SMALL_IMAGE_LIST_SIZE         16


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShapesBarBase::CShapesBarBase()
{
}
CShapesBarBase::~CShapesBarBase()
{
}
BEGIN_MESSAGE_MAP(CShapesBarBase, CSizingControlBarG)
	//{{AFX_MSG_MAP(CShapesBarBase)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapesBarBase message handlers

int CShapesBarBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	CreateOutlookBar();
	
	

	m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

    // Create an 8-point MS Sans Serif font for the list box.
    m_font.CreatePointFont (80, _T ("MS Sans Serif"));

	return 0;
}

HBRUSH CShapesBarBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSizingControlBarG::OnCtlColor(pDC, pWnd, nCtlColor);
	return m_brush;
}

void CShapesBarBase::CreateOutlookBar()
{
	DWORD dwf =CGfxOutBarCtrl::fAnimation;

	m_wndOutBarCtrl.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,IDC_LIST_SHAPES, dwf);
	
	m_largeIL.Create(LARGE_IMAGE_LIST_SIZE, LARGE_IMAGE_LIST_SIZE, ILC_COLOR24|ILC_MASK, 0, 4);
	m_smallIL.Create(SMALL_IMAGE_LIST_SIZE, SMALL_IMAGE_LIST_SIZE, ILC_COLOR24|ILC_MASK,0,4);

	m_wndOutBarCtrl.SetImageList(&m_largeIL, CGfxOutBarCtrl::fLargeIcon);
	m_wndOutBarCtrl.SetImageList(&m_smallIL, CGfxOutBarCtrl::fSmallIcon);
	
	m_wndOutBarCtrl.SetAnimationTickCount(6);
}

void CShapesBarBase::ClearAllContent()
{
	// remove all items
	while (m_wndOutBarCtrl.GetFolderCount())
	{
		m_wndOutBarCtrl.RemoveFolder(0);
	}
	while (m_largeIL.GetImageCount())
	{
		m_largeIL.Remove(0);
	}
	while (m_smallIL.GetImageCount())
	{
		m_smallIL.Remove(0);
	}
}

void CShapesBarBase::AddLargeImage( HBITMAP hBitmap, COLORREF crMask )
{
	CBitmap bmp;
	bmp.Attach(BitmapSaver::ScaleBitmap(hBitmap, LARGE_IMAGE_LIST_SIZE, LARGE_IMAGE_LIST_SIZE));
	m_largeIL.Add(&bmp, crMask);
}

void CShapesBarBase::AddSmallImage( HBITMAP hBitmap, COLORREF crMask )
{
	CBitmap bmp;
	bmp.Attach(BitmapSaver::ScaleBitmap(hBitmap, SMALL_IMAGE_LIST_SIZE, SMALL_IMAGE_LIST_SIZE));
	m_smallIL.Add(&bmp, crMask);
}