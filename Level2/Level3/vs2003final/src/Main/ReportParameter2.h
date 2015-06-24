// ReportParameter2.h: interface for the ReportParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTPARAMETER2_H__0A369C0D_A803_4D0B_9DC7_BF5B39F2F6B4__INCLUDED_)
#define AFX_REPORTPARAMETER2_H__0A369C0D_A803_4D0B_9DC7_BF5B39F2F6B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "common\elaptime.h"

class ReportParameter  
{
public:
	ReportParameter();
	virtual ~ReportParameter();

	void SetStartTime(const ElapsedTime& tStart) { m_tStart = tStart; }
	void SetEndTime(const ElapsedTime& tEnd) { m_tEnd = tEnd; }
	void SetInterval(int nInterval) { m_nInterval = nInterval; }
	void SetReportType(int nReportType) { m_nReportType = nReportType; }
	void SetThreshold(int nThreshold) { m_nThreshold = nThreshold; }
	void SetPassagerType(int nPassagerType) { m_nPassagerType = nPassagerType; }
	void SetReportParamter(const ElapsedTime& tStart, const ElapsedTime& tEnd, 
		                   int nInterval, int nReportType, 
						   int nThreshold, int nPassagerType);

	const ElapsedTime& GetStartTime() const { return m_tStart; }
	const ElapsedTime& GetEndTime() const { return m_tEnd; }
	int GetInterval() const { return m_nInterval; }
	int GetReportType() const { return m_nReportType; }
	int GetThreshold() const { return m_nThreshold; }
	int GetPassagerType() const { return m_nPassagerType; }
	void GetReportParamter(ElapsedTime& tStart, ElapsedTime& tEnd, 
		                   int& nInterval, int& nReportType, 
						   int& nThreshold, int& nPassagerType) const;

	BOOL LoadData(const CString& strPath);

private:
	ElapsedTime m_tStart;
	ElapsedTime m_tEnd;
	int m_nInterval;
	int m_nReportType;
	int m_nThreshold;
	int m_nPassagerType;
};

#endif // !defined(AFX_REPORTPARAMETER2_H__0A369C0D_A803_4D0B_9DC7_BF5B39F2F6B4__INCLUDED_)
