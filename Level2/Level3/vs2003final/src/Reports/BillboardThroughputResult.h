#pragma once

#include <vector>
#include "..\common\elaptime.h"

class BillboardThroughputItem
{
public:
	BillboardThroughputItem()
	{
		m_startTime = 0L;
		m_endTime = 0L;
		m_lExposureCount = 0L;
		m_lTotalThroughput = 0L;	
	}

	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	long m_lExposureCount;
	long m_lTotalThroughput;

};

typedef std::vector<BillboardThroughputItem> BILLBOARDTHROUGHTOUTRESULT;


class CBillboardThroughputResult
{
private:
	BILLBOARDTHROUGHTOUTRESULT m_vResult;
public:
	CBillboardThroughputResult(void);
	~CBillboardThroughputResult(void);

	void InitItem( ElapsedTime _startTime, ElapsedTime _intervalTime,int _iIntervalCount );
	//0,total,1 exposure count
	void IncreaseCount( ElapsedTime& _time ,int nTotalOrExposure );
	
	//stop count 
	void IncreaseExposureCount( ElapsedTime& _time )
	{
		IncreaseCount(_time,1);
	}
	//throughput count
	void IncreaseTotalCount(ElapsedTime& _time )
	{
		IncreaseCount(_time,0);
	}

	int GetResultItemCount () const { return m_vResult.size(); };
	BillboardThroughputItem& operator [] ( int _iIndex );

};
