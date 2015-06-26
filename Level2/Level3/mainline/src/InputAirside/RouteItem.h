#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include <iostream>
#include <vector>
#include "InputAirsideAPI.h"

using namespace std;
class ALTObject;
class CAirsideImportFile;
class CAirsideExportFile;

class INPUTAIRSIDE_API CRouteItem : public DBElementCollection<CRouteItem>
{
public:
	CRouteItem(void);
	virtual ~CRouteItem(void);

public:
	void SetParentID(int nParentID);
	int GetParentID();
	void SetRouteID(int nRouteID);
	int GetRouteID();
	void SetALTObjectID(int nALTObjectID);
	int GetALTObjectID();
	void SetIntersectNodeID(int nINodeID);
	int GetIntersectNodeID(); // only use it in GUI, unreliable method if used in engine
	void SetItemName(CString strName);
	CString GetItemName();
	void SetPrioirtyID(int nPriorityID);
	int GetPriorityID()const;

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



private:
	int m_nParentID;
	int m_nRouteID;
	int m_nAltObjectID;      //taxiway id, ground group id
	int m_nIntersectNodeID;  //the node id which intersect with it's parent object, only use it in GUI, unreliable if used in engine
	CString m_strItemName;
	int m_nPriorityID;
};
