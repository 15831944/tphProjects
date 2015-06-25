#pragma once
#include <common\ModelDatabase.hpp>

class CAircraftModel;
class CAirportDatabase;
class AirportDatabaseConnection;
#define STR_AC_MODL_DBTABLE _T("AIRCRAFT_MODEL_DB")

class CAircraftModelDatabase : public CModelDatabase<CAircraftModel,CAirportDatabase>
{
public:
	CAircraftModelDatabase(CAirportDatabase* pParent);
	~CAircraftModelDatabase();

public:	
	//return -1 if none
	CAircraftModel* getModelByName(const CString& strName)const;

	AirportDatabaseConnection* GetConnection() const;

	//read data
	BOOL loadDatabase(CAirportDatabase* pAirportDB = NULL);
	BOOL saveDatabase(CAirportDatabase* pAirportDB /* = NULL */);
	BOOL SaveData();

	void RemoveItem(CString sName);

	CAircraftModel * AddItem(CString sName);

	CAirportDatabase* m_pAirportDB;

private:
};
