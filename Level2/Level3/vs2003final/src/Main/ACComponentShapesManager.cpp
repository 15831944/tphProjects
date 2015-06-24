#include "StdAfx.h"
#include ".\accomponentshapesmanager.h"

#include <Inputs/IN_TERM.h>
#include <Common/AirportDatabase.h>

#include "ACComponentShapesBar.h"

#include "MainFrm.h"

CACComponentShapesManager::CACComponentShapesManager(void)
	: m_pShapesBar(NULL)
{
}

CACComponentShapesManager::~CACComponentShapesManager(void)
{
}

CACComponentShapesManager& CACComponentShapesManager::GetInstance()
{
	static CACComponentShapesManager manager;
	return manager;
}

BOOL CACComponentShapesManager::Reload(CAircraftComponentModelDatabase* pDB)
{
// 	if (pDB)
// 	{
		//
		if (m_pShapesBar)
		{
			//
			m_pShapesBar->ReloadContent(pDB);
			return TRUE;
		}
// 	}
	return FALSE;
}

void CACComponentShapesManager::ShowShapesBar( BOOL bShow ) const
{
	if (m_pShapesBar)
	{
		CWnd* pMainWnd = AfxGetMainWnd();
		ASSERT(pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));
		ASSERT(m_pShapesBar == &((CMainFrame*)pMainWnd)->m_wndACComponentShapesBar);
		((CMainFrame*)pMainWnd)->ShowControlBar(m_pShapesBar, bShow, FALSE);
	}
}

BOOL CACComponentShapesManager::IsShapesBarShow() const
{
	if (m_pShapesBar)
	{
		return m_pShapesBar->IsWindowVisible();
	}
	return FALSE;
}