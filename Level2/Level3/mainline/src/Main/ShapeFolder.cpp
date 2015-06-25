// ShapeFolder.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ShapeFolder.h"
#include "afxdialogex.h"
#include "Common\ProjectManager.h"

// CShapeFolder dialog

IMPLEMENT_DYNAMIC(CShapeFolder, CDialog)
int CShapeFolder::folder_id = 0;
CShapeFolder::CShapeFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeFolder::IDD, pParent)
{
	if(folder_id != 0)
		folderName.Format("New Folder(%d)",folder_id);
	else
		folderName = "New Folder";
	folderPath = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
	m_style = NEW;
}

CShapeFolder::CShapeFolder(CString name, CString path,Folder_Style style /*= NEW*/,CWnd* pParent /* = NULL */)
	: CDialog(CShapeFolder::IDD, pParent)
{
	folderName = name;
	folderPath = path;
	m_style = style;
}

CShapeFolder::~CShapeFolder()
{
}

void CShapeFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_folderName);
	DDX_Control(pDX, IDC_EDIT2, m_folderPath);
}


BEGIN_MESSAGE_MAP(CShapeFolder, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CShapeFolder::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CShapeFolder::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CShapeFolder::OnBnClickedOk)
END_MESSAGE_MAP()


// CShapeFolder message handlers


BOOL CShapeFolder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	switch(m_style)
	{
	case NEW:
		break;
	case NAME:
		((CWnd*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
		break;
	case PATH:
		m_folderName.EnableWindow(FALSE);
		break;
	}
	m_folderName.SetWindowText(folderName);
	m_folderPath.SetWindowText(folderPath);

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

void CShapeFolder::OnBnClickedButton1()
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
		folderPath = szPath;
		int n = folderPath.ReverseFind('\\');
		if (n == folderPath.GetLength()-1)
		{
			folderPath = folderPath.Left(n);
		}
		folderPath = folderPath + "\\";
		m_folderPath.SetWindowText(folderPath);
	}
//     else   
//         AfxMessageBox("folder path is inviable,please select again");

}


void CShapeFolder::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CShapeFolder::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_folderName.GetWindowText(folderName);
	CDialog::OnOK();
}
