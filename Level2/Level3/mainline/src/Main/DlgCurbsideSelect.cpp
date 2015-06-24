// DlgCurbsideSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgCurbsideSelect.h"
#include "../Common/ALT_TYPE.h"
#include "../Common/ALTObjectID.h"
#include "Resource.h"
#include <vector>
#include <map>
// CDlgCurbsideSelect dialog

IMPLEMENT_DYNAMIC(CDlgLandsideObjectSelect, CDialog)
CDlgLandsideObjectSelect::CDlgLandsideObjectSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLandsideObjectSelect::IDD, pParent)
{
}
CDlgLandsideObjectSelect::CDlgLandsideObjectSelect(LandsideFacilityLayoutObjectList *tmpList,ALTOBJECT_TYPE tmpAltType,CWnd *pParent/*=NULL*/)
:CDialog(CDlgLandsideObjectSelect::IDD,pParent)
{
   objectList=tmpList;
   m_AltType=tmpAltType;
}
BOOL CDlgLandsideObjectSelect::OnInitDialog()
{
	CDialog::OnInitDialog();
	switch(m_AltType)
	{
	case ALT_LCURBSIDE:
		strRootName="Curbside Family";
		break;
	case ALT_LPARKINGLOT:
		strRootName="Parkinglot Family";
		break;
	case  ALT_LSTRETCH:
		strRootName="Stretch Family";
		break;
	case ALT_LEXT_NODE:
		strRootName="OD Family";
		break;
	default:
		strRootName="AllObject Family";
		break;
	}
	CString strCaption;
	strCaption.Format("select %s",strRootName);
	this->SetWindowText(strCaption);
	initTree();
	return true;
}
int CDlgLandsideObjectSelect::getLevel(HTREEITEM hitem)
{
	int j=0;
	if (hitem==TVI_ROOT)
		return 0;
	while(true)
	{
		hitem=m_TreeCtrl.GetParentItem(hitem);
		if (hitem)
		{
			j++;
		}
		else 
		{
			break;
		}
	}
	return j;
}
void CDlgLandsideObjectSelect::AddObjectToTree(HTREEITEM hObjRoot,ALTObjectID objName)
{
	ASSERT(hObjRoot);

	/*ALTObjectID objName;
	pObject->getObjName(objName);*/
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
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else 
			{
				//HTREEITEM hTreeItem = m_TreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else if (strname!=_T(""))
		{
			HTREEITEM hTreeItem = m_TreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//			m_TreeStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgLandsideObjectSelect::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
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
BOOL CDlgLandsideObjectSelect::isGroupNode(CString strNameNode,int level, HTREEITEM &hParentItem)
{
	HTREEITEM hRootItem=NULL;
	HTREEITEM hChildItem=NULL;
	int tmpLevel;
	CString strParentName="";
   
	while (hParentItem!=NULL)
	{   tmpLevel=getLevel(hParentItem);
	     if (hParentItem==TVI_ROOT)
		 {
			 strParentName="";
		 }
		 else 
		 {
			 strParentName=m_TreeCtrl.GetItemText(hParentItem);
		 }
		if (strParentName==strNameNode &&tmpLevel==level)
			return true;
		if (m_TreeCtrl.ItemHasChildren(hParentItem))
		{
			hParentItem=m_TreeCtrl.GetChildItem(hParentItem);
			tmpLevel=getLevel(hParentItem);
			if(!isGroupNode(strNameNode,level,hParentItem)&&tmpLevel==level)
			{
				hParentItem=m_TreeCtrl.GetNextSiblingItem(m_TreeCtrl.GetParentItem(hParentItem));
			}
			else if(tmpLevel==level)
			{
				return true;
			}
		}
		else 
		{
			hParentItem=m_TreeCtrl.GetNextSiblingItem(hParentItem);
			if (hParentItem==NULL)
			{
				return false;
			}
		}
	}
	return false;
}
void CDlgLandsideObjectSelect::initTree()
{   
	
  
	HTREEITEM HRootItem=m_TreeCtrl.InsertItem(strRootName,TVI_ROOT);
	HTREEITEM hParentItem=HRootItem;
	HTREEITEM hChildItem=NULL;
	std::vector<HTREEITEM>hChildItemList;
	//std::map<int,std::map<CString,HTREEITEM>> maps;
	std::map<CString ,HTREEITEM>tmpMaps;
	std::map<int,HTREEITEM>::iterator iter;
	CString str;
	CString strPrev;
	//int level;
	CString strLast="";
	std::vector<ALTObjectID> alts;
//	HTREEITEM hParentItem=TVI_ROOT;
	BOOL b=FALSE;
	for (int i=0;i<objectList->getCount();i++)
	{
		LandsideFacilityLayoutObject *tmpLayout=objectList->getObject(i);
		if(tmpLayout->GetType()==m_AltType)
		{
			ALTObjectID altID=tmpLayout->getName();
			
			AddObjectToTree(HRootItem,altID);
			/*for (int k=(int)hChildItemList.size()-1;k>=0;k--)
			{
					m_TreeCtrl.Expand(hChildItemList.at(k),TVE_EXPAND);
			}*/
			
		}
	
	}
		m_TreeCtrl.Expand(HRootItem,TVE_EXPAND);
}
CDlgLandsideObjectSelect::~CDlgLandsideObjectSelect()
{
}

void CDlgLandsideObjectSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

void CDlgLandsideObjectSelect::btnOk()
{

	HTREEITEM hItemsel=m_TreeCtrl.GetSelectedItem();
	CString strName;
	CString strOtherName;
	int i=0;
	CString strMsg;
	if (hItemsel==NULL||m_TreeCtrl.GetItemText(hItemsel).Compare(strRootName)==0)
	{
		strMsg.Format("please select %s child node",strRootName);
		MessageBox(strMsg);
		return;
	}
	else
	{
			while (hItemsel)
			{
               strName=m_TreeCtrl.GetItemText(hItemsel);
			   if ( strName.Compare(strRootName) && strName.Compare(_T("")))
			   {
				   if (i==0)
				   {
					   /*m_CurbsideName.Append()*/
						m_CurbsideName.Format("%s",strName);
				   }
				   else 
				   {
					  
					   m_CurbsideName.Format("%s-%s",strName,strOtherName);
				   }
				   i++;
				   strOtherName=m_CurbsideName;
			   }
			   hItemsel=m_TreeCtrl.GetParentItem(hItemsel);

			}
			m_altID.FromString(m_CurbsideName);
	}
	CDialog::OnOK();
}
ALTObjectID  CDlgLandsideObjectSelect::getALTObjectID()
{
	return m_altID;
}
CString CDlgLandsideObjectSelect::getCurbSideName()
{
	return m_CurbsideName;
}
LandsideFacilityLayoutObject* CDlgLandsideObjectSelect ::getLayoutObject()
{
	return m_LayoutObject;
}
void CDlgLandsideObjectSelect::btnCancel()
{
	CDialog::OnCancel();

}


BEGIN_MESSAGE_MAP(CDlgLandsideObjectSelect, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CURBSIDESELECTOK,btnOk)
	ON_BN_CLICKED(IDC_BUTTON_CURBSIDESELECTCANCEL,btnCancel)
END_MESSAGE_MAP()


// CDlgCurbsideSelect message handlers
