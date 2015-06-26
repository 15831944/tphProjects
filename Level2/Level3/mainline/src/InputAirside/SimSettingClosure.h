#pragma once 
#include "../Common/elaptime.h" 
#include "AirsideImportExportManager.h"
#include "../Database/adodatabase.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CRunwayClosureNode
{
public:
	CRunwayClosureNode(void);
	CRunwayClosureNode(CADORecordset &adoRecordset);
	virtual ~CRunwayClosureNode(void);
protected:
	int m_nID;//id
	int m_nParentID;//parent id ,i.e. CClosureAtAirport's id
public:
	int m_nRunwayID;//runway id
	int m_nIntersectNodeIDFrom;//intersect node id from...
	int m_nIntersectNodeIDTo;//intersect node id to...
	ElapsedTime m_startTime;//start time
	ElapsedTime m_endTime;//end time 
public:
	virtual void SaveData(int nParentID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CTaxiwayClosureNode
{
public:
	CTaxiwayClosureNode(void);
	CTaxiwayClosureNode(CADORecordset &adoRecordset);
	virtual ~CTaxiwayClosureNode(void);
protected:
	int m_nID;//id
	int m_nParentID;//parent id ,i.e. CClosureAtAirport's id
public:
	int m_nTaxiwayID;//taxiway id 
	int m_nIntersectNodeIDFrom;//intersect node id from...
	int m_nIntersectNodeIDTo;//intersect node id to...
	ElapsedTime m_startTime;//start time
	ElapsedTime m_endTime;//end time 
public:
	virtual void SaveData(int nParentID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CStandGroupClosureNode
{
public:
	CStandGroupClosureNode(void);
	CStandGroupClosureNode(CADORecordset &adoRecordset);
	virtual ~CStandGroupClosureNode(void);
protected:
	int m_nID;//id
	int m_nParentID;//parent id ,i.e. CClosureAtAirport's id
public:
	int m_nStandGroupID;//taxiway id 
	ElapsedTime m_startTime;//start time
	ElapsedTime m_endTime;//end time 
public:
	virtual void SaveData(int nParentID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CClosureAtAirport
{
public:
	CClosureAtAirport(void); 
	CClosureAtAirport(CADORecordset &adoRecordset); 
	virtual ~CClosureAtAirport(void);
protected:
	int m_nID;//id 
	int m_nProjID;//porject id
public:
	int m_nAirportID;//airport id
	int m_nObjectMark;//obect mark:1,runway;2,taxiway;3,stand group;
public:
	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CRunwayClosureAtAirport:public CClosureAtAirport
{
public:
	CRunwayClosureAtAirport(void);
	CRunwayClosureAtAirport(CADORecordset &adoRecordset);
	virtual ~CRunwayClosureAtAirport(void); 
public:
	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	CRunwayClosureNode * FindRunwayClosureNode(int nRunwayID);
	bool AddNode(CRunwayClosureNode * pNewNode);
	bool DeleteNode(CRunwayClosureNode * pNodeDelete);

	int GetRunwayClosureNodeCount(void);
	CRunwayClosureNode * GetRunwayClosureNode(int nIndex);
protected:
	std::vector <CRunwayClosureNode * > m_vrRunwayClosure;
	std::vector <CRunwayClosureNode * > m_vrRunwayClosureDeleted;//deleted
protected:
	void Clear(void);
};


class INPUTAIRSIDE_API CTaxiwayClosureAtAirport:public CClosureAtAirport
{
public:
	CTaxiwayClosureAtAirport(void);
	CTaxiwayClosureAtAirport(CADORecordset &adoRecordset);
	virtual ~CTaxiwayClosureAtAirport(void); 
public:
	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	CTaxiwayClosureNode * FindTaxiwayClosureNode(int nTaxiwayID);
	bool AddNode(CTaxiwayClosureNode * pNewNode);
	bool DeleteNode(CTaxiwayClosureNode * pNodeDelete);

	int GetTaxiwayClosureNodeCount(void);
	CTaxiwayClosureNode * GetTaxiwayClosureNode(int nIndex);
protected:
	std::vector <CTaxiwayClosureNode * > m_vrTaxiwayClosure;
	std::vector <CTaxiwayClosureNode * > m_vrTaxiwayClosureDeleted; //deleted
protected:
	void Clear(void);
};


class INPUTAIRSIDE_API CStandGroupClosureAtAirport:public CClosureAtAirport
{
public:
	CStandGroupClosureAtAirport(void);
	CStandGroupClosureAtAirport(CADORecordset &adoRecordset);
	virtual ~CStandGroupClosureAtAirport(void); 
public:
	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	CStandGroupClosureNode * FindStandGroupClosureNode(int nStandGroupID);
	bool AddNode(CStandGroupClosureNode * pNewNode);
	bool DeleteNode(CStandGroupClosureNode * pNodeDelete);

	int GetStandGroupClosureNodeCount(void);
	CStandGroupClosureNode * GetStandGroupClosureNode(int nIndex);
protected:
	std::vector <CStandGroupClosureNode * > m_vrStandGroupClosure;
	std::vector <CStandGroupClosureNode * > m_vrStandGroupClosureDeleted; //deleted
protected:
	void Clear(void);
};

class INPUTAIRSIDE_API CClosure
{
public:
	CClosure(int nProjID);
	virtual ~CClosure(void);
public:
	virtual CClosureAtAirport * FindClosure(int nAirport);

	virtual int GetAirportCount(void);
	virtual CClosureAtAirport * GetClosureAtAirport(int nIndex);

	virtual bool AddClosure(CClosureAtAirport * pNewAirportNode);
	virtual bool DeleteClosure(CClosureAtAirport * pAirportNodeDelete); 

	virtual void ReadData(void);
	virtual void SaveData(void);
	virtual void DeleteData(void);
	virtual void ImportData(CAirsideImportFile& importFile);
	virtual void ExportData(CAirsideExportFile& exportFile);

	virtual void Clear(void);
protected:
	int m_nProjID;//project
	int m_nObjectMark;//obect mark:1,runway;2,taxiway;3,stand group;
	std::vector<CClosureAtAirport * > m_vrClosureAtAirport;
	std::vector<CClosureAtAirport * > m_vrClosureAtAirportDeleted;//deleted
};

class INPUTAIRSIDE_API CAirsideRunwayClosure:public CClosure
{
public:
	CAirsideRunwayClosure(int nProjID);
	virtual ~CAirsideRunwayClosure(void);
public:
	CRunwayClosureAtAirport * FindRunwayClosure(int nAirport);

	int GetAirportCount(void);
	CRunwayClosureAtAirport * GetRunwayClosureAtAirport(int nIndex);

	bool AddRunwayClosure(CRunwayClosureAtAirport * pNewNode);
	bool DeleteRunwayClosure(CRunwayClosureAtAirport * pNodeDelete);
};

class INPUTAIRSIDE_API CAirsideTaxiwayClosure:public CClosure 
{
public:
	CAirsideTaxiwayClosure(int nProjID);
	virtual ~CAirsideTaxiwayClosure(void);
public:
	CTaxiwayClosureAtAirport* FindTaxiwayClosure(int nAirport);
	
	int GetAirportCount(void);
	CTaxiwayClosureAtAirport * GetTaxiwayClosureAtAirport(int nIndex);

	bool AddTaxiwayClosure(CTaxiwayClosureAtAirport * pNewNode);
	bool DeleteTaxiwayClosure(CTaxiwayClosureAtAirport * pNodeDelete);
};

class INPUTAIRSIDE_API CAirsideStandGroupClosure:public CClosure
{
public:
	CAirsideStandGroupClosure(int nProjID);
	virtual ~CAirsideStandGroupClosure(void);
public:
	CStandGroupClosureAtAirport* FindStandGroupClosure(int nAirport);

	int GetAirportCount(void);
	CStandGroupClosureAtAirport * GetStandGroupClosureAtAirport(int nIndex);

	bool AddStandGroupClosure(CStandGroupClosureAtAirport * pNewNode);
	bool DeleteStandGroupClosure(CStandGroupClosureAtAirport * pNodeDelete);
};

class INPUTAIRSIDE_API CAirsideSimSettingClosure
{
public:
	CAirsideSimSettingClosure(int nProjID);
	virtual ~CAirsideSimSettingClosure(void);
public:
	virtual void ReadData(void);
	virtual void SaveData(void);
	virtual void DeleteData(void);
	virtual void ImportData(CAirsideImportFile& importFile);
	virtual void ExportData(CAirsideExportFile& exportFile);
public:
	static void ImportSimSettingClosure(CAirsideImportFile& importFile);
	static void ExportSimSettingClosure(CAirsideExportFile& exportFile);


	CAirsideTaxiwayClosure& getTaxiwayClosure();
	bool IsVehicleService();
	bool AllowCyclicGroundRoute();
	void SetVehicleService(bool bService){ m_bVehicleService = bService; }
	void SetCyclicGroundRoute(bool bCyclic){ m_bAllowCyclicGroundRoute = bCyclic; }

	bool IsItinerantFlight() const;
	void SetItinerantFlight(bool bEnable);

	bool IsTrainingFlight() const;
	void SetTrainingFlight(bool bEnable);

	

protected:
	void Clear(void);
protected:
	int m_nProjID;
	int m_nID;
	bool m_bVehicleService;
	bool m_bAllowCyclicGroundRoute;

	// 0, disable; 1, enable
	int m_nItinerantFlight;
	int m_nTrainingFlight;

public:
	CAirsideRunwayClosure m_airsideRunwayClosure;
	CAirsideTaxiwayClosure m_airsideTaxiwayClosure;
	CAirsideStandGroupClosure m_airsideStandGroupClosure;

};