#pragma once
#include "..\Database\ADODatabase.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
//
//class QueueNodeItem
//{
//public:
//	QueueNodeItem():m_nID(-1),m_nRootID(-1),m_nParentID(-1),m_nOrderID(-1),m_nQueueNodeID(-1){}
//	~QueueNodeItem(){}
//public:
//	int m_nID;
//	int m_nRootID;
//	int m_nParentID;
//	int m_nOrderID;
//
//	int m_nQueueNodeID;
//};
class INPUTAIRSIDE_API TakeoffQueuesItem
{
public:
	TakeoffQueuesItem();
	~TakeoffQueuesItem();

	int getID(){return m_nID;}
	void InitData();
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nTakeoffQueueID);
	void UpdateData(int nTakeoffQueueID);
//	void SetTakeoffQueueID(int nTakeoffQueueID){m_nTakeoffQueueID = nTakeoffQueueID;}
	void SetParentID(int nParentID){m_nParentID = nParentID;}
	int GetParentID()const{return m_nParentID;}
	void SetTaxiwayID(int nTaxiwayID){m_nTaxiwayID = nTaxiwayID;}
	int GetTaxiwayID()const{return m_nTaxiwayID;}
	void SetUniID(int nUniID){m_nUniID = nUniID;}
	int GetUniID()const{return m_nUniID;}
	void SetIdx(int idx){m_idx = idx;}
	int GetIdx()const{return m_idx;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nTakeoffQueueID);
private:
	int m_nUniID;			   // tree id
	int m_nParentID;		   // parent tree id 
	int m_nID;                 // database id
//	int m_nTakeoffQueueID;
	int m_nTaxiwayID;
	int m_idx;
};
