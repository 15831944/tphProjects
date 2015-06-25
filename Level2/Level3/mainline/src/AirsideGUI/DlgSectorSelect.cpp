// DlgSectorSelect.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "DlgSectorSelect.h"
#include ".\dlgsectorselect.h"


// DlgSectorSelect dialog

IMPLEMENT_DYNAMIC(DlgSectorSelect, CDialog)
DlgSectorSelect::DlgSectorSelect(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSectorSelect::IDD, pParent)
{
}
DlgSectorSelect::DlgSectorSelect(int projid): CDialog(DlgSectorSelect::IDD, NULL),sectorDoc(NULL)
{
      sectorDoc = new CSectorSelectDoc(projid) ;
}
DlgSectorSelect::~DlgSectorSelect()
{
}

void DlgSectorSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SECTOR, sector_tree);
}

BOOL DlgSectorSelect::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	loadTree() ;
	return true ;
}
void DlgSectorSelect::loadTree()
{
  if (sectorDoc == NULL)
      return ;
  CSectorSelectDoc::ITERATOR_SECTOR_CONST iter_beg = sectorDoc->getSectors().begin();
  CSectorSelectDoc::ITERATOR_SECTOR_CONST iter_end = sectorDoc->getSectors().end() ;
  HTREEITEM hroot = sector_tree.InsertItem(_T("Sector Name:"));
  for( ; iter_beg != iter_end ;++iter_beg)
	  AddObjectToTree(hroot,(ALTObject*)(&(*iter_beg))) ;
  sector_tree.Expand(hroot, TVE_EXPAND);
}
void DlgSectorSelect::AddObjectToTree(HTREEITEM hObjRoot, ALTObject* pObject)
{
	ASSERT(hObjRoot);
    
	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");
    if(objName.m_val[0].size() == 0)
		return ;
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		int id = pObject->getID() ;
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
				HTREEITEM hTreeItem = sector_tree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;
				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = sector_tree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				  sector_tree.SetItemData(hTreeItem,id) ;
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = sector_tree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			  sector_tree.SetItemData(hTreeItem,id) ;
			//			m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM DlgSectorSelect::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = sector_tree.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = sector_tree.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = sector_tree.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}
BEGIN_MESSAGE_MAP(DlgSectorSelect, CDialog)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgSectorSelect message handlers

void DlgSectorSelect::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(setSelectedNameAndID() != 0)
	    OnOK();
	else
		OnCancel() ;
}
CString DlgSectorSelect::getName()
{

	return selectedName ;
}
int DlgSectorSelect::getNameID()
{
	return nameID ;
}
int  DlgSectorSelect::setSelectedNameAndID()
{

	HTREEITEM hSelItem = sector_tree.GetSelectedItem();
	nameID = sector_tree.GetItemData(hSelItem) ;
	HTREEITEM hParentItem = sector_tree.GetChildItem(hSelItem);
	if (hParentItem != NULL)
	{
		MessageBox("Please select a Sector!", "Error", MB_OK);
		return 0;
	}

	ALTObjectID objName;

	switch(GetCurStandGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = sector_tree.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			selectedName.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(0) = sector_tree.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			selectedName.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(1) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(0) = sector_tree.GetItemText(hSelItem);
			objName.at(3) = "";
			selectedName.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(2) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(1) = sector_tree.GetItemText(hSelItem);
			hSelItem = sector_tree.GetParentItem(hSelItem);
			objName.at(0) = sector_tree.GetItemText(hSelItem);
			//hSelItem = m_TreeStandFamily.GetParentItem(hSelItem);
			//CString strAirport = m_TreeStandFamily.GetItemText(hSelItem);
			selectedName.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			selectedName = "";
			MessageBox("Please select a Sector!", "Error", MB_OK);
			return 0;
		}		
		break;
	}
	return  1 ;
}
int DlgSectorSelect::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = sector_tree.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = sector_tree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = sector_tree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = sector_tree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = sector_tree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}
void DlgSectorSelect::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
