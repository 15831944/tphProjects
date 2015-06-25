#pragma once
#include "AirsideReportBaseResult.h"
#include "TaxiwayDelayReport.h"
class CTaxiwayDelayReportDetailDataItem ;
class CTaxiwayDelayParameters ;
class CTaxiwayDelayReportResult : public CAirsideReportBaseResult
{
public:
	CTaxiwayDelayReportResult(CTaxiwayDelayParameters* _parmeter) ;
	virtual ~CTaxiwayDelayReportResult(void) ;
protected:
	CTaxiwayDelayParameters* m_parmeter ;
public:
		virtual void GenerateResult() = 0;
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
};

class CTaxiwayDelayDetailReportResult : public CTaxiwayDelayReportResult
{
protected:
		class CSegmentDelay
		{
		public:
			ElapsedTime m_startTime ;
			ElapsedTime m_endTime ;

			ElapsedTime m_ArrivalDelay ;
			ElapsedTime m_DepDelay ;
		public:
			void AddDelayData(CTaxiwayDelayReportDetailDataItem* _delayData) 
			{
				if(_delayData == NULL)
					return ;
				if(!_delayData->m_IsDep)
					m_ArrivalDelay += _delayData->m_DelayTime ;
				else
					m_DepDelay += _delayData->m_DelayTime ;
			}
		};
		class CTaxiDelayReportData 
		{
		public:
			CString m_TaxiwayName ;
			CString m_FromNodeName ;
			CString m_ToNodeName ;
		public:
			std::vector<CSegmentDelay*> m_Data ;
		public:
			CTaxiDelayReportData(ElapsedTime _startTime , int intervalNum,ElapsedTime _interval) 
			{
				CSegmentDelay* segmentDelay = NULL ;
				ElapsedTime startTime = _startTime ;
				for (int i = 0 ; i < intervalNum ;i++)
				{
					segmentDelay = new CSegmentDelay ;
					segmentDelay->m_startTime = startTime ;
					segmentDelay->m_endTime = startTime + _interval ;
					startTime = segmentDelay->m_endTime ;

					m_Data.push_back(segmentDelay) ;
				}
			};
			~CTaxiDelayReportData()
			{
				for (int i = 0 ; i < (int)m_Data.size() ;i++)
				{
					delete m_Data[i] ;
				}
				m_Data.clear() ;
			}
		public:
			void AddDelayDataToDate(CTaxiwayDelayReportDetailDataItem* _delayData) 
			{
				CSegmentDelay* PsegmentDelay = 	FindSegmentData(_delayData) ;
				if(PsegmentDelay)
					PsegmentDelay->AddDelayData(_delayData) ;
			}
			CSegmentDelay* FindSegmentData(CTaxiwayDelayReportDetailDataItem* _dataItem)
			{
				for (int i = 0 ; i < (int)m_Data.size() ;i++)
				{
					if( _dataItem->m_startTime == m_Data[i]->m_startTime)
						return m_Data[i] ;
				}
				return NULL ;
			}
		};
public:
	CTaxiwayDelayDetailReportResult(std::vector<CTaxiwayDelayReportDetailDataItem*>* _dataSet,CTaxiwayDelayParameters* _parmeter):m_DetailReport(_dataSet),CTaxiwayDelayReportResult(_parmeter) 
	{} ;
	~CTaxiwayDelayDetailReportResult() 
	{
		ClearGraphData() ;
	};
protected:
	std::vector<CTaxiwayDelayReportDetailDataItem*>* m_DetailReport ;
	std::vector<CTaxiDelayReportData*> m_GraphData ;
	int m_Intervals ;
protected:
	CTaxiwayDelayDetailReportResult::CTaxiDelayReportData* FindData(CTaxiwayDelayReportDetailDataItem* _delayData);
	void AddDelayData(CTaxiwayDelayReportDetailDataItem* _delayData) ;
	void ClearGraphData() ;
public:
	virtual void GenerateResult() ;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
};


class CTaxiwayDelaySummaryReportResult : public CTaxiwayDelayReportResult
{
public:
	CTaxiwayDelaySummaryReportResult(std::vector<CTaxiwayDelayReportSummaryDataItem*>* _dataset ,CTaxiwayDelayParameters* _parmeter) ;
	~CTaxiwayDelaySummaryReportResult() {};
protected:
	std::vector<CTaxiwayDelayReportSummaryDataItem*>* m_SummaryData ;
public:
	virtual void GenerateResult() ;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
};