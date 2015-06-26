#pragma once

#include <vector>
#include "..\Database\ADODatabase.h"
#include "InputAirsideAPI.h"

class TakeoffQueuePosi;
class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API TakeoffQueues
{
public:
	typedef std::vector<TakeoffQueuePosi*> TakeoffQueuePosiVector;
	typedef std::vector<TakeoffQueuePosi*>::iterator TakeoffQueuePosiIter;
public:
	explicit TakeoffQueues(int nProjID);
	~TakeoffQueues(void);

	void ReadData();
	void SaveData();

	int GetTakeoffQueuePosiCount();
	TakeoffQueuePosi* GetItem(int nIndex);

	void AddTakeoffQueuePosi(TakeoffQueuePosi* pItem);
	void DeleteTakeoffQueuePosi(TakeoffQueuePosi* pItem);

	void RemoveAll();
	void InitTakeOffQueues();
	void InsertDataIntoStructure(TakeoffQueuePosiVector vReadTakeoffQueuePosi);
	int GetMaxUniID(){return m_nMaxUniID;}
	void SetMaxUniID(int nMaxUniID){m_nMaxUniID = nMaxUniID;}
	static void ExportTakeoffQueues(CAirsideExportFile& exportFile);
	static void ImportTakeoffQueues(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
private:
	int m_nProjID;
	int m_nMaxUniID;
//	TakeoffQueuePosiVector m_vReadTakeoffQueuePosi;
	TakeoffQueuePosiVector m_vTakeoffQueuePosi;
	TakeoffQueuePosiVector m_vInvalidTakeoffQueuePosi;
};
