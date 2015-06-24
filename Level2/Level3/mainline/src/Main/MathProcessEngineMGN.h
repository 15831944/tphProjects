// MathProcessEngineMGN.h: interface for the CMathProcessEngineMGN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESSENGINEMGN_H__0E79D10B_4526_43F9_8BFB_8C1B392A471B__INCLUDED_)
#define AFX_MATHPROCESSENGINEMGN_H__0E79D10B_4526_43F9_8BFB_8C1B392A471B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "MathProcessEngine.h"

class CMathProcessEngineFactory;

class CMathProcessEngineMGN : public CMathProcessEngine
{
	friend class CMathProcessEngineFactory;

public:
	virtual ~CMathProcessEngineMGN();

private:
	CMathProcessEngineMGN();

protected:
	virtual void CalculateNormal(const std::vector<double>& vArrTime, int nServerCount);
	virtual void CalculateNormalLeaveTime(const std::vector<double>& vArrTime, std::vector<double>& vLeaTime);
};

#endif // !defined(AFX_MATHPROCESSENGINEMGN_H__0E79D10B_4526_43F9_8BFB_8C1B392A471B__INCLUDED_)
