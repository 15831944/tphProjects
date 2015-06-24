// DlgMultiReportSelectModel.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgMultiReportSelectModel.h"
#include "../Common/ProjectManager.h"

// CDlgMultiReportSelectModel dialog

IMPLEMENT_DYNAMIC(CDlgMultiReportSelectModel, CDialog)
CDlgMultiReportSelectModel::CDlgMultiReportSelectModel(const CCompareModelList& lstCmpModel,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMultiReportSelectModel::IDD, pParent)
{
	m_lstCompareModel = lstCmpModel;
}

CDlgMultiReportSelectModel::~CDlgMultiReportSelectModel()
{
}

void CDlgMultiReportSelectModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_cooltree);
	DDX_Control(pDX, IDC_STATIC_CONTAINER, m_ToolBarText);
}


BEGIN_MESSAGE_MAP(CDlgMultiReportSelectModel, CDialog)
	//{{AFX_MSG_MAP(CModelSelectionDlg)
	ON_COMMAND(ID_BUTTONADD,OnAddModel)
	ON_COMMAND(ID_BUTTONDELETE,OnDeleteModel)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, OnSetfocusTree1)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgMultiReportSelectModel message handlers
BOOL CDlgMultiReportSelectModel::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ToolBar.GetToolBarCtrl().SetCmdID(0, ID_BUTTONADD);
	m_ToolBar.GetToolBarCtrl().SetCmdID(1, ID_BUTTONDELETE);
	InitToolBar();

	//Disable EnableStatus effect
	//m_cooltree.EnableMutiTree();
	InitTree();


	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMultiReportSelectModel::InitTree()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	//	Add By Kevin Chang
	//	Last Modified: 2005-5-17 18:03
	//std::vector<CModelToCompare> vModels;
	
//	ArctermFile f;
//	f.openFile(_T("D:\\Arcterm\\Comparative Report\\HostInfo.txt"), READ, 2.2);
//	m_dsModel.readData(f);
//	f.closeOut();

	SetTimer(0, 10, NULL);

}
void CDlgMultiReportSelectModel::InitToolBar()
{
	CRect rc;
	m_ToolBarText.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_ToolBarText.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTONADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTONDELETE,FALSE );
}
void CDlgMultiReportSelectModel::OnOK() 
{
	// TODO: Add extra validation here
	//*frank test
	//	CModelToCompare model;
	//	CString str[5] = {"Model1", "Model2","Model3","Model4","Model5"};
	//	for (int i = 0; i < 5; i++)
	//	{
	//		model.SetModelName(str[i]);
	//		model.SetModelLocation("c:\\frank");
	//
	//		m_modelsManager.AddModel(model);
	//	}
	//------------------------

	SaveHostInfo();
	if(GetSelectionModelsFromTree() == -1)
		return;

	CDialog::OnOK();
}
void CDlgMultiReportSelectModel::OnAddModel()
{
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = BIF_USENEWUI;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;

	char pszPath[MAX_PATH] = {0};
	SHGetPathFromIDList(idl, pszPath);
	m_Path = pszPath;

	AddTreeItem(m_Path);
}
void CDlgMultiReportSelectModel::OnDeleteModel()
{
	HTREEITEM hItem=m_cooltree.GetSelectedItem();
	if(hItem && m_cooltree.ItemHasChildren(hItem))
	{
		if(MessageBox("Delete Model,Are you sure?",NULL,MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		{
			RemoveTreeItem(m_cooltree.GetItemText(hItem));
			m_cooltree.DeleteItem(hItem);
		}
	}
}
void CDlgMultiReportSelectModel::OnCancel() 
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}
void CDlgMultiReportSelectModel::OnSetfocusTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem=m_cooltree.GetSelectedItem();

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETE,TRUE);
	*pResult = 0;
}

BOOL CDlgMultiReportSelectModel::GetLocalProjectPath(CString& strPath)
{
	HKEY hKey = NULL;
	CString strSubKey = _T("SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths");

	if ( RegOpenKey(HKEY_LOCAL_MACHINE, strSubKey, &hKey) == ERROR_SUCCESS )
	{
		TCHAR szAppPath[128], szProjectPath[128];
		DWORD dwType;
		memset(szAppPath, 0, sizeof(szAppPath) / sizeof(TCHAR));
		memset(szProjectPath, 0, sizeof(szProjectPath) / sizeof(TCHAR));
		DWORD dwSize = sizeof(szAppPath) / sizeof(TCHAR);
		if ( RegQueryValueEx(hKey, _T("AppPath"), 0, &dwType, (BYTE*)szAppPath, &dwSize) == ERROR_SUCCESS ) 
		{
			dwSize = sizeof(szProjectPath) / sizeof(TCHAR);
			if ( RegQueryValueEx(hKey, _T("ProjectPath"), 0, &dwType, (BYTE*)szProjectPath, &dwSize) == ERROR_SUCCESS )
			{
				strPath = szAppPath;
				strPath += _T("\\");
				strPath += szProjectPath;

			}
			else
			{
				strPath = szAppPath;
				strPath += _T("\\Project");			

			}
			return TRUE;
		}

		RegCloseKey(hKey);
	}

	return FALSE;
}

CString CDlgMultiReportSelectModel::GetHostName(const CString& strFolder)
{
	CString str = _T("(LOCAL)");
	if (strFolder.Left(2) == _T("\\\\"))
	{
		int nPos = strFolder.Find(_T("\\"), 2);
		if (nPos != -1)
			str = strFolder.Mid(2, nPos - 2);
		else
			str = strFolder.Mid(2, strFolder.GetLength() - 2);
	}

	str.MakeUpper();
	return str;
}
void CDlgMultiReportSelectModel::SaveHostInfo()
{
	m_dsModel.saveDataSet(PROJMANAGER->GetAppPath(), false);
}
int CDlgMultiReportSelectModel::GetSelectionModelsFromTree()
{

	CCompareModelList selcModelList;
	//m_lstCompareModel.Clear();

	for (HTREEITEM hItem = m_cooltree.GetRootItem(); hItem != NULL; hItem = m_cooltree.GetNextSiblingItem(hItem))
	{
		if (m_cooltree.ItemHasChildren(hItem))
		{
			for (HTREEITEM hItemChild = m_cooltree.GetChildItem(hItem); hItemChild != NULL; 
				hItemChild = m_cooltree.GetNextSiblingItem(hItemChild))
			{
				if (m_cooltree.IsCheckItem(hItemChild))
				{
					CCompareModel model;
					CString strPath;
					model.SetModelName(m_cooltree.GetItemText(hItemChild));

					std::vector<CString> vstrPath;
					m_dsModel.GetFolders(vstrPath);
					for (int i = 0; i < static_cast<int>(vstrPath.size()); i++)
					{
						if (GetHostName(vstrPath[i]) == m_cooltree.GetItemText(hItem))
						{
							strPath = vstrPath[i];
							break;
						}
					}

					strPath += _T("\\") + m_cooltree.GetItemText(hItemChild);
					model.SetModelLocation(strPath);

					selcModelList.AddModel(model);

				}
			}
		}
	}
	//find the delete model
	int nOldModelCount = m_lstCompareModel.GetCount();
	
	for(int i =0; i < nOldModelCount; ++i)
	{
		bool bFind = false;
		int nNewCount = selcModelList.GetCount();
		for (int j =0; j < nNewCount; ++j)
		{
			if (m_lstCompareModel.GetModel(i).GetModelName().CompareNoCase(selcModelList.GetModel(j).GetModelName()) == 0 )
			{
				bFind = true;
				selcModelList.DelModel(j);
				break;
			}
		}
		if (bFind)
			continue;
		else	//delete 
		{
			m_lstCompareModel.DelModel(i);
			i = 0;
		}

	}

	for (int nNew =0; nNew < selcModelList.GetCount(); ++nNew)
	{
		m_lstCompareModel.AddModel(selcModelList.GetModel(nNew));
	}

	return 0;
}
void CDlgMultiReportSelectModel::AddTreeItem(const CString& strPath, BOOL bAdd)
{
	CString strHostName = GetHostName(strPath);
	strHostName.MakeUpper();

	BOOL bFound = FALSE;
	for (HTREEITEM hItem = m_cooltree.GetRootItem(); hItem != NULL; 
		hItem = m_cooltree.GetNextSiblingItem(hItem))
	{
		if (strHostName == m_cooltree.GetItemText(hItem))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		AfxMessageBox(_T("The Machine name is exists!"));
		return;
	}

	if (bAdd)
		m_dsModel.AddItem(strPath);


	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	std::vector<CString> vModels;
	if (CModelsManager::ListAllModelsByPath(strPath, vModels))
	{
		cni.nt = NT_CHECKBOX;
		HTREEITEM hRoot = m_cooltree.InsertItem(strHostName, cni, FALSE);

		for (int i = 0; i < static_cast<int>(vModels.size()); i++)
		{
			//cni.nt = NT_CHECKBOX;
			m_cooltree.InsertItem(vModels[i], cni, FALSE, FALSE, hRoot);
		}

		m_cooltree.Expand(hRoot, TVE_EXPAND);
	}
}
void CDlgMultiReportSelectModel::RemoveTreeItem(const CString &strHostName)
{
	std::vector<CString>::iterator iter;
	std::vector<CString>& vstrFolder = m_dsModel.GetFolders();
	for (iter = vstrFolder.begin(); iter != vstrFolder.end(); iter++)
	{
		if (GetHostName(*iter) == strHostName)
		{
			vstrFolder.erase(iter);
			break;
		}
	}
}
void CDlgMultiReportSelectModel::InitSelectedNodes()
{
	//std::vector<CModelToCompare> vModels = m_modelsManager.GetModelsList();
	for (int i = 0; i < static_cast<int>(m_lstCompareModel.GetCount()); i++)
	{
		BOOL bFound = FALSE;

		for (HTREEITEM hItem = m_cooltree.GetRootItem(); hItem != NULL; hItem = m_cooltree.GetNextSiblingItem(hItem))
		{
			if (m_cooltree.ItemHasChildren(hItem))
			{
				for (HTREEITEM hItemChild = m_cooltree.GetChildItem(hItem); hItemChild != NULL;
					hItemChild = m_cooltree.GetNextSiblingItem(hItemChild))
				{
					CString strModuleName = m_cooltree.GetItemText(hItemChild);
					strModuleName.MakeUpper();
					CString str = m_lstCompareModel.GetModel(i).GetModelName();
					str.MakeUpper();
					if ((m_cooltree.GetItemText(hItem) == GetHostName(m_lstCompareModel.GetModel(i).GetModelLocation())) &&
						(strModuleName == str))
					{
						if (!m_cooltree.IsCheckItem(hItem))
							m_cooltree.SetCheckStatus(hItem, TRUE);

						m_cooltree.SetCheckStatus(hItemChild, TRUE);
						bFound = TRUE;
						break;
					}
				}

				if (bFound)
					break;
			}
		}

	}
}
int CDlgMultiReportSelectModel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	
	return 0;
}
void CDlgMultiReportSelectModel::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 0)
	{
		KillTimer(0);

		m_dsModel.loadDataSet(PROJMANAGER->GetAppPath());

		std::vector<CString>	vFolders;
		if (m_dsModel.GetFolders(vFolders))
		{
			for (int i = 0; i < static_cast<int>(vFolders.size()); i++)
			{
				AddTreeItem(vFolders[i], FALSE);
			}
		}
		else
		{
			CString strPath;
			if (GetLocalProjectPath(strPath))
			{
				AddTreeItem(strPath);
			}
		}


		InitSelectedNodes();

		m_cooltree.SetFocus();
		HTREEITEM hRoot = m_cooltree.GetRootItem();
		if (hRoot)
			m_cooltree.SelectItem(hRoot);
	}
	CDialog::OnTimer(nIDEvent);
}

















