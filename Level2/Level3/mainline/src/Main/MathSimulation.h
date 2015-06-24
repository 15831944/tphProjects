// MathSimulation.h: interface for the CMathSimulation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHSIMULATION_H__5AC6D026_35DC_405F_9F64_B19803BFF1E4__INCLUDED_)
#define AFX_MATHSIMULATION_H__5AC6D026_35DC_405F_9F64_B19803BFF1E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CMathFlow;
class CMathProcess;
class CCompRepLogBar;
class CMathProcessEngine;
class CTermPlanDoc;
class ProbabilityDistribution;

class CMathSimulation  
{
public:
	CMathSimulation();
	CMathSimulation(CMathFlow* pMathFlow);
	virtual ~CMathSimulation();

	void SetTermPlanDoc(CTermPlanDoc* pDoc) { m_pTermPlanDoc = pDoc; }
	void SetLogWnd(CCompRepLogBar* pLogWnd) { m_pLogWnd = pLogWnd; }
	void SetInterval(int nInterval) { m_nInterval = nInterval; }

	bool Run();
	bool Pause();
	bool Resume();
	bool ExportLogs();
	bool PrintLogs();

private:
	CTermPlanDoc*	m_pTermPlanDoc;
	CMathFlow*		m_pMathFlow;
	CCompRepLogBar* m_pLogWnd;
	std::vector<CMathProcessEngine*> m_vpEngines;
	int m_nInterval;


	bool RunRootProcess();
	bool RunChildProcess(LPCTSTR lpszParentProcessName);
	bool RunOneProcess(LPCTSTR lpszProcessName);

	bool CreateEngines();
	void GenerateArrTime(LPCTSTR lpszParentProcessName, LPCTSTR lpszProcessName,
						int iPercent, int iDistance);
	bool GenerateArrTimeOneFlight(int indexFlight, CMathProcessEngine* pEngine);

	CMathProcess* FindProcess(LPCTSTR lpszProName);
	CMathProcessEngine* FindEngine(LPCTSTR lpszEngineName);
	CMathProcess* GetRootProcess();
	ProbabilityDistribution* FindDefaultPaxTypeDist();
	ProbabilityDistribution* FindDefaultSpeedDist();

	void AddProcessName(CMathProcess* pPro);
	bool HasResult(LPCTSTR lpszProcessName);
	bool CanRun(LPCTSTR lpszProcessName);

	void ShowLogInfo(LPCTSTR lpszLogInfo);
	void AddProgressPos();

};

#endif // !defined(AFX_MATHSIMULATION_H__5AC6D026_35DC_405F_9F64_B19803BFF1E4__INCLUDED_)
