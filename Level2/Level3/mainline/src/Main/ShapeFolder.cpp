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
	m_shapeBarLocation = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
}

CShapeFolder::CShapeFolder(CString strBarName, CString strLocation,CWnd* pParent /* = NULL */)
	: CDialog(CShapeFolder::IDD, pParent)
{
	m_shapeBarName = strBarName;
	m_shapeBarLocation = strLocation;
}

CShapeFolder::~CShapeFolder()
{
}

void CShapeFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BARNAME, m_editBarName);
	DDX_Control(pDX, IDC_EDIT_BARLOCATION, m_editBarLocation);
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

    m_editBarName.SetWindowText(m_shapeBarName);
    m_editBarLocation.SetWindowText(m_shapeBarLocation);
    SetWindowText(m_strTitle);
    // 20140805 10:10am: the shape bar's location can not be edited.
    if(m_strTitle.Compare("Edit Shape Bar") == 0)
    {
        GetDlgItem(IDC_BTN_LOADSHAPEBAR)->EnableWindow(FALSE);
        m_editBarLocation.EnableWindow(FALSE);
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
		m_shapeBarLocation = szPath;
		int n = m_shapeBarLocation.ReverseFind('\\');
		if (n == m_shapeBarLocation.GetLength()-1)
		{
			m_shapeBarLocation = m_shapeBarLocation.Left(n);
		}
		m_shapeBarLocation = m_shapeBarLocation + "\\";
		m_editBarLocation.SetWindowText(m_shapeBarLocation);
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
	m_editBarName.GetWindowText(m_shapeBarName);
	CDialog::OnOK();
}
