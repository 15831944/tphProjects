#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAddNewCmpReport.h"
#include "compare\ComparativeProject.h"
#include "Common\ProjectManager.h"

DlgAddNewCmpReport::DlgAddNewCmpReport(CWnd* pParent)
	: CDialog(DlgAddNewCmpReport::IDD, pParent)
	, m_strName(_T(""))
	, m_strDesc(_T(""))
    , m_oldCx(0)
    , m_oldCy(0)
{
}

void DlgAddNewCmpReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CMPREPORTNAME, m_strName);
	DDX_Text(pDX, IDC_CMPREPORTDESCRI, m_strDesc);
}


BEGIN_MESSAGE_MAP(DlgAddNewCmpReport, CDialog)
	ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

BOOL DlgAddNewCmpReport::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rect;
	GetClientRect(&rect);
	m_oldCx = rect.Width();
    m_oldCy = rect.Height();
	return TRUE;
}

void DlgAddNewCmpReport::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 300;
    lpMMI->ptMinTrackSize.y = 168;
    CDialog::OnGetMinMaxInfo(lpMMI);
}

void DlgAddNewCmpReport::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_CMPREPORTNAME);
	if(pCtrl == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_STATIC_CMPREPORTNAME), TopLeft,  TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_CMPREPORTDESCRI), TopLeft,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_CMPREPORTNAME), TopLeft,  TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_CMPREPORTDESCRI), TopLeft,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight,  BottomRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void DlgAddNewCmpReport::OnOK()
{
	UpdateData(TRUE);
	if(m_strName.IsEmpty())
	{
		MessageBox("The project name is empty, please input a name.", NULL, MB_ICONWARNING | MB_OK);
		return;
	}
	else
	{
		if(CMPPROJECTMANAGER->FindCmpProjectByName(m_strName) != NULL)
		{
			CString strTemp;
			strTemp.Format(_T("A Comparative Report named %s already exists."), m_strName);
			MessageBox(strTemp, NULL, MB_ICONWARNING | MB_OK);
			return;
		}
	}
	CDialog::OnOK();
}

void DlgAddNewCmpReport::OnCancel()
{
	CDialog::OnCancel();
}

void DlgAddNewCmpReport::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), cy+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, cx+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

DlgEditCmpReport::DlgEditCmpReport(CWnd* pParent) :
DlgAddNewCmpReport(pParent),
    m_pCmpProj(NULL)
{
}

void DlgEditCmpReport::SetCmpProj(const CComparativeProject* val)
{
    m_pCmpProj = val;
    m_strName = m_pCmpProj->GetName();
    m_strDesc = m_pCmpProj->GetDescription();
}

void DlgEditCmpReport::OnOK()
{
    UpdateData(TRUE);
    if(m_strName.IsEmpty())
    {
        MessageBox("The project name is empty, please input a name.", NULL, MB_ICONWARNING | MB_OK);
        return;
    }
    else
    {
        CComparativeProject* pCmpProj = CMPPROJECTMANAGER->FindCmpProjectByName(m_strName);
        if(pCmpProj != NULL && pCmpProj != m_pCmpProj)
        {
            CString strTemp;
            strTemp.Format(_T("A Comparative Report named %s already exists."), m_strName);
            MessageBox(strTemp, NULL, MB_ICONWARNING | MB_OK);
            return;
        }
    }
    CDialog::OnOK();
}


