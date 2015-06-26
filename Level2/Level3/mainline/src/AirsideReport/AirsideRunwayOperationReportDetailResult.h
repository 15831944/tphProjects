#pragma once
#include "airsiderunwayoperationreportbaseresult.h"
#include "../Common/elaptime.h"
#include "..\InputAirside\ApproachSeparationType.h"
#include "AirsideReportNode.h"
#include <vector>
#include <map>
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../common/FLT_CNST.H"
#include "../Results/AirsideRunwayOperationLog.h"
#include "../Engine\Airside\AircraftClassificationManager.h"
#include "AirsideReportBaseResult.h"


class AircraftClassificationManager;


class AirsideRunwayOperationReportParam;
class CAirsideRunwayOperationReportDetailResult :
	public CAirsideRunwayOperationReportBaseResult
{
public:
	//lead - trail
	class WakeVortexDetailValue
	{
	public:
		WakeVortexDetailValue()
		{
			m_nClassLead = 0;
			m_nClassTrail = 0;
			m_nFlightCount = 0;
		}
		~WakeVortexDetailValue()
		{

		}

	public:
		//lead type
		int m_nClassLead;
		CString m_strClassLeadName;

		//trail type
		int m_nClassTrail;
		CString m_strClassTrailName;

		int m_nFlightCount;
	};
	
	class RunwayOperationTimeValue
	{
	public:
		RunwayOperationTimeValue()
		{
			m_eTime = ElapsedTime(0L);
			m_nLandingCount = 0;
			m_nTakeOffCount = 0;
		}
		~RunwayOperationTimeValue()
		{

		}

	public:
		//time start
		ElapsedTime m_eTime;
		//landing count
		int m_nLandingCount;
		//take off count
		int m_nTakeOffCount;
	protected:

	private:
	};
	class RunwayClassSubTypeValue
	{
	public:
		RunwayClassSubTypeValue()
		{
			m_nTypeID = -1;
			m_nCount = 0;
		}
		~RunwayClassSubTypeValue()
		{

		}

	public:
		//classification type id
		int m_nTypeID;
		//classification type name
		CString m_strTypeName;

		//the flight count of the type
		int m_nCount;


	};

	class RunwayClassValue
	{
	public:
		RunwayClassValue()
		{
			m_classType = WakeVortexWightBased;
		}
		~RunwayClassValue()
		{

		}
	public:
		FlightClassificationBasisType m_classType;
		//int: classtype index
		//int nflight count

		std::vector<RunwayClassSubTypeValue> m_vSubTypeValue;
	};

	//----------------------------------------------------------------------------------
	//Summary:
	//		new data for add interval and list view
	//-----------------------------------------------------------------------------------
	class FlightOperationData
	{
	public:
		FlightOperationData()
		{

		}
		~FlightOperationData()
		{

		}
	public:
		CString		m_sFlightID;
		long		m_lTime;
		CString		m_sRunway;
		FlightClassificationBasisType m_classType;
		CString		m_stypeName;
		CString		m_sLeadTrail;
		CString		m_sOperation;
	};

	class RunwayOperationDetail
	{
	public:
		RunwayOperationDetail()
		{
			m_nTotalLandingCount = 0;
			m_nTotalTakeOffCount = 0;
			m_nMissApproachCount = 0;
		}
		~RunwayOperationDetail()
		{

		}
	
	public:
		//landing
		CAirsideReportRunwayMark m_runWaymark;
        int m_nTotalLandingCount;

		std::vector<RunwayClassValue> m_vLandingClassValue;

		//take off
		int m_nTotalTakeOffCount;
		std::vector<RunwayClassValue> m_vTakeOffClassValue;

		int m_nMissApproachCount;
		//lead - trail information
		std::vector<WakeVortexDetailValue> m_vWakeVortexDetailValue;
	
		//time count
		std::vector<RunwayOperationTimeValue> m_vRunwayTimeValue;

		void InitRunwatTimeOperation(ElapsedTime eStartTime,ElapsedTime eEndTime, ElapsedTime eInterval);
		void InitClassValue(AircraftClassificationManager* pAircraftManager ,CParameters * parameter );
		void InitWakeVortex(AircraftClassificationManager* pAircraftManager );
		std::vector<AirsideRunwayOperationLog::RunwayLogItem *> m_vRunwayOperations;
		
	private:
	};



public:
	CAirsideRunwayOperationReportDetailResult(void);
	~CAirsideRunwayOperationReportDetailResult(void);

public:
	std::vector<RunwayOperationDetail *> m_vRunwayOperationDetail;
	//new data for generate result data
	std::vector<FlightOperationData>	m_vFlightOperateValue;
protected:
	AirsideRunwayOperationLog m_airsideOperationLog;
	AircraftClassificationManager m_airCraftClassicfication;
	AirsideRunwayOperationReportParam *m_pParameter;
	RunwayOperationDetail* GetRunwayDetail(CAirsideReportRunwayMark& runwayMark);
	

	void InitRunway(CParameters * parameter);

	std::vector<CString> m_lstColumns;
	void GetListColumns(std::vector<CString>& lstColumns);
public:

	void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	//refresh chart
	void RefreshReport(CParameters * parameter);

protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
public:
	void GenerateResult(CParameters *pParameter);
	void Draw3DChart();
	bool fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc);

	bool fitClassification(AirsideFlightDescStruct fltDesc,CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail* pRunwayDetail,
		AirsideRunwayOperationLog::RunwayLogItem* pOperationItem,CParameters * parameter );
};

namespace AirsideRunwayOperationReportDetail
{
	class LandingTakeOffChartResult: public CAirsideReportBaseResult
	{
	public:
		//for chart, means one runway's landing and take off information
		class  CRunwayOperationItem
		{
		public:
			//landing
			std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> m_vLanding;
			//take off
			std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> m_vTakeOff;
		protected:
			//get the operation count in the range from eOccTimeFrom to  eOccTimeTo
			int GetTakeoffCount(CString sRunway,ElapsedTime eTimeFrom, ElapsedTime eTimeTo);
			int GetLandingCount(CString sRunway,ElapsedTime eTimeFrom, ElapsedTime eTimeTo);

			void GetLandingRunway(std::vector<CString>& vRunway);
			void GetTakeoffRunway(std::vector<CString>& vRunway);

		public:
			void InitResultData(std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> vOperationData);
			int GetOperationCount(bool bLanding,CString sRunway,ElapsedTime eTimeFrom, ElapsedTime eTimeTo);
			void GetRunway(bool bLanding,std::vector<CString>& vRunway);
			bool empty(bool bLanding);
		};

		LandingTakeOffChartResult(bool bLanding);
		~LandingTakeOffChartResult();

	public:
		void GenerateResult(std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *> * vRunwayOperationDetail);
		void GenerateResult(std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> vOperationData);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	protected:
		std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *>* m_vRunwayOperationDetail;
		CRunwayOperationItem m_OperationData;
	private:
		bool m_bLanding;
	};
	class LeadTrailChartResult : public CAirsideReportBaseResult
	{
	public:
		LeadTrailChartResult();
		~LeadTrailChartResult();
	public:
		void GenerateResult(std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *> * vRunwayOperationDetail);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	protected:
		std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *>* m_vRunwayOperationDetail;

	};

	class MovementsPerIntervalChartResult :public CAirsideReportBaseResult
	{
	public:
		MovementsPerIntervalChartResult();
		~MovementsPerIntervalChartResult();

	public:
		void GenerateResult(std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *> * vRunwayOperationDetail);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	protected:
		std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *>* m_vRunwayOperationDetail;

	};


}

































