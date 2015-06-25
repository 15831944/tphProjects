#pragma once
#if !defined(AFX_CFGDATA_H__A40CDB9A_0E44_49E6_A460_505D76BA6414__INCLUDED_)
#define AFX_CFGDATA_H__A40CDB9A_0E44_49E6_A460_505D76BA6414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
  comment : this class is used for .INI file operation , added by cjchen at 2008/8/27 10:00AM  .
*/
class CCfgData  
{
protected:
	//group mapping
	CMapStringToPtr m_StrMapMap;
	//current group
	CString m_strGroup; 
public:
	//struct function
	CCfgData();
	//
	virtual ~CCfgData();
	//load data from file
	/*
	parameter LPCTSTR strFileName load file name
	return success return true ,or return false
	*/
	BOOL LoadCfgData(LPCTSTR strFileName);
	//save data to file
	/*
	parameter LPCTSTR strFileName  save file name
	return  if success  ,return true or return false
	*/
	BOOL SaveCfgData(LPCTSTR strFileName);

	//save config data to string
	/*
	parameter CString* pstr  the point of save CString 
	return if success  ,return true or return false
	*/
	BOOL SaveToStr(CString* pstr);
	//set current handle group 
	/*
	parameter current group name
	return NULL
	*/
	void SetGroup(LPCTSTR strGroup);
	//modify or add a data of current group
	/*
	parameter LPCTSTR strKey the key string of data
	LPCTSTR strValue the data will been setted 
    returnif success  ,return true or return false
	comment if current group exist , modify or add data to current group ,otherwise create the group first !
	*/
	BOOL SetData(LPCTSTR strKey,LPCTSTR strValue);
	//get the data of current group by the key string
	/*
	parameter LPCTSTR strKey ,input : the key string
	LPCTSTR strValue  ,output : the string to data 
	return if find data return true ,otherwise return false 
	*/
	virtual BOOL GetStrData(LPCTSTR strKey,CString &strValue);
	//get long data 
	/*
	parameter LPCTSTR strKey ,input : the key string
	long lValue  ,output : the long to data 
	return if find data return true ,otherwise return false
	*/
	virtual BOOL GetLongData(LPCTSTR strKey,long &lValue);


protected:
	//release memory
	/*
	parameter NULL
	return NULL
	*/
	void RemoveAll();

};
#endif
#define  ARCTERM_INIFILE_NAME  "ARCPort.ini"
/*
the KeyString of  INI FILE For DataBase
*/
#define  ARCTERM_DATABASE     "ARCTerm DataBase"
#define  KEY_ARC_DB_NAME		 "DataBase"
#define  KEY_ARC_DB_PW			"Password" 
#define  KEY_ARC_DB_SERVER		 "Server"
#define  KEY_ARC_DB_USERNAME    "UserName"
#define  KEY_ARC_DB_VERSION     "Version"
/*
the KeyString of  INI FILE For paths
*/
#define  ARCTERM_PATH   "ARCTerm Path"
#define  KEY_ARC_PATH   "AppPath"
/*
the KeyString of  INI FILE For Resource
*/
#define  ARCTERM_RESOURCE    "ARCTerm Resource"
#define  KEY_ARC_RES_VERSION "Version" 
/*
the KeyString of  INI FILE For UNIT Manager
*/
#define  ARCTERM_UNITSMANGER "ARCTerm UnitsManager"
#define  KEY_ARC_UNIT_LEN    "LengthUnits"
#define  KEY_ARC_UNIT_WEIGHT "WeightUnits" 
/*
the KeyString of  INI FILE For Window Position 
*/
#define  ARCTERM_LICENSE  "ARCTerm WindowPosition"
#define  KEY_ARC_POSI_FONT "Fonts"
#define  KEY_ARC_POSI_HEIGHT "Height"
#define  KEY_ARC_POSI_LEFT "Left"
#define  KEY_ARC_POSI_SETTING "Setting"
#define  KEY_ARC_POSI_TOP  "Top"
#define  KEY_ARC_POSI_WIDTH "Width" 
class CINIFileHandle
{
public:
     CINIFileHandle() ;
	 virtual ~CINIFileHandle() ;
public: 
	virtual BOOL LoadFile(const CString& _ProPath) ;
	virtual BOOL SaveData(const CString& _ProPath) ;
    virtual void SetData(CString key , CString _val) ;
	virtual BOOL GetData(CString key,CString& _val);
	virtual void SetCurrentGroup(CString _groupName) ;
protected:
	CString m_currentGroup ;
	CString m_Path ;
	CCfgData  m_File ;
};
class CINIFileForDBHandle : public CINIFileHandle
{
public:
	CINIFileForDBHandle() ;
	virtual ~CINIFileForDBHandle(void);
public:
	void SetDataBaseName(const CString& _name) ;
	BOOL GetDataBaseName(CString& _name) ;

	void SetDBPassword(const CString& _PW) ;
	BOOL GetDBPassword(CString& _PW) ;

	void SetDBServer(const CString& _Ser) ;
	BOOL GetDBServer(CString& _ser) ;

	void SetDBUserName(const CString& _name) ;
	BOOL GetDBUserName(CString& _name) ;
	
	void SetDBVersion(const CString& _ver) ;
	BOOL GetDBVersion(CString& _ver) ;
};
class CINIFileForPathsHandle  : public CINIFileHandle
{
public:
     CINIFileForPathsHandle() ;
	 virtual ~CINIFileForPathsHandle() ;
public:
    void SetPathVal(const CString& _val) ;
	BOOL GetPathVal(CString& _val) ;
};
class CINIFileForResourseHandle : public CINIFileHandle
{
public:
    CINIFileForResourseHandle() ;
	virtual ~CINIFileForResourseHandle() ;
public:
	void SetResourseVersion(const CString& _version) ;
	BOOL GetResoueseVersion(CString& _version) ;
};
class CINIFileForUNITHandle : public CINIFileHandle
{
public:
    CINIFileForUNITHandle() ;
	~CINIFileForUNITHandle() ;
public:
    void SetUnitLenght(const CString& _val) ;
	BOOL GetUnitLenght(CString& _val) ;

	void SetUnitWeight(const CString& _val)  ;
	BOOL GetUnitWeight(CString& _val) ;
};
class CINIFileForLicenseHandle : public CINIFileHandle
{
public:
    CINIFileForLicenseHandle() ;
	~CINIFileForLicenseHandle() ;
public:
	void SetLicenseFONT(const BYTE* _val,int size) ;
	BOOL GetLicenseFont(BYTE* _val,int& size) ;

	void SetLicenseHeight(const CString& _val) ;
	BOOL GetLicenseHeight(CString& _val) ;

	void SetLicenseLeft(const CString& _val) ;
	BOOL GetLicenseLeft(CString& _val)  ;

	void SetLicenseSetting(const BYTE* _val,int size) ;
	BOOL GetlicenseSetting(BYTE* _val,int& size) ;

    void SetLicenseTop(const CString& _val) ;
	BOOL GetLicenseTop(CString& _val) ;

	void SetLicenseWidth(const CString& _val) ;
	BOOL GetLicenseWidth(CString& _val) ;

};
