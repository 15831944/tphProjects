#include "stdafx.h"
#include "CoolTreeEditSpin.h"
#include "Resource.h"
#include ".\cooltreeeditspin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCoolTreeEditSpin::CCoolTreeEditSpin(CWnd* pParent)
    : CDialog(IDD_COOLTREE_EDITSPIN, pParent)
    , m_bESC(FALSE)
{
    m_pParent = pParent;
}

void CCoolTreeEditSpin::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SPIN1, m_spinValue);
    DDX_Control(pDX, IDC_EDIT1, m_editValue);
}

BEGIN_MESSAGE_MAP(CCoolTreeEditSpin, CDialog)
    ON_WM_SIZE()
    ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
END_MESSAGE_MAP()

void CCoolTreeEditSpin::OnSize(UINT nType, int cx, int cy) 
{
    CDialog::OnSize(nType, cx, cy);
}

void CCoolTreeEditSpin::OnKillfocusEdit1() 
{
    ShowWindow(SW_HIDE);
    if(!m_bESC && m_pParent)
    {	
        CString strValue;
        m_editValue.GetWindowText(strValue);
        m_pParent->SendMessage(UM_CEW_EDITSPIN_END,0,(LPARAM)&strValue);
    }
    m_bESC = FALSE;
}

CWnd* CCoolTreeEditSpin::SetParent(CWnd* pWndNewParent)
{
    m_pParent=pWndNewParent;
    return CWnd::SetParent(pWndNewParent);
}

void CCoolTreeEditSpin::SetSpinRange(int nMin, int nMax)
{
    m_spinValue.SetRange(nMin,nMax);
}

BOOL CCoolTreeEditSpin::PreTranslateMessage(MSG* pMsg)
{
    if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
        m_bESC = TRUE;
    return CDialog::PreTranslateMessage(pMsg);
}
