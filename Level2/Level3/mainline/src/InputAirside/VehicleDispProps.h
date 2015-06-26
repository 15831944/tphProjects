#ifndef __VEHICLEDISPPROP_H__
#define __VEHICLEDISPPROP_H__

#pragma once
#include "VehicleDispPropNode.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CVehicleDispProps
{
public:
	CVehicleDispProps(void);
	~CVehicleDispProps(void);

	void SaveData(void);
	void ReadData(int nProjID);
	void DeleteData(void);
	void UpdateData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ImportVehicleDispProps(CAirsideImportFile& importFile);
	static void ExportDataVehicleDispProps(CAirsideExportFile& exportFile);

	int GetVehicleCount(void);
	CVehicleDispPropNode * GetVehicleDispProp(int nIndex);
	int GetCurSelVehicle(void);
	void SetCurSelVehicle(int nCurSelVehicle);
	void AddVehicleDispProp(CString strVehicleDispPropNodeName);
	CVehicleDispPropNode *  DeleteVehicleDispProp(int nIndex);//function return the pointer not delete and don't do the DeleteData();
	void SetProjID(int nProjID);

	CVehicleDispPropItem* FindBestMatch(int VehicleTypeID);
protected:
	void Clear(void);
protected:
	int m_nProjID;
	int m_nCurSelVehicle;
	std::vector<CVehicleDispPropNode *> m_vrVehicleDispProp;
};

#endif //__VEHICLEDISPPROP_H__