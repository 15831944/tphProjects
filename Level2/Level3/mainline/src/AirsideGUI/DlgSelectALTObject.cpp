// DlgSelectALTObject.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "DlgSelectALTObject.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"

CDlgSelectALTObject::CDlgSelectALTObject(int nProjID, ALTOBJECT_TYPE objectType, CWnd* pParent /*=NULL*/)
 : CDialog(CDlgSelectALTObject::IDD, pParent)
 , m_nProjectID(nProjID)
 , m_objectType(objectType)
{
}

CDlgSelectALTObject::~CDlgSelectALTObject()
{
}

void CDlgSelectALTObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndALTObjectTree);
}
 

BEGIN_MESSAGE_MAP(CDlgSelectALTObject, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelChangedTree)
END_MESSAGE_MAP()

BOOL CDlgSelectALTObject::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndALTObjectTree.InsertItem(airport.getName());
		InitTreeContentsForAirport(hAirport, *iterAirportID);
	}

	return TRUE;
}

void CDlgSelectALTObject::InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID)
{
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(m_objectType, nAirportID, vObject);

	int nObjectCount = vObject.size();
	for (int i = 0; i < nObjectCount; ++i)
	{
		ALTObject& altObject = vObject.at(i);
		ALTObjectID objName;
		altObject.getObjName(objName);

		HTREEITEM hParentItem = hAirport;
		for (int nLevel = 0; nLevel < OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (objName.m_val[nLevel].empty())
				break;

			HTREEITEM hNewItem = FindTreeChildItemByName(objName.m_val[nLevel].c_str(), hParentItem);
			if (hNewItem == NULL)
			{
				hNewItem = m_wndALTObjectTree.InsertItem(objName.m_val[nLevel].c_str(), hParentItem);
				m_wndALTObjectTree.Expand(hParentItem, TVE_EXPAND);
			}

			hParentItem = hNewItem;
		}
	}
}
 
HTREEITEM CDlgSelectALTObject::FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem)
{
	HTREEITEM hChildItem = m_wndALTObjectTree.GetChildItem(hParentItem);
	while (hChildItem != NULL)
	{
		if (m_wndALTObjectTree.GetItemText(hChildItem).CompareNoCase(lpszName) == 0)
			return hChildItem;

		hChildItem = m_wndALTObjectTree.GetNextSiblingItem(hChildItem);
	}

	return NULL;
}

int CDlgSelectALTObject::GetTreeItemDepthIndex(HTREEITEM hTreeItem)
{
	int nDepthIndex = 0;
	while (hTreeItem = m_wndALTObjectTree.GetParentItem(hTreeItem))
		++nDepthIndex;

	return nDepthIndex;
}

void CDlgSelectALTObject::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	for (int i  = 0; i < OBJECT_STRING_LEVEL; ++i)
		m_selectedALTObjectID.m_val[i].clear();

	BOOL bEnableOK = FALSE;
	int nItemDepthIndex = GetTreeItemDepthIndex(hSelItem);
	if (hSelItem != NULL && nItemDepthIndex != 0)
	{
		bEnableOK = TRUE;

		while (nItemDepthIndex > 0)
		{
			m_selectedALTObjectID.m_val[nItemDepthIndex - 1] = m_wndALTObjectTree.GetItemText(hSelItem);
			hSelItem = m_wndALTObjectTree.GetParentItem(hSelItem);
			--nItemDepthIndex;
		}
	}
		
	GetDlgItem(IDOK)->EnableWindow(bEnableOK);
	*pResult = 0;
}