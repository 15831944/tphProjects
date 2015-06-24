// DlgSelectLandsideObject.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectLandsideObject.h"
#include ".\dlgselectlandsideobject.h"

#include "../Landside/LandsideLayoutObject.h"
#include "Common/ALT_TypeString.h"
// CDlgSelectLandsideObject dialog
IMPLEMENT_DYNAMIC(CDlgSelectLandsideObject, CDialog)
CDlgSelectLandsideObject::CDlgSelectLandsideObject(LandsideFacilityLayoutObjectList *tmpList, CWnd* pParent /*=NULL*/)
	: CDialog(/*CDlgSelectLandsideObject::IDD*/IDD_DIALOG_SELECTLANDSIDEOBJECT, pParent)
{
	objectItem = NULL;
	objectList = tmpList;
}

CDlgSelectLandsideObject::~CDlgSelectLandsideObject()
{
}

void CDlgSelectLandsideObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SELECTEDOJECT, m_edtSelect);
	DDX_Control(pDX, IDC_TREE_OBJECTS, m_TreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSelectLandsideObject, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECTS, OnTvnSelchangedTreeObjects)
END_MESSAGE_MAP()


// CDlgSelectLandsideObject message handlers

BOOL CDlgSelectLandsideObject::OnInitDialog()
{
	CDialog::OnInitDialog();
	if ((int)m_vObjType.size() > 1)
	{
		SetWindowText("Select Facility Family");
	}
	
	InitTree();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectLandsideObject::InitTree()
{
	m_hItemALL = m_TreeCtrl.InsertItem("All",TVI_ROOT);

	//if not defined the specified object type, default treat as all type


	if(m_vObjType.size() == 0)
	{
		for (int nDefType = ALT_LTYPE_START; nDefType <= ALT_LSPEEDSIGN; ++ nDefType)
		{
			m_vObjType.push_back(nDefType);
		}
	}


	std::vector<int> ObjType;
	if (objectList->getCount() == 0)
	{
		/*MessageBox(_T("There is no object defined, please define the object before !"),NULL,MB_OK);*/
		return;
	}
	ObjType.push_back(objectList->getObject(0)->GetType());	
	for (int i = 1; i < objectList->getCount(); i++)
	{
		LandsideFacilityLayoutObject* pLayout = objectList->getObject(i);
		if ((std::find(ObjType.begin(),ObjType.end(),(int)pLayout->GetType()) == ObjType.end()))
		{
			ObjType.push_back(pLayout->GetType());
		}
																		
	}

	for (int j = 0; j < (int)ObjType.size(); j ++)
	{
		if (TypeNeedToShow(ObjType.at(j)))
		{
			CString NodeName;
			NodeName = ALT_TypeString::Get((ALTOBJECT_TYPE)ObjType.at(j));
			objectItem = m_TreeCtrl.InsertItem(NodeName,TVI_ROOT);
			ObjectMap.insert(std::make_pair(ObjType.at(j),objectItem));
		}		
	}

 	for (int i=0;i<objectList->getCount();i++)
 	{
 		LandsideFacilityLayoutObject *tmpLayout=objectList->getObject(i);
 		ASSERT(tmpLayout != NULL);
 		if(tmpLayout == NULL)
 			continue;
 		
 		ALTObjectID altID=tmpLayout->getName();
		std::map<int,HTREEITEM>::iterator iter = ObjectMap.find(tmpLayout->GetType());
 		if(TypeNeedToShow(tmpLayout->GetType()))
			AddObjectToTree(iter->second,altID);
		m_TreeCtrl.Expand(iter->second,TVE_EXPAND);
	}
}

void CDlgSelectLandsideObject::AddObjectToTree( HTREEITEM hObjRoot,ALTObjectID objName )
{
	ASSERT(hObjRoot);

	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");
   CString strname="";
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		strname=objName.m_val[nLevel].c_str();
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (strname != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				hParentItem = hTreeItem;
				continue;
			}
			else 
			{
				break;
			}

		}
		else if (strname!=_T(""))
		{
			HTREEITEM hTreeItem = m_TreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			break;
		}
	}
}
HTREEITEM CDlgSelectLandsideObject::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrl.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrl.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CDlgSelectLandsideObject::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	HTREEITEM hItemsel=m_TreeCtrl.GetSelectedItem();

	CString strItemName = m_TreeCtrl.GetItemText(hItemsel);
	if (TypeNeedToShow(ALT_LPARKINGLOT))
	{
		if (strItemName.Compare(_T("All")) == 0)
		{
			MessageBox("The parking lot can't select the item 'All'.");
				return;
		}
	}
	if (FindRootItem(hItemsel) ||hItemsel == NULL)
	{
		CString strMsg;
		strMsg.Format("Please select one node.");
		MessageBox(strMsg);
		return;
	}
	/*
		m_altID = ALTObjectID();
	else
	{	
		CString strSelectName;

		int i=0;
		CString strOtherName;

		while (hItemsel)
		{
			CString strItemName = m_TreeCtrl.GetItemText(hItemsel);
			strItemName.Trim();
			if (!FindRootItem(hItemsel) && !strItemName.IsEmpty())
			{
				if (i==0)
				{
					strSelectName.Format("%s",strItemName);
				}
				else 
				{

					strSelectName.Format("%s-%s",strItemName,strOtherName);
				}
				i++;
				strOtherName = strSelectName;
			}
			hItemsel=m_TreeCtrl.GetParentItem(hItemsel);

		}
		m_altID.FromString(strSelectName);
	}*/

	CDialog::OnOK();
}

void CDlgSelectLandsideObject::OnTvnSelchangedTreeObjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	

	HTREEITEM hItemsel=m_TreeCtrl.GetSelectedItem();
	m_altID = ALTObjectID();

	if (hItemsel == NULL)
	{	
		m_edtSelect.SetWindowText("");
		return;
	}
	else if (hItemsel == m_hItemALL)
	{
		m_altID = ALTObjectID();
		m_altID.FromString("All");
		m_edtSelect.SetWindowText("All");
		return;
	}
	else if (FindRootItem(hItemsel))
	{
		m_altID = ALTObjectID();
		CString strItemName = m_TreeCtrl.GetItemText(hItemsel);
		m_edtSelect.SetWindowText(strItemName);
		return;
	}
	else
	{	
		CString strSelectName;

		int i=0;
		CString strOtherName;
		CString strRootName;
		while (hItemsel)
		{
			CString strItemName = m_TreeCtrl.GetItemText(hItemsel);
			strItemName.Trim();
			if (!FindRootItem(hItemsel) && !strItemName.IsEmpty())
			{
				if (i==0)
				{
					strSelectName.Format("%s",strItemName);
				}
				else 
				{

					strSelectName.Format("%s-%s",strItemName,strOtherName);
				}
				i++;
				strOtherName = strSelectName;
			}
			strRootName = strItemName;
			hItemsel=m_TreeCtrl.GetParentItem(hItemsel);
			if (hItemsel == NULL)
			{
				break;
			}
		}
		m_altID.FromString(strSelectName);
		CString strName;
		strName.Format(_T("%s:%s"),strRootName,m_altID.GetIDString());
		m_edtSelect.SetWindowText(strName);
	
	}



	*pResult = 0;
}

ALTObjectID CDlgSelectLandsideObject::getSelectObject()
{
	return m_altID;
}

void CDlgSelectLandsideObject::AddObjType( int nType )
{
	m_vObjType.push_back(nType);
}

bool CDlgSelectLandsideObject::TypeNeedToShow( int nType ) const
{
	if (m_vObjType.size() == 0)
		return true;

	if(std::find(m_vObjType.begin(), m_vObjType.end(), nType) != m_vObjType.end())
		return true;

	return false;

}

void CDlgSelectLandsideObject::SetObjectTypeVector( std::vector<int> objectTypeList )
{
	m_vObjType = objectTypeList;
}

bool CDlgSelectLandsideObject::FindRootItem( HTREEITEM hObjRoot )
{
	for (int i = 0; i < (int)m_vObjType.size(); i++)
	{
		std::map<int,HTREEITEM>::iterator iter = ObjectMap.find(m_vObjType.at(i));
		if (iter->second == hObjRoot)
		{
			CString strItemName = m_TreeCtrl.GetItemText(hObjRoot);
			m_edtSelect.SetWindowText(strItemName);
			return true;
		}
	}
	return false;
}
















