// ACFurnishingEditFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ACFurnishingEditFrame.h"

#include "ACFurnishingEdit3DView.h"
#include "../ACFurnishingShapesManager.h"
#include "Renderer/RenderEngine/3DObjModelEditContext.h"
#include ".\acfurnishingeditframe.h"
// CACFurnishingEditFrame

IMPLEMENT_DYNCREATE(CACFurnishingEditFrame, C3DObjModelEditFrame)

CACFurnishingEditFrame::CACFurnishingEditFrame()
	: m_pACFurnishingManager(NULL)
{
}

CACFurnishingEditFrame::~CACFurnishingEditFrame()
{
}

BOOL CACFurnishingEditFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return C3DObjModelEditFrame::OnCreateClient(lpcs, pContext);
}

BEGIN_MESSAGE_MAP(CACFurnishingEditFrame, C3DObjModelEditFrame)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CACFurnishingEditFrame message handlers


CRuntimeClass* CACFurnishingEditFrame::Get3DViewClass() const
{
	return RUNTIME_CLASS(CACFurnishingEdit3DView);
}

C3DObjModelEditContext* CACFurnishingEditFrame::CreateEditContext() const
{
	return new C3DObjModelEditContext();
}

void CACFurnishingEditFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (SC_CLOSE == nID)
	{
		if (m_pACFurnishingManager)
		{
			ACFURNISHINGSHAPESMANAGER.Reload(m_pACFurnishingManager);
		}
		else
			ASSERT(FALSE);
	}

	C3DObjModelEditFrame::OnSysCommand(nID, lParam);
}
