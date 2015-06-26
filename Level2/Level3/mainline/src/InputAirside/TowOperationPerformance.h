#pragma once
#include "..\Common\Flt_cnst.h"
#include "..\Database\DBElementCollection_Impl.h"
//#include "..\Database\DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class InputTerminal;
class CProbDistEntry;
class CProDistrubution;
class CADORecordset;
class CAirportDatabase;
class INPUTAIRSIDE_API TowOperationPerformance: public DBElement
{
	friend class DlgTowOperationPerformance;
public:
	TowOperationPerformance(void);
	~TowOperationPerformance(void);

	CProDistrubution* getPushbackSpeed()const{ return m_pPushbackSpeedDist; }
	CProDistrubution* getUnhookAndTaxiTime()const {return m_pUnhookAndTaxiTimeDist;}
	CProDistrubution* getHookupTime()const {return m_pHookupTime;}
	CProDistrubution* getAcceleration()const {return m_pAttachedAccelerationDist;}
	CProDistrubution* getDeceleration()const {return m_pAttachedDecelerationDist;}
	CProDistrubution* getTowSpeed()const {return m_pTowSpeedDist;}

	int GetID(){ return m_nID; }
	const FlightConstraint& GetFltType() { return m_fltType; }

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	virtual void InitFromDBRecordset(CADORecordset& recordset);

	void SaveData();
	void SaveData(int nParentID);
	void RemoveData();

	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

protected:	
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	void InitDistValue();

private:
	int m_nID;	//database key id
	FlightConstraint m_fltType;		//flight type
	CProDistrubution* m_pPushbackSpeedDist;	// push back speed
	CProDistrubution* m_pUnhookAndTaxiTimeDist;		//unhook and taxi time
	CProDistrubution* m_pHookupTime;	//hook up time
	CProDistrubution* m_pAttachedAccelerationDist;		//attached acceleration
	CProDistrubution* m_pAttachedDecelerationDist;		//attached deceleration
	CProDistrubution* m_pTowSpeedDist;			//tow speed
	CAirportDatabase* m_pAirportDB;
};

/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API TowOperationPerformanceList : public DBElementCollection<TowOperationPerformance>
{
public:
	TowOperationPerformanceList(CAirportDatabase* pAirportDB);
	virtual ~TowOperationPerformanceList();
public:
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }	
protected:
	InputTerminal*	m_pInputTerminal;
	CAirportDatabase* m_pAirportDB;
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
public:
	virtual void ReadData(int nParentID);
	void SaveData();
	void SortFlightType();
};
