// AircarftModelBar.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircarftModelBar.h"
#include "..\InputOnBoard\cInputOnboard.h"
#include "..\Common\AirportDatabase.h"
#include <InputOnboard\AircraftComponentModelDatabase.h>
#include "../Common/WinMsg.h"
#include "MainFrm.h"

CAircarftModelBar::CAircarftModelBar()
{
	m_bCreated = FALSE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAircarftModelBar, CSizingControlBarG)
	//{{AFX_MSG_MAP(CShapesBar)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAircarftModelBar message handlers
int CAircarftModelBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

	// Create an 8-point MS Sans Serif font for the list box.
	m_font.CreatePointFont (80, _T ("MS Sans Serif"));

	return 0;
}

LRESULT CAircarftModelBar::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	// TRACE("CShapesBar::OnSLBSelChanged()\n");
	//ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	CWnd* pWnd = GetParent();
	while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pWnd = pWnd->GetParent();
	}
	if(pWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return 0;
	}
//	pWnd->SendMessage(WM_SLB_SELCHANGED, wParam, lParam);
	return 0;
}

HBRUSH CAircarftModelBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSizingControlBarG::OnCtlColor(pDC, pWnd, nCtlColor);
	return m_brush;
}

void CAircarftModelBar::CreateModellookBar(InputOnboard * pInputOnboard)
{
	/*if (pInputOnboard && !m_bCreated)
	{
		CAircraftComponentModelDatabase* pModelDB = pInputOnboard->GetAirportDB()->getACCompModelDB();
		DWORD dwf =CGfxOutBarCtrl::fAnimation;

		m_wndModelBarCtrl.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,IDC_LIST_SHAPES, dwf);

		m_largeIL.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);
		m_smallIL.Create(16,16, ILC_COLOR24|ILC_MASK,0,4);

		m_wndModelBarCtrl.SetImageList(&m_largeIL, CGfxOutBarCtrl::fLargeIcon);
		m_wndModelBarCtrl.SetImageList(&m_smallIL, CGfxOutBarCtrl::fSmallIcon);

		m_wndModelBarCtrl.SetAnimationTickCount(6);
		for (int i = 0; i < pModelDB->GetCatCount(); i++)
		{
			CString strData = _T("");
			strData = pModelDB->GetCategory(i)->GetName();
			m_wndModelBarCtrl.AddFolder(strData, i);
			for (int k = 0; k < pModelDB->GetCategory(i)->GetModelCount(); k++)
			{
				CComponentMeshModel* pCategory = pModelDB->GetCategory(i)->GetModel(k);
				CString strPicPath = _T("");
				strPicPath = pInputOnboard->GetAirportDB()->getFolder() + "\\" + pCategory->GetName() + _T(".bmp");
				HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,strPicPath , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				CBitmap bm;
				bm.Attach(hBitmap);
				int nR = m_largeIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				hBitmap = (HBITMAP) ::LoadImage(NULL,strPicPath , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				bm.Attach(hBitmap);
				m_smallIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				m_wndModelBarCtrl.InsertItem(i, -1,pCategory->GetName() ,nR, (DWORD)pCategory);
			}
		}
		m_wndModelBarCtrl.SetSelFolder(0);
		m_bCreated = TRUE;
	}*/
}

void CAircarftModelBar::SetInputOnboard(InputOnboard* pInputOnboard)
{
	CreateModellookBar(pInputOnboard);
}


