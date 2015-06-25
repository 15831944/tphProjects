#pragma once
#include "Common\elaptime.h"
#include "AirsideControllerWorkloadReport.h"
#include <vector>

class CAirsideControllerWorkloadParam;
class CXListCtrl;
class CARC3DChart;
class CParameters;
namespace AirsideControllerWorkload
{
	class CResultItem
	{
	public:
		CResultItem();
		~CResultItem();
	public:
		//member
		ElapsedTime m_eStartTime;
		ElapsedTime m_eEndTime;


		int m_nToatalIndex;

		int  m_nPushBacks;
		int  m_nTaxiStart;
		int  m_nTakeOff_NoConflict;
		int  m_nTakeOff_Conflict;
		int  m_nLanding_NoConflict;
		int  m_nLanding_Conflict;
		int  m_nGoArround;
		int  m_nVacateRunway;
		int  m_nCrossActive;
		int  m_nHandOffGround;
		int  m_nHandOffAir;
		int  m_nAltitudeChange;
		int  m_nVectorAir;
		int  m_nHolding;
		int  m_nDirectRoute_Air;
		int  m_nRerouteGround;
		int  m_nContribution;
		int  m_nExtendedDownwind;

	private:
	};


	class CReportResult
	{
	public:
		CReportResult(void);
		~CReportResult(void);

	public:
		void GenerateResult(CBGetLogFilePath pCBGetLogFilePath, CAirsideControllerWorkloadParam *pParam);
		void GenerateIntervalTime(CAirsideControllerWorkloadParam *pParam);
		void FillListContent( CXListCtrl& cxListCtrl, CAirsideControllerWorkloadParam * parameter );

		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

		CString FormartInt(int nInt) const;
	public:
		std::vector<CResultItem > m_vResult;


	protected:
		CString formatInt(int nValue) const;



	};

}
