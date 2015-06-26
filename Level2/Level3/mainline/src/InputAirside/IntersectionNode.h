#pragma once

#include "../Common/Point2008.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class CAirsideImportFile;
class CAirsideExportFile;

#include <InputAirside/IntersectItem.h>

class INPUTAIRSIDE_API IntersectionNode
{
public:
	IntersectionNode();
	IntersectionNode(const IntersectionNode&);
	IntersectionNode& operator = (const IntersectionNode& );

	bool operator==(const IntersectionNode& other) const;

	~IntersectionNode(void);

	int GetIndex()const;
	IntersectionNode& SetIndex(int idx);

	CString GetName()const;
	IntersectionNode& SetName(const CString& strName);

	void UpdateName();	
	
	ALTObject * GetObject(int nObjID);
	DistanceUnit GetDistance(int objID)const;

	IntersectionNode& SetPosition(const CPoint2008& pos);
	CPoint2008 GetPosition()const;

	//Database related
	int GetID()const;
	IntersectionNode& SetID(int nID);
	//
	BOOL IsIdentical(const IntersectionNode& node)const;
	//the intersect items are all the same
	BOOL IsIdenticalWithOutIndex(const IntersectionNode& node)const;

protected:
	//Database relate to items 
	void SaveItems();
	void ReadItems();
	void DeleteItems();

public:
    void SaveData(int nAirportID);
	void UpdateData();
	void DeleteData();
	void ReadData(int nID);
	void ReadData(CADORecordset& adoRecordset);

	void ExportIntersectionNode(CAirsideExportFile& exportFile);
	void ImportIntersectionNode(CAirsideImportFile& importFile);
	static bool  GetNodeName(int ObjID1,int objID2,int idx,CString & nodeName);

	//
	static void ReadNodeList(int nAirportID, std::vector<IntersectionNode>& nodelist);
	static void ReadIntersecNodeListWithObjType(int thisObjId,ALTOBJECT_TYPE OtherobjType, std::vector<IntersectionNode>& lstIntersecNode);
	static void ReadIntersecNodeListWithObjType(int thisObjId,std::vector<ALTOBJECT_TYPE> vOtherobjType, std::vector<IntersectionNode>& lstIntersecNode);
	static void ReadIntersecNodeList(int objID, std::vector<IntersectionNode>& lstIntersecNode);
	static void ReadIntersectNodeList(int objID1, int ObjID2,std::vector<IntersectionNode>& lstIntersecNode);
	static void ReadIntersectNodeList(std::vector<IntersectionNode>& lstIntersecNode);
public:
	void AddItem(IntersectItem* newItem);
	std::vector<IntersectItem*> CloneItems()const;
	
	std::vector<ALTObject*> GetOtherObjectList(int nObjID);
	std::vector<int> GetOtherObjectIDList(int nObjID);

	bool HasObject(int nObjID)const;
	bool HasObjectIn(const ALTObjectUIDList& vObjIDs)const;
	bool HasLeadInLine(int nInLineID )const;
	bool HasLeadOutLine(int nOutLineID)const;
	bool HasOtherObjectType(int nObjID, ALTOBJECT_TYPE OtherobjType);
	bool HasDeiceInContrain(int nObjID)const;
	bool HasDeiceOutContrain(int nObjID)const;

	RunwayIntersectItem* GetRunwayItem(int nRwID);
	TaxiwayIntersectItem* GetTaxiwayItem(int nTaxiID)const;
	StandLeadInLineIntersectItem* GetStandLeadInLineItem(int nStandID, int nLeadinLineIdx);
	StandLeadOutLineIntersectItem* GetStandLeadOutLineItem(int nStandID, int nLeadoutLineIdx);
	StretchIntersectItem* GetStretchItem(int nStretchID);
	CircleStretchIntersectItem* GetCircleStretchItem(int nCircleStretchID);

	IntersectItem* GetObjectItem(int nObjID)const;
	std::vector<IntersectItem*> GetObjectTypeItems(ALTOBJECT_TYPE objType);
	

	
	std::vector<RunwayIntersectItem*> GetRunwayIntersectItemList()const; 
	std::vector<TaxiwayIntersectItem*> GetTaxiwayIntersectItemList()const;
	std::vector<StandLeadInLineIntersectItem*> GetStandLeadInLineItemList()const;
	std::vector<StandLeadOutLineIntersectItem*> GetStandLeadOutLineItemList()const;
	std::vector<StretchIntersectItem*> GetStretchIntersectItem()const;
	std::vector<CircleStretchIntersectItem*> GetCircleStretchIntersectItemList()const;
	std::vector<DeicePadInContrainItem*> GetDeicePadInContrainItemList()const;
	std::vector<DeicePadOutConstrainItem*> GetDeicePadOutConstrainItemList()const;

	//return items with type
	std::vector<IntersectItem*> GetTypeItemList(IntersectItem::TYPE nType)const;
	//return all items
	std::vector<IntersectItem*> GetItemList()const;

	void Merge(const IntersectionNode& otherNode);
	
	void SetObject(ALTObjectList objList);

	void RemoveObjectItem(int nObjID);
	
	IntersectItem* GetItemByID(int nID)const;

	bool IsValid()const;  //at least 2 intersect items

	void ClearItemList();

	int GetAirportID()const;
	IntersectionNode& SetAirportID(int nID);
	//true, runway intersection node,  
	bool IsRunwayIntersectionNode()const;

	//judge whether the node is intersected by runways
	bool IsRunwaysIntersection() const;
protected:
	int m_nID;
	int m_nidx;
	int m_nAirportID;

	CString m_strName;
	CPoint2008 m_pos;

	std::vector<IntersectItem*> m_vItems;
	
};

typedef std::vector<IntersectionNode> IntersectionNodeList;
