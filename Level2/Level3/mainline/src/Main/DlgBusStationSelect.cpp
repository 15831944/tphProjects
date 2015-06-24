// DlgBusStationSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgBusStationSelect.h"
#include ".\dlgbusstationselect.h"


// CDlgBusStationSelect dialog

IMPLEMENT_DYNAMIC(CDlgBusStationSelect, CDialog)
CDlgBusStationSelect::CDlgBusStationSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBusStationSelect::IDD, pParent)
{
}
CDlgBusStationSelect::CDlgBusStationSelect(LandsideFacilityLayoutObjectList* objlist,CWnd* pParent /*=NULL*/)
: CDialog(CDlgBusStationSelect::IDD, pParent)
{
	m_pObjlist=objlist;
	m_pSelectBusStation=NULL;
}
CDlgBusStationSelect::~CDlgBusStationSelect()
{
}

void CDlgBusStationSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BUSSTATIONSELECT, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgBusStationSelect, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BUSSTATIONSELECT, OnTvnSelchangedTreeBusstationselect)
END_MESSAGE_MAP()


// CDlgBusStationSelect message handlers

BOOL CDlgBusStationSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBusStationSelect::LoadTree()
{
	ALTObjectIDList busStationNameList;
	m_mapBusStation.clear();
	m_treeCtrl.DeleteAllItems();
	for(int i=0;i<m_pObjlist->getCount();i++)
	{
		LandsideFacilityLayoutObject* pObj = m_pObjlist->getObject(i);
		if(pObj->GetType() == ALT_LBUSSTATION)
		{
			busStationNameList.push_back(pObj->getName());
			m_mapBusStation.insert(BusStationMap::value_type(pObj->getName(),(LandsideBusStation *)pObj));

		}
	}

	SortedStringList levelName;
	busStationNameList.GetLevel1StringList(levelName);
	SortedStringList::iterator iter = levelName.begin();
	for (; iter != levelName.end(); ++iter)
	{
		HTREEITEM hItem = m_treeCtrl.InsertItem(*iter,TVI_ROOT);

		SortedStringList level1Name;
		CString strLevel1Name = *iter;
		busStationNameList.GetLevel2StringList(*iter,level1Name);
		if ((int)level1Name.size() == 0 && m_mapBusStation.find(ALTObjectID(strLevel1Name))!=m_mapBusStation.end())
		{		
			m_treeCtrl.SetItemData(hItem,(DWORD_PTR)m_mapBusStation[ALTObjectID(strLevel1Name)]);			
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
			busStationNameList.GetLevel3StringList(*iter,*iter1,level2Name);
			if ((int)level2Name.size() == 0 && m_mapBusStation.find(ALTObjectID(strLevel2Name))!=m_mapBusStation.end())
			{		
				m_treeCtrl.SetItemData(hlevel1Item,(DWORD_PTR)m_mapBusStation[ALTObjectID(strLevel2Name)]);			
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
				busStationNameList.GetLevel4StringList(*iter,*iter1,*iter2,level3Name);
				if ((int)level3Name.size() == 0 && m_mapBusStation.find(ALTObjectID(strLevel3Name))!=m_mapBusStation.end())
				{		
					m_treeCtrl.SetItemData(hlevel2Item,(DWORD_PTR)m_mapBusStation[ALTObjectID(strLevel3Name)]);			
				}else
				{
					m_treeCtrl.SetItemData(hlevel2Item,NULL);	
				}
				SortedStringList::iterator iter3 = level3Name.begin();
				for (; iter3 != level3Name.end(); ++iter3)
				{
					CString strLevel4Name = strLevel3Name + CString("-") + CString(*iter3);
					HTREEITEM hlevel3Item = m_treeCtrl.InsertItem(*iter3,hlevel2Item,TVI_LAST);
					if(m_mapBusStation.find(ALTObjectID(strLevel4Name))!=m_mapBusStation.end())
					{		
						m_treeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)m_mapBusStation[ALTObjectID(strLevel4Name)]);			
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

void CDlgBusStationSelect::OnTvnSelchangedTreeBusstationselect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM curItem=m_treeCtrl.GetSelectedItem();
	m_pSelectBusStation=(LandsideBusStation *)m_treeCtrl.GetItemData(curItem);
	if (m_pSelectBusStation==NULL)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}else
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	}
}
