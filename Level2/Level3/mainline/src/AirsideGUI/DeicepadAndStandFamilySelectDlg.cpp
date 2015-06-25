#include "StdAfx.h"
#include "resource.h"
#include ".\deicepadandstandfamilyselectdlg.h"

#include "InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside/stand.h"
#include "InputAirside\ALTObjectGroup.h"
#include <iostream>

IMPLEMENT_DYNAMIC(CDeicepadAndStandFamilySelectDlg,CDialog )
CDeicepadAndStandFamilySelectDlg::CDeicepadAndStandFamilySelectDlg(int nProjID, CWnd* pParent)
:CDialog(CDeicepadAndStandFamilySelectDlg::IDD, pParent)
, m_nProjID(nProjID)
, m_bAllDeice(FALSE)
, m_bAllStand(FALSE)
{

}

CDeicepadAndStandFamilySelectDlg::~CDeicepadAndStandFamilySelectDlg(void)
{
}
BEGIN_MESSAGE_MAP(CDeicepadAndStandFamilySelectDlg, CDialog)	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RUNWAYEXIT, OnTvnSelchangedTreeDeicePads)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDFAMILY, OnTvnSelchangedTreeStandFamily)
	ON_NOTIFY(NM_KILLFOCUS,IDC_TREE_STANDFAMILY,OnNmKillfocusTreeStandFamily)
	ON_NOTIFY(NM_KILLFOCUS,IDC_TREE_RUNWAYEXIT,OnNmKillfocusTreeDeicePads)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDeicepadAndStandFamilySelectDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEICEPADS, m_TreeCtrlDeicepads);
	DDX_Control(pDX, IDC_TREE_STANDFAMILY, m_TreeCtrlStandFamily);

}

BOOL CDeicepadAndStandFamilySelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

//	m_TreeCtrlDeicepads.EnableMultiSelect();
//	m_TreeCtrlStandFamily.EnableMultiSelect();

	m_TreeCtrlDeicepads.ModifyStyle(0,TVS_SHOWSELALWAYS);
	m_TreeCtrlStandFamily.ModifyStyle(0,TVS_SHOWSELALWAYS);

	SetDeicePadTreeContent();
	SetStandFamilyTreeContent();

	return TRUE;
}

void CDeicepadAndStandFamilySelectDlg::SetDeicePadTreeContent( void )
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_TreeCtrlDeicepads.InsertItem(airport.getName());
		m_TreeCtrlDeicepads.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlDeicepads.InsertItem("All", hAirport);
		m_TreeCtrlDeicepads.SetItemData(hAll, -2);

		ALTObjectList vDeicepads;
		ALTAirport::GetDeicePadList(*iterAirportID,vDeicepads);

		for(int i = 0;i< (int)vDeicepads.size(); ++i)
		{
			ALTObject* pDeice = vDeicepads.at(i).get();
			AddObjectToDeiceTree(hAirport,pDeice);
		}

		m_TreeCtrlDeicepads.Expand(hAirport, TVE_EXPAND);
	}
}

void CDeicepadAndStandFamilySelectDlg::SetStandFamilyTreeContent( void )
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_TreeCtrlStandFamily.InsertItem(airport.getName());
		m_TreeCtrlStandFamily.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlStandFamily.InsertItem("All", hAirport);
		m_TreeCtrlStandFamily.SetItemData(hAll, -2);

		ALTObjectList vStands;
		ALTAirport::GetStandList(*iterAirportID,vStands);

		for(int i = 0;i< (int)vStands.size(); ++i)
		{
			Stand * pStand =(Stand*) vStands.at(i).get();
			AddObjectToStandTree(hAirport,pStand);
		}

		m_TreeCtrlStandFamily.Expand(hAirport, TVE_EXPAND);
	}
}

void CDeicepadAndStandFamilySelectDlg::AddObjectToStandTree( HTREEITEM hObjRoot,ALTObject* pObject )
{
	ASSERT(hObjRoot);

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");

	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjStandNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}

HTREEITEM CDeicepadAndStandFamilySelectDlg::FindObjStandNode( HTREEITEM hParentItem,const CString& strNodeText )
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrlStandFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrlStandFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrlStandFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CDeicepadAndStandFamilySelectDlg::AddObjectToDeiceTree( HTREEITEM hObjRoot,ALTObject* pObject )
{
	ASSERT(hObjRoot);

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");

	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjDeiceNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}

HTREEITEM CDeicepadAndStandFamilySelectDlg::FindObjDeiceNode( HTREEITEM hParentItem,const CString& strNodeText )
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrlDeicepads.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrlDeicepads.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrlDeicepads.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}


void CDeicepadAndStandFamilySelectDlg::OnNmKillfocusTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	*pResult = 0;
}

void CDeicepadAndStandFamilySelectDlg::OnNmKillfocusTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	*pResult = 0;
}

void CDeicepadAndStandFamilySelectDlg::OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlStandFamily.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlStandFamily.GetItemData(hItem);
		if (nId == -2)
			m_bAllStand = TRUE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}


void CDeicepadAndStandFamilySelectDlg::OnTvnSelchangedTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlDeicepads.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlDeicepads.GetItemData(hItem);
		if (nId == -2)
			m_bAllDeice = TRUE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}


void CDeicepadAndStandFamilySelectDlg::GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily)
{
	switch(GetCurStandGroupNamePos(hItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);

			strStandFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
			objName.at(2) = "";
			objName.at(3) = "";

			strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{
			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
			objName.at(3) = "";

			strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;

	case 3:
		{
			objName.at(3) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);

			strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		strStandFamily = "";
		break;
	}
}

int CDeicepadAndStandFamilySelectDlg::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeCtrlStandFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

int CDeicepadAndStandFamilySelectDlg::GetCurDeiceGroupNamePos( HTREEITEM hTreeItem )
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeCtrlDeicepads.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

void CDeicepadAndStandFamilySelectDlg::GetDeiceAltID( HTREEITEM hItem, ALTObjectID& objName, CString& strDeicePads )
{
	switch(GetCurDeiceGroupNamePos(hItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);

			strDeicePads.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);
			objName.at(2) = "";
			objName.at(3) = "";

			strDeicePads.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{
			objName.at(2) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);
			objName.at(3) = "";

			strDeicePads.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;

	case 3:
		{
			objName.at(3) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(2) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);

			strDeicePads.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		strDeicePads = "";
		break;
	}
}

void CDeicepadAndStandFamilySelectDlg::OnBnClickedOk()
{
	m_strDeices = "";
	m_strStandGroups = "";
	m_vDeiceSelID.clear();
	m_vStandSelID.clear();

	if(!m_bAllDeice)
	{
		int nSelCount = 1;//m_TreeCtrlDeicepads.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlDeicepads.GetSelectedItem();

		for (int i = 0; i < nSelCount;++i) 
		{		
			if (hSelItem != NULL )
			{	
				ALTObjectGroup altObjGroup;
				ALTObjectID objName;
				CString strName;

				GetDeiceAltID(hSelItem,objName,strName);

				altObjGroup.setType(ALT_DEICEBAY);
				altObjGroup.setName(objName);
				int nSelDeiceFamilyID = altObjGroup.InsertData(m_nProjID);

				m_vDeiceSelID.push_back(nSelDeiceFamilyID);

				if (m_strDeices =="")
				{
					m_strDeices = strName;
				}
				else
				{
					m_strDeices += ", ";
					m_strDeices += strName;
				}
			}
			//hSelItem = m_TreeCtrlDeicepads.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strDeices = "All";

	if(!m_bAllStand)
	{
		int nSelCount = 1;//m_TreeCtrlStandFamily.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlStandFamily.GetSelectedItem();

		for (int i = 0; i < nSelCount;++i) 
		{		
			if (hSelItem != NULL )
			{	
				ALTObjectGroup altObjGroup;
				ALTObjectID objName;
				CString strName;

				GetStandFamilyAltID(hSelItem,objName,strName);

				altObjGroup.setType(ALT_STAND);
				altObjGroup.setName(objName);
				int nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);

				m_vStandSelID.push_back(nSelStandFamilyID);

				if (m_strStandGroups =="")
				{
					m_strStandGroups = strName;
				}
				else
				{
					m_strStandGroups += ", ";
					m_strStandGroups += strName;
				}
			}
			//hSelItem = m_TreeCtrlStandFamily.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strStandGroups = "All";

	if (!m_strStandGroups.Compare(""))
	{
		MessageBox("Please select stand family!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	if (!m_strDeices.Compare(""))
	{
		MessageBox("Please select deicepad family!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}
	OnOK();
}
