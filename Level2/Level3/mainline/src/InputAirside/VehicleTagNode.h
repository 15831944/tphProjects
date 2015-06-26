#ifndef _VEHEICLETAGNODE_HEADER_
#define _VEHEICLETAGNODE_HEADER_

#pragma once 
#include "VehicleTagItem.h"
#include "AirsideImportExportManager.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CVehicleTagNode
{
public:
	CVehicleTagNode(int nProjID);
	~CVehicleTagNode(void);

	void ReadData(int nID);
	void SaveData(void);
	void DeleteData(void);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	void AddItem(CVehicleTagItem * pVehicleTagItemAdding);
	CVehicleTagItem * DeleteItem(int nIndex); 
	CVehicleTagItem * GetItem(int nIndex);
	bool SetCurSelItemIndex(int nIndex);
	int GetItemCount(void);
	int GetCurSelItemIndex(void);
	void Clear(void);

	static void GetVehicleTagNodeList(int nProjID,std::vector<int> & vrVehicleTagNodeID);

	int GetID(void);
protected:
	int m_nID;
	int m_nProjID;
	int m_nCurSelItem;
	std::vector<CVehicleTagItem *> m_vrTagItem;
public:
	CString m_strVehicleTagNodeName;
};

#endif