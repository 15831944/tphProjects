#pragma once
#include "../Common/FLT_CNST.H"
#include <CommonData/procid.h>
#include "../Database/ADODatabase.h"

class CGateAssignmentMgr ;
class  CFlightOperationForGateAssign ;
class CGateOverLapFlightTYItem
{
private:
	FlightConstraint m_FlightTy ;
public:
	CGateOverLapFlightTYItem(FlightConstraint _FlightTy):m_FlightTy(_FlightTy) {} ;
	FlightConstraint GetFlightTy() { return m_FlightTy ;} ;
	void SetFlightTy(FlightConstraint& _flightty) { m_FlightTy = _flightty ;} ;
};

class InputTerminal;
class CGateOverLapFlightTY
{
public:
	typedef std::vector<CGateOverLapFlightTYItem*> DATA_TY ;
	typedef DATA_TY::iterator DATA_TY_ITER ;
private:
	DATA_TY m_OverLap ;
	int m_ID ;
public:
	 CGateOverLapFlightTY() ;
	~CGateOverLapFlightTY() ;
public:
	 void readData( int _parID,InputTerminal* _Terminal) ;
	 void WriteData( int _parID) ;

public:
	CGateOverLapFlightTYItem*  AddFlightTy(FlightConstraint& _flightTy) ;
	void DelFlightTy(CGateOverLapFlightTYItem* _Item) ;
	BOOL CheckTheFlightTYExist(FlightConstraint& _flightTy) ;
	BOOL IsEmpty() { return m_OverLap.empty() ;};
	DATA_TY* GetData() {return &m_OverLap ;} ;
	void ClearData() ;
};

class CGatePreferenceSubItem
{
private:
	FlightConstraint m_Flight ;
	long m_Duration ;
    int m_ID ;
	CGateOverLapFlightTY m_OverLap ;
public:
	CGatePreferenceSubItem(FlightConstraint& _Flight) ;
	~CGatePreferenceSubItem() ;
public:
	static void ReadData(std::vector<CGatePreferenceSubItem*>& _DataSet , int _ParID,InputTerminal* _Terminal) ;
	static void WriteData(std::vector<CGatePreferenceSubItem*> _DataSet , int _ParID) ;
	static void DeleteData(std::vector<CGatePreferenceSubItem*> _DataSet) ;
protected:
	static void ReadDataItem(CGatePreferenceSubItem* _DataSet , int _ParID,InputTerminal* _Terminal) ;
	static void WriteDataItem(CGatePreferenceSubItem* _DataSet , int _ParID) ;
	static void DeleteDataItem(CGatePreferenceSubItem* _DataSet) ;
public:
	void SetFlightTY(FlightConstraint _flightTy) { m_Flight = _flightTy ;} ;
	FlightConstraint GetFlightTY() { return m_Flight ;} ;

	void SetDuration(long _time) { m_Duration = _time ;} ;
	long GetDuration() { return m_Duration ;} ;

	void SetDataBaseIndex(int _id) { m_ID = _id ;} ;
	int GetDataBaseIndex() { return m_ID ;} ;

	CGateOverLapFlightTY* GetOverLapFlightTY() { return &m_OverLap ;} ;
};

class CGateAssignPreferenceItem
{
public:
	enum ASSIGN_PREFERENCE_TYPE{Dep_Preference,Arrival_Preference};
public:
	typedef std::vector<CGatePreferenceSubItem*> DATA_TY ;
	typedef DATA_TY::iterator DATA_TY_ITER ;
private:
	ProcessorID m_GateID ;
	ASSIGN_PREFERENCE_TYPE m_type ;
	int m_ID ;
	DATA_TY m_SubItems ;
	DATA_TY m_DelItem ;
public:
    CGateAssignPreferenceItem(ProcessorID _gate , ASSIGN_PREFERENCE_TYPE type) ;
	~CGateAssignPreferenceItem() ;

	int GetDataIndex() { return m_ID ;} ;
	void SetDataIndex(int _id) { m_ID = _id ;} ;
	ProcessorID GetGateID() { return m_GateID ;} ;
	void SetGateID(ProcessorID& _gateId) { m_GateID = _gateId ;} ;
	DATA_TY* GetSubItemData() { return &m_SubItems ;} ;
	DATA_TY* GetSubItemDelData() { return &m_DelItem ;} ;
	CGatePreferenceSubItem* FindGatePreferenceSubItem(FlightConstraint& _Flight) ;

	ASSIGN_PREFERENCE_TYPE GetType() { return m_type ;} ;
	BOOL GetFlightDurationtime(FlightConstraint& _const,ElapsedTime& _DurationTime) ;

	bool operator < (const CGateAssignPreferenceItem& gateMap)const;
	
public:
	 static void ReadData(std::vector<CGateAssignPreferenceItem*>& _dataSet ,ASSIGN_PREFERENCE_TYPE type ,InputTerminal* _Terminal) ;
	 static void WriteData(std::vector<CGateAssignPreferenceItem*> _dataSet) ;
	 static  void DeleteData(std::vector<CGateAssignPreferenceItem*> _dataSet) ;
protected:
	static void ReadItemData(CADORecordset& _DBSet ,CGateAssignPreferenceItem** _PreferenceItem,InputTerminal* _Terminal) ;
	static void WriteItemData(CGateAssignPreferenceItem* _PreferenceItem) ;
	static void DeleteItemData(CGateAssignPreferenceItem* _PreferenceItem) ;
public:
	CGatePreferenceSubItem* AddItemData(FlightConstraint& _flightTy) ;
	void DelItemData(CGatePreferenceSubItem* _ItemData) ;
protected:
	void Cleardata() ;
	void ClearDelData() ;
}; 

class CGateAdjacency
{
public:
    CGateAdjacency();
    ~CGateAdjacency();
private:
    ProcessorID m_originalGate;
    ProcessorID m_adjacentGate;
    bool m_bReciprocate;
public:
    ProcessorID GetOriginalGate() const { return m_originalGate; }
    void SetOriginalGate(ProcessorID procID) { m_originalGate = procID; }
    ProcessorID GetAdjacentGate() const { return m_adjacentGate; }
    void SetAdjacentGate(ProcessorID procID) { m_adjacentGate = procID; }
    bool IsReciprocate() const { return m_bReciprocate; }
    void SetReciprocate(bool reci) { m_bReciprocate = reci; }
};

class CGateAssignPreferenceMan
{
public:
	CGateAssignPreferenceMan(CGateAssignmentMgr* _GateAssignMgr);
	virtual ~CGateAssignPreferenceMan(void);
	CGateAssignPreferenceItem::ASSIGN_PREFERENCE_TYPE m_type ;
public:
	typedef std::vector<CGateAssignPreferenceItem*>  DATA_TYPE ;
	typedef DATA_TYPE::iterator DATA_TYPE_ITER ;
protected:
	DATA_TYPE m_GateAssignPreference ;
	DATA_TYPE m_DelGateAssignPreference ;
    CGateAssignmentMgr* m_GateAssignMgr ;
    std::vector<CGateAdjacency> m_vAdjacency;
public:
	void ReadData(InputTerminal* _Terminal) ;
	void SaveData() ;
public:
	CGateAssignPreferenceItem* AddPreferenceItem(ProcessorID& _GateID) ;
	//----------------------------------------------------------
	//Summary:
	//	UI need sort reference item by detail
	//-----------------------------------------------------------
	int FindPreferenceIndex(CGateAssignPreferenceItem* pItem);

	void DelPreferenceItem(CGateAssignPreferenceItem* _Item) ;


public:
	 BOOL CheckTheGateByPreference( ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight , bool& bPrefer);
	//---------------------------------------------------------------
	//Summary:
	//		Check Gate whether define flight type
	//---------------------------------------------------------------
	 bool FlightAvailableByPreference(const ProcessorID& gateID,CFlightOperationForGateAssign* pFlight);

protected:
	virtual BOOL CheckTheGate(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer) = 0;
	char GetAssignMode()const;
protected:
	void ClearData() ;
	void ClearDelData() ;
public:
    CGateAssignPreferenceItem* FindItemByGateID(ProcessorID& _GateID) ;
public:
	DATA_TYPE* GetPreferenceData() { return &m_GateAssignPreference ;} ;
	CGateAssignmentMgr* GetGateAssignMgr() { return m_GateAssignMgr ;} ;
	void SetGateAssignMgr(CGateAssignmentMgr* _gateAssignmgr) { m_GateAssignMgr = _gateAssignmgr ;} ;
    std::vector<CGateAdjacency>* GetGateAdjacency() { return &m_vAdjacency; }
};
class CArrivalGateAssignPreferenceMan : public CGateAssignPreferenceMan
{
public:
	CArrivalGateAssignPreferenceMan(CGateAssignmentMgr* _GateAssignMgr):CGateAssignPreferenceMan(_GateAssignMgr)
	{
		m_type = CGateAssignPreferenceItem::Arrival_Preference;
	}
	~CArrivalGateAssignPreferenceMan() {} ;

protected:
	BOOL CheckTheGate(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer) ;
;
};
class CDepGateAssignPreferenceMan : public CGateAssignPreferenceMan
{
public:
	CDepGateAssignPreferenceMan(CGateAssignmentMgr* _GateAssignMgr):CGateAssignPreferenceMan(_GateAssignMgr)
	{
m_type = CGateAssignPreferenceItem::Dep_Preference;
	}
	~CDepGateAssignPreferenceMan() {} ;

protected:
	BOOL CheckTheGate(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer) ;

};
