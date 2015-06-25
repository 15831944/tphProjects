// DlgDeicePadFamily.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgDeicePadFamily.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/stand.h"

// CDlgDeicePadFamily dialog

IMPLEMENT_DYNAMIC(CDlgDeicePadFamily, CDialog)
CDlgDeicePadFamily::CDlgDeicePadFamily(int nProjID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDeicePadFamily::IDD, pParent)
	,m_nProjID(nProjID)
{
}

CDlgDeicePadFamily::~CDlgDeicePadFamily()
{
}

void CDlgDeicePadFamily::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeStandFamily);
}


BEGIN_MESSAGE_MAP(CDlgDeicePadFamily, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreestandfamily)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDeicePadFamily message handlers
// CDlgDeicePadFamily message handlers
void CDlgDeicePadFamily::LoadTree()
{
	HTREEITEM hStandFamily = m_TreeStandFamily.InsertItem(_T("Deice Pad Family:"));
	m_TreeStandFamily.InsertItem(_T("All Deice Pad"),hStandFamily);
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		//HTREEITEM hAirport = m_TreeStandFamily.InsertItem(airport.getName(),hStandFamily);

		//m_TreeStandFamily.SetItemData(hAirport, *iterAirportID);
		ALTObjectList vStands;
		ALTAirport::GetDeicePadList(*iterAirportID,vStands);

		for(int i = 0;i< (int)vStands.size(); ++i)
		{
			ALTObject* pStand =(ALTObject*) vStands.at(i).get();
			AddObjectToTree(hStandFamily,pStand);
		}

		//std::vector<int> vStands;
		//ALTAirport::GetStandsIDs(*iterAirportID, vStands);
		//for (std::vector<int>::iterator iterStandID = vStands.begin(); iterStandID != vStands.end(); ++iterStandID)
		//{
		//	Stand stand;
		//	stand.ReadObject(*iterStandID);

		//	AddObjectToTree(hStandFamily, &stand);	
		//}

		//m_TreeStandFamily.Expand(hAirport, TVE_EXPAND);
	}
	m_TreeStandFamily.Expand(hStandFamily, TVE_EXPAND);
	//----------------------------------------------------------
}
void CDlgDeicePadFamily::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_TreeStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//			m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgDeicePadFamily::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeStandFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeStandFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeStandFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

BOOL CDlgDeicePadFamily::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDeicePadFamily::OnTvnSelchangedTreestandfamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}
void CDlgDeicePadFamily::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(StandFamilyInTree())
		OnOK();
}

bool CDlgDeicePadFamily::StandFamilyInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_TreeStandFamily.GetSelectedItem();
	if(m_TreeStandFamily.GetItemText(hSelItem) == "All Deice Pad")
	{
		m_nSelStandFamilyID = -1;
		return 1;
	}
	HTREEITEM hParentItem = m_TreeStandFamily.GetParentItem(hSelItem);

	if (!hParentItem)
	{
		MessageBox("Please select a Deice Pad Family!", "Error", MB_OK);
		return 0;
	}

	ALTObjectID objName;

	switch(GetCurStandGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeStandFamily.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			m_strStandFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeStandFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			m_strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeStandFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			m_strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(2) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeStandFamily.GetItemText(hSelItem);
			hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeStandFamily.GetItemText(hSelItem);
			//hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			//CString strAirport = m_TreeStandFamily.GetItemText(hSelItem);
			m_strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			m_strStandFamily = "";
			MessageBox("Please select a Deice Pad Family!", "Error", MB_OK);
			return 0;
		}		
		break;
	}

	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_DEICEBAY);
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	return 1;
}

int CDlgDeicePadFamily::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeStandFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}