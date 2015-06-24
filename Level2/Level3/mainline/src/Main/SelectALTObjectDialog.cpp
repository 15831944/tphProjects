#include "StdAfx.h"
#include "resource.h"
#include ".\selectaltobjectdialog.h"
#include "boost\array.hpp"


CSelectALTObjectDialog::CSelectALTObjectDialog( int prjID, int airportID, CWnd* pParent /*= NULL */ )
: CDialog(CSelectALTObjectDialog::IDD, pParent){
	m_nAirportID = airportID;
	prjID = prjID;
	m_nObjType = ALT_UNKNOWN;

}
CSelectALTObjectDialog::~CSelectALTObjectDialog(void)
{
}

void CSelectALTObjectDialog::SetType( ALTOBJECT_TYPE objType )
{
	m_nObjType = objType;
}



void CSelectALTObjectDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectALTObjectDialog)
	DDX_Control(pDX, IDCANCEL, m_btCancle);
	DDX_Control(pDX, ID_BTN_SELECT, m_btOk);
	DDX_Control(pDX, IDC_EDIT_ALTOBJECT, m_editResult);
	DDX_Control(pDX, IDC_TREE_ALTOBJECT, m_ObjectTree);
	DDX_Text(pDX, IDC_EDIT_ALTOBJECT, m_strObjID);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CSelectALTObjectDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectALTObjectDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROCESSOR, OnDblclkTreeALTObject)
	ON_BN_CLICKED(ID_BTN_SELECT, OnBtnSelect)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJECT, OnSelchangedTreeALTObject)
	//ON_LBN_SELCHANGE(IDC_PROCESS_LIST, OnSelchangeALTObjectList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelectALTObjectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString windowText;
	windowText.Format("Select %s",ALTObject::GetTypeName(m_nObjType));
	SetWindowText(_T(windowText));
	GetDlgItem(ID_SELOBJECT_GROUPBOX)->SetWindowText(ALTObject::GetTypeName(m_nObjType));
	GetDlgItem(IDC_EDIT_ALTOBJECT)->SetWindowText(m_strObjID);

	LoadObjectsTree(TVI_ROOT);
	
	return true;

}

 void CSelectALTObjectDialog::OnDblclkTreeALTObject( NMHDR* pNMHDR, LRESULT* pResult )
{

}

 void CSelectALTObjectDialog::OnBtnSelect()
{
	
	{
		HTREEITEM hItem = m_ObjectTree.GetSelectedItem();
		if (hItem == NULL) return;
		
		CString selectStr = GetTreeItemFullString(hItem);
		if( selectStr.IsEmpty() ) {
			m_strObjID = selectStr;
			return;
		}

		hItem = m_ObjectTree.GetFirstSelectedItem();
		m_vStrList.clear();
		while(hItem){
			CString selectStr = GetTreeItemFullString(hItem);
			m_vStrList.push_back(selectStr);
			hItem = m_ObjectTree.GetNextSelectedItem(hItem);
		}
	}
	
	CDialog::OnOK();
}

 void CSelectALTObjectDialog::OnSelchangedTreeALTObject( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = NULL;
	hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL) return;

	CString itemfullname = GetTreeItemFullString(hItem);

	GetDlgItem(IDC_EDIT_ALTOBJECT)->SetWindowText(itemfullname);

	//m_bSelectProcessor = true;

	*pResult = 0;
}

 void CSelectALTObjectDialog::OnSelchangeALTObjectList()
{

}




bool CSelectALTObjectDialog::LoadObjectsTree(HTREEITEM hObjRoot)
{
	
	std::vector<ALTObject> vObject;
	
	ALTObjectIDList objIDList;

	ALTObject::GetObjectList(m_nObjType,m_nAirportID,vObject);
	
	boost::array<std::pair<CString,HTREEITEM>,OBJECT_STRING_LEVEL> LastObjectItem;

	int nObjectCount = vObject.size();
	for (int i =0; i < nObjectCount; ++ i)
	{
		ALTObject& altObject = vObject.at(i);
		ALTObjectID objName;
		altObject.getObjName(objName);

		//Level1
		int nLevel = 0;
		CString levelstr = objName.m_val[nLevel].c_str();

		if (levelstr != LastObjectItem[nLevel].first)
		{
			for (int j=nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}


			HTREEITEM hTreeItem = AddTreeItem(objName.m_val[nLevel].c_str(),hObjRoot);
			LastObjectItem[nLevel].first = objName.m_val[nLevel].c_str();
			LastObjectItem[nLevel].second = hTreeItem;


			//CNodeData* pNodeData= NULL;
			if(objName.m_val[nLevel+1] == _T("")) //object node
			{
				
				continue;
			}
			else
			{
			
			}

		}

		//level2
		nLevel = 1;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{		
			for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = AddTreeItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
			
			LastObjectItem[nLevel].first = objName.m_val[nLevel].c_str();
			LastObjectItem[nLevel].second = hTreeItem;
			
			if(objName.m_val[nLevel+1] == _T("")) //object node
			{
				
				continue;	
			}
			else
			{
				
			}

		}

		//level3
		nLevel = 2;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{	

			for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = AddTreeItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
			
			LastObjectItem[nLevel].first = levelstr;
			LastObjectItem[nLevel].second = hTreeItem;
			//CNodeData* pNodeData= NULL;
			if(objName.m_val[nLevel+1] == _T("")) //object node
			{
			
				continue;	
			}
			else
			{
				
			}
		}

		nLevel = 3;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{
			HTREEITEM hTreeItem = AddTreeItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);
			LastObjectItem[nLevel].first = levelstr;
			LastObjectItem[nLevel].second = hTreeItem;			
			continue;
		}

	}

	GetTreeCtrl().Expand(hObjRoot,TVE_EXPAND);
	
	ALTObjectID objID(m_strObjID);
	HTREEITEM selItem = GetObjectTreeItem(objID);
	if(selItem != TVI_ROOT && selItem != NULL)
	{
		GetTreeCtrl().Expand(selItem,TVE_EXPAND);
		GetTreeCtrl().SelectItem(selItem);
	}
	return true;
}

HTREEITEM CSelectALTObjectDialog::AddTreeItem( const CString& strNode,HTREEITEM hParent,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
{
	return GetTreeCtrl().InsertItem(strNode,nImage,nSelectImage,hParent,hInsertAfter);
}

CString CSelectALTObjectDialog::GetTreeItemFullString( HTREEITEM hItem )
{
	CString reslt;
	if( hItem == NULL ) return reslt;
	
	reslt = m_ObjectTree.GetItemText(hItem);
	HTREEITEM parentItem = hItem;
	while( parentItem = m_ObjectTree.GetParentItem(parentItem) ){
		CString newreslt = m_ObjectTree.GetItemText(parentItem);
		newreslt = newreslt + '-' + reslt;
		reslt = newreslt;	
	}
	return reslt;
}

bool CSelectALTObjectDialog::GetObjectIDString( CString& objIDstr )
{
	if(!m_strObjID.IsEmpty()){
		objIDstr = m_strObjID;
		return true;
	}
	else return false;

}

int CSelectALTObjectDialog::GetObjectIDStringList( std::vector<CString>& strList )
{
	strList = m_vStrList;
	return strList.size();
}

void CSelectALTObjectDialog::SetSelectString( const CString& str )
{
	ALTObjectID objID(_T(str));
	
	//get first select level
	m_strObjID = str;
}


HTREEITEM CSelectALTObjectDialog::GetObjectTreeItem( const ALTObjectID& objID )
{
	HTREEITEM lastItem = TVI_ROOT;
	int nidLen = objID.idLength();
	int i=0;
	for(;i<nidLen;i++)
	{
		CString levelStr = objID.at(i).c_str();
		HTREEITEM childItem  = GetTreeCtrl().GetChildItem(lastItem);
		while ( childItem )
		{
			CString itemText = GetTreeCtrl().GetItemText(childItem);
			if(levelStr.CompareNoCase(itemText) == 0)
			{
				lastItem = childItem;
				break;
			}
			childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
		}
		if(childItem){
			lastItem = childItem;			
		}else
		{
			break;
		}
	}
	if(i==nidLen)
		return lastItem;
	return NULL;
	
}