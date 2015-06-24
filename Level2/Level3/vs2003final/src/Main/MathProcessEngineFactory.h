// MathProcessEngineFactory.h: interface for the CMathProcessEngineFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESSENGINEFACTORY_H__21F21DE0_814D_479C_A407_34CB49B2CD17__INCLUDED_)
#define AFX_MATHPROCESSENGINEFACTORY_H__21F21DE0_814D_479C_A407_34CB49B2CD17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMathProcessEngine;
class ProbabilityDistribution;

class CMathProcessEngineFactory  
{
public:
	CMathProcessEngineFactory();
	CMathProcessEngineFactory(ProbabilityDistribution* pServiceTimeProbDist);
	virtual ~CMathProcessEngineFactory();

	void SetServiceTimeProbDist(ProbabilityDistribution* pServiceTimeProbDist);
	CMathProcessEngine* CreateInstance();

private:
	CMathProcessEngine* m_pMathProcessEngine;
	ProbabilityDistribution* m_pServiceTimeProbDist;
};

#endif // !defined(AFX_MATHPROCESSENGINEFACTORY_H__21F21DE0_814D_479C_A407_34CB49B2CD17__INCLUDED_)
