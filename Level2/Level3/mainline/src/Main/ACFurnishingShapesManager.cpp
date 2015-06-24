#include "StdAfx.h"
#include ".\acfurnishingshapesmanager.h"

#include <Inputs/IN_TERM.h>
#include <Common/AirportDatabase.h>

#include "MainFrm.h"
#include "ACFurnishingShapesBar.h"
#include "MainFrm.h"

CACFurnishingShapesManager::CACFurnishingShapesManager(void)
	: m_pShapesBar(NULL)
{
}

CACFurnishingShapesManager::~CACFurnishingShapesManager(void)
{
}

CACFurnishingShapesManager& CACFurnishingShapesManager::GetInstance()
{
	static CACFurnishingShapesManager manager;
	return manager;
}

BOOL CACFurnishingShapesManager::Reload(CAircraftFurnishingSectionManager* pManager)
{
// 	if (pDB)
// 	{
		//
		if (m_pShapesBar)
		{
			//
			m_pShapesBar->ReloadContent(pManager);
			return TRUE;
		}
// 	}
	return FALSE;
}
void CACFurnishingShapesManager::ShowShapesBar( BOOL bShow ) const
{
	if (m_pShapesBar)
	{
		CWnd* pMainWnd = AfxGetMainWnd();
		ASSERT(pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));
		ASSERT(m_pShapesBar == &((CMainFrame*)pMainWnd)->m_wndACFurnishingShapesBar);
		((CMainFrame*)pMainWnd)->ShowControlBar(m_pShapesBar, bShow, FALSE);
	}
}

BOOL CACFurnishingShapesManager::IsShapesBarShow() const
{
	if (m_pShapesBar)
	{
		return m_pShapesBar->IsWindowVisible();
	}
	return FALSE;
}
