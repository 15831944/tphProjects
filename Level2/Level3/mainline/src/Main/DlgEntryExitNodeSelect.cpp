// DlgEntryExitNodeSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgEntryExitNodeSelect.h"
#include ".\dlgentryexitnodeselect.h"


// CDlgEntryExitNodeSelect dialog

IMPLEMENT_DYNAMIC(CDlgEntryExitNodeSelect, CDialog)
CDlgEntryExitNodeSelect::CDlgEntryExitNodeSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEntryExitNodeSelect::IDD, pParent)
{
}

CDlgEntryExitNodeSelect::CDlgEntryExitNodeSelect(LandsideFacilityLayoutObjectList* objlist,CWnd* pParent /*=NULL*/)
: CDialog(CDlgEntryExitNodeSelect::IDD, pParent)
{
	m_pObjlist=objlist;
	m_pEntryExitNode=NULL;
}

CDlgEntryExitNodeSelect::~CDlgEntryExitNodeSelect()
{
}

void CDlgEntryExitNodeSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BUSSTATIONSELECT, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgEntryExitNodeSelect, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BUSSTATIONSELECT, OnTvnSelchangedTreeBusstationselect)
END_MESSAGE_MAP()

BOOL CDlgEntryExitNodeSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	SetWindowText(_T("Entry/Exit Select"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CDlgEntryExitNodeSelect message handlers

void CDlgEntryExitNodeSelect::LoadTree()
{

		ALTObjectIDList entryExitNameList;
		m_mapEntryExit.clear();
		m_treeCtrl.DeleteAllItems();
		for(int i=0;i<m_pObjlist->getCount();i++)
		{
			LandsideFacilityLayoutObject* pObj = m_pObjlist->getObject(i);
			if(pObj->GetType() == ALT_LEXT_NODE)
			{
				entryExitNameList.push_back(pObj->getName());
				m_mapEntryExit.insert(EntryExitMap::value_type(pObj->getName(),(LandsideExternalNode *)pObj));
	
			}
		}
	
		SortedStringList levelName;
		entryExitNameList.GetLevel1StringList(levelName);
		SortedStringList::iterator iter = levelName.begin();
		for (; iter != levelName.end(); ++iter)
		{
			HTREEITEM hItem = m_treeCtrl.InsertItem(*iter,TVI_ROOT);
	
			SortedStringList level1Name;
			CString strLevel1Name = *iter;
			entryExitNameList.GetLevel2StringList(*iter,level1Name);
			if ((int)level1Name.size() == 0 && m_mapEntryExit.find(ALTObjectID(strLevel1Name))!=m_mapEntryExit.end())
			{		
				m_treeCtrl.SetItemData(hItem,(DWORD_PTR)m_mapEntryExit[ALTObjectID(strLevel1Name)]);			
			}else
			{
				m_treeCtrl.SetItemData(hItem,NULL);	
			}
			SortedStringList::iterator iter1 = level1Name.begin();
			for (; iter1 != level1Name.end(); ++iter1)
			{
				HTREEITEM hlevel1Item = m_treeCtrl.InsertItem(*iter1,hItem,TVI_LAST);
	
				SortedStringList level2Name;
				CString strLevel2Name = strLevel1Name + CString("-") + CString(*iter1);
				entryExitNameList.GetLevel3StringList(*iter,*iter1,level2Name);
				if ((int)level2Name.size() == 0 && m_mapEntryExit.find(ALTObjectID(strLevel2Name))!=m_mapEntryExit.end())
				{		
					m_treeCtrl.SetItemData(hlevel1Item,(DWORD_PTR)m_mapEntryExit[ALTObjectID(strLevel2Name)]);			
				}else
				{
					m_treeCtrl.SetItemData(hlevel1Item,NULL);	
				}
				SortedStringList::iterator iter2 = level2Name.begin();
				for (; iter2 != level2Name.end(); ++iter2)
				{
					HTREEITEM hlevel2Item = m_treeCtrl.InsertItem(*iter2,hlevel1Item,TVI_LAST);
	
					SortedStringList level3Name;
					CString strLevel3Name = strLevel2Name + CString("-") + CString(*iter2);
					entryExitNameList.GetLevel4StringList(*iter,*iter1,*iter2,level3Name);
					if ((int)level3Name.size() == 0 && m_mapEntryExit.find(ALTObjectID(strLevel3Name))!=m_mapEntryExit.end())
					{		
						m_treeCtrl.SetItemData(hlevel2Item,(DWORD_PTR)m_mapEntryExit[ALTObjectID(strLevel3Name)]);			
					}else
					{
						m_treeCtrl.SetItemData(hlevel2Item,NULL);	
					}
					SortedStringList::iterator iter3 = level3Name.begin();
					for (; iter3 != level3Name.end(); ++iter3)
					{
						CString strLevel4Name = strLevel3Name + CString("-") + CString(*iter3);
						HTREEITEM hlevel3Item = m_treeCtrl.InsertItem(*iter3,hlevel2Item,TVI_LAST);
						if(m_mapEntryExit.find(ALTObjectID(strLevel4Name))!=m_mapEntryExit.end())
						{		
							m_treeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)m_mapEntryExit[ALTObjectID(strLevel4Name)]);			
						}else
						{
							m_treeCtrl.SetItemData(hlevel3Item,NULL);	
						}
						//m_treeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)m_pSeatDataSet->GetSeatByIDName(ALTObjectID(strLevel4Name)));
					}
					m_treeCtrl.Expand(hlevel2Item,TVE_EXPAND);
				}
				m_treeCtrl.Expand(hlevel1Item,TVE_EXPAND);
			}
			m_treeCtrl.Expand(hItem,TVE_EXPAND);
		}
	
}

void CDlgEntryExitNodeSelect::OnTvnSelchangedTreeBusstationselect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM curItem=m_treeCtrl.GetSelectedItem();
	m_pEntryExitNode=(LandsideExternalNode *)m_treeCtrl.GetItemData(curItem);
	if (m_pEntryExitNode==NULL)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}else
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	}
}
