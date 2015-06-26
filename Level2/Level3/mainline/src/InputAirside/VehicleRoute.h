#pragma once

#include <vector>
#include "..\Database\ADODatabase.h"
class CAirsideExportFile;
class CAirsideImportFile;

#include "InputAirsideAPI.h"

//--------------------CStretchNode--------------------

class INPUTAIRSIDE_API CStretchNode
{
public:
	CStretchNode();
	~CStretchNode();

	int GetID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nVehicleRouteID);
	void UpdateData();
	void SetParentID(int nParentID){m_nParentID = nParentID;}
	int GetParentID()const{return m_nParentID;}
	void SetStretchID(int nStretchID){m_nStretchID = nStretchID;}
	int GetStretchID()const{return m_nStretchID;}
	void SetUniqueID(int nUniqueID){m_nUniqueID = nUniqueID;}
	int GetUniqueID()const{return m_nUniqueID;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nVehicleRouteID);
private:
	int m_nID;
	int m_nUniqueID;	
	int m_nParentID;
	int m_nStretchID;
};

//--------------------CVehicleRoute--------------------

class INPUTAIRSIDE_API CVehicleRoute
{
public:
	typedef std::vector<CStretchNode*> StretchNodeVector;
	typedef std::vector<CStretchNode*>::iterator StretchNodeIter;
	typedef std::vector<CStretchNode*>::const_iterator StretchNodeConstIter;
public:
	CVehicleRoute();
	~CVehicleRoute();

	int GetID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void ReadStretchNodeData();
	void SaveData(int nVehicleRouteListID);
	void UpdateData();
	void DeleteData();

	int GetStretchNodeCount();
	CStretchNode* GetStretchNodeItem(int nIndex);
	CStretchNode* GetStretchNodeItemByID(int nNodeID);

	StretchNodeVector GetChildNodes(int nParentNodeID);

	void AddStretchNodeItem(CStretchNode* pItem);
	void DelStretchNodeItem(CStretchNode* pItem);

	void RemoveAll();

	void SetRouteName(CString strRouteName){m_strRouteName = strRouteName;}
	CString GetRouteName(){return m_strRouteName;}

	void DelFromvStretchNode(CStretchNode* pStretchNode);
	void UpdatevStretchNode(CStretchNode* pStretchNode);
	void DelChildren(int nID);
	int GetMaxUniID()const;
	
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
private:
	int m_nID;
	CString m_strRouteName;
	
	StretchNodeVector m_vStretchNode;
	StretchNodeVector m_vNeedDelStretchNode;
};
//-------------------CVehicleRouteList---------------------
class INPUTAIRSIDE_API CVehicleRouteList
{
public:
	typedef std::vector<CVehicleRoute*> VehicleRouteVector;
	typedef std::vector<CVehicleRoute*>::iterator VehicleRouteIter;
public:
	CVehicleRouteList();
	~CVehicleRouteList();

	void ReadData(int nProjID);
	void SaveData(int nProjID);

	int GetVehicleRouteCount();
	CVehicleRoute* GetVehicleRouteItem(int nIndex);

	void AddVehicleRouteItem(CVehicleRoute* pItem);
	void DelVehicleRouteItem(CVehicleRoute* pItem);

	void RemoveAll();

	static void ExportVehicleRoutes(CAirsideExportFile& exportFile);
	static void ImportVehicleRoutes(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
private:
	VehicleRouteVector m_vVehicleRoute;
	VehicleRouteVector m_vNeedDelVehicleRoute;
};
