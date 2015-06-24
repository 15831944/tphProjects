// FrameInputData.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FrameProcessDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WINDOWWIDTH  780
#define WINDOWHEIGHT 530  

/////////////////////////////////////////////////////////////////////////////
// CFrameProcessDefine

IMPLEMENT_DYNCREATE(CFrameProcessDefine, CMDIChildWnd)

CFrameProcessDefine::CFrameProcessDefine()
{
}

CFrameProcessDefine::~CFrameProcessDefine()
{
}


BEGIN_MESSAGE_MAP(CFrameProcessDefine, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFrameProcessDefine)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameProcessDefine message handlers

void CFrameProcessDefine::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);
	
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!
	
	CDocument* pDocument = GetActiveDocument();
	if(bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, pDocument->GetTitle() + CString(_T("         Process Define")));

		this->SetWindowText(szText);
	}
}

BOOL CFrameProcessDefine::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	//Define the windows Rect
	cs.cx = WINDOWWIDTH ;
	cs.cy = WINDOWHEIGHT ;	
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}

