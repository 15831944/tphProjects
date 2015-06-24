// EconListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "EconListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEconListCtrl

CEconListCtrl::CEconListCtrl()
{
}

CEconListCtrl::~CEconListCtrl()
{
}


BEGIN_MESSAGE_MAP(CEconListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEconListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEconListCtrl message handlers

void CEconListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}


void CEconListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	//for this notification, the structure is actually a
	// NMLVCUSTOMDRAW that tells you what's going on with the custom
	// draw action. So, we'll need to cast the generic pNMHDR pointer.
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch(lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;          // ask for item notifications.
			break;

		case CDDS_ITEMPREPAINT:
			{
				*pResult = CDRF_DODEFAULT;

				int iRow = lplvcd->nmcd.dwItemSpec;
				int nData = GetItemData( iRow );
				if( nData != 0 )
				{
					if( nData == 1 )
						lplvcd->clrText = RGB( 255, 128, 64 );
					else if( nData == 2 )
						lplvcd->clrText = RGB( 0, 128, 0 );

					*pResult = CDRF_NEWFONT;
				}
			}
			break;

		default:
			*pResult = CDRF_DODEFAULT;
	}
}
