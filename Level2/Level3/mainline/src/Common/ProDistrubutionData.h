#ifndef PRODIST_H
#define PRODIST_H
#include "ProbabilityDistribution.h"
class CProbDistEntry;

class CProDistrubution
{
public:
	CProDistrubution();
	CProDistrubution(const CProDistrubution& proDist);
	const CProDistrubution& operator = (const CProDistrubution& proDist);
	~CProDistrubution();

public:
	void ReadData(int ndistID);
	void RemoveData();
	void UpdateData();
	void SaveData();

	void SetProDistrubution(CProbDistEntry* pPDEntry);

	void setID(int nID);
	const int& getID()const;

	const CString& getDistName()const;

	void setDistName(const CString& val);
	ProbTypes getProType() const;
	void setProType(ProbTypes val);
	
	CString getPrintDist() const;
	void setPrintDist(CString val);

	ProbabilityDistribution* GetProbDistribution() const;

	void initProbilityDistribution();
	
	ProbabilityDistribution* copyProbalityDistribution();
private:
	int ID;
	CString distName;

	ProbTypes proType;

	CString printDist;
	ProbabilityDistribution* pProbailityDistrution;
};
#endif