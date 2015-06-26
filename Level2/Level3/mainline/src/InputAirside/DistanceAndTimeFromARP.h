#pragma once
#include "../Database/dbelement.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ProbabilityDistribution.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API CDistanceAndTimeFromARP : public DBElement
{
public:
	CDistanceAndTimeFromARP(void);
	~CDistanceAndTimeFromARP(void);

public:
	void SetAirPortDB(CAirportDatabase* pAirportDB)     {    m_pAirportDatabase = pAirportDB;     }
	void SetFlightTypeID(int nFltTypeID) {      m_nFltTypeID = nFltTypeID;         }
	int GetFlightTypeID()                {      return m_nFltTypeID;               }
	int GetDistance()                    {      return  m_nDistanceFromARP;        }
	void SetDistance(int nDistance)      {      m_nDistanceFromARP = nDistance;    }

	void SetTime(CString& strTime);
	CString& GetTime();
	void SetProbTypes(ProbTypes enumProbType);                 
	void SetPrintDist(CString strprintDist);         
	ProbabilityDistribution* GetServiceTimeDistribution();
	void SetDistScreenPrint(CString strScreenPrint);             
	CString GetDistScreenPrint();           

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void SaveData(int nID);

public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

private:
	int m_nFltTypeID;
	int m_nDistanceFromARP;
	CAirportDatabase* m_pAirportDatabase;

	CString                  m_strTime;
	ProbTypes                m_enumProbType;
	CString	                 m_strDistScreenPrint;
	CString                  m_strprintDist;
	ProbabilityDistribution* m_pDistribution;
};