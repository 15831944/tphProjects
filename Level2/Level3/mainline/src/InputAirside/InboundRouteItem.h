#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include <iostream>
#include <vector>
#include "InputAirsideAPI.h"

using namespace std;
class CAirsideImportFile;
class CAirsideExportFile;

class ALTObject;
#define  INBOUDROUTE_ITEM   0x0100 ;
class INPUTAIRSIDE_API CInboundRouteItem : public DBElementCollection<CInboundRouteItem>
{
public:
	CInboundRouteItem(void);
	virtual ~CInboundRouteItem(void);
	typedef vector<CInboundRouteItem*> INBOUNDROUTEITEMLIST;

public:
	void SetParentID(int nParentID);
	int GetParentID();
	void SetInboundRouteID(int nInboundRouteID);
	int GetInboundRouteID();
	void SetALTObjectID(int nALTObjectID);
	int GetALTObjectID();
	void SetIntersectNodeID(int nINodeID);
	int GetIntersectNodeID();
	void SetItemName(CString strName);
	CString GetItemName();
	void GetTailRouteItems(INBOUNDROUTEITEMLIST *pInboundRouteItemList);
    

public:
	void DeleteDataFromDB();
	void SaveDataToDB();

	//DBElementCollection
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

private:
	int m_nLevel;
	int m_nParentID;
	int m_nInboundRouteID;
	int m_nAltObjectID;  //taxiway id, ground group id
	int m_nIntersectNodeID;  //the node id which intersect with it's parent object
	CString m_strItemName;
};
