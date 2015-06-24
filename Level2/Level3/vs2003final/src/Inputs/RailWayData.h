// RailWayData.h: interface for the CRailWayData class.
#pragma once
#include "..\common\dataset.h"
#include "..\engine\RailwayInfo.h"

class CRailWayData : public DataSet  
{
private:
	std::vector<RailwayInfo*> m_allRailWayFromFile;

public:
	CRailWayData();
	virtual ~CRailWayData();

	virtual void readData (ArctermFile& p_file);
	
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Railway Info File" ;};
	virtual const char *getHeaders (void) const {return "all railway info " ;};
	virtual void clear (void);

	const std::vector<RailwayInfo*>& GetAllRailWayInfo() const { return m_allRailWayFromFile;}
	void SetAllRailWayInfoVector(const std::vector<RailwayInfo*>& _railWayVector);

	RailwayInfo* FindRailWayPointer( const RailwayInfoKey& _anotherRailWayKey );
};
