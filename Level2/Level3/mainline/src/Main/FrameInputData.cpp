// FrameInputData.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FrameInputData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameInputData

IMPLEMENT_DYNCREATE(CFrameInputData, CMDIChildWnd)

CFrameInputData::CFrameInputData()
{
}

CFrameInputData::~CFrameInputData()
{
}


BEGIN_MESSAGE_MAP(CFrameInputData, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFrameInputData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameInputData message handlers

void CFrameInputData::OnUpdateFrameTitle(BOOL bAddToTitle)
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
			lstrcpy(szText, pDocument->GetTitle() + CString(_T("         Input Data Report")));

		this->SetWindowText(szText);
	}
		
}
