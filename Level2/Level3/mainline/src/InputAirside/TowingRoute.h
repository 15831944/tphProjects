#pragma once

#include <vector>
#include "..\Database\ADODatabase.h"
class CAirsideExportFile;
class CAirsideImportFile;

#include "InputAirsideAPI.h"

//--------------------CTaxiwayNode--------------------

class INPUTAIRSIDE_API CTaxiwayNode
{
public:
	CTaxiwayNode();
	~CTaxiwayNode();

	int GetID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nTowingRouteID);
	void UpdateData();
	void SetParentID(int nParentID){m_nParentID = nParentID;}
	int GetParentID(){return m_nParentID;}
	void SetTaxiwayID(int nTaxiwayID){m_nTaxiwayID = nTaxiwayID;}
	int GetTaxiwayID(){return m_nTaxiwayID;}
	void SetUniqueID(int nUniqueID){m_nUniqueID = nUniqueID;}
	int GetUniqueID(){return m_nUniqueID;}
	void SetIdx(int idx){m_idx = idx;}
	int GetIdx(){return m_idx;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nTowingRouteID);
private:
	int m_nID;
	int m_nUniqueID;	
	int m_nParentID;
	int m_nTaxiwayID;
	int m_idx;
};

//--------------------CTowingRoute--------------------

class INPUTAIRSIDE_API CTowingRoute
{
public:
	typedef std::vector<CTaxiwayNode*> TaxiwayNodeVector;
	typedef std::vector<CTaxiwayNode*>::iterator TaxiwayNodeIter;
public:
	CTowingRoute();
	~CTowingRoute();

	int GetID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void ReadTaxiwayNodeData();
	void SaveData();
	void UpdateData();
	void DeleteData();

	int GetTaxiwayNodeCount();
	CTaxiwayNode* GetTaxiwayNodeItem(int nIndex);
	CTaxiwayNode* GetTaxiwayNodeByParentID(int nParentID);

	void AddTaxiwayNodeItem(CTaxiwayNode* pItem);
	void DelTaxiwayNodeItem(CTaxiwayNode* pItem);

	void RemoveAll();

	void SetName(const CString& strName){m_strName = strName; }
	const CString& GetName(){ return m_strName; }

	//void SetFromType(int nFromType){m_nFromType = nFromType;}
	//int GetFromType(){return m_nFromType;}

	//void SetFromID(int nFromID){m_nFromID = nFromID;}
	//int GetFromID(){return m_nFromID;}

	//void SetToType(int nToType){m_nToType = nToType;}
	//int GetToType(){return m_nToType;}

	//void SetToID(int nToID){m_nToID = nToID;}
	//int GetToID(){return m_nToID;}

	void DelFromvTaxiwayNode(CTaxiwayNode* pTaxiwayNode);
	void UpdatevTaxiwayNode(CTaxiwayNode* pTaxiwayNode);
	void DelChildren(int nID);
	int GetMaxUniID(){return m_nMaxUniID;}
	void SetMaxUniID(int nMaxUniID){m_nMaxUniID = nMaxUniID;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	std::vector<CTaxiwayNode*> GetFirstItems();
	std::vector<CTaxiwayNode*> GetExitItems();
private:
	int m_nID;
	//int m_nFromType;
	//int m_nFromID;
	//int m_nToType;
	//int m_nToID;
	CString m_strName;
	int m_nMaxUniID;
	TaxiwayNodeVector m_vTaxiwayNode;
	TaxiwayNodeVector m_vNeedDelTaxiwayNode;
};
//-------------------CTowingRouteList---------------------
class INPUTAIRSIDE_API CTowingRouteList
{
public:
	typedef std::vector<CTowingRoute*> TowingRouteVector;
	typedef std::vector<CTowingRoute*>::iterator TowingRouteIter;
public:
	CTowingRouteList();
	~CTowingRouteList();

	void ReadData();
	void SaveData();

	int GetTowingRouteCount();
	CTowingRoute* GetTowingRouteItem(int nIndex);
	CTowingRoute* GetTowingRouteByID(int nID);

	void AddTowingRouteItem(CTowingRoute* pItem);
	void DelTowingRouteItem(CTowingRoute* pItem);

	void RemoveAll();

	static void ExportTowingRoutes(CAirsideExportFile& exportFile);
	static void ImportTowingRoutes(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
private:
	TowingRouteVector m_vTowingRoute;
	TowingRouteVector m_vNeedDelTowingRoute;
};
