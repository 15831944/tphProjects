// ShapeFolder.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ShapeFolder.h"
#include "afxdialogex.h"
#include "Common\ProjectManager.h"

// CShapeFolder dialog

IMPLEMENT_DYNAMIC(CShapeFolder, CDialog)
CShapeFolder::CShapeFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeFolder::IDD, pParent)
{
	shapeBarLocation = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
	m_style = NEW;
}

CShapeFolder::CShapeFolder(CString strBarName, CString strLocation,Folder_Style style /*= NEW*/,CWnd* pParent /* = NULL */)
	: CDialog(CShapeFolder::IDD, pParent)
{
	shapeBarName = strBarName;
	shapeBarLocation = strLocation;
	m_style = style;
}

CShapeFolder::~CShapeFolder()
{
}

void CShapeFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BARNAME, m_shapeBarName);
	DDX_Control(pDX, IDC_EDIT_BARLOCATION, m_shapeBarLocation);
}


BEGIN_MESSAGE_MAP(CShapeFolder, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOADSHAPEBAR, &CShapeFolder::OnLoadShapeBar)
	ON_BN_CLICKED(IDCANCEL, &CShapeFolder::OnCancel)
	ON_BN_CLICKED(IDOK, &CShapeFolder::OnOk)
END_MESSAGE_MAP()


// CShapeFolder message handlers


BOOL CShapeFolder::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_shapeBarName.SetWindowText(shapeBarName);
    m_shapeBarLocation.SetWindowText(shapeBarLocation);

    switch(m_style)
    {
    case NEW:
        SetWindowText(_T("New Shape Bar"));
        break;
    case NAME:
        SetWindowText(_T("Rename"));
        ((CWnd*)GetDlgItem(IDC_BTN_LOADSHAPEBAR))->EnableWindow(FALSE);
        m_shapeBarLocation.EnableWindow(FALSE);
        break;
    case PATH:
        SetWindowText(_T("Change Location"));
        m_shapeBarName.EnableWindow(FALSE);
        break;
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM , LPARAM lpData)   
{
	if(uMsg == BFFM_INITIALIZED)
	{
		CTreeCtrl	treePath;
		HTREEITEM	hItemSel;
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
		treePath.SubclassWindow(::GetDlgItem(hWnd, 0x3741));
		hItemSel	= treePath.GetSelectedItem();
		treePath.Expand(hItemSel, TVE_COLLAPSE);
		treePath.UnsubclassWindow();
	} 
	return 0;  
}

void CShapeFolder::OnLoadShapeBar()
{
	// TODO: Add your control notification handler code here
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));   
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = "Select a folder path:";   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, szPath))   
	{
		shapeBarLocation = szPath;
		int n = shapeBarLocation.ReverseFind('\\');
		if (n == shapeBarLocation.GetLength()-1)
		{
			shapeBarLocation = shapeBarLocation.Left(n);
		}
		shapeBarLocation = shapeBarLocation + "\\";
		m_shapeBarLocation.SetWindowText(shapeBarLocation);
	}
//     else   
//         AfxMessageBox("folder path is inviable,please select again");

}


void CShapeFolder::OnCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CShapeFolder::OnOk()
{
	// TODO: Add your control notification handler code here
	m_shapeBarName.GetWindowText(shapeBarName);
	CDialog::OnOK();
}
