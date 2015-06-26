#pragma once
#ifndef ENGINE_PARA_H
#define ENGINE_PARA_H

#include "../Database/ADODatabase.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/NewPassengerType.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class CProbDistEntry;
class CAirportDatabase;

class INPUTAIRSIDE_API CEngineParametersItem
{
public:
	CEngineParametersItem();
	~CEngineParametersItem();
public:
	void setJetBlastAngle(double dJetBlastAngle) {m_dJetBlastAngle = dJetBlastAngle;}
	double  getJetBlastAngle()const{return m_dJetBlastAngle;}

	void setJetBlastDistanceStart(double dJetBlastDistanceStart){m_dJetBlastDistanceStart = dJetBlastDistanceStart;}
	double getJetBlastDistanceStart()const {return m_dJetBlastDistanceStart;}

	void setJetBlastDistanceTaxi(double dJetBlastDistanceTaxi){m_dJetBlastDistanceTaxi = dJetBlastDistanceTaxi;}
	double getJetBlastDistanceTaxi()const {return m_dJetBlastDistanceTaxi;}

	void setJetBlastDistanceTakeoff(double dJetBlastDistanceTakeoff){m_dJetBlastDistanceTakeoff = dJetBlastDistanceTakeoff;}
	double getJetBlastDistanceTakeoff()const {return m_dJetBlastDistanceTakeoff;}

	void setStartTime(CProbDistEntry* pPDEntry) {m_StartTime.SetProDistrubution(pPDEntry);}
	const CProDistrubution& getStartTime()const {return m_StartTime;}


	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void SaveData(int nParentID);
	void UpdateData();
	void DeleteData();
	void InitFromDBRecordset(CADORecordset& recordset);
private:
	void InitStartTime();

private:
	int m_nID;
	int m_nProjID;
	double m_dJetBlastAngle;
	double m_dJetBlastDistanceStart;
	double m_dJetBlastDistanceTaxi;
	double m_dJetBlastDistanceTakeoff;
	CProDistrubution m_StartTime;
	FlightConstraint m_fltType;
	CAirportDatabase* m_pAirportDB;
};

class INPUTAIRSIDE_API CEngineParametersList
{
public:
	CEngineParametersList(CAirportDatabase* pAirportDB);
	~CEngineParametersList();
public:
	void addItem(CEngineParametersItem* pItem);
	void removeItem(CEngineParametersItem* pItem);
	CEngineParametersItem* getItem(int ndx);
	bool findItem(CEngineParametersItem* pItem);
	int getCount();
	////////database
	void SaveData(int nProjID);
	void ReadData(int nProjID);
private:
	std::vector<CEngineParametersItem*> m_vEngineItem;
	std::vector<CEngineParametersItem*> m_vDelEngineItem;
	CAirportDatabase* m_pAirportDB;
};
#endif