#pragma once
#include "ALTAirport.h"
#include "Runway.h"
#include "../common/elaptime.h"
#include "Taxiway.h"
#include "..\AirsideGUI\NodeViewDbClickHandler.h"
#include "../common/FLT_CNST.H"
#include "RunwayExitDiscription.h"
#include "InputAirsideAPI.h"

//class IntersectNodeList;
class IntersectNode;
class FlightConstraint;
class CAirportDatabase;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API CRunwayTakeOffTimeRangeFltType
{
public:
	CRunwayTakeOffTimeRangeFltType();
	~CRunwayTakeOffTimeRangeFltType(){}

	enum BACKTRACK
	{
		MaxBacktrack = 0,
		MinBacktrack,
		NoBacktrack
	};

public:
	void ReadData(CADORecordset& adoRecordset);
	int getID(){ return m_nID;}
	void setID(int nID){ m_nID = nID;}
	void DeleteData();
	void SaveData(int nTimeRangeID);
	void UpdateData(int nTimeRangeID);
	void SetTimeRangeID(int nTimeRangeID){m_nTimeRangeID = nTimeRangeID;}
	//void SetFltType(CString strFltType){m_strFltType = strFltType;}
	//CString GetFltType(){ return m_strFltType;}
	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nTimeRangeID);
	void ImportSaveData(int nTimeRangeID,CString strFltType);
	void SetBacktrack(BACKTRACK eBacktrack){ m_eBacktrackOp = eBacktrack; }
	BACKTRACK getBacktrakOp(){ return m_eBacktrackOp; }
	
protected:

	int m_nID;
//	CString m_strFltType;
	FlightConstraint m_fltType;
	int m_nTimeRangeID;
	CAirportDatabase* m_pAirportDB;
	BACKTRACK m_eBacktrackOp;
private:
};


class INPUTAIRSIDE_API CRunwayTakeOffTimeRangeData
{
public:
	CRunwayTakeOffTimeRangeData(){m_nID = -1;}
	~CRunwayTakeOffTimeRangeData();

public:
	int getID(){ return m_nID;}
	void setID(int nID){ m_nID = nID;}
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nRwTwIntID);
	void UpdateData(int nRwTwIntID);
	void DeleteData();
	void SetRwTwIntID(int nRwTwIntID){ m_nRwTwIntID = nRwTwIntID;}
	void SetStartTime(ElapsedTime& startTime){ m_startTime = startTime;}
	void SetEndTime(ElapsedTime& endTime){ m_endTime = endTime;}
	void AddData(CRunwayTakeOffTimeRangeFltType* pData){m_vFltType.push_back(pData);}
	void DelData(CRunwayTakeOffTimeRangeFltType* pData);
	void UpdateVData(CRunwayTakeOffTimeRangeFltType* pData1,CRunwayTakeOffTimeRangeFltType* pData2);
	ElapsedTime GetStartTime(){return m_startTime;}
	ElapsedTime GetEndTime(){return m_endTime;}
	int GetFltTypeCount(){	return (int)m_vFltType.size(); }
	CRunwayTakeOffTimeRangeFltType* GetFltTypeItem(int nIndex);
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nRwTwIntID);
protected:

	ElapsedTime m_startTime;
	ElapsedTime m_endTime;


	int m_nID;
	int m_nRwTwIntID;
	CAirportDatabase* m_pAirportDB;

	std::vector<CRunwayTakeOffTimeRangeFltType* > m_vFltType;
	std::vector<CRunwayTakeOffTimeRangeFltType* > m_vFltTypeNeedDel;
	void ReadFltType();
private:
};

//runway taxiway intersection data
class INPUTAIRSIDE_API CRunwayTakeOffPositionData
{
public:
	CRunwayTakeOffPositionData();
	~CRunwayTakeOffPositionData();


public:
	void setID(int nID){ m_nID = nID;}
	int getID(){ return m_nID;}

//	void setTaxiwayID(int nID){ m_nTaxiwayID = nID;}
	//int GetTaxiwayID(){ return m_nTaxiwayID;}


	void ReadData(CADORecordset &adoRecordset);

	void SaveData(int nRwMKDataID);
	void DeleteData();

	const RunwayExitDescription& GetRunwayExitDescription()const {return m_runwayExitDescription;}

	void CopyData(CRunwayTakeOffPositionData *pIntersecData);
	void Clear(){ m_vTimeRangeData.clear();  }

	void SetExitID(const RunwayExit& rwyExit);
	void AddData(CRunwayTakeOffTimeRangeData* pData){m_vTimeRangeData.push_back(pData);}
	void DelData(CRunwayTakeOffTimeRangeData* pData);
	void UpdateVData(CRunwayTakeOffTimeRangeData* pData1,CRunwayTakeOffTimeRangeData* pData2);
	int GetTimeCount(){	return (int)m_vTimeRangeData.size(); }
	CRunwayTakeOffTimeRangeData* GetTimeItem(int nIndex);
	void SetEligible(BOOL bEligible){m_bEligible = bEligible;}
	BOOL GetEligible(){return m_bEligible;}
	void UpdateData(int nRwMKDataID/*,int nTaxiwayID,int nOrder*/);
	void SetRemainFt(double dRemainFt){m_dRemainFt = dRemainFt;}
	double GetRemainFt(){return m_dRemainFt;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile,int nTaxiwayID,int nOrder);
	void ImportData(CAirsideImportFile& importFile,int nRwMKDataID);
protected:

	
	int m_nID;
	//int m_nTaxiwayID;
	int m_nRwyExitID;
	int m_nIntType;//intersection type;threshold,common

	RunwayExitDescription m_runwayExitDescription;
	//int m_nOrder;

	int m_nRunwayMarkDataID;

	BOOL m_bEligible;
	double m_dRemainFt;

private:
	void ReadTimeRangeData();
	std::vector<CRunwayTakeOffTimeRangeData *> m_vTimeRangeData;
	std::vector<CRunwayTakeOffTimeRangeData *> m_vTimeRangeDataNeedDel;
	CAirportDatabase* m_pAirportDB;

};

//class INPUTAIRSIDE_API CRunwayTakeOffRMTaxiwayData
//{
//public:
//	CRunwayTakeOffRMTaxiwayData();
//	~CRunwayTakeOffRMTaxiwayData();
//
//public:
//	//void InitializeData(); 
//	//int getTaxiwayID(){ return m_pIntersectObject.getID();}
//	//void setTaxiwayID(int nID){ m_pIntersectObject.ReadObject(nID);}
//	void setIntersectObject(ALTObject * pIntersectObject)
//	{
//		ASSERT(NULL == m_pIntersectObject);     //should not have been set before
//		m_pIntersectObject = pIntersectObject;
//	}
//	ALTObject* getIntersectObject(){return m_pIntersectObject;}
//
//	void AddRwyTakeoffPosData(CRunwayTakeOffPositionData *pData)
//	{
//		m_vInterData.push_back(pData);
//	}
//	
//	//taxiway or runway
//	ALTObject* m_pIntersectObject;
//
//	void SaveData(int nRWMKDataID);
//	void DeleteData();
//	void ReadData(int nParentID);
//
//	//bool InsertDataToStructure(CRunwayTakeOffPositionData *pIntersecData);
//
//	void InitTakeoffPostionsData(IntersectionNodeList& intersectNodeList);
//	int GetInterCount(){	return (int)m_vInterData.size(); }
//	CRunwayTakeOffPositionData* GetInterItem(int nIndex);
//	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
//	void ExportData(CAirsideExportFile& exportFile);
//	void ImportData(CAirsideImportFile& importFile,int nRWMKDataID);
//protected:
//	std::vector<CRunwayTakeOffPositionData *> m_vInterData;
//	CAirportDatabase* m_pAirportDB;
//
//private:
//};

//runway with mark data
class INPUTAIRSIDE_API CRunwayTakeOffRunwayWithMarkData 
{
public:
	CRunwayTakeOffRunwayWithMarkData(int m_nPrjID);
	~CRunwayTakeOffRunwayWithMarkData();

public:
	void setID(int nID){ m_nID = nID;}

	//void setAirportDataID(int nID){ m_nAirportDataID = nID;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadTakeoffPostionData();

	void SaveData(int nRunwayID,int nMarkIndex);
	void DeleteData();

	int getRunwayID(){return m_nRunwayID;}
	void setRunwayID(int nRunwayID){ m_nRunwayID = nRunwayID;}
	int getRunwayMarkIndex(){ return m_nMarkIndex;}
	void setRunwayMarkIndex(int nMarkIndex){ m_nMarkIndex = nMarkIndex;}

	//void InitTakeoffPostionsData(std::map<int,IntersectionNodeList>& mapIntersectNode);

	void AddRwyTakeoffPosData(CRunwayTakeOffPositionData* pData);
	void UpdateRwyTakeoffPosData(const RunwayExitList& exitList);
	void DelRwyTakeoffPosData();
	bool IfInvalidData(CRunwayTakeOffPositionData* pPosItem,const RunwayExitList& exitList);
	void RemoveInvalidData(const RunwayExitList& exitList);
	std::vector<CRunwayTakeOffPositionData *>& GetInvalidTakeoffPos(){return m_vInvalidRwyTakeoffPosData;}
	void KeepInvalidData();

	int GetRwyTakeoffPosCount(){	return (int)m_vRwyTakeoffPosData.size(); }
	CRunwayTakeOffPositionData* GetRwyTakeoffPosItem(int nIndex);
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile,int nRunwayID,int nMarkIndex);
	void ImportData(CAirsideImportFile& importFile);

protected:
	int m_nID;
	int m_nMarkIndex;
	std::vector<CRunwayTakeOffPositionData *> m_vRwyTakeoffPosData;
	std::vector<CRunwayTakeOffPositionData *> m_vInvalidRwyTakeoffPosData;

	bool IfRwyTakeoffPosDataIncludeRunwayExit(const RunwayExit& exit);
private:
	int m_nPrjID;
	int m_nRunwayID;

	//std::vector<CRunwayTakeOffPositionData *> m_vAllIntersectData;
	CAirportDatabase* m_pAirportDB;

public:

};

//runway data
class INPUTAIRSIDE_API CRunwayTakeOffRunwayData
{
public:
	CRunwayTakeOffRunwayData();
	~CRunwayTakeOffRunwayData();

public:
	Runway* getRunway(){ return &m_runway;}
	void ReadData(int nPrjID);
	void SaveData(int nProjectID);
	void DeleteData();
	void setRunway(const Runway& runway){ m_runway = runway;}
	bool InsertDataToStructure(CRunwayTakeOffRunwayWithMarkData *pRunwayMarkData);
	CRunwayTakeOffRunwayWithMarkData* getRunwayMark1Data(){return m_pRunwayMark1Data;}
	CRunwayTakeOffRunwayWithMarkData* getRunwayMark2Data(){return m_pRunwayMark2Data;}
	//void InitData();
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
protected:
	int m_nAirportDataID;
	int m_nID;

	Runway m_runway;
	CRunwayTakeOffRunwayWithMarkData* m_pRunwayMark1Data;
	CRunwayTakeOffRunwayWithMarkData* m_pRunwayMark2Data;
	CAirportDatabase* m_pAirportDB;
private:

	void UpdateData(int nAirportDataID);
};


class INPUTAIRSIDE_API CRunwayTakeOffAirportData
{
public:
	CRunwayTakeOffAirportData(){m_nID = -1;};
	~CRunwayTakeOffAirportData();


public:

	void setID(int nID){ m_nID = nID;}
	int getID(){ return m_nID;}

	void setAirportID(int nID){ m_airport.setID(nID);}
	int getAirportID(){ return m_airport.getID();}

	void setAirport(const ALTAirport& airport);
	ALTAirport* getAirport(){return &m_airport;}

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nProjectID);
	void DeleteData();
	void UpdateData(int nProjectID);
	void InitData(int nProjID);

	//bool InsertDataToStructure(CRunwayTakeOffRunwayWithMarkData *pRunwayMarkData);
	int GetRunwayCount(){	return (int)m_vRunwayTakeoffData.size(); }
	CRunwayTakeOffRunwayData* GetRunwayItem(int nIndex);
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
protected:
	ALTAirport m_airport;
	int m_nID;
	CAirportDatabase* m_pAirportDB;
	std::vector<CRunwayTakeOffRunwayData *> m_vRunwayTakeoffData;
	std::vector<CRunwayTakeOffRunwayData *> m_vInvalidRunwayTakeoffData;	



};

class INPUTAIRSIDE_API CRunwayTakeOffPosition
{
public:
	explicit CRunwayTakeOffPosition(int nProjID);
	~CRunwayTakeOffPosition(void);
	
public:
	void InitTakeOffPosition();
	void ReadData();
	void SaveData();
	int GetAirportCount(){	return (int)m_vAirportData.size(); }
	CRunwayTakeOffAirportData* GetAirportItem(int nIndex);
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	static void ExportRunwayTakeOffPosition(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportRunwayTakeOffPosition(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	bool IfHaveInvalidData();
	void KeepInvalidData();
	void RemoveInvalidData();
	void UpdateRwyTakeoffPosData();

protected:
	RunwayExitList GetRunwayExit(CRunwayTakeOffRunwayWithMarkData* runwayMark1Data)const;

//protected:
//	void InsertDataIntoStructure(std::vector<CRunwayTakeOffRunwayWithMarkData *>& vRunwayMakeData);

private:
	int m_nProjectID;
	CAirportDatabase* m_pAirportDB;
private:
	std::vector<CRunwayTakeOffAirportData *> m_vAirportData;
	std::vector<CRunwayTakeOffAirportData *> m_vInvalidAirportData;
};
