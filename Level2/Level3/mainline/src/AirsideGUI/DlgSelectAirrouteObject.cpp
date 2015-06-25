#include "stdafx.h"
#include "Resource.h"
#include "DlgSelectAirrouteObject.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"


CDlgSelectAirrouteObject::CDlgSelectAirrouteObject(int nProjID, CAirRoute::RouteType emType, CWnd* pParent /*=NULL*/)
: CDialog(CDlgSelectAirrouteObject::IDD, pParent)
, m_nProjectID(nProjID)
, m_emType(emType)
{
}

CDlgSelectAirrouteObject::~CDlgSelectAirrouteObject()
{
}

void CDlgSelectAirrouteObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndALTObjectTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectAirrouteObject, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelChangedTree)
END_MESSAGE_MAP()

BOOL CDlgSelectAirrouteObject::OnInitDialog()
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

void CDlgSelectAirrouteObject::InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID)
{
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjectID);
	int nObjectCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nObjectCount; ++i)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		if (pAirRoute->getRouteType() != m_emType)
			continue;

		ALTObjectID objName;
		objName.FromString(pAirRoute->getName());

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

HTREEITEM CDlgSelectAirrouteObject::FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem)
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

int CDlgSelectAirrouteObject::GetTreeItemDepthIndex(HTREEITEM hTreeItem)
{
	int nDepthIndex = 0;
	while (hTreeItem = m_wndALTObjectTree.GetParentItem(hTreeItem))
		++nDepthIndex;

	return nDepthIndex;
}

void CDlgSelectAirrouteObject::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
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