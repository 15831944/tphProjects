#pragma once
#include "AirsideIntersectionReportResult.h"


class CArisideIntersectionDetailBaseChartResult
{
public:
	CArisideIntersectionDetailBaseChartResult(){}
	virtual ~CArisideIntersectionDetailBaseChartResult(){}	

	virtual void GenerateResult(vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

protected:
private:
};


class CArisideIntersectionDetailCrossingChartResult : public CArisideIntersectionDetailBaseChartResult
{
public:
	CArisideIntersectionDetailCrossingChartResult(void);
	virtual ~CArisideIntersectionDetailCrossingChartResult(void);

	class CrossingData
	{
	public:
		CrossingData()
		{
			nNodeID = -1;
			nDelayCount = 0;
			nNonDelayCount = 0;
		}
		~CrossingData()
		{
		}
	public:
		int nNodeID;
		CString strNodeName;
		int nDelayCount;
		int nNonDelayCount;

	};

	//node id is key
	std::map<int, CrossingData> m_mapNodeCorssing;
public:
	virtual void GenerateResult(vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};

class CArisideIntersectionDetailBusyChartResult: public CArisideIntersectionDetailBaseChartResult
{
public:
	CArisideIntersectionDetailBusyChartResult();
	~CArisideIntersectionDetailBusyChartResult();
public:
	class BusyFactorData
	{
	public:
		BusyFactorData()
		{
			nNodeID = -1;
		}
		~BusyFactorData()
		{

		}

	public:
		int nNodeID;
		CString strNodeName;
		ElapsedTime eTime;
	};

	//node id is key
	std::map<int, BusyFactorData> m_mapNodeBusyFactor;
public:
	virtual void GenerateResult(vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};

class CArisideIntersectionDetailDelayChartResult: public CArisideIntersectionDetailBaseChartResult
{
public:
	CArisideIntersectionDetailDelayChartResult();
	~CArisideIntersectionDetailDelayChartResult();

public:
	class DelayTimeData
	{
	public:
		DelayTimeData()
		{
			nNodeID = -1;
		}
		~DelayTimeData()
		{

		}


	public:
		int nNodeID;
		CString strNodeName;
		ElapsedTime eTime;
	};



public:
	virtual void GenerateResult(vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

protected:
		//node id is key
	std::map<int, DelayTimeData> m_mapNodeDelayTime;
};


class CArisideIntersectionDetailConflictChartResult: public CArisideIntersectionDetailBaseChartResult
{
public:
	CArisideIntersectionDetailConflictChartResult();
	~CArisideIntersectionDetailConflictChartResult();
protected:
private:
public:
	class ConflictCountData
	{
	public:
		ConflictCountData()
		{
			nNodeID = -1;
			nCount = 0;
		}
		~ConflictCountData()
		{

		}


	public:
		int nNodeID;
		CString strNodeName;
		int nCount;
	};

public:
	virtual void GenerateResult(vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

protected:
		//node id is key
	std::map<int, ConflictCountData> m_mapNodeConflict;


};











































