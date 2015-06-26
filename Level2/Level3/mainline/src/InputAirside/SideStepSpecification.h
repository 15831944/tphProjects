#pragma once

#include <vector>
#include "..\Database\ADODatabase.h"
#include "..\Common\Flt_cnst.h"

#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

//--------------------CSideStepToRunway--------------------
class INPUTAIRSIDE_API CSideStepToRunway
{
public:
	CSideStepToRunway();
	~CSideStepToRunway();

	int getID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nApproachRunwayID);
	void UpdateData();
	int GetRunwayID(){return m_nRunwayID;}
	void SetRunwayID(int nRunwayID){m_nRunwayID = nRunwayID;}
	int GetMarkIndex(){return m_nMarkIndex;}
	void SetMarkIndex(int nMarkIndex){m_nMarkIndex = nMarkIndex;}
	int GetMinFinalLeg(){return m_nMinFinalLeg;}
	void SetMinFinalLeg(int nMinFinalLeg){m_nMinFinalLeg = nMinFinalLeg;}
	int GetMaxTurnAngle(){return m_nMaxTurnAngle;}
	void SetMaxTurnAngle(int nMaxTurnAngle){m_nMaxTurnAngle = nMaxTurnAngle;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nApproachRunwayID);
private:
	int m_nID;
	int m_nRunwayID;
	int m_nMarkIndex;
	int m_nMinFinalLeg;
	int m_nMaxTurnAngle;

};

//--------------------CSideStepApproachRunway--------------------

class INPUTAIRSIDE_API CSideStepApproachRunway
{
public:
	typedef std::vector<CSideStepToRunway*> SideStepToRunwayVector;
	typedef std::vector<CSideStepToRunway*>::iterator SideStepToRunwayIter;
public:
	CSideStepApproachRunway();
	~CSideStepApproachRunway();

	int getID(){return m_nID;}
	void setID(int nID){m_nID = nID;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadToRunwayData();
	void SaveData(int nFlightTypeID);
	void UpdateData();
	void DeleteData();

	int GetToRunwayCount();
	CSideStepToRunway* GetToRunwayItem(int nIndex);

	void AddToRunwayItem(CSideStepToRunway* pItem);
	void DeleteToRunwayItem(CSideStepToRunway* pItem);

	void RemoveAll();
	void Clear(){m_vToRunway.clear();}///
	void CopyData(CSideStepApproachRunway* pApproachRunway);///
	int GetRunwayID(){return m_nRunwayID;}
	void SetRunwayID(int nRunwayID){m_nRunwayID = nRunwayID;}
	int GetMarkIndex(){return m_nMarkIndex;}
	void SetMarkIndex(int nMarkIndex){m_nMarkIndex = nMarkIndex;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nFlightTypeID);
private:
	int m_nID;
	int m_nRunwayID;
	int m_nMarkIndex;
	SideStepToRunwayVector m_vToRunway;
	SideStepToRunwayVector m_vInvalidToRunway;
};

//--------------------CSideStepFlightType--------------------

class INPUTAIRSIDE_API CSideStepFlightType
{
public:
	typedef std::vector<CSideStepApproachRunway*> SideStepApproachRunwayVector;
	typedef std::vector<CSideStepApproachRunway*>::iterator SideStepApproachRunwayIter;
public:
	CSideStepFlightType();
	~CSideStepFlightType();

	int getID(){return m_nID;}
	void setID(int nID){m_nID = nID;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadApproachRunwayData();
	void SaveData(int nProjID);
	void UpdateData();
	void DeleteData();

	int GetApproachRunwayCount();
	CSideStepApproachRunway* GetApproachRunwayItem(int nIndex);

	void AddApproachRunwayItem(CSideStepApproachRunway* pItem);
	void DeleteApproachRunwayItem(CSideStepApproachRunway* pItem);

	void RemoveAll();
	void Clear(){m_vApproachRunway.clear();}///
	void CopyData(CSideStepFlightType* pFlightType);///
	//void SetFltType(CString strFlightType){m_strFlightType = strFlightType;}
	//CString GetFltType(){return m_strFlightType;}
	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nProjID,CString strFltType);

private:
	int m_nID;
//	CString m_strFlightType;
	FlightConstraint m_fltType;
	SideStepApproachRunwayVector m_vApproachRunway;
	SideStepApproachRunwayVector m_vInvalidApproachRunway;
	CAirportDatabase* m_pAirportDB;
};

//-------------------CSideStepSpecification---------------------
class INPUTAIRSIDE_API CSideStepSpecification
{
public:
	typedef std::vector<CSideStepFlightType*> SideStepFlightTypeVector;
	typedef std::vector<CSideStepFlightType*>::iterator SideStepFlightTypeIter;
public:
	explicit CSideStepSpecification(int nProjID);
	~CSideStepSpecification(void);

	void ReadData();
	void SaveData();

	int GetFlightTypeCount();
	CSideStepFlightType* GetFlightTypeItem(int nIndex);

	void AddFlightTypeItem(CSideStepFlightType* pItem);
	void DeleteFlightTypeItem(CSideStepFlightType* pItem);
	void RemoveAll();
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	static void ExportSideStepSpecification(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	static void ImportSideStepSpecification(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
private:
	int m_nProjID;
	SideStepFlightTypeVector m_vFlightType;
	SideStepFlightTypeVector m_vInvalidFlightType;
	CAirportDatabase* m_pAirportDB;
};

