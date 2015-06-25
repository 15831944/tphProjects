#pragma once
#include "AirsideRunwayCrossingsBaseResult.h"
#include "../Common/ALTObjectID.h"
class CARC3DChart;
class CParameters;
class CRunwayCrossingBaseDetailResult
{
public:
	CRunwayCrossingBaseDetailResult();
	virtual ~CRunwayCrossingBaseDetailResult();

	class RunwayCrossingDataItem
	{
	public:
		RunwayCrossingDataItem()
		{

		}
		~RunwayCrossingDataItem()
		{

		}
	public:
		ALTObjectID m_runwayID;
		std::vector<double>m_vCrossingsData;
	};

	class RunwayCrossingNodeData
	{
	public:
		RunwayCrossingNodeData()
		{

		}
		~RunwayCrossingNodeData()
		{

		}
	public:
		CString m_sNodeName;
		std::vector<double>m_vCrossingCount;
	};
protected:
	std::vector<CString> m_vTimeRange;
	std::vector<RunwayCrossingDataItem>m_vData;
	std::vector<RunwayCrossingNodeData>m_vCountData;
public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	std::vector<ALTObjectID> GetRunwayList(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult);
	std::vector<CString> GetNodeNameList(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult);
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CRunwayCrossingDetailCountResult : public CRunwayCrossingBaseDetailResult
{
public:
	CRunwayCrossingDetailCountResult();
	~CRunwayCrossingDetailCountResult();

public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CRunwayCrossingDetailWaitTimeResult : public CRunwayCrossingBaseDetailResult
{
public:
	CRunwayCrossingDetailWaitTimeResult();
	~CRunwayCrossingDetailWaitTimeResult();

public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CRunwayCrossingDetailRunwayBayTaxiwayResult : public CRunwayCrossingBaseDetailResult
{
public:
	CRunwayCrossingDetailRunwayBayTaxiwayResult();
	~CRunwayCrossingDetailRunwayBayTaxiwayResult();

public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};