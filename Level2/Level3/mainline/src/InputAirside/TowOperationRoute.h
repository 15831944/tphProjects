#pragma once
#include "../Database/DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CTowOperationRouteItem : public DBElementCollection<CTowOperationRouteItem>
{
public:
	enum ItemType
	{
		ItemType_Unknown = 0,
		ItemType_Taxiway,
		ItemType_Runway,
		ItemType_TowRoute,



		//add new one before this line
		ItemType_Count
	};
public:
	CTowOperationRouteItem();
	~CTowOperationRouteItem();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	
	virtual void SaveData(int nParentID);
	virtual void DeleteData();
public:
	int GetParentID() const { return m_nParentID; }
	void SetParentID(int nParentID) { m_nParentID = nParentID;}

	CTowOperationRouteItem * GetParentItem() const { return m_pParentItem; }
	void SetParentItem(CTowOperationRouteItem * pParentItem) { m_pParentItem = pParentItem; }
	
	int GetIntersectionID() const { return m_nIntersectionID; }
	void SetIntersectionID(int nNodeID) { m_nIntersectionID = nNodeID; }


	CTowOperationRouteItem::ItemType GetEnumType() const { return m_enumType; }
	void SetEnumType(CTowOperationRouteItem::ItemType type) { m_enumType = type; }

	int GetObjectID() const { return m_nObjectID; }
	void SetObjectID(int nObjID) { m_nObjectID = nObjID; }

	int GetRouteID() const { return m_nRouteID; }
	void SetRouteID(int nRouteID) { m_nRouteID = nRouteID; }
	
	int GetRouteType() const { return m_nRouteType; }
	void SetRouteType(int nType) { m_nRouteType = nType; }


	CString GetName() const { return m_strName; }
	void SetName(CString strName) { m_strName = strName; }
	
	int GetItemID() const { return m_nID; }
	void SetItemID(int nID) { m_nID = nID; }
protected:
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

protected:
	int m_nRouteID;//route id
	int m_nRouteType;

	//parent item id
	int m_nParentID;

	CTowOperationRouteItem *m_pParentItem;

	ItemType m_enumType;

	int m_nObjectID;//according to ItemType

					//if runway ,runway id
					// taxiway, taxiway id
					// TowRoute, tow route id
	int m_nIntersectionID;

	CString m_strName;

};

class INPUTAIRSIDE_API CTowOperationRoute :public DBElementCollection<CTowOperationRouteItem >
{
public:

public:
	//TowOperationFlightData ID
	CTowOperationRoute(int nFltTypeDataID,int nType);
	~CTowOperationRoute(void);
public:
	virtual void ReadData(int nFltTypeDataID);
	virtual void SaveData(int nFltTypeDataID);
public:
	void DeleteDataFromDB();
	
protected:
	int m_nType;
protected:
	//only use while reading data from database
	//create new tree
	void ParserItemTree(std::vector<CTowOperationRouteItem *>& vRouteItem);
	//find parent item
	CTowOperationRouteItem * FindParentItem(int nParentID,std::vector<CTowOperationRouteItem *>& vRouteItem );
	//get head items
	void ParserHeadItem(std::vector<CTowOperationRouteItem *>& vRouteItem);
};
