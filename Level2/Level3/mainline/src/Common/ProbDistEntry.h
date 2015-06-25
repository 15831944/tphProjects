// ProbDist.h: interface for the CProbDistEntry class.
#pragma once
class ProbabilityDistribution;

class CProbDistEntry
{
public:
	CProbDistEntry( CString _csName, ProbabilityDistribution* _pProbDist );
	virtual ~CProbDistEntry();


	CString m_csName;
	ProbabilityDistribution* m_pProbDist;
protected:
	int m_ID ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
};

typedef std::vector<CProbDistEntry*> CPROBDISTLIST;
