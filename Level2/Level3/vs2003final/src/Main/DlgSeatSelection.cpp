// DlgSeatSelection.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatSelection.h"
#include "../InputOnboard/Seat.h"


// CDlgSeatSelection dialog

IMPLEMENT_DYNAMIC(CDlgSeatSelection, CDialog)
CDlgSeatSelection::CDlgSeatSelection(CSeatDataSet* pSeatDataSet,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSeatSelection::IDD, pParent)
	,m_pSeatDataSet(pSeatDataSet)
	,m_pSeat(NULL)
{
	
}

CDlgSeatSelection::~CDlgSeatSelection()
{
}

void CDlgSeatSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSeatSelection, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelchangedTree)
END_MESSAGE_MAP()


// CDlgSeatSelection message handlers

BOOL CDlgSeatSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitIDNameList();
	InitTreeCtrl();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgSeatSelection::InitTreeCtrl()
{
	SortedStringList levelName;
	m_ObjIDList.GetLevel1StringList(levelName);
	SortedStringList::iterator iter = levelName.begin();
	for (; iter != levelName.end(); ++iter)
	{
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(*iter,TVI_ROOT);

		SortedStringList level1Name;
		CString strLeve1Name = *iter;
		m_ObjIDList.GetLevel2StringList(*iter,level1Name);
		if ((int)level1Name.size() == 0)
		{
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pSeatDataSet->GetSeatByIDName(ALTObjectID(strLeve1Name)));
		}
		SortedStringList::iterator iter1 = level1Name.begin();
		for (; iter1 != level1Name.end(); ++iter1)
		{
			HTREEITEM hlevel1Item = m_wndTreeCtrl.InsertItem(*iter1,hItem,TVI_LAST);

			SortedStringList level2Name;
			CString strLevel2Name = strLeve1Name + CString("-") + CString(*iter1);
			m_ObjIDList.GetLevel3StringList(*iter,*iter1,level2Name);
			if ((int)level2Name.size() == 0)
			{
				m_wndTreeCtrl.SetItemData(hlevel1Item,(DWORD_PTR)m_pSeatDataSet->GetSeatByIDName(ALTObjectID(strLevel2Name)));
			}
			SortedStringList::iterator iter2 = level2Name.begin();
			for (; iter2 != level2Name.end(); ++iter2)
			{
				HTREEITEM hlevel2Item = m_wndTreeCtrl.InsertItem(*iter2,hlevel1Item,TVI_LAST);

				SortedStringList level3Name;
				CString strLevel3Name = strLevel2Name + CString("-") + CString(*iter2);
				m_ObjIDList.GetLevel4StringList(*iter,*iter1,*iter2,level3Name);
				if ((int)level3Name.size() == 0)
				{
					m_wndTreeCtrl.SetItemData(hlevel2Item,(DWORD_PTR)m_pSeatDataSet->GetSeatByIDName(ALTObjectID(strLevel3Name)));
				}
				SortedStringList::iterator iter3 = level3Name.begin();
				for (; iter3 != level3Name.end(); ++iter3)
				{
					CString strLevel4Name = strLevel3Name + CString("-") + CString(*iter3);
					HTREEITEM hlevel3Item = m_wndTreeCtrl.InsertItem(*iter3,hlevel2Item,TVI_LAST);
					m_wndTreeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)m_pSeatDataSet->GetSeatByIDName(ALTObjectID(strLevel4Name)));
				}
				m_wndTreeCtrl.Expand(hlevel2Item,TVE_EXPAND);
			}
			m_wndTreeCtrl.Expand(hlevel1Item,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
}
void CDlgSeatSelection::InitIDNameList()
{
	for (int i = 0; i < m_pSeatDataSet->GetItemCount(); i++)
	{
		CSeat* pSeat = m_pSeatDataSet->GetItem(i);
		m_ObjIDList.push_back(pSeat->GetIDName());
	}
}

void CDlgSeatSelection::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (m_wndTreeCtrl.GetChildItem(hItem) == NULL)
	{
		m_pSeat = (CSeat*)m_wndTreeCtrl.GetItemData(hItem);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		m_pSeat = NULL;
	}
	*pResult = 0;
}

CSeat* CDlgSeatSelection::getSeat()
{
	return m_pSeat;
}