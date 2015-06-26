#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "..\Common\Flt_cnst.h"
#include "..\common\ProbabilityDistribution.h"
#include "InputAirsideAPI.h"

class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
/////////////////////////////////////////////////////////////
//CPushBackNEC
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CPushBackNEC : public DBElement
{
public:
	CPushBackNEC();
	virtual ~CPushBackNEC();

protected:	
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
public:
	int GetID() { return m_nID; }
	void SetID(int nID) { m_nID = nID; }

	int GetProjID() { return m_nProjID; }
	void SetProjID(int nID) { m_nProjID = nID; }

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
    
	void SetSpeedDistName(CString strDistName){m_strSpeedDistName = strDistName;}
	CString GetSpeedDistName(){return m_strSpeedDistName;}
	void SetSpeedProbTypes(ProbTypes enumProbType){m_enumSpeedProbType = enumProbType;}
	ProbabilityDistribution* GetSpeedProbDistribution();
	void SetSpeedPrintDist(CString strprintDist){m_strSpeedPrintDist = strprintDist;}
	CString GetSpeedDistScreenPrint(void);

	void SetTimeDistName(CString strDistName){m_strTimeDistName = strDistName;}
	CString GetTimeDistName(){return m_strTimeDistName;}
	void SetTimeProbTypes(ProbTypes enumProbType){m_enumTimeProbType = enumProbType;}
	ProbabilityDistribution* GetTimeProbDistribution();
	void SetTimePrintDist(CString strprintDist){m_strTimePrintDist = strprintDist;}
	CString GetTimeDistScreenPrint(void);

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}


protected:
	int					m_nProjID;
	FlightConstraint	m_fltType;

	//Push Back Operation Speed£¨Speed Distribution£©
	CString		m_strSpeedDistName;
	ProbTypes m_enumSpeedProbType;  // distribution(for Engin)
	CString m_strSpeedPrintDist;
	ProbabilityDistribution* m_pSpeedDistribution;

	//Unhook and taxi time£¨Time Distribution£©
	CString		m_strTimeDistName;
	ProbTypes m_enumTimeProbType;  // distribution(for Engin)
	CString m_strTimePrintDist;
	ProbabilityDistribution* m_pTimeDistribution;

	CAirportDatabase* m_pAirportDB;

	bool m_bImport;//true,import;false,not;
	CString m_strImportFltType;//import,so save FlightConstraint data in this string;
};
typedef std::vector<CPushBackNEC> PushBackNECList;

/////////////////////////////////////////////////////////////
//CPushBack
/////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CPushBack : public DBElementCollection<CPushBackNEC>
{
public:
	CPushBack(CAirportDatabase* pAirportDB);
	virtual ~CPushBack();
public:
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }	
protected:
	InputTerminal*	m_pInputTerminal;
	CAirportDatabase* m_pAirportDB;
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
public:
	static void ImportPushBacks(CAirsideImportFile& importFile);
	static void ExportPushBacks(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	virtual void ReadData(int nParentID);
	virtual	void ImportData(CAirsideImportFile& importFile);
	virtual void ImportData(int nCount,CAirsideImportFile& importFile);
	void SortFlightType();
private:
	//dll symbols export
	void DoNotCall();
};
