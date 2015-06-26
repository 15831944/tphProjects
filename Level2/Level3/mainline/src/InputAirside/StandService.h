#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "..\Common\Flt_cnst.h"
#include "..\common\ProbabilityDistribution.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API StandService: public DBElement
{
public:
	StandService(void);
	~StandService(void);

protected:	
	void GetInsertSQL(int nParentID,CString& strSQL) const;
	void GetUpdateSQL(CString& strSQL) const;
	void GetDeleteSQL(CString& strSQL) const;
	void GetSelectSQL(int nID,CString& strSQL)const;
public:
	void InitFromDBRecordset(CADORecordset& recordset);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
public:
	int GetID() { return m_nID; }
	void SetID(int nID) { m_nID = nID; }

	int GetProjID() { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	//time distribution description
	//and create time distribution
	void SetTimeDistributionDesc(CString strDistName,ProbTypes enumProbType,CString strprintDist );
	CString GetTimeDistName(){return m_strTimeDistName;}
	//void SetTimeProbTypes(ProbTypes enumProbType){m_enumTimeProbType = enumProbType;}

	//void SetTimePrintDist(CString strprintDist){m_strTimePrintDist = strprintDist;}
	CString GetTimeDistScreenPrint(void);

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	//return distribution
	ProbabilityDistribution* GetTimeProbDistribution();

protected:
	//create distribution by distribution type and string
	void GenerateDistribution();


private:
	int					m_nProjID;
	FlightConstraint	m_fltType;


	//Min turn around time£¨Time Distribution£©
	CString		m_strTimeDistName;
	ProbTypes m_enumTimeProbType;  
	CString m_strTimePrintDist;
	ProbabilityDistribution* m_pTimeDistribution;

	CAirportDatabase* m_pAirportDB;
};

//Stand service List
class INPUTAIRSIDE_API StandServiceList :public DBElementCollection<StandService>
{
public:
	StandServiceList(CAirportDatabase* pAirportDB);
	~StandServiceList();
public:
	void ReadData(int nParentID);
	void SortFlightType();

private:
	void GetSelectElementSQL(int nParentID,CString& strSQL) const;

private:
	CAirportDatabase* m_pAirportDB;

private:
	//dll symbols export
	void DoNotCall();
};