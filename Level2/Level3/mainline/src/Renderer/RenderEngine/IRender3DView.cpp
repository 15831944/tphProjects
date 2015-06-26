#include "StdAfx.h"
#include ".\irender3dview.h"
IMPLEMENT_DYNCREATE(IRender3DView, CView)

IRender3DView::IRender3DView()
{
	m_bBusy = FALSE;
}

void IRender3DView::UpdateFloorOverlay( int nLevelID, CPictureInfo& picInfo )
{

}

void IRender3DView::OnDraw( CDC* pDC )
{
	__super::OnDraw(pDC);
}

BEGIN_MESSAGE_MAP(IRender3DView, CView)	
END_MESSAGE_MAP()