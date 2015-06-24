#include "StdAfx.h"
#include "resource.h"
#include ".\altobjecttreectrl.h"

CALTObjectTreeCtrl::CALTObjectTreeCtrl(void)
{
}

CALTObjectTreeCtrl::~CALTObjectTreeCtrl(void)
{
}

void CALTObjectTreeCtrl::LoadData( int nAptID, ALTOBJECT_TYPE nType )
{
	std::vector<ALTObject> vObject;

	ALTObjectIDList objIDList;

	ALTObject::GetObjectList(nType,nAptID,vObject);

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


			HTREEITEM hTreeItem = AddTreeItem(objName.m_val[nLevel].c_str(),TVI_ROOT);
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
	Expand(TVI_ROOT,TVE_EXPAND);

}

HTREEITEM CALTObjectTreeCtrl::AddTreeItem( const CString& strNode,HTREEITEM hParent /*= TVI_ROOT*/,HTREEITEM hInsertAfter /*= TVI_LAST*/,int nImage /*= ID_NODEIMG_DEFAULT*/,int nSelectImage /*= ID_NODEIMG_DEFAULT*/ )
{
	return InsertItem(strNode,nImage,nSelectImage,hParent,hInsertAfter);
}

CString CALTObjectTreeCtrl::GetTreeItemFullString( HTREEITEM hItem )
{

	CString reslt;
	if( hItem == NULL ) return reslt;

	reslt = GetItemText(hItem);
	HTREEITEM parentItem = hItem;
	while( parentItem = GetParentItem(parentItem) ){
		CString newreslt = GetItemText(parentItem);
		newreslt = newreslt + '-' + reslt;
		reslt = newreslt;	
	}
	return reslt;
}