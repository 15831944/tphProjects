// MathProcessEngineMM1.h: interface for the CMathProcessEngineMM1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESSENGINEMM1_H__5860B08C_1B8D_4892_B926_44DF761DC86A__INCLUDED_)
#define AFX_MATHPROCESSENGINEMM1_H__5860B08C_1B8D_4892_B926_44DF761DC86A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "MathProcessEngine.h"

class CMathProcessEngineFactory;

class CMathProcessEngineMMN : public CMathProcessEngine  
{
	friend class CMathProcessEngineFactory;
	
public:
	virtual ~CMathProcessEngineMMN();

private:
	CMathProcessEngineMMN();
	

protected:
	virtual void CalculateNormal(const std::vector<double>& vArrTime, int nServerCount);
	virtual void CalculateNormalLeaveTime(const std::vector<double>& vArrTime, std::vector<double>& vLeaTime);

	std::vector<double> m_vProbability;
};

#endif // !defined(AFX_MATHPROCESSENGINEMM1_H__5860B08C_1B8D_4892_B926_44DF761DC86A__INCLUDED_)
