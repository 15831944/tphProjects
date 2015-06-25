// DlgAddNewCmpReport.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAddNewCmpReport.h"
#include "afxdialogex.h"
#include "compare\ComparativeProject.h"
#include "Common\ProjectManager.h"

IMPLEMENT_DYNAMIC(DlgAddNewCmpReport, CDialog)

DlgAddNewCmpReport::DlgAddNewCmpReport(CWnd* pParent)
	: CDialog(DlgAddNewCmpReport::IDD, pParent)
	, m_strName(_T(""))
	, m_strDesc(_T(""))
{

}

DlgAddNewCmpReport::~DlgAddNewCmpReport()
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
END_MESSAGE_MAP()

BOOL DlgAddNewCmpReport::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rect;
	GetClientRect(&rect);
	int width, height;
	width = rect.Width();
	height = rect.Height();
	LayoutAllControl(width, height);
	return TRUE;
}

void DlgAddNewCmpReport::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_CMPREPORTNAME);
	if(pCtrl == NULL)
		return;

	LayoutAllControl(cx, cy);
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
		CComparativeProjectDataSet dsProj;
		dsProj.loadDataSet(PROJMANAGER->GetAppPath());
		std::vector<CComparativeProject *> vProjs;
		if (dsProj.GetProjects(vProjs))
		{
			for (int i = 0; i < static_cast<int>(vProjs.size()); i++)
			{
				if (vProjs[i]->GetName().CompareNoCase(m_strName) == 0)
				{
					CString strTemp;
					strTemp.Format(_T("The Comparative Report \"%s\" already exists."), m_strName);
					MessageBox(strTemp, NULL, MB_ICONWARNING | MB_OK);
					return;
				}
			}
		}
	}
	CDialog::OnOK();
}

void DlgAddNewCmpReport::OnCancel()
{
	CDialog::OnCancel();
}

void DlgAddNewCmpReport::LayoutAllControl( int cx, int cy )
{
	CWnd* pCtrl = NULL;
	pCtrl = GetDlgItem(IDC_STATIC_CMPREPORTNAME);
	pCtrl->SetWindowPos(NULL, 10, 10, 50, 12, NULL);

	pCtrl = GetDlgItem(IDC_CMPREPORTNAME);
	pCtrl->SetWindowPos(NULL, 10, 24, cx-20, 22, NULL);

	pCtrl = GetDlgItem(IDC_STATIC_CMPREPORTDESCRI);
	pCtrl->SetWindowPos(NULL, 10, 66, 80, 12, NULL);

	pCtrl = GetDlgItem(IDC_CMPREPORTDESCRI);
	pCtrl->SetWindowPos(NULL, 10, 80, cx-20, cy-80-43, NULL);

	pCtrl = GetDlgItem(IDOK);
	pCtrl->SetWindowPos(NULL, cx-156, cy-33, 68, 23, NULL);

	pCtrl = GetDlgItem(IDCANCEL);
	pCtrl->SetWindowPos(NULL, cx-78, cy-33, 68, 23, NULL);
	InvalidateRect(NULL);
}


