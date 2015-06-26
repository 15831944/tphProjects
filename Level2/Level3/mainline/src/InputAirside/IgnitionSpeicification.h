#pragma once
#include "ALTObjectGroup.h"
#include "../Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class CADORecordset;
class CAirportDatabase;
class DlgIgnitionSpecification;
enum EngineStartType
{
	//push back only
	CommencingPushback =0,
	ReadyToPowerOut,
	CompletionOfPushback,
	AfterUnhookofPushTractor,
	//Push and Tow/Tow
	AtReleasePoint,
	AfterUnhookByTowTractor,
	WithinADistanceOfReleasePoint
};

class INPUTAIRSIDE_API FlightIgnitionData
{
	friend class DlgIgnitionSpecification;
public:
	FlightIgnitionData(void);
	~FlightIgnitionData(void);

	void ReadData(CADORecordset& adoRecord, CAirportDatabase* pAirportDB);
	void SaveData(int nParentID);
	void UpdateData();
	void DeleteData();

	const FlightConstraint& getFltType(){ return m_FltType; }
	EngineStartType getEngineStartType() const{ return m_eEngineStartType; }
	int getID() const { return m_nID; }
	int getDistanceToStand() const { return m_nDistance; }

private:
	int m_nID;				//database key
	FlightConstraint m_FltType;			//flight type
	EngineStartType m_eEngineStartType;		//Engine start type
	int m_nDistance;				//distance to stand
};

////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API IgnitionData
{
	friend class DlgIgnitionSpecification;
public:
	IgnitionData(int nPrjID);
	~IgnitionData(void);

	void ReadData(CADORecordset& adoRecord, CAirportDatabase* pAirportDB);
	void SaveData();
	void UpdateData();
	void DeleteData();

	int GetDataCount(){ return (int)m_vDataList.size(); }
	const FlightIgnitionData* GetData(int nIdx);
	const ALTObjectGroup& getStandGroup(){ return m_StandGroup; }
	void DelData(FlightIgnitionData* pData);
	int getID(){ return m_nID; }

private:
	int m_nID;			//database key
	int m_nPrjID;			//project id
	ALTObjectGroup m_StandGroup;		//stand group
	std::vector<FlightIgnitionData*> m_vDataList;    //save data list
	std::vector<FlightIgnitionData*> m_vDelDataList;		//need delete data list
};

////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API IgnitionSpeicification
{
	friend class DlgIgnitionSpecification;
public:
	IgnitionSpeicification(void);
	~IgnitionSpeicification(void);

	void ReadData(int nPrjID,CAirportDatabase* pAirportDB);
	void SaveData();

	int GetDataCount(){ return (int)m_vDataList.size(); }
	const IgnitionData* GetData(int nIdx);
	void DelData(IgnitionData* pData);

private:
	std::vector<IgnitionData*> m_vDataList;    //save data list
	std::vector<IgnitionData*> m_vDelDataList;		//need delete data list
};
