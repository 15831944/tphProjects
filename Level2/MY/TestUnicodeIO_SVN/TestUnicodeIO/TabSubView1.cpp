#include "stdafx.h"
#include "Resource.h"
#include "TabSubView1.h"

IMPLEMENT_DYNAMIC(CTabSubView1, CDialog)

CTabSubView1::CTabSubView1(CWnd* pParent) : CDialog(CTabSubView1::IDD, pParent)
, m_oldCx(0)
, m_oldCy(0)
{
}

CTabSubView1::~CTabSubView1()
{
}

void CTabSubView1::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_READ, m_editRead);
    DDX_Control(pDX, IDC_EDIT_WRITE, m_editWrite);
    DDX_Control(pDX, IDC_EDIT_FLOAT, m_editFloat);
    DDX_Control(pDX, IDC_COMBO_TEST, m_cbTest);
}

BEGIN_MESSAGE_MAP(CTabSubView1, CDialog)
    ON_BN_CLICKED(IDC_BTN_READ, OnBnClickedBtnRead)
    ON_BN_CLICKED(IDC_BTN_WRITE, OnBnClickedBtnWrite)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FLOAT, OnDeltaposSpinFloat)
    ON_CBN_EDITCHANGE(IDC_COMBO_TEST, OnCbnEditchangeComboTest)
    ON_WM_SIZE()
END_MESSAGE_MAP()

void CTabSubView1::OnBnClickedBtnRead()
{
    FILE* file = fopen("d:\\ÄãºÃ.txt", "a+");
    if(file)
    {
        fprintf(file, "ÄãºÃ");
    }
    fclose(file);
}


void CTabSubView1::OnBnClickedBtnWrite()
{
}

void CTabSubView1::OnBnClickedButton1()
{
    CString str;
    m_editFloat.GetWindowText(str);
    double fValue = _wtof(str);
    CString strMsg, strFormat;
    MessageBox(strMsg);
}

void CTabSubView1::OnDeltaposSpinFloat(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    CString strEdit;
    m_editFloat.GetWindowText(strEdit);
    double fValue = _wtof(strEdit);
    if(pNMUpDown->iDelta > 0)
    {
        fValue += 1;
    }
    else
    {
        fValue -= 1;
    }
    CString strFormat;
    m_editFloat.SetWindowText(strEdit);
    *pResult = 0;
}


void CTabSubView1::OnCbnEditchangeComboTest()
{
    CString strCombo;
    m_cbTest.GetWindowText(strCombo);
    int nIndex = m_cbTest.FindString(-1, strCombo);
    if(nIndex != CB_ERR && 
        m_cbTest.GetLastPressKey() != VK_BACK &&
        m_cbTest.GetLastPressKey() != VK_DELETE)
    {
        CString strMatch;
        m_cbTest.GetLBText(nIndex, strMatch);
        m_cbTest.SetWindowText(strMatch);
        m_cbTest.SetEditSel(strCombo.GetLength(), strMatch.GetLength()+1);
    }
}

BOOL CTabSubView1::OnInitDialog()
{
    CDialog::OnInitDialog();

    CRect rect;
    GetClientRect(&rect);
    m_oldCx = rect.Width();
    m_oldCy = rect.Height();

    m_cbTest.ResetContent();
    m_cbTest.AddString(_T("123"));
    m_cbTest.AddString(_T("12345"));
    m_cbTest.AddString(_T("12333"));
    m_cbTest.AddString(_T("23456"));
    m_cbTest.SetCueBanner(_T("Select an item..."));

    return TRUE;
}

void CTabSubView1::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    
    if(m_editRead.GetSafeHwnd() == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(&m_editRead, TopLeft, TopRight, cx, cy);
    LayoutControl(&m_editWrite, TopLeft, TopRight, cx, cy);
    LayoutControl(&m_editFloat, TopLeft, TopRight, cx, cy);
    LayoutControl(&m_cbTest, TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_SPIN_FLOAT), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_READ), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_WRITE), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BUTTON1), TopRight, TopRight, cx, cy);
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}


void CTabSubView1::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldCx;
    int deltaY = cy - m_oldCy;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

