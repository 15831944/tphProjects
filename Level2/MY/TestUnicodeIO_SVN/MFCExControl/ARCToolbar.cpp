#include "stdafx.h"
#include "ARCToolbar.h"
#include "XTPCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CARCToolbarButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CButtonST::DrawItem( lpDrawItemStruct );

}

CARCToolbar::CARCToolbar()
:m_pImageList(NULL)
,m_bBtnRightAlign(FALSE)
,m_bGradient(FALSE)
,m_strTitle(_T(""))
{
	m_pImageList = NULL;
	//m_clrBackground.SetStandardValue(RGB(212,208,200), RGB(186,179,168));
}

CARCToolbar::~CARCToolbar()
{
	while (!m_Buttons.IsEmpty())
		delete m_Buttons.RemoveHead();
}


BEGIN_MESSAGE_MAP(CARCToolbar, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CARCToolbar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CXTPClientRect rClient((CWnd*)this);
	
	//if(m_bGradient)	XTPDrawHelpers()->GradientFill((CDC*)&dc, rClient, m_clrBackground, FALSE);
	
	if(!m_bBtnRightAlign)
		return;

	HFONT hfontOld;
	CFont* pFont = GetFont();
	if(pFont)
		hfontOld = (HFONT)SelectObject(dc.m_hDC, (HFONT)pFont->m_hObject);
	::SetBkMode(dc, TRANSPARENT);
	//spacing
	rClient.left += 2;
	::DrawText(dc, m_strTitle, -1, &rClient, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	if(pFont)
		::SelectObject(dc.m_hDC, hfontOld);

}

void CARCToolbar::AddButton(int nID, int nImage, LPCTSTR pszToolTip)
{
	ASSERT(!::IsWindow(m_hWnd));

	FLATBUTTON *pBtn = new FLATBUTTON;
	pBtn->ID = nID;
	pBtn->ImageNumber = nImage;
	pBtn->Tooltip = pszToolTip;

	m_Buttons.AddTail(pBtn);
}

void CARCToolbar::SetImageList(CImageList *pImageList)
{
	ASSERT(!::IsWindow(m_hWnd));
	ASSERT(pImageList);
	m_pImageList = pImageList;
}

void CARCToolbar::PreSubclassWindow() 
{
	if(NULL == m_pImageList)
		return CStatic::PreSubclassWindow();

	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.top+=1; rc.bottom-=1;
	rc.right-=1;

	if(m_bBtnRightAlign)
	{
		rc.left = rc.right-24;
		POSITION pos = m_Buttons.GetTailPosition();
		while (pos)
		{
			FLATBUTTON *fb = m_Buttons.GetPrev(pos);
			fb->btn.Create(NULL, WS_VISIBLE|BS_ICON|BS_OWNERDRAW|BS_CENTER|BS_VCENTER, rc, this, fb->ID);
			fb->btn.SetIcon(m_pImageList->ExtractIcon(fb->ImageNumber));
			fb->btn.SetTooltipText(fb->Tooltip);

			rc.OffsetRect(-25,0);
		}
	}
	else
	{
		rc.right = 24;
		POSITION pos = m_Buttons.GetHeadPosition();
		while (pos)
		{
			FLATBUTTON *fb = m_Buttons.GetNext(pos);
			fb->btn.Create(NULL, WS_VISIBLE|BS_ICON|BS_OWNERDRAW|BS_CENTER|BS_VCENTER, rc, this, fb->ID);
			fb->btn.SetIcon(m_pImageList->ExtractIcon(fb->ImageNumber));
			fb->btn.SetTooltipText(fb->Tooltip);

			rc.OffsetRect(25,0);
		}
	}

}

BOOL CARCToolbar::DestroyWindow() 
{
	POSITION pos = m_Buttons.GetHeadPosition();
	while (pos)
		m_Buttons.GetNext(pos)->btn.DestroyWindow();
	
	return CStatic::DestroyWindow();
}

LRESULT CARCToolbar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_COMMAND)
	{
		if (HIWORD(wParam) == BN_CLICKED)
		{
			// redirect to parent window:
			GetParent()->SendMessage(message, wParam, lParam);
		}
	}
	
	return CStatic::DefWindowProc(message, wParam, lParam);
}

void CARCToolbar::EnableButton(int nID, bool bEnable)
{
	CWnd *pWnd = GetDlgItem(nID);
	if (pWnd)
		pWnd->EnableWindow(bEnable);
}
