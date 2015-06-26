#pragma once

#include <vector>
#include "ALTAirspace.h"
#include "Topography.h"

#include "InputAirsideAPI.h"

const static int ERRORDBVERSION = -1;
class CAirportDatabase;
class ALTAirport;
class Runway;
class Taxiway;
class ALTObject;
class ALTAirportLayout;
#include "HoldShortLines.h"

class INPUTAIRSIDE_API InputAirside
{
public:

	InputAirside();
	~InputAirside();
	static void GetAirportList(int nProjID,std::vector<int>& vAirport);

	static int GetProjectID(const CString& strName);

	static int GetAirspaceID(int nProjID);

	//return the airport info of specified object
	static bool IsProjectNameExist(const CString& strprojName);
	static bool DeleteProject(const CString& strProjectName);
	CAirportDatabase*		m_pAirportDB;
	static int GetAirportID(int nProjID);


	//data zone
public:
	int GetAirportCount()const;
	//airport index, must less than GetAirportCount()
	//if not exists, return NULL
	ALTAirportLayout *GetAirportbyIndex(int nIndex)const;
	// airport ID in database, larger than 0
	//if not find, return NULL
	ALTAirport *GetAirportbyID(int nAirportID);
	ALTAirport *GetAirport(const CGuid& guid);

	ALTAirportLayout * GetAirportLayoutByID(int aptID)const;
	//void initialize Data, if not success, throw CADOException
	void InitData(int nProjID);

	void ClearData();
	//get runwaybyid

	Runway *GetRunwayByID(int nID);
	Taxiway*GetTaxiwayByID(int nID);

	ALTObject *GetObjectByID(int nID);
	
	TaxiInterruptLineList& GetTaxiInterruptLines();
	const TaxiInterruptLineList& GetTaxiInterruptLines() const;

	void SetProjID(int nProjID);
	ALTAirspace* GetAirspace();
	Topography* GetTopography();

	void LoadInputAirside();

	ALTAirportLayout *GetActiveAirport();
protected:

	void LoadTopography();
	void LoadAirspace();
protected:
	std::vector<ALTAirportLayout *> m_vAirport;
	TaxiInterruptLineList m_taxiInterruptLines;
	ALTAirspace m_airspace;
	Topography m_topography;

	int m_nProjectID;

	//topography load flag
	bool m_bTopograhyLoaded;
	//flag to stand that the airspace has been load or not
	bool m_bLoaded;

};

class INPUTAIRSIDE_API CAirsideDatabase
{
public:
	CAirsideDatabase(){}
	~CAirsideDatabase(){}

public:
	static int GetDatabaseVersion();
protected:
private:
};
