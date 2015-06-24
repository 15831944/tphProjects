// ModelSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ModelSelectionDlg.h"
#include "common\fileman.h"
#include "GlobalDBManDlg.h"
#include ".\modelselectiondlg.h"
#include "compare/ModelToCompare.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelSelectionDlg dialog


CModelSelectionDlg::CModelSelectionDlg(CModelsManager*	modelsManager,CWnd* pParent /*=NULL*/)
	: CDialog(CModelSelectionDlg::IDD, pParent)
{
	m_modelsManager = modelsManager;
	//{{AFX_DATA_INIT(CModelSelectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModelSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelSelectionDlg)
	DDX_Control(pDX, IDC_TREE1, m_cooltree);
	DDX_Control(pDX, IDC_STATIC_CONTAINER, m_ToolBarText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CModelSelectionDlg)
	ON_COMMAND(ID_BUTTONADD,OnAddModel)
	ON_COMMAND(ID_BUTTONDELETE,OnDeleteModel)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, OnSetfocusTree1)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnTvnItemexpandingTree1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelSelectionDlg message handlers

BOOL CModelSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ToolBar.GetToolBarCtrl().SetCmdID(0, ID_BUTTONADD);
	m_ToolBar.GetToolBarCtrl().SetCmdID(1, ID_BUTTONDELETE);
	InitToolBar();
	
	//Disable EnableStatus effect
	m_cooltree.EnableMutiTree();
	InitTree();

	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModelSelectionDlg::InitTree()
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

void CModelSelectionDlg::InitToolBar()
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

void CModelSelectionDlg::OnOK() 
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

void CModelSelectionDlg::OnAddModel()
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

void CModelSelectionDlg::OnDeleteModel()
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



void CModelSelectionDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CModelSelectionDlg::OnSetfocusTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem=m_cooltree.GetSelectedItem();

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETE,TRUE);
	*pResult = 0;
}


BOOL CModelSelectionDlg::GetLocalProjectPath(CString& strPath)
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

CString CModelSelectionDlg::GetHostName(const CString& strFolder)
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

void CModelSelectionDlg::SaveHostInfo()
{
	m_dsModel.saveDataSet(PROJMANAGER->GetAppPath(), false);
}

int CModelSelectionDlg::GetSelectionModelsFromTree()
{
	std::vector<CModelToCompare *> vOldModels = m_modelsManager->GetModelsList();
	m_modelsManager->GetModelsList().clear();

	for (HTREEITEM hItem = m_cooltree.GetRootItem(); hItem != NULL; hItem = m_cooltree.GetNextSiblingItem(hItem))
	{
		if (m_cooltree.ItemHasChildren(hItem))
		{
			for (HTREEITEM hItemChild = m_cooltree.GetChildItem(hItem); hItemChild != NULL; 
				hItemChild = m_cooltree.GetNextSiblingItem(hItemChild))
			{
				if (m_cooltree.IsCheckItem(hItemChild))
				{
					CString strModelName = m_cooltree.GetItemText(hItemChild);
					CString strPath;
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
					
					CModelToCompare* model = NULL;
					//check the Model is in the old model list
					bool bFindInOldModel = false;
					std::vector<CModelToCompare *>::iterator iterModel = vOldModels.begin();
					std::vector<CModelToCompare *>::iterator iterModelEnd = vOldModels.end();
					for (; iterModel != iterModelEnd; ++iterModel)
					{
						if(((*iterModel)->GetModelName().CompareNoCase(strModelName) == 0)&&
							(strPath.CompareNoCase((*iterModel)->GetModelLocation()) == 0))
						{
							model = *iterModel;
							m_modelsManager->AddModel(model);
							vOldModels.erase(iterModel);
							bFindInOldModel = true;
							break;
						}
					}
				

					if (bFindInOldModel == false)
					{
						model = new CModelToCompare;
						model->SetModelName(strModelName);
						model->SetModelLocation(strPath);
						CString strUniqueName = _T("");
						strUniqueName.Format(_T("%s.%s"),GetHostName(strPath),strModelName);
						model->SetUniqueName(strUniqueName);
						m_modelsManager->AddModel(model);
					}

					model->ClearSimResult();

					//get sim result selection
					for (HTREEITEM hItemResult = m_cooltree.GetChildItem(hItemChild); hItemResult != NULL;
						hItemResult = m_cooltree.GetNextSiblingItem(hItemResult))
					{
						if (m_cooltree.IsCheckItem(hItemResult))
						{			
							model->AddSimResult(m_cooltree.GetItemText(hItemResult));
						}
					}

					if (model->GetSimResultCount() == 0)
					{
						MessageBox("Please select one simResult at least:Model -- " + model->GetUniqueName(),"ARCTerm Comparative Report");
						return -1;
					}
//					
				}
			}
		}
	}

	return 0;
}

void CModelSelectionDlg::AddTreeItem(const CString& strPath, BOOL bAdd)
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
//	std::vector<CModelToCompare> vModels;
	std::vector<CString> vModels;
	if (CModelsManager::ListAllModelsByPath(strPath, vModels))
	{
		cni.nt = NT_CHECKBOX;
		HTREEITEM hRoot = m_cooltree.InsertItem(strHostName, cni, FALSE);
		
		m_cooltree.SetItemData(hRoot,(DWORD)(int)ITEM_HOST);
		
		for (int i = 0; i < static_cast<int>(vModels.size()); i++)
		{
			//cni.nt = NT_CHECKBOX;
//			HTREEITEM hProjItem =  m_cooltree.InsertItem(vModels[i].GetModelName(), cni, FALSE, FALSE, hRoot);
			HTREEITEM hProjItem =  m_cooltree.InsertItem(vModels[i], cni, FALSE, FALSE, hRoot);
			m_cooltree.SetItemData(hProjItem,(DWORD)(int)ITEM_MODEL);
			HTREEITEM hSimResultItem =  m_cooltree.InsertItem(_T("SimResult0"), cni, FALSE, FALSE, hProjItem);	
			m_cooltree.SetItemData(hSimResultItem,(DWORD)(int)ITEM_SIMRESULT_UNLOAD);
	
		}

		m_cooltree.Expand(hRoot, TVE_EXPAND);
	}
}

void CModelSelectionDlg::RemoveTreeItem(const CString &strHostName)
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

void CModelSelectionDlg::InitSelectedNodes()
{
	std::vector<CModelToCompare *> vModels = m_modelsManager->GetModelsList();
	for (int i = 0; i < static_cast<int>(vModels.size()); i++)
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
					CString str = vModels[i]->GetModelName();
					str.MakeUpper();
					if ((m_cooltree.GetItemText(hItem) == GetHostName(vModels[i]->GetModelLocation())) &&
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

int CModelSelectionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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




void CModelSelectionDlg::OnTimer(UINT nIDEvent)
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

void CModelSelectionDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

void CModelSelectionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CModelSelectionDlg::OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if(pNMTreeView->action == TVE_EXPAND)
	{
//		

		int nItemType = (int)m_cooltree.GetItemData(pNMTreeView->itemNew.hItem);
		if (nItemType == ITEM_MODEL)//if the item isn't model,skip
		{
			HTREEITEM hModelItem = pNMTreeView->itemNew.hItem;
			HTREEITEM hChildItem =m_cooltree.GetChildItem(hModelItem);
			nItemType = (int)m_cooltree.GetItemData(hChildItem);
			if (nItemType == ITEM_SIMRESULT_UNLOAD) //the  child item is load or not, if not ,load the sub item 
			{
				HTREEITEM hHostItem = m_cooltree.GetParentItem(hModelItem);

				CString strPath = _T("");
				std::vector<CString> vstrPath;
				m_dsModel.GetFolders(vstrPath);
				for (int i = 0; i < static_cast<int>(vstrPath.size()); i++)
				{
					if (GetHostName(vstrPath[i]) == m_cooltree.GetItemText(hHostItem))
					{
						strPath = vstrPath[i];
						break;
					}
				}

				CString strModelName = m_cooltree.GetItemText(hModelItem);
				strPath += _T("\\") + strModelName;
				
				CString strUniqueName = _T("");
				strUniqueName.Format("%s.%s",GetHostName(strPath),strModelName);
				int nCount = GetSimCountOfModel(strPath);
				m_cooltree.DeleteItem(hChildItem);
				if (nCount == 0)
				{
					return;
				}

				//check the Model is in the old model list
				std::vector<CModelToCompare *> vModels = m_modelsManager->GetModelsList();
				CModelToCompare *model = NULL;
				std::vector<CModelToCompare *>::iterator iterModel = vModels.begin();
				std::vector<CModelToCompare *>::iterator iterModelEnd = vModels.end();
				for (; iterModel != iterModelEnd; ++iterModel)
				{
					if((*iterModel)->GetUniqueName().CompareNoCase(strUniqueName) == 0)
					{
						model = *iterModel;
						break;
					}
				}
//				/////
//				//test code
//					if(model != NULL)
//					{
//						model->AddSimResult("SimResult0");
////						model->AddSimResult("SimResult0");
//					
//					}
//				
//				///


				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this, cni);
				cni.nt = NT_CHECKBOX;
				for (int nResult = 0; nResult < nCount; ++nResult)
				{
					CString strText = _T("");
					strText.Format(_T("SimResult%d"),nResult);
					HTREEITEM hResultItem = m_cooltree.InsertItem(strText,cni, FALSE, FALSE, hModelItem);
					m_cooltree.SetItemData(hResultItem,(DWORD)(int)ITEM_SIMRESULT_LOAD);
					if (model != NULL)
					{
						int nSelSimResult = model->GetSimResultCount();
						for (int nIndex =0; nIndex <nSelSimResult; ++nIndex)
						{
							if (strText.CompareNoCase(model->GetSimResult(nIndex)) == 0)
							{
								if (!m_cooltree.IsCheckItem(hResultItem))
									m_cooltree.SetCheckStatus(hResultItem, TRUE);
								break;
							}
						}
					} 
				}
			}
		}
	}

	*pResult = 0;
}
int  CModelSelectionDlg::GetSimCountOfModel(CString strPath)
{
	const static int BufferSize = 10240;

	strPath +=_T("\\INPUT\\SimAndReport.sim");

	std::ifstream fpIn;
	fpIn.open(strPath,std::ios::in);
	if (fpIn.bad())
	{
		return 0;
	}
	char szBuffer[BufferSize] = {0};
	fpIn.getline(szBuffer,BufferSize);
	CString strFileHeader(szBuffer);
	int nPos =  strFileHeader.Find(",");

	CString strVersion = strFileHeader.Mid(nPos+1);

	double fVersion = ::atof(strVersion);
	if (fVersion <= 2.41 ) //version 2.40
	{
		for (int i=0; i< 5; ++i)
		{
			fpIn.getline(szBuffer,BufferSize);
		}

		//simresults
		memset(szBuffer,0,BufferSize);
		
		fpIn.getline(szBuffer,BufferSize);
		
		int nSimResults= ::atoi(szBuffer);
		if (nSimResults ==0)
		{
			return 0;
		}
		//sim result
		memset(szBuffer,0,BufferSize);
		fpIn.getline(szBuffer,BufferSize);
		int nSimResult = atoi(szBuffer);

		return nSimResult;

	}
	return 0;

}