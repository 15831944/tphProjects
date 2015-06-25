// DlgSelectDeicePads.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectDeicePads.h"
#include "resource.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/DeicePad.h"

// CDlgSelectDeicePads dialog

IMPLEMENT_DYNAMIC(CDlgSelectDeicePads, CDialog)
CDlgSelectDeicePads::CDlgSelectDeicePads(int nProjID,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SELECTDEICEPADS, pParent)
	,m_nProjID(nProjID)
{
}

CDlgSelectDeicePads::~CDlgSelectDeicePads()
{
}

void CDlgSelectDeicePads::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEICEPADS, m_TreeDeicepadsFamily);
}


BEGIN_MESSAGE_MAP(CDlgSelectDeicePads, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDFAMILY, OnTvnSelchangedTreeDeicepadsfamily)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectDeicePads message handlers
BOOL CDlgSelectDeicePads::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectDeicePads::LoadTree()
{
	HTREEITEM hDeicepadsFamily = m_TreeDeicepadsFamily.InsertItem(_T("Deicepads Family:"));
	m_TreeDeicepadsFamily.InsertItem(_T("All Deicepads"),hDeicepadsFamily);
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
	
		ALTObjectList vDeicepads;
		ALTAirport::GetDeicePadList(*iterAirportID,vDeicepads);

		for(int i = 0;i< (int)vDeicepads.size(); ++i)
		{
			DeicePad * pDeicepad =(DeicePad*) vDeicepads.at(i).get();
			AddObjectToTree(hDeicepadsFamily,pDeicepad);
		}
	}
	m_TreeDeicepadsFamily.Expand(hDeicepadsFamily, TVE_EXPAND);
}

void CDlgSelectDeicePads::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_TreeDeicepadsFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeDeicepadsFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeDeicepadsFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			break;
		}
	}
}

HTREEITEM CDlgSelectDeicePads::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeDeicepadsFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeDeicepadsFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeDeicepadsFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CDlgSelectDeicePads::OnTvnSelchangedTreeDeicepadsfamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}

void CDlgSelectDeicePads::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(DeicepadsFamilyInTree())
		OnOK();
}

bool CDlgSelectDeicePads::DeicepadsFamilyInTree()
{
	HTREEITEM hSelItem = m_TreeDeicepadsFamily.GetSelectedItem();
	if(m_TreeDeicepadsFamily.GetItemText(hSelItem) == "All Deicepads")
	{
		m_nSelDeicepadsFamilyID = -1;
		m_strDeicepadsFamily = "All Deicepads";
		return 1;
	}
	HTREEITEM hParentItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);

	if (!hParentItem)
	{
		MessageBox("Please select a Deicepad Family!", "Error", MB_OK);
		return 0;
	}

	ALTObjectID objName;

	switch(GetCurDeicepadsGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeDeicepadsFamily.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			m_strDeicepadsFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			m_strDeicepadsFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			objName.at(3) = "";
			m_strDeicepadsFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(2) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(1) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			hSelItem = m_TreeDeicepadsFamily.GetParentItem(hSelItem);
			objName.at(0) = m_TreeDeicepadsFamily.GetItemText(hSelItem);
			m_strDeicepadsFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			m_strDeicepadsFamily = "";
			MessageBox("Please select a Stand Family!", "Error", MB_OK);
			return 0;
		}		
		break;
	}

	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_STAND);
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nSelDeicepadsFamilyID = altObjGroup.InsertData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	return 1;
}

int CDlgSelectDeicePads::GetCurDeicepadsGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeDeicepadsFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeDeicepadsFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeDeicepadsFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeDeicepadsFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeDeicepadsFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}