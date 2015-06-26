#pragma once
#include "..\Common\FLT_CNST.H"
class CTakeoffFlightTypeRunwayAssignment ;
class ALTObjectID;

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CTakeoffStandRunwayAssignItem
{
public:
	CTakeoffStandRunwayAssignItem();
	~CTakeoffStandRunwayAssignItem(void);

	typedef std::vector<CTakeoffFlightTypeRunwayAssignment*> TY_DATA ;
	typedef TY_DATA::iterator TY_DATA_ITER ;
protected:
	int m_ID ;
	CString m_Stand ;
public:
	TY_DATA m_FlightData ;
	TY_DATA m_DelFlightData ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;

	void SetStand(const CString& _str) { m_Stand = _str ;} ;
	CString GetStand() { return m_Stand ;} ;

	void RemoveAllData() ;
	void RemoveDeleteData() ;

protected:
	static CString GetSelectSQL(int _parID) ;
	static CString GetUpdateSQL(CTakeoffStandRunwayAssignItem* _Pitem , int _parID) ;
	static CString GetInsertSQL(CTakeoffStandRunwayAssignItem* _Pitem , int _parID) ;
	static CString GetDeleteSQL(CTakeoffStandRunwayAssignItem* _Pitem) ;
public:
	static void ReadData(std::vector<CTakeoffStandRunwayAssignItem*>& _dataSet,int _ParID,CAirportDatabase* _AirportDB) ;
	static void WriteData(std::vector<CTakeoffStandRunwayAssignItem*>&  _dataSet,int _parID) ;
	static void DeleteData(std::vector<CTakeoffStandRunwayAssignItem*>&  _dataSet) ;
public:
	CTakeoffFlightTypeRunwayAssignment* AddTakeoffFlightType(const FlightConstraint& _FltTy) ;
	void RemoveTakeOffFlightType(CTakeoffFlightTypeRunwayAssignment* _item) ;
	CTakeoffFlightTypeRunwayAssignment* FindFlightTypeItem(const FlightConstraint& _FltTy) ;
};

class INPUTAIRSIDE_API CTakeoffStandRunwayAssign 
{
public:
	CTakeoffStandRunwayAssign(int _proID,CAirportDatabase* _airportDB) ;
	~CTakeoffStandRunwayAssign() ;
public:
	void ReadData() ;
	void WriteData() ;
protected:
	std::vector<CTakeoffStandRunwayAssignItem*> m_Data ;
	std::vector<CTakeoffStandRunwayAssignItem*> m_DelData ;
	int m_ProID ;
	CAirportDatabase* m_AirportDB ;
public:
	CTakeoffStandRunwayAssignItem* AddTakeoffStandItem(const CString& _stand) ;
	void RemoveStandItem(CTakeoffStandRunwayAssignItem* _standItem) ;
	CTakeoffStandRunwayAssignItem* FindStandItem(const CString& _stand) ;
public:
	void ClearData() ;
	void ClearDelData() ;
	void GeStandRunwayAssignData(const ALTObjectID& standName, std::vector<CTakeoffStandRunwayAssignItem*>& vFitStandData);
	void Sort();

	std::vector<CTakeoffStandRunwayAssignItem*>* GetStandData() { return &m_Data ;} ;
};
