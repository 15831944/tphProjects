// Aircraft.h: interface for the CAircraft class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRCRAFT_H__731EF4B8_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
#define AFX_AIRCRAFT_H__731EF4B8_3C85_11D4_9307_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
//#include "commondll.h"
#include "DataSet.h"
#define  ACTYPEBMP_COUNT 4
class ArctermFile;
class CACTypesManager;
#include "../Database/ADODatabase.h"
#include "ACTypeDoor.h"
class CACType
{
public:
	CACType();
	CACType(const CACType& acType);
	virtual ~CACType();
protected:
	CString m_sIATACode;//the code use as name of aircraft
	CString m_sICAOCode;//new version this code no more use
public:
	void setIATACode (const char *p_str) { m_sIATACode = p_str; }
	CString GetIATACode() { return m_sIATACode ;} ;

	void SetICAOCode(const CString& _str ){ m_sICAOCode = _str ;} ;
	CString GetICAOCode() { return m_sICAOCode ;} ;
public:
	CString m_sDescription;
	CString m_sFileName;
	float m_fHeight;
	float m_fLen;
	float m_fSpan;
	float m_fMZFW;//kg
	float m_fOEW; //kg
	float m_fMTOW;//kg
	float m_fMLW;//kg
	int m_iCapacity;
    CString m_sOtherCodes;
	bool m_bLoad;
	double m_dCabinWidth;
	//add new version member
	CString m_sCompany;
	CString m_sICAOGroup;
	CString m_sWakeVortexCategory;
	CString m_sWeightCategory;
	float m_fMinTurnRad;


	HBITMAP m_hBmp[ACTYPEBMP_COUNT];

	ACTYPEDOORLIST m_vACDoorList;

	int getDefaultCapacity (void) const { return m_iCapacity; }

	void LoadAllBMPHandle(const CString& strDBPath);
	void LoadBMPHandle(HBITMAP& _hBmp,const CString& _strPicPath);
	const HBITMAP* GetBmpHandle()const{return m_hBmp;}
	void ClearBMPHandle();
	void RemoveFiles(const CString& strDBPath);

	CString getThumbXPath(const CString& strDBPath)const;
	CString getThumbYPath(const CString& strDBPath)const;
	CString getThumbZPath(const CString& strDBPath)const;
	CString getThumbISOPath(const CString& strDBPath)const;

    int operator == (const CACType& p_acType) const
        { return m_sIATACode.CompareNoCase( p_acType.m_sIATACode) == 0; }
    int operator < (const CACType& p_acType) const
               { return m_sIATACode.CompareNoCase(  p_acType.m_sIATACode) < 0; }
	void operator = (const CACType& p_actype) ;

	//----------------------------------------------------------------------------
	//Summary:
	//		avoid actype data does not modified and update database
	//---------------------------------------------------------------------------
	void Update(){m_bNeedUpdate = true;}
private:
	bool m_bNeedUpdate;
public:
///////new version database read and save////////////////////////////////////////////////
	bool deleteDoorDatabase(CAirportDatabase* pAirportDatabase);
	bool saveDoorDatabase(CAirportDatabase* pAirportDatabase);
	bool loadDoorDatabase(CAirportDatabase* pAirportDatabase);

	bool loadDatabase(CAirportDatabase* pAirportDatabase,CADORecordset& aodRecordset);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	static bool deleteAllDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
///////old code version,no more us///////////////////////////////////////////////////////////////////
		//the interface for DataBase 
		//added for convert File to DB
public:
	static void ReadDataFromDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void WriteDataToDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void DeleteDataFromDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type) ;

	static void DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type ) ;
protected:
	static void DeleteActypeFromDB(CACType* _actype,DATABASESOURCE_TYPE type) ;
	static void UpDateActypeToDB(CACType* _actype ,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void WriteActypeToDB(CACType* _actype ,int _airportDBID,DATABASESOURCE_TYPE type) ;
protected:
	static void LoadAllBMPHandleFromDB(CACType* _actype,DATABASESOURCE_TYPE type) ;
	static void WriteAllBMPToDB(CACType* _actype,DATABASESOURCE_TYPE type) ;
protected:
	static void LoadAllBMPHandleFromDB() ;
	static void SaveAllBMPToDB() ;
private:
	int m_ID ;
	BOOL m_HasEdit ;

public:
	int GetID() { return m_ID ;} ;
	void SetID(int _id) { m_ID = _id ;} ;
	void ResetDoorID() ;
	BOOL IsEdit() { return m_HasEdit ;} ;
	void IsEdit(BOOL _res) { m_HasEdit = _res ;} ;
public:
	BOOL FitActype(const CString& _val) ;
	BOOL FitActype(const TCHAR* _val) ;
protected:
	std::vector<ACTypeDoor*> m_DelDoors ;
public:
	void AddDoor(ACTypeDoor* _door) ;
	void RemoveDoor(ACTypeDoor* _door) ;
	void clearAllDoorData() ;
    ACTypeDoor* FindACTypeDoorByDoorName(CString strDoorName);
public:
	void DeleteDoorData(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
public:
	void AddDoorMessager(const ACTYPEDOORLIST& _door) ;
//////////////////////////////////////////////////////////////////////////
};
typedef std::vector<CACType*> CACTYPELIST;
typedef struct _actypefilter {
	CString code;
	/*
	CString description;
	float height;
	float len;
	float span;
	float MZFW;
	float OEW;
	float MTOW;
	float MLW;
	int capacity;
    CString sICAOCode;
    CString sOtherCodes;

	*/
} CACTYPEFILTER;

class COMMON_TRANSFER CACCategory
{
public:
	CACCategory( CACTypesManager* _pAcMan );
	virtual ~CACCategory();
	void ClacACTypeList();

	void AddFilter(CACTYPEFILTER actFilter);

	void GetACTypeList(CACTYPELIST& ACTList);

	CString m_sName;
	bool	m_bHasScript;
	std::vector<CACTYPEFILTER> m_vACTypeFilters;
	std::vector<CString> m_vDirectACTypes;

	void setCatName (const char *p_str) { m_sName = p_str; }

    void getCatName (char *p_str) const { strcpy (p_str, m_sName); }

    int operator == (const CACCategory& p_cat) const
               { return m_sName.CompareNoCase(  p_cat.m_sName) == 0 ; }
    int operator < (const CACCategory& p_cat) const
               { return m_sName.CompareNoCase(  p_cat.m_sName) < 0; }


    virtual int contains (const char *p_acType) const;

protected:
	CACTypesManager* m_pAcMan;
	CACTYPELIST m_vACTList;
/////////////////////////////new version database read and save/////////////////////////////
public:
	bool loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
protected:
	bool loadScriptFilter(CAirportDatabase* pAirportDatabase);
	bool saveScriptFilter(CAirportDatabase* pAirportDatabase);
	bool deleteScriptFilter(CAirportDatabase* pAirportDatabase);

	bool loadDirectFilter(CAirportDatabase* pAirportDatabase);
	bool saveDirectFilter(CAirportDatabase* pAirportDatabase);
	bool deleteDirectFilter(CAirportDatabase* pAirportDatabase);
//////////////////////////old code version, no more use////////////////////////////////////////////////
//the interface to DB
protected:
	int m_ID ;
public:
	void SetID(int id) { m_ID = id ;} ;
	int GetID() { return m_ID ;} ;
public:
	static void ReadDataFromDB(std::vector<CACCategory*>& _dataSet , int _airportID,CACTypesManager* m_pAcMan,DATABASESOURCE_TYPE type) ;
    static void WriteDataToDB(std::vector<CACCategory*>& _dataSet , int _airportID,DATABASESOURCE_TYPE type) ;
	static void DeleteDataFromDB(std::vector<CACCategory*>& _dataSet ,DATABASESOURCE_TYPE type) ;
protected:
	static void ReadDataWithScript(CACCategory* _CAccategory,DATABASESOURCE_TYPE type) ;
	static void ReadDataWithDirectActype(CACCategory* _CAccategory,DATABASESOURCE_TYPE type) ;

	static void WriteDataWithScript(CACCategory* _CAccategory,DATABASESOURCE_TYPE type) ;
	static void WriteDataWithDirectActype(CACCategory* _CAccategory,DATABASESOURCE_TYPE type) ;
protected:
	static void WriteData(CACCategory* _caccategory,int _airportID,DATABASESOURCE_TYPE type) ;
	static void UpdateDate(CACCategory* _caccategory,int _airportID,DATABASESOURCE_TYPE type) ;
	static void DeleteData(CACCategory* _caccategory,DATABASESOURCE_TYPE type) ;
//////////////////////////////////////////////////////////////////////////

};
typedef std::vector<CACCategory*> CACCATEGORYLIST;

class CACCategoryDataSet : public DataSet
{
public:
	CACCategoryDataSet();
	virtual ~CACCategoryDataSet();
	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char *getTitle(void) const { return "AircraftCategories Data"; }
	virtual const char *getHeaders (void) const{  return "Category,IsScript,AC Types,,,"; }

	void SetACCategoryList(CACCATEGORYLIST* pACCategoryList);
	void SetACTypesManager(CACTypesManager* pAcMan);

protected:
	CACCATEGORYLIST* m_pACCategoryList;
	CACTypesManager* m_pAcMan;

//////////////////////new version database read and save/////////////////////////////////////
public:
	//current version read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
//////////////////////old code version,no more use////////////////////////////////////////////////////
//the interface for DataBase operator
protected:
	CACCATEGORYLIST* m_DelCategoryList ;
public:
	void SetDelACCategoryList(CACCATEGORYLIST* pACCategoryList) { m_DelCategoryList = pACCategoryList ;} ;
public:
void ReadDataFromDB(int _airportID,DATABASESOURCE_TYPE type ) ;
void SaveDataToDB(int _airportID,DATABASESOURCE_TYPE type ) ;
void DeleteAllData(int airportID,DATABASESOURCE_TYPE type) ;

//////////////////////////////////////////////////////////////////////////
};


#endif // !defined(AFX_AIRCRAFT_H__731EF4B8_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
