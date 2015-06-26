#pragma once
#include "LSVehicleThroughputResult.h"

class LANDSIDEREPORT_API LSThroughputDetailResult : public LSVehicleThroughputResult
{
public:
	LSThroughputDetailResult(void);
	~LSThroughputDetailResult(void);
	enum ChartType
	{
		CT_PAXSERVED = 0,
	};

	class ThroughputInterval
	{
	public:
		ThroughputInterval()
		{

		}
		~ThroughputInterval()
		{

		}

		ElapsedTime m_eStartTime;
		ElapsedTime m_eEndTime;

		std::vector<ThroughputResourceData> m_vThrougput;
	};

	class ThroughputDetailData
	{
	public:
		ThroughputDetailData()
			:m_iType(0)
		{

		}
		~ThroughputDetailData()
		{

		}

		bool operator == (const ThroughputDetailData& tgData)const
		{
			if (m_resouceName != tgData.m_resouceName)
				return false;

			if (m_iType != tgData.m_iType)
				return false;

			return true;
		}

		CString m_resouceName;
		int m_iType; //0, resouce of landside. 1, portal
		std::vector<ThroughputInterval> m_vInterval;
	};
	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );
	virtual LSGraphChartTypeList GetChartList() const;

	virtual BOOL ReadReportData( ArctermFile& _file );

	virtual BOOL WriteReportData( ArctermFile& _file );
private:
	void InitResultData(ThroughputDetailData& tgData,LandsideBaseParam* pParameter);
	void CaculateVehicleCorssResult(const std::vector<LandsideVehicleEventStruct>& vPointEvent,int nVehicleID,LSVehicleThroughputParam* pThroughputPara);

	void DrawPaxServedChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	CString GetXtitle( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const;
protected:
	std::vector<ThroughputDetailData> m_vDetailResult;
};
