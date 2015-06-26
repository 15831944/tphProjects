#pragma once
#include "../Database/DBElementCollection_Impl.h"
#include <iostream>
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
using namespace std;

enum WEATHERCONDITION
{
	DEFAULT_WEATHERCONDITION = 0,
	VFR_NONFREEZINGPRECP,
	VFR_FREEZINGDRIZZLE,
	IFR_NONFREEZINGPRECP,
	IFR_FREEZINGDRIZZLE,
	IFR_LIGHTSONW,
	IFR_MEDIUMSNOW,
	IFR_HEAVYSONW,
	CAT_NONFREEZINGPRECP,
	CAT_FREEZINGDRIZZLE,
	CAT_LIGHTSONW,
	CAT_MEDIUMSNOW,
	CAT_HEAVYSONW,


	//add item before this line
	WEATHERCONDITION_COUNT
};

class INPUTAIRSIDE_API CFlightPerformance : public DBElement
{
public:
	CFlightPerformance();
	~CFlightPerformance ();

public:
	void SetFlightType(CString FlightType);
	CString GetFlightType();
	void SetLandingDisPerf(double LandingDisperf);
	double GetLandingDisPerf();
	void SetTaxiSpedPerf(double TaxiSpedPerf);
	double GetTaxiSpedPerf();
	void SetHoldShortLineOffset(double HoldShortLineOffset);
	double GetHoldShortLineOffset();
	void SetTakeoffDisPerf(double TakeoffDisPerf);
	double GetTakeoffDisPerf();
	void SetDeicingTimeChange(double DeicingTimeChange);
	double GetDeicingTimeChange();

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID,WEATHERCONDITION WeatherCondition ,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	CString   m_FlightType;
	double     m_LandingDisPerf;
	double     m_TaxiSpedPerf;
	double     m_HoldShortLineOffset;
	double     m_TakeoffDisPerf;
	double     m_DeicingTimeChange;

public:
	//void ExportData(CAirsideExportFile& exportFile,WEATHERCONDITION WeatherCondition);
	void ImportData(CAirsideImportFile& importFile);

public:
	void Save(int nParentID,WEATHERCONDITION WeatherCondition);
};

class INPUTAIRSIDE_API CWeatherImpact : public DBElementCollection<CFlightPerformance>
{
public:
	typedef DBElementCollection<CFlightPerformance> BaseClass;
	CWeatherImpact();
	virtual ~CWeatherImpact(void);

public:
	void SetWeatherCondition(WEATHERCONDITION WeatherCondition);
	WEATHERCONDITION GetWeatherCondition();
	void GetItemByFlightType(CString strFlightType, CFlightPerformance ** ppFlightPerf);
	//DBElementCollection
	virtual void SaveData(int nParentID);
	//DBElement
	void InitFromDBRecordset(CADORecordset& recordset);
private:
	WEATHERCONDITION  m_WeatherCondition;

public:
	void ExportData(CAirsideExportFile& exportFile);
};

class INPUTAIRSIDE_API CWeatherImpactList : public DBElementCollection<CWeatherImpact>
{
public:
	typedef DBElementCollection<CWeatherImpact> BaseClass;
	CWeatherImpactList();

public:
	void GetItemByWeatherCondition(WEATHERCONDITION WeatherCondition, CWeatherImpact **WeathImpact);
	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;

	virtual void ReadData(int nProjectID);
	void InitFromDBRecordset(CADORecordset& recordset);

public:
	static void ExportWeatherImpact(CAirsideExportFile& exportFile);
	static void ImportWeatherImpcat(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};