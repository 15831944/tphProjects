#pragma once
#include "../Reports/StatisticalTools.h"
#include "AirsideReportSummaryData.h"
#include "../Common/ALTObjectID.h"
class ArctermFile;
///stand operation data for detail and summary base class/////////////////////////////////////////////
class AIRSIDEREPORT_API CTaxiwayUtilizationData
{
public:
	enum TaxiwayUtilizationDataType
	{
		TaxiwayUtilization_Detail_Data,
		TaxiwayUtilization_Summary_Data
	};
	CTaxiwayUtilizationData()
		:m_sTaxiway(_T(""))
		,m_sStartNode(_T(""))
		,m_sEndNode(_T(""))
	{

	}
	virtual ~CTaxiwayUtilizationData(){}
	virtual TaxiwayUtilizationDataType getType()const = 0;
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	bool operator ==(const CTaxiwayUtilizationData& data)const;
	bool operator < (const CTaxiwayUtilizationData& data)const;

public:
	CString m_sTaxiway;
	CString m_sStartNode;
	CString m_sEndNode;
};

/////////////////////////////detail data/////////////////////////////////////////////////////////////
struct TaxiwayUtilizationFlightInfo
{
	TaxiwayUtilizationFlightInfo()
		:m_sAirline(_T(""))
		,m_sActype(_T(""))
		,m_lMovements(0)
		,m_avaSpeed(0)
		,m_lOccupancy(0)
	{

	}

	bool operator == (const TaxiwayUtilizationFlightInfo& infData)const
	{
		if(m_sAirline == infData.m_sAirline &&
			m_sActype == infData.m_sActype)
			return true;
		return false;
	}

 	bool operator > (const TaxiwayUtilizationFlightInfo& infData)const
 	{
 		return (m_lMovements > infData.m_lMovements);
 	}

	bool operator < (const TaxiwayUtilizationFlightInfo& infData)const
	{
		return (m_lMovements < infData.m_lMovements);
	}
	CString m_sAirline;
	CString m_sActype;
	long m_lMovements;
	double m_avaSpeed;
	long m_lOccupancy;
};

typedef std::vector<TaxiwayUtilizationFlightInfo> FlightInfoList;
class CTaxiwayUtilizationDetailData : public CTaxiwayUtilizationData
{
public:
	CTaxiwayUtilizationDetailData();
	~CTaxiwayUtilizationDetailData();

public:
	long m_intervalStart;
	long m_intervalEnd;
	long m_totalMovement;
	double m_avaSpeed;
	double m_minSpeed;
	double m_maxSpeed;
	double m_sigmaSpeed;
	long m_lOccupiedTime;
	FlightInfoList m_vFlightInfo;
	CStatisticalTools<double> m_statisticalTool;


public:
	virtual TaxiwayUtilizationDataType getType()const {return TaxiwayUtilization_Detail_Data;}
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
	TaxiwayUtilizationFlightInfo* GetFlightInfo(const CString& sAirline,const CString& sActype);
};

///////////////////////////summary data//////////////////////////////////////////////////////////////
struct TaxiwayUtilzationSummaryData 
{
	TaxiwayUtilzationSummaryData()
		:m_dTotalValue(0)
		,m_dMinValue(0)
		,m_dMaxValue(0)
		//,m_lIntervalOfMin(0)
		,m_lIntervalOfMax(0)
		,m_dVerageValue(0.0)
	{

	}
	double m_dTotalValue;
	double m_dMinValue;
	double m_dMaxValue;
//	long m_lIntervalOfMin;
	long m_lIntervalOfMax;
	double m_dVerageValue;
	CAirsideReportSummaryData m_statisticalTool;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
};
class CTaxiwayUtilizationSummaryData : public CTaxiwayUtilizationData
{
public:
	CTaxiwayUtilizationSummaryData();
	virtual ~CTaxiwayUtilizationSummaryData();

	TaxiwayUtilzationSummaryData m_movementData;
	TaxiwayUtilzationSummaryData m_speedData;
	TaxiwayUtilzationSummaryData m_occupancyData;

public:
	virtual TaxiwayUtilizationDataType getType()const {return TaxiwayUtilization_Summary_Data;}
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
};
