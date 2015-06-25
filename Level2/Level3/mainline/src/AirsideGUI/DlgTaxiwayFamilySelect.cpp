//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "DlgTaxiwayFamilySelect.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
// CDlgTaxiwayFamilySelect dialog

IMPLEMENT_DYNAMIC(CDlgTaxiwayFamilySelect, CDialog)
CDlgTaxiwayFamilySelect::CDlgTaxiwayFamilySelect(int nProjID, ITaxiwayFamilyFilter* pTaxiwayFamilyFilter/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_TAXIWAYFAMILY, pParent)
	, m_nProjID(nProjID)
	, m_pTaxiwayFamilyFilter(pTaxiwayFamilyFilter)
{
}

CDlgTaxiwayFamilySelect::~CDlgTaxiwayFamilySelect()
{
}


LPCTSTR CDlgTaxiwayFamilySelect::m_sAllFamilyString = _T("All Taxiway");

void CDlgTaxiwayFamilySelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAXIWAYFAMILY, m_TreeTaxiwayFamily);
}


BEGIN_MESSAGE_MAP(CDlgTaxiwayFamilySelect, CDialog)
END_MESSAGE_MAP()


// CDlgTaxiwayFamilySelect message handlers
void CDlgTaxiwayFamilySelect::LoadTree()
{
	HTREEITEM hTaxiwayFamily = m_TreeTaxiwayFamily.InsertItem(_T("Taxiway Family:"));
	m_TreeTaxiwayFamily.InsertItem(m_sAllFamilyString,hTaxiwayFamily);
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		//HTREEITEM hAirport = m_TreeTaxiwayFamily.InsertItem(airport.getName(),hTaxiwayFamily);

		//m_TreeTaxiwayFamily.SetItemData(hAirport, *iterAirportID);
		ALTObjectList vTaxiways;
		ALTAirport::GetTaxiwayList(*iterAirportID,vTaxiways);

		for(int i = 0;i< (int)vTaxiways.size(); ++i)
		{
			AddObjectToTree(hTaxiwayFamily,vTaxiways.at(i).get());
		}

	}
	m_TreeTaxiwayFamily.Expand(hTaxiwayFamily, TVE_EXPAND);
	//----------------------------------------------------------
}
void CDlgTaxiwayFamilySelect::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_TreeTaxiwayFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeTaxiwayFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeTaxiwayFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeTaxiwayFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeTaxiwayFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//			m_TreeTaxiwayFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgTaxiwayFamilySelect::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeTaxiwayFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeTaxiwayFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeTaxiwayFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

BOOL CDlgTaxiwayFamilySelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTaxiwayFamilySelect::OnOK()
{
	// TODO: Add your control notification handler code here
	if(TaxiwayFamilyInTree())
	{
		// check the filter
		CString strError;
		if (m_pTaxiwayFamilyFilter && FALSE == m_pTaxiwayFamilyFilter->IsAllowed(m_nSelTaxiwayFamilyID, strError))
		{
			MessageBox(strError);
			return;
		}

	HTREEITEM hSelItem = m_TreeTaxiwayFamily.GetSelectedItem();

	if (m_TreeTaxiwayFamily.GetChildItem(hSelItem) != NULL)
		m_bIsSingleTaxiwaySel = false;
	else
		m_bIsSingleTaxiwaySel = true;

		CDialog::OnOK();
	}
}

bool CDlgTaxiwayFamilySelect::TaxiwayFamilyInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_TreeTaxiwayFamily.GetSelectedItem();
	if(m_TreeTaxiwayFamily.GetItemText(hSelItem) == m_sAllFamilyString)
	{
		m_nSelTaxiwayFamilyID = -1;
		m_strTaxiwayFamily = m_sAllFamilyString;
		return 1;
	}
	HTREEITEM hParentItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);

	if (!hParentItem)
	{
		MessageBox("Please select a taxiway family!", NULL, MB_OK);
		return 0;
	}

	ALTObjectID objName;

	switch(GetCurTaxiwayGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeTaxiwayFamily.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			m_strTaxiwayFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			m_strTaxiwayFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			m_strTaxiwayFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(2) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			//hSelItem = m_TreeTaxiwayFamily.GetParentItem(hSelItem);
			//CString strAirport = m_TreeTaxiwayFamily.GetItemText(hSelItem);
			m_strTaxiwayFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			m_strTaxiwayFamily = "";
			MessageBox("Please select a taxiway family!", NULL, MB_OK);
			return 0;
		}		
		break;
	}

	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_TAXIWAY);
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nSelTaxiwayFamilyID = altObjGroup.InsertData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	return 1;
}

int CDlgTaxiwayFamilySelect::GetCurTaxiwayGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeTaxiwayFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeTaxiwayFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeTaxiwayFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeTaxiwayFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeTaxiwayFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

