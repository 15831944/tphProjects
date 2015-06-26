#pragma once
#include "../Common/FLT_CNST.H"
#include "../Common/AirportDatabase.h"

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CConditionItem
{
public:
     CConditionItem() ;
	 ~CConditionItem() ;
protected:
	FlightConstraint m_FlightTy ;
	CString m_Condition ;
	int m_ID ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() const { return m_ID ;}

	FlightConstraint* GetFlightType() { return &m_FlightTy ;} ;
	void SetFlightTy(const FlightConstraint& _constraint) { m_FlightTy = _constraint ;} ;
	CString GetCondition()const { return m_Condition ;} ;
	void SetCondition(CString _condition) { m_Condition = _condition ;} ;
public:
	static void ReadData(std::vector<CConditionItem*>* _datas ,int _eventID,CAirportDatabase* _database) ;
	static void WriteData(std::vector<CConditionItem*>* _Datas , int _eventID) ;
	static void DelData(std::vector<CConditionItem*>* _Datas) ;
protected:
	static void WriteData(CConditionItem* _item,int _eventID) ;
	static void DelData(CConditionItem* _item) ;
};

class INPUTAIRSIDE_API CSurfacesInitalEvent
{
public:
    CSurfacesInitalEvent() ;
	~CSurfacesInitalEvent() ;
public:
	typedef std::vector<CConditionItem*> TY_DATA ;
	typedef TY_DATA::iterator TY_DATA_ITER ;
protected:
	TY_DATA m_Data ; 
	TY_DATA m_DelData ;
	int m_ID ;
	ElapsedTime m_startTime ;
	ElapsedTime m_EndTime ;
	CString m_Name ;
protected:
	void ClearData() ;
	void ClearDelData() ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
    
	TY_DATA* GetConditions() { return &m_Data ;} ;
	const TY_DATA* GetConditions()const{ return &m_Data; }
	TY_DATA* GetDelCondition() { return &m_DelData ;} ;
	void SetStartTime(ElapsedTime _time) { m_startTime = _time ;} ;
	ElapsedTime GetStartTime() { return m_startTime ;} ;

	void SetEndTime(ElapsedTime _time) { m_EndTime = _time ;} ;
	ElapsedTime GetEndTime() { return m_EndTime ;} ;

	void SetName(CString _name) { m_Name = _name ;} ;
	CString GetName() { return m_Name ;} ;

public:
	CConditionItem* AddConditionItem(FlightConstraint& _constraint) ;
	void DelConditionItem(CConditionItem* _item) ;
	void DelAllConditionItems() ;
public:
	static void ReadData(std::vector<CSurfacesInitalEvent*>* _datas,CAirportDatabase* _database) ;
    static void WriteData(std::vector<CSurfacesInitalEvent*>* _datas) ;
    static void DelData(std::vector<CSurfacesInitalEvent*>* _datas) ;
protected:
    static void ReadData(CSurfacesInitalEvent* _event ) ;
	static void WriteData(CSurfacesInitalEvent* _event) ;
	static void DelData(CSurfacesInitalEvent* _event) ;
} ;
class INPUTAIRSIDE_API CAircraftSurfaceSetting
{
public:
	CAircraftSurfaceSetting(CAirportDatabase* _database);;
	~CAircraftSurfaceSetting() ;
public:
	typedef std::vector<CSurfacesInitalEvent*> TY_DATA ;
	typedef TY_DATA::iterator TY_DATA_ITER ;
protected:
	TY_DATA m_Data ;
	TY_DATA m_DelData ;
    CAirportDatabase* m_AirportDataBase ; 
public:
	TY_DATA* GetData() { return &m_Data ;} ;
	void ReadData() ;
	void SaveData() ;
	void ReSetData() ;
protected:
	void ClearData() ; 
	void ClearDelData() ;
public:
	 CSurfacesInitalEvent*  AddSurfacesInitalEvent(CString _Name) ;
	void DelSurfacesInitalEvent(CSurfacesInitalEvent* _event) ;

	//get fit surface 


};
