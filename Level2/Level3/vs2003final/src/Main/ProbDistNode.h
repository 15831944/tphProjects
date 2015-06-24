// ProbDistNode.h: interface for the CProbDistNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROBDISTNODE_H__69CB0053_973C_44A1_A7C6_2351C5095AB7__INCLUDED_)
#define AFX_PROBDISTNODE_H__69CB0053_973C_44A1_A7C6_2351C5095AB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TVNode.h"

class CProbDist;

class CProbDistNode : public CTVNode  
{
	DECLARE_SERIAL(CProbDistNode);
public:
	CProbDistNode(CProbDist* pDist);
	virtual ~CProbDistNode();

	CProbDist* GetProbDist() { return m_pDist; }

protected:
	CProbDistNode();

private:
	CProbDist* m_pDist;


};

#endif // !defined(AFX_PROBDISTNODE_H__69CB0053_973C_44A1_A7C6_2351C5095AB7__INCLUDED_)
