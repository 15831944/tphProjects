#pragma once
#include <vector>
#include "Aircraft.h"
#include "ACTypesManager.h"
#include <map>
#include "AirportDatabase.h"
using namespace std;

class AircraftAliasItem
{
public:
	typedef std::vector<CString> AssocAircraftVector;
	typedef AssocAircraftVector::iterator AssocAircraftIter;

public:
	AircraftAliasItem();
	~AircraftAliasItem(void);

	bool addAssocItem(CString strName);
	bool deleteAssocItem(CString strName);
	bool fit(CString& strName);
	
	void setSharpName(CString strShapeName);
	CString getSharpName();
	CString getRecordString();

	int	getCount();
	CString getItem(int nIndex);

protected:
	

	CString m_strShapeName;
	AssocAircraftVector m_vectAssocItem;
};

class AircraftAliasManager
{
public: 
	typedef std::vector<AircraftAliasItem*> AircraftAliasItemVector;
	typedef AircraftAliasItemVector::iterator  AircraftAliasItemIter;
	typedef std::map<CString,CString> AirCraftShape;
	typedef AirCraftShape::iterator airCraftshapeiter;

public:
	AircraftAliasManager(void);
	virtual ~AircraftAliasManager(void);

public:
	CString getOglShapeName(CString& strName);
	bool isAssociated(CString&strName);
	bool isAssociatedShapeName(CString &strShapeName,int nIdex);

	bool readData(CAirportDatabase *pAirportDatabase);
	bool saveData(CAirportDatabase *pAirportDatabase);
	void SetCACTypeManager(CACTypesManager *m_CACManager){m_pACManager=m_CACManager;}
	CACTypesManager* GetCACTypeManager(){return m_pACManager;}
	int	getCount();
	CString getItem(int nIndex);
	void addItem(CString Aircraft,CString ShapeName);
	void GetAllDefaultAirCraftShape();
	void GetAllNonDefaultAirCraftShape(CAirportDatabase*pAirportDataBase);
	void deleteItem(CString StrACName);
	
	bool loadDatabase(CAirportDatabase *pAirportDatabase);

	static AircraftAliasManager* GetInstance();
	static void ReleaseInstance();
	static void SetInstance(AircraftAliasManager* pInst);


protected:
	bool m_bIsDataLoad;
	AircraftAliasItemVector m_vectAircraftAliasItem;
	
	void reset();

	static AircraftAliasManager* m_pInstance;
	AirCraftShape AirCraftShapeList;
	CACTypesManager *m_pACManager;
};

#define ACALIASMANAGER AircraftAliasManager::GetInstance()
#define DESTROY_ACALIASMANAGER AircraftAliasManager::ReleaseInstance()
