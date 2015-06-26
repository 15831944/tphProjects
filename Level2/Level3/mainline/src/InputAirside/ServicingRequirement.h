#pragma once
#include "..\Database\ADODatabase.h"
#include "..\Common\ProbabilityDistribution.h"
#include "VehicleDefine.h"
#include "../Database/DBElement.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CServicingRequirement : public DBElement
{
public:
	CServicingRequirement(void);
	~CServicingRequirement(void);

	void SetServicingRequirementNameID(int strServicingRequirementNameID);
	int GetServicingRequirementNameID();

	void SetNum(int nNum);
	int GetNum();

	void SetServiceTime(CString& strServiceTime);
	CString& GetServiceTime();

	void SetSubServiceTime(CString& strServiceTime);
	CString& GetSubServiceTime();

	void SetFltTypeID(int nFltTypeID);
	int GetFltTypeID();

	void SetConditionType(enumVehicleTypeCondition nConditionType);
	enumVehicleTypeCondition  GetConditionType(void);

	void SetDistScreenPrint(CString strScreenPrint){m_strDistScreenPrint = strScreenPrint;}
	CString GetDistScreenPrint(){return m_strDistScreenPrint;}
	void SetProbTypes(ProbTypes enumProbType){m_enumProbType = enumProbType;}
	void SetPrintDist(CString strprintDist){m_strprintDist = strprintDist;}
	ProbabilityDistribution* GetServiceTimeDistribution();
	

	void SetSubDistScreenPrint(CString strScreenPrint){m_subDistScreenPrint = strScreenPrint;}
	CString GetSubDistScreenPrint(){return m_subDistScreenPrint;}
	void SetSubProbTypes(ProbTypes enumProbType){m_subProbType = enumProbType;}
	void SetSubPrintDist(CString strprintDist){m_subPrintDist = strprintDist;}
	ProbabilityDistribution* GetSubServiceTimeDistribution();
	
public:
	void DeleteDataFromDB();
	void SaveDataToDB();

	//DBElementCollection
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

private:
	int         m_nServicingRequirementNameID;
	int         m_nNum;     //Number of vericle,if vehicle Type is "Baggage cart" or "Pax bus" The number is -1(variable) 
	int         m_nFltTypeID;
	enumVehicleTypeCondition       m_nConditionType;    //condition type

	CString m_strServiceTime;
	CString	m_strDistScreenPrint;
	ProbTypes m_enumProbType;
	CString m_strprintDist;
	ProbabilityDistribution* m_pDistribution;

	CString m_subServiceTime;
	CString m_subDistScreenPrint;
	ProbTypes m_subProbType;
	CString m_subPrintDist;
	ProbabilityDistribution* m_pSubDistribution;

	CAirportDatabase* m_pAirportDB;

};
