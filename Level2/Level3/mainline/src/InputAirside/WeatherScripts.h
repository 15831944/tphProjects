#pragma once
#include "../Common/elaptime.h"

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CPrecipitationTypeEventItem
{
public:
	CPrecipitationTypeEventItem():m_ID(-1) {} ;
	~CPrecipitationTypeEventItem() {};
protected:
	int m_ID ;
	ElapsedTime m_startTime ;
	ElapsedTime m_endTime ;
	CString m_Type ;
	CString m_Name ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID()const { return m_ID ;} ;

	void SetStartTime(ElapsedTime _time) { m_startTime = _time ;} ;
	ElapsedTime GetStartTime()const { return m_startTime ;} ;

	void SetEndTime(ElapsedTime _time) { m_endTime = _time ;} ;
	ElapsedTime GetEndTime()const { return m_endTime ;} ; 

	CString GetType()const { return m_Type ;} ;
	void SetType(CString type) { m_Type = type ;} ;

	CString GetName()const { return m_Name ;} ;
	void SetName(CString _name) { m_Name = _name ;} ;
public:
	static void ReadData(std::vector<CPrecipitationTypeEventItem*>* _Data ) ;
	static void WriteData(std::vector<CPrecipitationTypeEventItem*>* _Data ) ;
	static void DelData(std::vector<CPrecipitationTypeEventItem*>* _Data ) ;

	
protected:

	 static void WriteData(CPrecipitationTypeEventItem* _data) ;
	static void DelData(CPrecipitationTypeEventItem* _data) ;
};

class INPUTAIRSIDE_API  CWeatherScripts
{
public:
	CWeatherScripts() {};
	~CWeatherScripts() ;
public:
	typedef std::vector<CPrecipitationTypeEventItem*> TY_DATA ;
	typedef TY_DATA::iterator TY_DATA_ITER ;

	const TY_DATA* GetData()const { return &m_data ;} ;
	TY_DATA* GetData(){ return &m_data ;} ;
	void ReSetData() ;
protected:
	TY_DATA m_data ;
	TY_DATA m_DelData ;
protected:
	void ClearData() ;
	void ClearDelData() ;
public:
	CPrecipitationTypeEventItem* AddPrecipitationTypeEventItem(CString& _name) ;
	void DelPrecipitationTypeEventItem(CPrecipitationTypeEventItem* _Item) ;
public:
	void ReadData() ;
	void SaveData() ;
};
