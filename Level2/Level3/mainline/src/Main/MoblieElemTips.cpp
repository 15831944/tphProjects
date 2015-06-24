// MoblieElemTips.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MoblieElemTips.h"
#include "termplandoc.h"
#include "inputs\MobileElemConstraint.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoblieElemTips

CMoblieElemTips::CMoblieElemTips()
{
}

CMoblieElemTips::~CMoblieElemTips()
{
}


BEGIN_MESSAGE_MAP(CMoblieElemTips, CToolTipCtrl)
	//{{AFX_MSG_MAP(CMoblieElemTips)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoblieElemTips message handlers

void CMoblieElemTips::InitTooltips(CWnd *pWnd, int iMaxWidth)
{
	ASSERT( pWnd );
	//if( Create( pWnd->GetParent(),TTS_ALWAYSTIP) && AddTool( pWnd ) ) 
	if( Create( pWnd,TTS_ALWAYSTIP) && AddTool( pWnd ) ) 
	{
		SendMessage(TTM_SETMAXTIPWIDTH, 0,iMaxWidth);
	}
	Activate( TRUE );
	m_pWnd = pWnd;
}

void CMoblieElemTips::ShowTips( CString& strTips ) 
{
	DelTool( m_pWnd );
	AddTool( m_pWnd, strTips);	
}
