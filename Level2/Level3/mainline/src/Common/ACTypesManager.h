// ACTypesManager.h: interface for the CACTypesManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTYPESMANAGER_H__9BB1C907_AAE7_48F0_8187_256C0C753A25__INCLUDED_)
#define AFX_ACTYPESMANAGER_H__9BB1C907_AAE7_48F0_8187_256C0C753A25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Aircraft.h"
//#include "commondll.h"

class InputTerminal;
class Terminal;
class CAirportDatabase;
class SuperTypeRelationData ;
#define ACTYPESMANAGER_  

class CACTypesManager  
{
public:
	CACTypesManager(CAirportDatabase* pAirportDatabase);
	virtual ~CACTypesManager();

	// find the entry and return the index
	// if not find add and return the index
	// return -1 for invalid.
public:
	int FindOrAddEntry( CString _csACType);
public:
	CACTYPELIST* GetACTypeList() { return & m_vACTypes; }
	void GetACTypesByFilter(LPCSTR strFilterScript, CACTYPELIST& list);

	CACCATEGORYLIST* GetACCategoryList() { return &m_vACCategories; }

	void AddACCategory(CString sCatName, CACTYPEFILTER actFilter);

	int findCatItem( CACCategory* _pACCat );

	CACCategory* getCatItem( int _nIdx ){ return m_vACCategories[_nIdx]; }

	int getCatCount()const { return (int)m_vACCategories.size(); }

	int findACTypeItem( CACType* _pACType );

	CACType* getACTypeItem( int _nIdx ){ return m_vACTypes[_nIdx]; }

	int getACTypeCount()const;

	BOOL LoadData( const CString& _strDBPath );

	void ClacCategoryACTypeList();

	CACType *getACTypeItem(const CString& _csACType);
	const CString& getDBPath(){return m_strDBPath;}
	void SetDBPath(const CString& _csActypePath) { m_strDBPath = _csActypePath ;} ;
	enum LogicalOperator
	{
		EQUAL,
		NOT_EQUAL,
		LESS,
		LESS_EQUAL,
		GRATER,
		GRATER_EQUAL
	};
private:
	
	CACTYPELIST m_vACTypes;
	CACCATEGORYLIST m_vACCategories;

	bool ParseFilterString(const CString& strFilter, int& iFieldIndex, LogicalOperator& LO, CString& strValue);

	CString		m_strDBPath;
	CString     m_strCommonDBPath ;
	CACCategoryDataSet m_ACTypeDataSet;

	SuperTypeRelationData* m_RelationShip ;
	CAirportDatabase* m_pAirportDatabase;

public:
	//current version read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool loadARCDatabase(CAirportDatabase* pAirportDatabase);
public:

	BOOL LoadDataFromOtherFile(const CString& _strDBPath) ;
	void Clear();

	CString GetCommonDBPath() { return m_strCommonDBPath ;} ;
	void SetCommonDBPath(const CString& _path) { m_strCommonDBPath = _path ;}  ;
public:
	void InitRelationShip() ;
	SuperTypeRelationData* GetActypeRelationShip() ;
////////////////old code version//////////////////////////////////////////////////////////
// the interface to DB 
public:
	void ReadDataFromDB(int airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS ) ;
	void WriteDataToDB(int airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
protected:
	CACTYPELIST m_DelActype ;
	CACCATEGORYLIST m_DelCaccate ;
	int m_AirportID ;
public:
	void SetID(int _id) { m_AirportID = _id ;} ;
	int GetID() { return m_AirportID ;} ;
public: 
    void RemoveActype(CACType* _actype) ;
	void RemoveCategories(CACCategory* _categories) ;
protected:
	void ClearDeleteActype() ;
	void ClearDelCategories() ;
public:
	void ReSetAllID() ;
	void DeleteAllDataFromDB(int airportID,DATABASESOURCE_TYPE type= DATABASESOURCE_TYPE_ACCESS) ;
public:
	CACType* FilterActypeByStrings(std::vector<CString>& _ActypeNames) ;
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//the interface for common data 
// the common data for actype 
// this data is maintained by ARC ,they can not been edited in application. 
public:
   BOOL ReadCommonActype(const CString& _path);
   //void GetCommonActypes(CACTYPELIST& _list) ;

public:
	////find actype by unique name , if fond return ,else return NULL
	//CACType* FindActypeByUniqueName(cons CString& _str) ;

	//find actype by IATACode and ICAOcode , if _str == IATACode or ICAOCode ,return the item 
	//else return NULL,
	//it will return the first matched item .
	CACType* FindActypeByIATACodeOrICAOCode(const CString& _str) ;

	//find actype by IATACode and ICAOcode , if _str == IATACode or ICAOCode
	//_Actypelist : out , 
	// if find item return TRUE ,else return FALSE 
	BOOL FindActypeByIATACodeOrICAOCode(const CString& _str,std::vector<CACType*>& _Actypelist) ;

	//find actype by IATACode  , if _str == IATACode 
	//_Actypelist : out , 
	// if find item return TRUE ,else return FALSE 
	BOOL FindActypeByIATACode(const CString& _str,std::vector<CACType*>& _Actypelist) ;


	//find actype by  ICAOcode , if _str ==   ICAOCode
	//_Actypelist : out , 
	// if find item return TRUE ,else return FALSE 
	BOOL FindActypeByICAOCode(const CString& _str,std::vector<CACType*>& _Actypelist) ;

	//get all IATA code 
	BOOL GetAllIATACode(std::vector<CString>& _CodeList) ;


	//get all ICAO code
	BOOL GetAllICAOCode(std::vector<CString>& _CodeList) ;

	BOOL FindAndRemoveFromDefaultActype(std::vector<CACType*>& _OutData ,  std::vector<CACType*> _inOut, CACType* _ActypeItem);
};

// global function
//CACTypesManager* _g_GetActiveACMan( InputTerminal* _pInTerm );
//CACTypesManager* _g_GetActiveACMan( Terminal* _pTerm );
CACTypesManager * _g_GetActiveACMan(CAirportDatabase * pAirportDB);

// called by std::remove_copy_if only
class ACTypeRemove
{
public:
	ACTypeRemove(const int iFieldIndex, const CACTypesManager::LogicalOperator LO, LPCSTR strValue);
	bool operator() (CACType* pACType);

private:
	int m_iFieldIndex;
	CACTypesManager::LogicalOperator m_LO;
	CString m_strValue;

	bool IsDoubleValueEqual(double ldValue, double rdValue);
	bool IsDoubleValueLessEqual(double ldValue, double rdValue);
	bool IsDoubleValueGraterEqual(double ldValue, double rdValue);

	bool CompareValue(const double d1stValue, const CACTypesManager::LogicalOperator LO, const double d2ndValue);
	bool CompareValue(const int i1stValue, const CACTypesManager::LogicalOperator LO, const int i2ndValue);
};

#endif // !defined(AFX_ACTYPESMANAGER_H__9BB1C907_AAE7_48F0_8187_256C0C753A25__INCLUDED_)
