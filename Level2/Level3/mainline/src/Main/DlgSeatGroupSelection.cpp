// DlgSeatSelection.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatGroupSelection.h"
#include "../InputOnboard/Seat.h"


// CDlgSeatGroupSelection dialog

IMPLEMENT_DYNAMIC(CDlgSeatGroupSelection, CXTResizeDialog)
CDlgSeatGroupSelection::CDlgSeatGroupSelection(CSeatDataSet* pSeatDataSet,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSeatGroupSelection::IDD, pParent)
	,m_pSeatDataSet(pSeatDataSet)
	,m_pSeat(NULL)
{
	
}

CDlgSeatGroupSelection::~CDlgSeatGroupSelection()
{
}

void CDlgSeatGroupSelection::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSeatGroupSelection, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelchangedTree)
END_MESSAGE_MAP()


// CDlgSeatGroupSelection message handlers

BOOL CDlgSeatGroupSelection::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();	
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_EDIT_SEATGROUP,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
	SetResize(IDOK,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);	
	
	SetDlgItemText(IDC_EDIT_SEATGROUP,_T(""));

	InitIDNameList();
	InitTreeCtrl();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgSeatGroupSelection::InitTreeCtrl()
{
	SortedStringList levelName;
	m_ObjIDList.GetLevel1StringList(levelName);
	ALTObjectID *pSeatGroup=new ALTObjectID(_T(""));
	HTREEITEM hDefaultItem = m_wndTreeCtrl.InsertItem("ALL SEAT",TVI_ROOT);
	m_wndTreeCtrl.SetItemData(hDefaultItem,(DWORD_PTR)pSeatGroup);
	SortedStringList::iterator iter = levelName.begin();
	for (; iter != levelName.end(); ++iter)
	{
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(*iter,TVI_ROOT);

		SortedStringList level1Name;
		CString strLevel1Name = *iter;
		m_ObjIDList.GetLevel2StringList(*iter,level1Name);
// 		if ((int)level1Name.size() == 0)
// 		{
			pSeatGroup=new ALTObjectID(strLevel1Name);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pSeatGroup);
// 		}
		SortedStringList::iterator iter1 = level1Name.begin();
		for (; iter1 != level1Name.end(); ++iter1)
		{
			HTREEITEM hlevel1Item = m_wndTreeCtrl.InsertItem(*iter1,hItem,TVI_LAST);

			SortedStringList level2Name;
			CString strLevel2Name = strLevel1Name + CString("-") + CString(*iter1);
			m_ObjIDList.GetLevel3StringList(*iter,*iter1,level2Name);
// 			if ((int)level2Name.size() == 0)
// 			{
				pSeatGroup=new ALTObjectID(strLevel2Name);
				m_wndTreeCtrl.SetItemData(hlevel1Item,(DWORD_PTR)pSeatGroup);
// 			}
			SortedStringList::iterator iter2 = level2Name.begin();
			for (; iter2 != level2Name.end(); ++iter2)
			{
				HTREEITEM hlevel2Item = m_wndTreeCtrl.InsertItem(*iter2,hlevel1Item,TVI_LAST);

				SortedStringList level3Name;
				CString strLevel3Name = strLevel2Name + CString("-") + CString(*iter2);
				m_ObjIDList.GetLevel4StringList(*iter,*iter1,*iter2,level3Name);
// 				if ((int)level3Name.size() == 0)
// 				{
					pSeatGroup=new ALTObjectID(strLevel3Name);
					m_wndTreeCtrl.SetItemData(hlevel2Item,(DWORD_PTR)pSeatGroup);
// 				}
				SortedStringList::iterator iter3 = level3Name.begin();
				for (; iter3 != level3Name.end(); ++iter3)
				{
					CString strLevel4Name = strLevel3Name + CString("-") + CString(*iter3);
					HTREEITEM hlevel3Item = m_wndTreeCtrl.InsertItem(*iter3,hlevel2Item,TVI_LAST);
					pSeatGroup=new ALTObjectID(strLevel4Name);
					m_wndTreeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)pSeatGroup);
				}
// 				m_wndTreeCtrl.Expand(hlevel2Item,TVE_EXPAND);
			}
// 			m_wndTreeCtrl.Expand(hlevel1Item,TVE_EXPAND);
		}		
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hDefaultItem,TVE_EXPAND);
	}
}
void CDlgSeatGroupSelection::InitIDNameList()
{
	for (int i = 0; i < m_pSeatDataSet->GetItemCount(); i++)
	{
		CSeat* pSeat = m_pSeatDataSet->GetItem(i);
		m_ObjIDList.push_back(pSeat->GetIDName());
	}
}
ALTObjectID CDlgSeatGroupSelection::getSeatGroup()
{
	return m_objID;
}

void CDlgSeatGroupSelection::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
// 	if (m_wndTreeCtrl.GetChildItem(hItem) == NULL)
// 	{
// 	m_pSeat = (CSeat*)m_wndTreeCtrl.GetItemData(hItem);
	m_objID=*((ALTObjectID *)m_wndTreeCtrl.GetItemData(hItem));
	SetDlgItemText(IDC_EDIT_SEATGROUP,m_objID.GetIDString());
	if (m_objID.GetIDString()==_T(""))
	{
		SetDlgItemText(IDC_EDIT_SEATGROUP,_T("ALL SEAT"));
	}
 	GetDlgItem(IDOK)->EnableWindow(TRUE);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDOK)->EnableWindow(FALSE);
// 		m_pSeat = NULL;
// 	}
	*pResult = 0;
}

CSeat* CDlgSeatGroupSelection::getSeat()
{
	return m_pSeat;
}