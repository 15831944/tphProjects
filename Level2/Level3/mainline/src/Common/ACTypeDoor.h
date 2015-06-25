#pragma once
class ArctermFile;
class CACTypesManager;
#include "../Database/ADODatabase.h"
class CAirportDatabase;
class ACTypeDoor
{
	friend class ACTypeDoorManager;
public:
	ACTypeDoor(void);
	~ACTypeDoor(void);

	enum DoorDir{
		LeftHand =0,
		RightHand,
		BothSide
	};

	CString m_strACName;
	CString m_strName;
	double m_dNoseDist;
	double m_dSillHeight;
	double m_dHeight;
	double m_dWidth;
	DoorDir m_enumDoorDir;

	int GetID();
	void SetID(int nID);

	int GetACTypeID();
	void SetACID(int nID);

////new version read and save/////////////////////////////////////////////////////////////
public:
	bool loadDatabase(CADORecordset& aodRecordset);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);

protected:
	ACTypeDoor::DoorDir GetDoorDirByString(const CString& sDoorDir);

///////old code version,no more use///////////////////////////////////////////////////////
public:
	static void ReadDataFromDB(std::vector<ACTypeDoor*>& _Doors , int _airportID, DATABASESOURCE_TYPE type) ;
	static void WriteDataToDB(std::vector<ACTypeDoor*>& _Doors , int _airportID ,  DATABASESOURCE_TYPE type ) ;
	static void DeleteDataFromDB(std::vector<ACTypeDoor*>& _Doors , int _AcTypeID, DATABASESOURCE_TYPE type) ;

	static void DeleteAllDataFromDB(int _AcTypeID,DATABASESOURCE_TYPE type ) ;
	static CString GetStringByHand(DoorDir _dir) ;
protected:
	static void DeleteACDoorFromDB(ACTypeDoor* _door,DATABASESOURCE_TYPE type) ;
	static void UpDateACDoorToDB(ACTypeDoor* _door ,int _airportID ,DATABASESOURCE_TYPE type) ;
	static void WriteACDoorToDB(ACTypeDoor* _door ,int airportID ,DATABASESOURCE_TYPE type) ;

private:
	int m_nID;
	int m_nACID;//this member should not use any more
public:
	void operator = (const ACTypeDoor& p_actype) ;
};
typedef std::vector<ACTypeDoor*> ACTYPEDOORLIST;
typedef ACTYPEDOORLIST::iterator ACTYPEDOORLIST_ITER ;

class ACTypeDoorManager
{
public:
	ACTypeDoorManager(CACTypesManager* _pAcMan );
	~ACTypeDoorManager(void);

public:
	void ReadDataFromDB(int airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS ) ;
	void WriteDataToDB(int airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;

	ACTYPEDOORLIST* GetAcTypeDoorList(const CString& sACCode);

	ACTypeDoorManager * _g_GetActiveACDoorMan(CAirportDatabase * pAirportDB);

	BOOL ReadCommonACDoors(const CString& _path);

	bool AddACDoorItem(const CString& strACName, ACTypeDoor* pDoor);

	//current read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);

private:
	CACTypesManager* m_pAcMan;

};
