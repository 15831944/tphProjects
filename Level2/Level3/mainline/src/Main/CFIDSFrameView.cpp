/*
  Name:         CFIDSFrameView.cpp (implementation)
  Copyright:    Aviation Research Corporation
  Description:  CFIDSFrameView class implementation...
*/

// Pre-procs...
#include "stdafx.h"
#include "CFIDSFrameView.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CFIDSFrameView, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CFIDSFrameView, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFIDSFrameView)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Constructor...
CFIDSFrameView::CFIDSFrameView()
{
    // Initialize variables to defaults...
    hTitleIcon = NULL;
}

// Create event...
int CFIDSFrameView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // Create window and check for error...
    if(CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    // Set title bar icon...
    
        // Load icon...
        hTitleIcon = LoadIcon(GetModuleHandle(NULL), 
                              MAKEINTRESOURCE(CFIDSFRAMEVIEW_IDI_TITLE));

            // Failed...
            if(!hTitleIcon)
                return -1;

        // Set...
        SetIcon(hTitleIcon, TRUE);

    // Done ok...
	return 0;
}

// Windows is querying maximum window dimensions...
void CFIDSFrameView::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
    /* Set minimum and maximum width...
    lpMMI->ptMaxTrackSize.x = 368 + 368 + 10;
    lpMMI->ptMinTrackSize.x = 368 + 368 + 10;*/
}

// Set caption...
void CFIDSFrameView::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    // Set...
    SetWindowText("Flight Information System");
}

// Deconstructor...
CFIDSFrameView::~CFIDSFrameView()
{
    // Free title icon...
    if(hTitleIcon)
        DeleteObject(hTitleIcon);
}


// Check object...
#ifdef _DEBUG
void CFIDSFrameView::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

// Dump object context...
void CFIDSFrameView::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif

