// .cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "DlgALTObjectGroup.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
// CDlgALTObjectGroup dialog

IMPLEMENT_DYNAMIC(CDlgALTObjectGroup, CDialog)
CDlgALTObjectGroup::CDlgALTObjectGroup(int nProjID, ALTOBJECT_TYPE altobjType, IALTObjectGroupFilter* pALTObjectGroupFilter/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_STANDFAMILY, pParent)
	, m_nProjID(nProjID)
	, m_altobjType(altobjType)
	, m_pALTObjectGroupFilter(pALTObjectGroupFilter)
{
	ASSERT(m_altobjType>ALT_UNKNOWN && m_altobjType<ALT_ATYPE_END);
	m_strALTObjectTypeName = ALTObject::GetTypeName(m_altobjType);
}

CDlgALTObjectGroup::~CDlgALTObjectGroup()
{
}

void CDlgALTObjectGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WAYPOINTANDSTANDFAMILY, m_treeALTObjectGroup);
}


BEGIN_MESSAGE_MAP(CDlgALTObjectGroup, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgALTObjectGroup message handlers
void CDlgALTObjectGroup::LoadTree()
{
	HTREEITEM hFamily = m_treeALTObjectGroup.InsertItem(m_strALTObjectTypeName + _T(" Family:"));
	m_treeALTObjectGroup.InsertItem(_T("All ") + m_strALTObjectTypeName,hFamily);
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		//HTREEITEM hAirport = m_treeALTObjectGroup.InsertItem(airport.getName(),hFamily);

		//m_treeALTObjectGroup.SetItemData(hAirport, *iterAirportID);
		std::vector<ALTObject> vALTObjects;
		ALTObject::GetObjectList(m_altobjType, *iterAirportID, vALTObjects);

		for(size_t i = 0;i< vALTObjects.size(); ++i)
		{
			AddObjectToTree(hFamily,&vALTObjects[i]);
		}

	}
	m_treeALTObjectGroup.Expand(hFamily, TVE_EXPAND);
	//----------------------------------------------------------
}
void CDlgALTObjectGroup::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_treeALTObjectGroup.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_treeALTObjectGroup.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_treeALTObjectGroup.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_treeALTObjectGroup.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_treeALTObjectGroup.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//			m_treeALTObjectGroup.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgALTObjectGroup::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_treeALTObjectGroup.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_treeALTObjectGroup.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_treeALTObjectGroup.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

BOOL CDlgALTObjectGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(_T("Select ") + m_strALTObjectTypeName + _T(" Familiy"));
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgALTObjectGroup::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(ALTObjectGroupInTree())
	{
		// check the filter
		CString strError;
		if (m_pALTObjectGroupFilter && FALSE == m_pALTObjectGroupFilter->IsAllowed(m_nSelALTObjectGroupID, strError))
		{
			MessageBox(strError);
			return;
		}

		OnOK();
	}
}

bool CDlgALTObjectGroup::ALTObjectGroupInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_treeALTObjectGroup.GetSelectedItem();
	if(m_treeALTObjectGroup.GetItemText(hSelItem) == _T("All ") + m_strALTObjectTypeName)
	{
		m_nSelALTObjectGroupID = -1;
		m_strSelALTObjectGroupName = _T("All ") + m_strALTObjectTypeName;
		return true;
	}
	HTREEITEM hParentItem = m_treeALTObjectGroup.GetParentItem(hSelItem);

	if (!hParentItem)
	{
		MessageBox("Please select a " + m_strALTObjectTypeName +" Family!", NULL, MB_OK);
		return false;
	}

	ALTObjectID objName;

	switch(GetCurALTObjectGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = m_treeALTObjectGroup.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			m_strSelALTObjectGroupName.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(0) = m_treeALTObjectGroup.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			m_strSelALTObjectGroupName.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(1) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(0) = m_treeALTObjectGroup.GetItemText(hSelItem);
			objName.at(3) = "";
			m_strSelALTObjectGroupName.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(2) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(1) = m_treeALTObjectGroup.GetItemText(hSelItem);
			hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			objName.at(0) = m_treeALTObjectGroup.GetItemText(hSelItem);
			//hSelItem = m_treeALTObjectGroup.GetParentItem(hSelItem);
			//CString strAirport = m_treeALTObjectGroup.GetItemText(hSelItem);
			m_strSelALTObjectGroupName.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			m_strSelALTObjectGroupName = "";
			MessageBox("Please select a " + m_strALTObjectTypeName +" Family!", NULL, MB_OK);
			return false;
		}		
		break;
	}

	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(m_altobjType);
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nSelALTObjectGroupID = altObjGroup.InsertData(m_nProjID);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	return 1;
}

int CDlgALTObjectGroup::GetCurALTObjectGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_treeALTObjectGroup.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_treeALTObjectGroup.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_treeALTObjectGroup.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_treeALTObjectGroup.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_treeALTObjectGroup.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

