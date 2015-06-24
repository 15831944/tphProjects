// MathProcessEngine.h: interface for the CMathProcessEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESSENGINE_H__859BB22D_461E_4399_9108_7E4EEE2CA8FF__INCLUDED_)
#define AFX_MATHPROCESSENGINE_H__859BB22D_461E_4399_9108_7E4EEE2CA8FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CMathProcessEngineFactory;
class CMathResultManager;
class ProbabilityDistribution;
struct MathResult;

static const double ZERO_PROBABILITY = 1e-6;

class CMathProcessEngine
{
	friend class CMathProcessEngineFactory;

protected:
	CMathProcessEngine();
	
public:
	virtual ~CMathProcessEngine();

	bool Run();
	void AddArrTime(double dArrTime);
	void SortArrTimes();
	void set_ServerCount(int nCount) { m_nServerCount = nCount; }
	void set_PrePaxCount(int nPaxCount) { m_nPrePassengerCount = nPaxCount; }
	void set_MathResManager(CMathResultManager* pMathResManager) { m_pMathResultManager = pMathResManager; }
	void set_Name(LPCTSTR lpszName) { m_strName = lpszName; }
	void set_Interval(int nInterval) { m_nInterval = nInterval;	}
	static double Times(int iNum);
	
	double get_QTime() const { return m_dQTime; }
	double get_QLength() const { return m_dQLength; }
	double get_Throughput() const { return m_dThroughput; }
	int get_PassengerCount() const { return m_nPassengerCount; }
	double get_Utilization() const { return m_dUtilization; }
	LPCTSTR get_Name() const { return m_strName; }
	int get_Interval() const { return m_nInterval; }
	std::vector<double>& get_ArrTime()  { return m_vArrTime; }
	std::vector<double>& get_LeaTime()  { return m_vLeaTime; }

protected:
	// parameters for this math engine
	std::vector<double> m_vArrTime;
	std::vector<double> m_vLeaTime;
	int m_nServerCount;
	ProbabilityDistribution* m_pServiceTimeProbDist;
	int m_nInterval;

	// results for this math engine
	double m_dQTime;			// Queue time
	double m_dQLength;			// Queue Length
	double m_dThroughput;		// Throughput
	int m_nPassengerCount;		// Number of Passenger in system
	int m_nPrePassengerCount;	// Number of Passenger in last time span
	double m_dUtilization;		// Utilization

	CString m_strName;
	CMathResultManager* m_pMathResultManager;
	int m_iStartTime;
	int m_nSpecialRho;
	int m_nBeginSpecialCount;

	bool Calculate(const std::vector<double>& vArrTime);
	virtual void CalculateNormal(const std::vector<double>& vArrTime, int nServerCount) = 0;
	virtual void CalculateNormalLeaveTime(const std::vector<double>& vArrTime, std::vector<double>& vLeaTime) = 0;
	void CalculateBusyLeaveTime(const std::vector<double>& vArrTime, std::vector<double>& vLeaTime, int nServerCount);

	bool CalculateBusy(const std::vector<double>& vArrTime, int nServerCount);
	bool CalculateSpecial(const std::vector<double>& vArrTime, int nServerCount);

	void AddResult();
	void AddDefaultResult();
	void AddResult(MathResult& rs, const double dValue, const double dDefaultValue = 0.0);
	static void PumpMessages();
};

#endif // !defined(AFX_MATHPROCESSENGINE_H__859BB22D_461E_4399_9108_7E4EEE2CA8FF__INCLUDED_)
