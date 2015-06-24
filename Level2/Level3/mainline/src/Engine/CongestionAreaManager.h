#pragma once

#include "..\Common\dataset.h"
#include "..\Inputs\Pipe.h"
#include "..\Inputs\AreasPortals.h"
#include "..\engine\PERSON.H"
#include "..\engine\CongestionAreaEvent.h"
#include <vector>

struct CongestionParam
{
	double dLambda;
	double dTrigger;

	CongestionParam()
	{
		dLambda = 0.0;
		dTrigger = 0.0;
	}
	CongestionParam(const CongestionParam& _other)
	{
		*this = _other;
	}
	CongestionParam& operator=(const CongestionParam& _other)
	{
		dLambda = _other.dLambda;
		dTrigger = _other.dTrigger;
		return *this;
	}
	bool operator==(const CongestionParam& _other) const 
	{
		return ((dLambda == _other.dLambda) && (dTrigger == _other.dTrigger));
	}

};

typedef std::vector<int> INTVECTOR;

class Terminal;
class CCongestionAreaGrid;

class CCongestionAreaManager : public DataSet
{
public:
	CCongestionAreaManager(void);
	~CCongestionAreaManager(void);
public:	
	//initialize congestion grid for every floor
	void InitGrid(Terminal* pTerminal, int nFloor);
	//check if the line through the congestion area, 
	BOOL CheckAndHandleCongestion(Person* pPerson, ElapsedTime eventTime, BOOL bAreaCong);
	//process congestion event and generate the next event
	int ProcessCongestionEvent(CongestionAreaEvent* pEvent, ElapsedTime eventTime);

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Congestion Parameters";}
	virtual const char* getHeaders() const{return "Lambda,Trigger Value,Areas,Pipes";}
public:
	void SetCongParam(const CongestionParam& param){ m_congParam = param; }
	const CongestionParam& GetCongParam() const { return m_congParam;	}
	void SetVector(const INTVECTOR& vectAreas, const INTVECTOR& vectPipes)
	{
		m_vectAreas = vectAreas;
		m_vectPipes = vectPipes;
	}
	INTVECTOR* GetAreasVect() { return &m_vectAreas; }
	INTVECTOR* GetPipesVect() { return &m_vectPipes; }

	void SetAreas(CAreas* pAreas) { m_pAreas = pAreas; }
	CAreas* GetAreas() const { return m_pAreas;	}
	void SetPipes(std::vector<CPipe>* pPipes) { m_pPipes = pPipes; }
	const std::vector<CPipe>* GetPipes() const { return m_pPipes; }
protected:
	//data for read data from file
	CongestionParam m_congParam;
	INTVECTOR m_vectAreas;
	INTVECTOR m_vectPipes;
	//data for initialize cell 
	CAreas* m_pAreas;
	std::vector<CPipe>* m_pPipes;
	std::vector<CCongestionAreaGrid*> m_vGrid;
protected:
	//get the new speed, nCount is the number of the person
	DistanceUnit GetNewSpeed(Person* pPerson, int nCount);

};
