// ACComponentEditFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ACModelEditFrame.h"

#include "ACModelEdit3DView.h"
#include "Renderer/RenderEngine/3DObjModelEditContext.h"
#include ".\acmodeleditframe.h"
// CACModelEditFrame

IMPLEMENT_DYNCREATE(CACModelEditFrame, C3DObjModelEditFrame)

CACModelEditFrame::CACModelEditFrame()
{
}

CACModelEditFrame::~CACModelEditFrame()
{
}

BOOL CACModelEditFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return C3DObjModelEditFrame::OnCreateClient(lpcs, pContext);
}

BEGIN_MESSAGE_MAP(CACModelEditFrame, C3DObjModelEditFrame)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CACModelEditFrame message handlers


CRuntimeClass* CACModelEditFrame::Get3DViewClass() const
{
	return RUNTIME_CLASS(CACModelEdit3DView);
}

C3DObjModelEditContext* CACModelEditFrame::CreateEditContext() const
{
	return new C3DObjModelEditContext();
}

void CACModelEditFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (SC_CLOSE == nID)
	{
// 		if (m_pACFurnishingManager)
// 		{
// 			ACFURNISHINGSHAPESMANAGER.Reload(m_pACFurnishingManager);
// 		}
// 		else
// 			ASSERT(FALSE);
	}

	C3DObjModelEditFrame::OnSysCommand(nID, lParam);
}
