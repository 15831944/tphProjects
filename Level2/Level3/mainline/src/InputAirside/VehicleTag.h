#ifndef _VEHICLETAG_HEADER_
#define _VEHICLETAG_HEADER_

#pragma once 
#include "VehicleTagNode.h"
#include "AirsideImportExportManager.h"
#include "InputAirsideAPI.h"

//vehicle tag flag
#define VEHICLE_TAG_FLAG_NONE					0x0000000000000000  //none
#define VEHICLE_TAG_FLAG_VEHICLE_ID				0x0000000000000001  //vehicle id
#define VEHICLE_TAG_FLAG_POOL					0x0000000000000002  //pool
#define VEHICLE_TAG_FLAG_SERVICE_FLIGHT			0x0000000000000004  //service flight
#define VEHICLE_TAG_FLAG_SERVICE_STAND			0x0000000000000008  //service stand
#define VEHICLE_TAG_FLAG_SERVICE_COUNT_LEFT		0x0000000000000010  //service count left

class INPUTAIRSIDE_API CVehicleTag
{
public:
	//CVehicleTag(void);
	CVehicleTag(void);
	~CVehicleTag(void);

	void ReadData(int nProjID);
	void SaveData(void);
	void DeleteData(void);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	static void ExportVehicleTag(CAirsideExportFile& exportFile);
	static void ImportVehicleTag(CAirsideImportFile& importFile);
public:
	void AddNode(CVehicleTagNode * pVehicleTagNodeAdding);	
	CVehicleTagNode * GetNode(int nIndex);

	int GetCurSelNodeIndex(void);
	int GetNodeCount(void);
	int GetCurSelItemIndexInCurSelNode(void);
	int GetItemCountInCurSelNode(void);

	bool SetCurSelNodeIndex(int nSelNodeIndex);
	bool SetCurSelItemIndexInCurSelNode(int nSelItemIndex);
	void Clear(void);

	bool AddNode(CString strVehicleTagNodeName);
	CVehicleTagNode * DeleteNode(int nIndex);
	bool UpdateNodeName(int nNodeIndex,CString strNewName);
	CString GetNodeName(int nIndex);

	bool AddTagItemToNode(int nNodeIndex,int nVehicleSpeID,BOOL bShowTags,DWORD m_dwTagInfo);
	CVehicleTagItem * DeleteItemInNode(int nNodeIndex,int nItemIndex);
	bool UpdateItemVehicleSpeInNode(int nNodeIndex,int nItemIndex,int nVehicleSpeID);
	bool UpdateItemShowTagsFlagInNode(int nNodeIndex,int nItemIndex,BOOL bShowTags);
	bool UpdateItemTagInfoInNode(int nNodeIndex,int nItemIndex,DWORD dwTagInfo);
	bool GetItemVehicleSpeInNode(int nNodeIndex,int nItemIndex,CVehicleSpecificationItem & vehicleSpeItem);//fail will return invalid CVehicleSpecificationItem object
	BOOL GetItemShowTagsFlagInNode(int nNodeIndex,int nItemIndex,BOOL &bShowTags);//bShowTags == FALSE if fail
	BOOL GetItemTagInfoInNode(int nNodeIndex,int nItemIndex,DWORD & dwTagInfo);//dwTagInfo == -1 if fail
protected:
	int m_nProjID;
	std::vector <CVehicleTagNode *> m_vrTagNode;
	int m_nCurSelNode;
};

#endif