#pragma once
#include "LSVehicleThroughputResult.h"
class ALTObjectID;

class LANDSIDEREPORT_API LSThroughputSummaryResult : public LSVehicleThroughputResult
{
public:
	enum ChartType
	{
		CT_VEHICLEBYGROUP = 0,
		CT_VEHICLEPERRES,
		CT_GROUPTHROUGHPUT,
		CT_RESTHROUGHPUTPERHOUR,
	};
	class ThroughputSummaryData
	{
	public:
		ThroughputSummaryData()
			:m_iType(0)
		{

		}
		~ThroughputSummaryData()
		{

		}

		bool operator == (const ThroughputSummaryData& tgData)const
		{
			if (tgData.m_iType != m_iType)
				return false;
			
			if (tgData.m_resouceName != m_resouceName)
				return false;
			
			return true;
		}
		CString m_resouceName;
		int m_iType; //0, resouce of landside. 1, portal
		std::vector<ThroughputResourceData> m_vThroughput;
	};
	LSThroughputSummaryResult(void);
	~LSThroughputSummaryResult(void);

	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);

	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;

	virtual BOOL ReadReportData( ArctermFile& _file );

	virtual BOOL WriteReportData( ArctermFile& _file );

private:
	void CaculateVehicleCorssResult(const std::vector<LandsideVehicleEventStruct>& vPointEvent,int nVehicleID,LSVehicleThroughputParam* pThroughputPara);

	int GetResourceCount(const ALTObjectID& objID);
	int GetResourceVehicleCount(const ALTObjectID& objID);
	int GetPortalVehicleCount();

	void DrawVehicleByGroupChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	void DrawVehiclePerResChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	void DrawGroupThroughputChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	void DrawResThroughputChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
private:
	std::vector<ThroughputSummaryData> m_vSummrayResult;
};
