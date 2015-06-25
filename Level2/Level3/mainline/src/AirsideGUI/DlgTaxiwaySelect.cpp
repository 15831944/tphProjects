//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "DlgTaxiwaySelect.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
// CDlgTaxiwaySelect dialog

IMPLEMENT_DYNAMIC(CDlgTaxiwaySelect, CDialog)
CDlgTaxiwaySelect::CDlgTaxiwaySelect(int nProjID, ITaxiwayFilter* pTaxiwayFilter/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_TAXIWAYFAMILY, pParent)
	, m_nProjID(nProjID)
	, m_pTaxiwayFilter(pTaxiwayFilter)
{
}

CDlgTaxiwaySelect::~CDlgTaxiwaySelect()
{
}



void CDlgTaxiwaySelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAXIWAYFAMILY, m_TreeTaxiway);
}


BEGIN_MESSAGE_MAP(CDlgTaxiwaySelect, CDialog)
END_MESSAGE_MAP()


// CDlgTaxiwaySelect message handlers
void CDlgTaxiwaySelect::LoadTree()
{
	HTREEITEM hTaxiwayRoot = m_TreeTaxiway.InsertItem(_T("Taxiway:"));
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		//HTREEITEM hAirport = m_TreeTaxiway.InsertItem(airport.getName(),hTaxiwayRoot);

		//m_TreeTaxiway.SetItemData(hAirport, *iterAirportID);
		ALTObjectList vTaxiways;
		ALTAirport::GetTaxiwayList(*iterAirportID,vTaxiways);

		for(int i = 0;i< (int)vTaxiways.size(); ++i)
		{
			AddObjectToTree(hTaxiwayRoot,vTaxiways.at(i).get());
		}

	}
	m_TreeTaxiway.Expand(hTaxiwayRoot, TVE_EXPAND);
	//----------------------------------------------------------
}
void CDlgTaxiwaySelect::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeTaxiway.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				m_TreeTaxiway.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeTaxiway.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				m_TreeTaxiway.SetItemData(hTreeItem, pObject->getID());
				m_TreeTaxiway.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeTaxiway.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			m_TreeTaxiway.SetItemData(hTreeItem, pObject->getID());
			m_TreeTaxiway.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgTaxiwaySelect::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeTaxiway.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeTaxiway.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeTaxiway.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

BOOL CDlgTaxiwaySelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(_T("Taxiway"));
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTaxiwaySelect::OnOK()
{
	// TODO: Add your control notification handler code here
	if(TaxiwayInTree())
	{
		// check the filter
		CString strError;
		if (m_pTaxiwayFilter && FALSE == m_pTaxiwayFilter->IsAllowed(m_nSelTaxiwayID, strError))
		{
			MessageBox(strError);
			return;
		}

		CDialog::OnOK();
	}
}

bool CDlgTaxiwaySelect::TaxiwayInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_TreeTaxiway.GetSelectedItem();
	HTREEITEM hChildItem = m_TreeTaxiway.GetChildItem(hSelItem);
	if (!hSelItem || hChildItem)  // must selected and has no child
	{
		MessageBox(_T("Please select a taxiway!"), NULL, MB_OK);
		return 0;
	}

	m_nSelTaxiwayID = m_TreeTaxiway.GetItemData(hSelItem);
	ASSERT(m_nSelTaxiwayID);

	ALTObject obj;
	obj.ReadObject(m_nSelTaxiwayID);
	m_strTaxiway = obj.GetObjectName().GetIDString();
	return 1;
}

int CDlgTaxiwaySelect::GetCurTaxiwayGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeTaxiway.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeTaxiway.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeTaxiway.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeTaxiway.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeTaxiway.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

