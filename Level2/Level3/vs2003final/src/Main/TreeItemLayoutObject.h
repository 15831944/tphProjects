#pragma once
#include "TreeCtrlItem.h"
#include "MSVNodeData.h"
#include "common\ALT_TYPE.h"
class LandsideFacilityLayoutObject;
class ALTObjectID;
class LandsideFacilityLayoutObjectList;


class CTreeItemMSVNode : public CTreeCtrlItem
{
public:
	TREEIREM_CONVERSION_DEF(CTreeItemMSVNode,CTreeCtrlItem)

	MSV::enumNodeType getNodeType()const;

	MSV::CNodeData* InitNodeData(MSV::enumNodeType eType, MSV::CNodeDataAllocator* pNodeAllocate);
	void SetResourceID(UINT idRes);

	MSV::CNodeData* getNodeData(){ return (MSV::CNodeData*)GetData(); }

	virtual bool GetLayoutObjectList(LandsideFacilityLayoutObjectList& layoutList);
protected:
	MSV::CNodeData* createNodeData(MSV::enumNodeType eType);

};

class LandsideLayoutObject;

class CTreeItemLayoutObject : public CTreeItemMSVNode
{
public:
	TREEIREM_CONVERSION_DEF(CTreeItemLayoutObject,CTreeItemMSVNode)

	LandsideLayoutObject* getObject();
	void setObject(LandsideLayoutObject* pobj);

	virtual bool GetLayoutObjectList(LandsideFacilityLayoutObjectList& layoutList);

	bool IsValid()const;
};

class CTreeItemLayoutObjectGroup : public CTreeItemMSVNode
{
public:
	TREEIREM_CONVERSION_DEF(CTreeItemLayoutObjectGroup,CTreeItemMSVNode)
	//
	CTreeItemMSVNode AddGroupItem(const CString& str);
	CTreeItemMSVNode AddObjectItem(const CString& str);


	//assert the child are sorted, return the insert before item if it not the str item return the 
	HTREEITEM FindInsertPos(const CString& str,bool& bInsert);

	CTreeItemLayoutObject FindObjectItem( LandsideLayoutObject* pObj );
	virtual bool GetLayoutObjectList(LandsideFacilityLayoutObjectList& layoutList);
	bool TravelTreeNode(CTreeItemLayoutObject hItem,LandsideFacilityLayoutObjectList& layoutList);
};


class ILandsideLayoutObjectList;
class LandsideLayoutObject;

class CTreeItemLayoutObjectRoot : public CTreeItemLayoutObjectGroup
{
public:
	TREEIREM_CONVERSION_DEF(CTreeItemLayoutObjectRoot,CTreeItemLayoutObjectGroup)
	
	ALTOBJECT_TYPE getObjectType();

	bool AddFcObject(LandsideFacilityLayoutObject* pObj);
	bool AddObject(LandsideLayoutObject* pObj);	

	bool RemoveObject(LandsideLayoutObject* pObj);	

	CTreeItemMSVNode AddPortalItem(const CString& str);
	CTreeItemMSVNode AddAreaItem(const CString& str);

	void setObjectType(ALTOBJECT_TYPE type);
	
	void AddObjectList(ILandsideLayoutObjectList& objlist);

	CString GetMenuAddCaption();
public:
	//will remove empty child items
	void _RemoveObjItem(CTreeItemMSVNode& objItem);
};
//////////////////////////////////////////////////////////////////////////
class CLevelTreeItem : public CTreeItemMSVNode
{
public:
	TREEIREM_CONVERSION_DEF(CLevelTreeItem,CTreeItemMSVNode)
	void SetActive(BOOL b);
	void SetFloorIndex(int idx);

	int GetFloorIndex();
	virtual bool IsValid()const;
};

class CFloors;
class CFloor2;
class CLevelRootTreeItem : public CTreeItemMSVNode
{
public:
	TREEIREM_CONVERSION_DEF(CLevelRootTreeItem,CTreeItemMSVNode)

	void AddLevelList(CFloors& floors);

	CLevelTreeItem AddLevelItem(CFloor2* pLevel);
	void RemoveLevelItem(int nFloor);

	void Init(MSV::CNodeDataAllocator* pNodedataAlc);

	virtual bool IsValid()const;
};


class LandsideLayoutObject;
//////////////////////////////////////////////////////////////////////////
class CTreeItemLayout : public CTreeItemMSVNode
{
public:
	TREEIREM_CONVERSION_DEF(CTreeItemLayout,CTreeItemMSVNode)

	CTreeItemLayoutObjectRoot FindObjectRoot(ALTOBJECT_TYPE type);

	bool AddLayoutObject(LandsideLayoutObject* pObj);
	bool RemoveLayoutObject(LandsideLayoutObject* pObj);
	bool ReNameLayoutObject(LandsideLayoutObject* pObj);
};


