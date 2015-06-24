#include "StdAfx.h"
#include ".\modelessdlgmanager.h"
#include "DlgMovie.h"
#include "DlgWalkthrough.h"
#include "DlgWalkthroughRender.h"
#include "3DView.h"
#include "Render3DView.h"

template<class T>
static void DestroyDlg(T*& dlg){
	if(dlg){
		dlg->DestroyWindow();
		delete dlg;
		dlg = NULL;
	}
}

CModelessDlgManager::CModelessDlgManager( CTermPlanDoc* pDoc )
{
	m_pDoc = pDoc;

	m_pDlgMovie = NULL;
	m_pDlgWalkthrough = NULL;
	m_pDlgWalkthroughRender = NULL;
}

CModelessDlgManager::~CModelessDlgManager(void)
{
	ClearDlgs();
}

void CModelessDlgManager::ShowMovieDialog(CWnd *pParent)
{
	DestroyDlg(m_pDlgMovie);
	m_pDlgMovie = new CDlgMovie( m_pDoc, pParent );
	m_pDlgMovie->Create( CDlgMovie::IDD, pParent );
	m_pDlgMovie->ShowWindow( SW_SHOW );
}

void CModelessDlgManager::ClearDlgs()
{
	DestroyDlg(m_pDlgMovie);
}


void CModelessDlgManager::ShowDialogWalkThrough( C3DView* pView )
{
	if(!m_pDlgWalkthrough)
	{
		m_pDlgWalkthrough = new CDlgWalkthrough(m_pDoc, pView, pView);
		m_pDlgWalkthrough->Create(IDD_DIALOG_WALKTHROUGH, pView);
	}
	m_pDlgWalkthrough->ShowWindow(SW_SHOW);
}

void CModelessDlgManager::ShowDialogWalkThroughRender( CRender3DView* pView )
{
	if(!m_pDlgWalkthroughRender)
	{
		m_pDlgWalkthroughRender = new CDlgWalkthroughRender(m_pDoc,  pView);
		m_pDlgWalkthroughRender->Create(IDD_DIALOG_WALKTHROUGH, pView);
	}
	m_pDlgWalkthroughRender->ShowWindow(SW_SHOW);
}

void CModelessDlgManager::DestroyDialogWalkThrough()
{
	DestroyDlg(m_pDlgWalkthrough);
}

void CModelessDlgManager::DestroyDialogWalkThroughRender()
{
	DestroyDlg(m_pDlgWalkthroughRender);
}

void CModelessDlgManager::OnSelectPax( UINT nPaxID, const CString& sDesc )
{
	if(m_pDlgWalkthrough){
		m_pDlgWalkthrough->SelectPAX(nPaxID,sDesc);
	}
	if(m_pDlgWalkthroughRender)
	{
		m_pDlgWalkthroughRender->SelectMob(CWalkthrough::Mob_Vehicle,nPaxID,sDesc);
	}
}
