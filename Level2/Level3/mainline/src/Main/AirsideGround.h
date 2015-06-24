#pragma once
#include "floor2.h"
class CAirsideImportFile;
class CAirsideExportFile;
class CADORecordset;


class CAirsideGround :
	public CFloor2
{
public:
	CAirsideGround();
	CAirsideGround(const CString& sName);

	virtual ~CAirsideGround(void);



public:

	void Rename(const CString& strName);
	void ReadData(int nID);

	void ReadData(CADORecordset& adoRecordset);


	//save the airsideGround information, include CAD info and Grid 
	void SaveData(int nAirportID);

	void SaveData(int nAirportID,int nOrder);

	void DeleteData(int nID);

    
	void ReadGridInfo(int nLevelID);
	void UpdateGridInfo(int nLevelID);
	void SaveGridInfo(int nLevelID);
	void DelGridInfo(int nLevelID);

	void ReadCADInfo(int nLevelID);
	void UpdateCADInfo(int nLevelID);	
	void SaveCADInfo(int nLevelID);
	void DelCADInfo(int nLevelID);

	void ReadOverlayInfo(int nLevelID);
	void UpdateOverlayInfo(int nLevelID);
	void SaveOverlayInfo(int nLevelID);
	void DelOverlayInfo(int nLevelID);

	int InsertDefaultLevel(int nAirportID, CString strName, int nOrder);

	void UpdateData(int nLevelId);
protected:
	void ReadMarker() ;
	void SaveMarker() ;
	
protected:



public:
	static void ExportAirsideGrounds(CAirsideExportFile& exportFile);
	static void ImportAirsideGrounds(CAirsideImportFile& importFile);


public:
	void ExportBaseInfo(CAirsideExportFile& exportFile,int nLevelID);	
	void ExportGridInfo(CAirsideExportFile& exportFile,int nLevelID);
	void ExportCADInfo(CAirsideExportFile& exportFile,int nLevelID);
	void ExportOverlayInfo(CAirsideExportFile& exportFile,int nLevelID);
	
	
	void ImportAirsideGround(CAirsideImportFile& importFile);
	void ImportBaseInfo(CAirsideImportFile& importFile);
	void ImportGridInfo(CAirsideImportFile& importFile);
	void ImportCADInfo(CAirsideImportFile& importFile);
	void ImportOverlayInfo(CAirsideImportFile& importFile);

	int GetAirportID()const{ return m_nAirportID; } 
protected:

	int m_nID;
	int m_nAirportID;
	int m_nOrder;
	int m_bMainLevel;

public:
	int getID(){ return m_nID;}

	void setMainLevel(bool bMain){ m_bMainLevel = bMain?1:0;}
public:
	void ReadFloorLayerFromDB(int nLevelID) ;
	void SaveFloorLayerToDB(int nLevelID) ;
	void DeleteFloorLayerFromDB(int nLevelID) ;
};

