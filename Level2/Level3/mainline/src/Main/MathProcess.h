// MathProcess.h: interface for the CMathProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESS_H__EE099120_16BF_4DFF_8BA8_2DE4CBB272A6__INCLUDED_)
#define AFX_MATHPROCESS_H__EE099120_16BF_4DFF_8BA8_2DE4CBB272A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Terminal;
class ProbabilityDistribution;

enum QueueDiscipline{QD_FCFS, QD_LCFS, QD_RSS, QD_PR, QD_GD};
enum QueueConfig{QC_LINEAR, QC_SNAKE};


static TCHAR* s_pszQDiscipline[] = {
	_T("FCFS"), _T("LCFS"), _T("RSS"), _T("PR"), _T("GD")};

static TCHAR* s_pszQConfig[] = {
		_T("Linear"), _T("Snake")};


class CMathProcess  
{
public:
	CMathProcess();
	virtual ~CMathProcess();

//	attributes
public:
	const CString& GetName() const;
	int GetProcessCount();
	int GetCapacity();
	ProbabilityDistribution* GetProbDist();
	QueueDiscipline	GetQDiscipline();
	QueueConfig		GetQConfig();
	CString GetProbDistName();


	void SetName(const CString& strName);
	void SetProcessCount(int nCount);
	void SetCapacity(int nCapacity);
//	void SetProbDist(ProbabilityDistribution* pProbDist);
	void SetQDiscipline(QueueDiscipline qd);
	void SetQConfig(QueueConfig qc);
	void SetProbDistName(CString strProbDist);
	void SetTerminal(Terminal* pTerminal);

	CMathProcess& operator=(const CMathProcess& _rhs);
	

protected:
	CString		m_strName;
	int			m_nProcCount;
	int			m_nCapacity;
	CString     m_strProbDist;

//	ProbabilityDistribution*	m_pProbDist;
	QueueDiscipline	m_qDiscipline;
	QueueConfig		m_qConfig;

	Terminal* m_pTerminal;
};

#endif // !defined(AFX_MATHPROCESS_H__EE099120_16BF_4DFF_8BA8_2DE4CBB272A6__INCLUDED_)
