#include "StdAfx.h"
#include ".\takeoffprocessdata.h"
#include "..\Common\termfile.h"
#include "AirsideTakeoffProcessSummaryResult.h"

CTakeoffProcessDetailData::CTakeoffProcessDetailData(void)
:m_lEnterQueue(0l)
,m_lExpectTimeToPosition(0l)
,m_lActualTimeToPosition(0l)
,m_lTakeoffQueueDelay(0l)
,m_lCountSameQueue(0l)
,m_lTotalCountQueue(0l)
,m_lHoldShortLine(0l)
,m_lDelayHoldShortLine(0l)
,m_lToPostionTime(0l)
,m_lArrivalPosition(0l)
,m_lTimeInPosition(0l)
,m_lTakeoffTime(0l)
,m_lTakeoffDelay(0l)
{
}



BOOL CTakeoffProcessDetailData::ReadReportData( ArctermFile& _file )
{
	_file.getInteger(m_lEnterQueue);
	_file.getField(m_sDistFromQueue.GetBuffer(1024),1024);
	m_sDistFromQueue.ReleaseBuffer();
	_file.getInteger(m_lExpectTimeToPosition);
	_file.getInteger(m_lActualTimeToPosition);

	_file.getInteger(m_lTakeoffQueueDelay);
	_file.getInteger(m_lCountSameQueue);

	_file.getInteger(m_lTotalCountQueue);
	_file.getInteger(m_lHoldShortLine);
	_file.getInteger(m_lDelayHoldShortLine);

	_file.getInteger(m_lToPostionTime);
	_file.getInteger(m_lArrivalPosition);

	_file.getInteger(m_lTimeInPosition);
	_file.getInteger(m_lTakeoffTime);
	_file.getInteger(m_lTakeoffDelay);

	_file.getField(m_sFlightID.GetBuffer(1024),1024);
	m_sFlightID.ReleaseBuffer();

	_file.getField(m_sFlightType.GetBuffer(1024),1024);
	m_sFlightType.ReleaseBuffer();

	_file.getField(m_sTakeoffPosition.GetBuffer(1024),1024);
	m_sTakeoffPosition.ReleaseBuffer();
	return TRUE;
}

BOOL CTakeoffProcessDetailData::WriteReportData( ArctermFile& _file )
{
	_file.writeInt(m_lEnterQueue);
	_file.writeField(m_sDistFromQueue);
	_file.writeInt(m_lExpectTimeToPosition);
	_file.writeInt(m_lActualTimeToPosition);

	_file.writeInt(m_lTakeoffQueueDelay);
	_file.writeInt(m_lCountSameQueue);

	_file.writeInt(m_lTotalCountQueue);
	_file.writeInt(m_lHoldShortLine);
	_file.writeInt(m_lDelayHoldShortLine);

	_file.writeInt(m_lToPostionTime);
	_file.writeInt(m_lArrivalPosition);

	_file.writeInt(m_lTimeInPosition);
	_file.writeInt(m_lTakeoffTime);
	_file.writeInt(m_lTakeoffDelay);

	_file.writeField(m_sFlightID);
	_file.writeField(m_sFlightType);
	_file.writeField(m_sTakeoffPosition);
	return TRUE;
}

BOOL CTakeoffProcessSummaryData::ReadReportData( ArctermFile& _file )
{
	_file.getField(m_sFlightType.GetBuffer(1024),1024);
	m_sFlightType.ReleaseBuffer();
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();
	for (int i = 0; i < nCount; i++)
	{
		CTakeoffProcessDetailData newData;
		newData.ReadReportData(_file);
		_file.getLine();
		detailDataList.push_back(newData);

		m_TakeoffQueue.AddNewData(newData.m_lTakeoffQueueDelay);
		m_TimeHoldShortLine.AddNewData(newData.m_lHoldShortLine);
		m_TaixToPosition.AddNewData(newData.m_lToPostionTime);
		m_TimeToPosition.AddNewData(newData.m_lArrivalPosition);
	}
	m_TakeoffQueue.SortData();
	m_TimeHoldShortLine.SortData();
	m_TaixToPosition.SortData();
	m_TimeToPosition.SortData();
	return TRUE;
}

BOOL CTakeoffProcessSummaryData::WriteReportData( ArctermFile& _file )
{
	_file.writeField(m_sFlightType.GetBuffer(1024));
	m_sFlightType.ReleaseBuffer();
	int nCount = (int)detailDataList.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int i = 0; i < nCount; i++)
	{
		CTakeoffProcessDetailData& data = detailDataList[i];
		data.WriteReportData(_file);
		_file.writeLine();
	}

	return TRUE;
}

CStatisticalTools<double>& CTakeoffProcessSummaryData::GetResultDataByType( int emChartType )
{
	switch(emChartType)
	{
	case CAirsideTakeoffProcessSummaryResult::TakeOffQueueTime:
		{
			return m_TakeoffQueue;
		}
		break;
	case CAirsideTakeoffProcessSummaryResult::TimeAtHoldShortLine:
		{
			return m_TimeHoldShortLine;
		}
		break;
	case CAirsideTakeoffProcessSummaryResult::TaxiToPosition:
		{
			return m_TaixToPosition;
		}
		break;
	case CAirsideTakeoffProcessSummaryResult::TimeInPosition:
		{
			return m_TimeToPosition;
		}
		break;
	default:
		return m_TakeoffQueue;
	}
}