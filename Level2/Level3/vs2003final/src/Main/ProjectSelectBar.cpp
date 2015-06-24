// ProjectSelectBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProjectSelectBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectSelectBar
/*
CProjectSelectBar::CProjectSelectBar()
{
}

CProjectSelectBar::~CProjectSelectBar()
{
}
*/


BEGIN_MESSAGE_MAP(CProjectSelectBar, CToolBar)
	//{{AFX_MSG_MAP(CProjectSelectBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	//ON_CBN_SELENDOK (IDC_COMBO_PROJSELECT, OnSelectProject)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectSelectBar message handlers

int CProjectSelectBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!LoadToolBar(IDR_PROJSELECTBAR))
		return -1;
	
	//
    // Create an 8-point MS Sans Serif font for the combo boxes.
	//
    CClientDC dc (this);
    m_font.CreatePointFont (80, _T ("MS Sans Serif"));
    CFont* pOldFont = dc.SelectObject (&m_font);

    TEXTMETRIC tm;
    dc.GetTextMetrics (&tm);
    int cxChar = tm.tmAveCharWidth;
    int cyChar = tm.tmHeight + tm.tmExternalLeading;

    dc.SelectObject (pOldFont);


	//
    // Add the shapes list box to the toolbar.
	//
    //SetButtonInfo(0, IDC_COMBO_PROJSELECT, TBBS_SEPARATOR, cxChar * 64);

	CRect rect;
	GetItemRect(0, &rect);
	rect.top += 9;
	rect.bottom = rect.top + (cyChar * 16);

	if(!m_wndProjSelect.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, rect, this, IDC_COMBO_PROJSELECT))
		return -1;

	m_wndProjSelect.SetFont(&m_font);
	m_wndProjSelect.AddString(_T("test1"));
	m_wndProjSelect.AddString(_T("test2"));

	return 0;
}
/*
void CProjectSelectBar::OnSelectProject()
{
}
*/