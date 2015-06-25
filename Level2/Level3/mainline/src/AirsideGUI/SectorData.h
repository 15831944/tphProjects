#pragma once
//////////////////////////////////////////////////////////////////////////
//Author : cjchen
//DATE : 4/20/2008
//////////////////////////////////////////////////////////////////////////
#ifndef _SECTOR_DATA_H
#define _SECTOR_DATA_H
#include <vector>
#include <cstring>
#define  PARSE_CHAR " &"
#define  CURRENT_CHAR  " "
class  CItem
{
public:
	int index ;
	CString val ;
};
class CInterventionrequirement
{
private:
	TCHAR  p_code[1024];

public:
	CInterventionrequirement();
    void setSelected(int index);
	void setNoSelected(int index);
	void setCode(const TCHAR* code) ;
	const TCHAR* getCode() ;
	BOOL isSelected(int index);
};
class CHoldPriority
{
private:
	int index ;
public:
	CHoldPriority();
	void setSelected(int index);
	int getSelected(); 
};
#define PRIORITY1_INDEX  0
#define PRIORITY2_INDEX  1
#define PRIORITY3_INDEX  2
#define PRIORITY4_INDEX  3
//////////////////////////////////////////////////////////////////////////
//This Class is used for parsing String .
//it can parse int value form string and set int value to the string .
//the format of string is setted  by  p_strTok .
//
//EX :  p_string = "1 2 3 4 "  p_strTok = " "
//getInt(0 ,p_string ,p_strTok)  return result : 1 ;
//setInt(1 ,5 ,p_string,p_strTok)   p_string : "1 5 3 4"
//
//////////////////////////////////////////////////////////////////////////
class CParseStringToInt
{
public:
	//return : if success , return val of index ,if error ,return -1 ;
	static int getInt(int index ,const TCHAR* p_string ,const TCHAR* p_strTok);
	
	//return  : val ;
	static int setInt(int index ,int val , TCHAR* p_string ,const TCHAR* p_strTok );
};

//////////////////////////////////////////////////////////////////////////
class CInterventionPriority
{

#define   STR_TOK   " "
private:
	TCHAR code[1024] ;
public:
	CInterventionPriority();
	void setPrioritySelect(int priority , int selected_index);  
	void setCode(const char* p_code) ;
	const TCHAR* getCode() ;
	int  getSelectedIndex(int priority); 
};
class CSectorData
{

private:
	CString name ;
	int nameid ;
    CInterventionrequirement    requirement ;
    CHoldPriority  HoldPriority;
	double adiusOfconcern ;
	CInterventionPriority  InterventionPriority;
	int  ID ;
	BOOL is_Modify ;
	int fixID ; 
public:
	void setInterventionRequirementSelect(int index);
	void setInterventionRequirementNoSelect(int index ) ;
	void setInterventionRequirementSelectCode(const TCHAR* code);
	void setSectorName(const CString& name);
	void setHoldPriority(int index);
	void setInterventionPriority(int priority ,int index);
	void setInterventionPriorityCode(const TCHAR* p_code) ;
	void setAdiusOfconcern(double val) ;
    void setFixID(int id) ;
	void setNameID(int id) ;
	 CInterventionrequirement& getInterventionRequirementSelect();
	 CHoldPriority& getHoldPriority();
	 CInterventionPriority& getInterventionPriority();
	 CString getName();
	 double getAdiusOfconcern();
	 int getFixID() ;
	 void setID(int id) ;
	 int getNameID() ;
     int getID() ;
	 void modified() ;
	 void noModified() ;
	 BOOL isModified() ;
public:
	CSectorData(void);
	~CSectorData(void);
};
#endif

