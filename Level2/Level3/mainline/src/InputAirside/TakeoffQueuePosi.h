#pragma once

#include <vector>
#include "..\Database\ADODatabase.h"
#include "RunwayExitDiscription.h"
#include "InputAirsideAPI.h"

class TakeoffQueuesItem;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API TakeoffQueuePosi
{
public:
	typedef std::vector<TakeoffQueuesItem*> TakeoffQueuesItemVector;
	typedef std::vector<TakeoffQueuesItem*>::iterator TakeoffQueuesItemIter;
public:
	TakeoffQueuePosi();
	~TakeoffQueuePosi();

	int getID(){return m_nID;}
	void setID(int nID){m_nID = nID;}

	void InitData(CADORecordset& adoRecordset);
	void ReadData(CADORecordset& adoRecordset);
	void ReadNodeData();
	void SaveData(int nProjID);
	void UpdateData(int nProjID);
	void DeleteData();

	int GetTakeoffQueuesItemCount()const;
	TakeoffQueuesItem* GetItem(int nIndex)const;
	TakeoffQueuesItem* GetItemByTreeId(int nidx)const;

	void AddTakeoffQueuesItem(TakeoffQueuesItem* pItem);
	void DeleteTakeoffQueuesItem(TakeoffQueuesItem* pItem);

	void RemoveAll();
	int  GetTakeoffPosiInterID(){return m_nTakeoffPosiInterID;}
	void Clear(){m_vTakeoffQueuesItem.clear();}
	void CopyData(TakeoffQueuePosi* pTakeoffQueuePosi);
	//CString GetStrRunTaxiway(){return m_strRunTaxiway;}
	//int GetTaxiwayID(){return m_nTaxiwayID;}
	TakeoffQueuesItemVector* GetQueuesItemVector(){return &m_vTakeoffQueuesItem;}
	void SetQueuesItemVector(TakeoffQueuesItemVector& vTakeoffQueuesItem){m_vTakeoffQueuesItem = vTakeoffQueuesItem;}
	void DelFromVTakeoffPosi(TakeoffQueuesItem* pTakeoffQueuesItem);
	void UpdateVTakeoffPosi(TakeoffQueuesItem* pTakeoffQueuesItem);
	void DelChildren(int nID);
	//int GetRunwayID(){return m_nRunwayID;}
	//int GetRunwayMKIndex(){return m_nRunwayMkIndex;}
	const RunwayExitDescription& GetRunwayExitDescription()const{ return m_runwayExitDescription; }

	bool IsLeafItem(TakeoffQueuesItem* pQueuItem)const;

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	void InitFromRunwayExit(const RunwayExit& runwayExit);

private:
	int m_nID;
	int m_nTakeoffPosiInterID; //runway exit id
	RunwayExitDescription m_runwayExitDescription;
	//int m_nTaxiwayID;
	//int m_nProjID;
	//CString m_strRunTaxiway;
	//int m_nRunwayID;
	//int m_nRunwayMkIndex;
	TakeoffQueuesItemVector m_vTakeoffQueuesItem;
	TakeoffQueuesItemVector m_vInvalidTakeoffQueuesItem;
};
