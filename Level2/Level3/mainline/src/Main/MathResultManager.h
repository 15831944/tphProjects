// MathResultManager.h: interface for the CMathResultManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHRESULTMANAGER_H__578131E0_147C_41BE_A393_0D5409500E98__INCLUDED_)
#define AFX_MATHRESULTMANAGER_H__578131E0_147C_41BE_A393_0D5409500E98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "../Common/elaptime.h"

struct MathResultRecord
{
	ElapsedTime time;
	std::vector<double> vProcResults;
};

struct MathResult
{
	std::vector<std::string> m_vstrProcess;
	std::vector<MathResultRecord> m_vResult;
};

class CMathResultManager
{
public:
	CMathResultManager();
	virtual ~CMathResultManager();

	void LoadResult();
	void SaveResult();
	void ClearResult();
	void SetResultFileDir(LPCTSTR lpszFileDir) { m_strResultFileDir = lpszFileDir; }
	MathResult& GetQTimeResult() { return m_rsQTime; }
	MathResult& GetQLengthResult() { return m_rsQLength; }
	MathResult& GetThroughputResult() { return m_rsThroughput; }
	MathResult& GetPaxCountResult() { return m_rsPaxCount; }
	MathResult& GetUtilizationResult() { return m_rsUtilization; }

protected:
	CString m_strResultFileDir;

	MathResult m_rsQTime;
	MathResult m_rsQLength;
	MathResult m_rsThroughput;
	MathResult m_rsPaxCount;
	MathResult m_rsUtilization;

	void CheckResult(MathResult& rs, double dDefaultValue = 0.0);
};

#endif // !defined(AFX_MATHRESULTMANAGER_H__578131E0_147C_41BE_A393_0D5409500E98__INCLUDED_)
