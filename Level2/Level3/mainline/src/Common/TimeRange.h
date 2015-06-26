#pragma once

#include "Common/Elaptime.h"



class  TimeRange
{
public:
	TimeRange();
	TimeRange(ElapsedTime etStart, ElapsedTime etEnd);
	~TimeRange(void);

public:
	ElapsedTime GetStartTime()const{	return m_etStart; }
	ElapsedTime GetEndTime()const{	return m_etEnd; }
	void SetStartTime(ElapsedTime etStart){	m_etStart = etStart; }
	void SetEndTime(ElapsedTime etEnd){	m_etEnd = etEnd; }
	
	CString GetString()const;
	void GetString(CString& strText) const;
	//void Format(COleTimeSpan time)
	CString MakeDatabaseString()const;
	void ParseDatabaseString(const CString& strDatabaseString);

	CString ScreenPrint()const;
	bool isTimeInRange(const ElapsedTime& t)const;

	ElapsedTime GetRandTime()const;

	bool IsOverlapped(const TimeRange& passedRange) const;

	bool operator == (const TimeRange& timeRange)const;
	bool operator < (const TimeRange& timeRange)const;

private:
//	void ConvertElapsedTime(int& nDay, COleDateTime& time, const ElapsedTime& elapsedTime);
	ElapsedTime m_etStart;
	ElapsedTime m_etEnd;
};
