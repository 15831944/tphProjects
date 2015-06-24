// MoveSideWalkSheet.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MoveSideWalkSheet.h"
#include "MoveSideWalkPaxDistDlg.h"
#include "MoveSideWalkPropDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkSheet

IMPLEMENT_DYNAMIC(CMoveSideWalkSheet, CPropertySheet)

CMoveSideWalkSheet::CMoveSideWalkSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMoveSideWalkSheet::CMoveSideWalkSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
		
		m_pProperPageArr[0] = new CMoveSideWalkPropDlg( pParentWnd );
		m_pProperPageArr[1] = new CMoveSideWalkPaxDistDlg(  pParentWnd );
		
		AddPage( m_pProperPageArr[0] );
		AddPage( m_pProperPageArr[1] );
}

CMoveSideWalkSheet::~CMoveSideWalkSheet()
{
}


BEGIN_MESSAGE_MAP(CMoveSideWalkSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMoveSideWalkSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkSheet message handlers
