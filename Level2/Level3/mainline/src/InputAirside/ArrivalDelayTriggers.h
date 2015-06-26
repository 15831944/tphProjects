#pragma once
#include "../Common/Flt_cnst.h"
#include "../InputAirside/Runway.h"
#include "../common/elaptime.h"
#include "..\Database\DBElementCollection_Impl.h"
/////////////////////////////////////////////////////////////////////
//
#include "InputAirsideAPI.h"

class CADORecordset;
class CAirportDatabase;

//class INPUTAIRSIDE_API CArrivalDelayTrigger
//{
//public:
//	CArrivalDelayTrigger();
//	~CArrivalDelayTrigger();
//
//public:
//	void setID(int nID){ m_nID = nID; }
//	int	 getID(){ return m_nID; }
//
//	void setArriDelayTriListID(int nArriDelayTriListID);
//	int	 getArriDelayTriListID();
//
//	void setRunway(Runway runway, int nRunwaymarkIndex);
//	Runway* getRunway();
//
//	int  getRunwayMarkIndex()const; 
//
//	void setWaitingTime(long dWaitingTime);
//	long getWaitingTime() const;
//
//	void setQueueLength(long lQueueLength);
//	long getQueueLength() const;
//
//private:
//	int m_nID;
//	int m_nArriDelayTriListID;
//	Runway m_Runway;
//	long m_waitingTime;
//	long m_lQueueLength;
//	int m_nRunwayIndex;
//
//
//public:
//	void ReadData(CADORecordset& adoRecordset);
//	void UpdateData();
//	void SaveData(int nFltTypeID);
//	void DeleteData();
//};
//
//
///////////////////////////////////////////////////////////////////////
////
//class INPUTAIRSIDE_API CArrivalDelayTriggerList
//{
//public:
//	CArrivalDelayTriggerList(const FlightConstraint& fltType);
//	explicit CArrivalDelayTriggerList(CAirportDatabase* pAirportDatbase);
//	~CArrivalDelayTriggerList();
//
//public:
//	void setID(int nID){ m_nID = nID; }
//	int	 getID(){ return m_nID; }
//
//	void setProjID(int nProjID){ m_nProjID = nProjID; }
//	int	 getProjID(){ return m_nProjID; }
//
//	CString GetFltTypeStr() const;
//	void SetFltType(const CString& strFltType); 
//
//    std::vector<CArrivalDelayTrigger*>& GetAllItem();
//	int GetItemCount();
//    	
//	CArrivalDelayTrigger* GetItem(int nIndex);
//	void DeleteItem(int nIndex);
//
//	void AddItem(CArrivalDelayTrigger* pArriDelayTri);
//
//	FlightConstraint& GetFltType();
//	FlightConstraint GetFltType()const;
//
//private:
//	int m_nID;
//	int m_nProjID;
//
//	FlightConstraint m_fltType;
//
//	// the list of RunwayData
//	std::vector<CArrivalDelayTrigger *> m_vArriDelayTri;
//
//	//the RunwayData need to delete
//	std::vector<CArrivalDelayTrigger*> m_vArriDelayTriNeedDel;
//
//public:
//	BOOL ReadData(CADORecordset& adoRecordset);
//	BOOL SaveData(int nProjID);
//	BOOL UpdateData();
//	BOOL DeleteData();
//protected:
//	BOOL ReadData();
//
//};
namespace AirsideArrivalDelayTrigger
{
	class INPUTAIRSIDE_API CTimeRange : public DBElement
	 {
	 public:
		 CTimeRange();
		 ~CTimeRange();
		ElapsedTime GetStartTime() const;
		ElapsedTime GetEndTime() const;
		void SetStartTime(const ElapsedTime& eStartTime);

		void SetEndTime(const ElapsedTime& strEndTime);


	 protected:
		 ElapsedTime m_eStartTime;
		 ElapsedTime m_eEndTime;

	 protected:
	 public:
		 virtual void InitFromDBRecordset(CADORecordset& recordset);
		 virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
		 virtual void GetUpdateSQL(CString& strSQL) const;
		 virtual void GetDeleteSQL(CString& strSQL) const;
		 virtual void GetSelectSQL(int nID,CString& strSQL)const;

	 private:
	 };

	 class INPUTAIRSIDE_API CTimeRangeList : public DBElementCollection<CTimeRange>
	 {
	 public:
		 CTimeRangeList();
		 ~CTimeRangeList();

	 public:
		 virtual void GetSelectElementSQL(int nFltTypeID,CString& strSQL) const;
		 bool IsTimeInTimeRangeList(const ElapsedTime& tTime);

	 protected:
		mutable int m_nFltTypeID;
	 private:
	 };

	 class INPUTAIRSIDE_API CTriggerCondition : public DBElement
	 {
	 public:
		 enum QueueType
		 {
			Final_Trigger,
			Enroute_Trigger
		 };
		 CTriggerCondition();
		 ~CTriggerCondition();
		 RunwayExit* GetTakeOffPosition();
		 int GetQueueLength();
		 double GetMinsPerAircraft();

		 void SetTakeOffPosition(int nExitID);
		 void SetQueueLength(int nQueueLength);
		 void SetMinsPerAircraft(int nMins);

		 void SetTriggerType(QueueType emType){m_emType = emType;}
		 QueueType GetTriggerType()const {return m_emType;}
	 protected:
		 RunwayExit m_takeOffPosition;
		 int m_nQueueLength;
		 double m_dMinPerAircraft;
		 QueueType m_emType;

	 public:
		 virtual void InitFromDBRecordset(CADORecordset& recordset);
		 virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
		 virtual void GetUpdateSQL(CString& strSQL) const;
		 virtual void GetDeleteSQL(CString& strSQL) const;
		 virtual void GetSelectSQL(int nID,CString& strSQL)const;
	 };


	 class INPUTAIRSIDE_API CTriggerConditionList: public DBElementCollection<CTriggerCondition>
	 {
	 public:
		 CTriggerConditionList();
		 ~CTriggerConditionList();
	
		 virtual void GetSelectElementSQL(int nFltTypeID,CTriggerCondition::QueueType emType,CString& strSQL) const;
		 void ReadData(int nParentID,CTriggerCondition::QueueType emType);
	 protected:
		 mutable int m_nFltTypeID;
	 };


	 class INPUTAIRSIDE_API CRunwayMark :public DBElement
	 {
	 public:
		 Runway* m_pRunway;
		 RUNWAY_MARK m_runwayMark;

	 public:
		 CRunwayMark();
		 ~CRunwayMark();

		 void SetRunwayMark(int nRunwayID, RUNWAY_MARK runwayMark);

		 CString GetRunwayMark();

	 public:
		 virtual void InitFromDBRecordset(CADORecordset& recordset);
		 virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
		 virtual void GetUpdateSQL(CString& strSQL) const;
		 virtual void GetDeleteSQL(CString& strSQL) const;
		 virtual void GetSelectSQL(int nID,CString& strSQL)const;

	 };

	class INPUTAIRSIDE_API CRunwayMarkList: public DBElementCollection <CRunwayMark>
	{
	public:
		CRunwayMarkList();
		~CRunwayMarkList();
	

	public:
		virtual void GetSelectElementSQL(int nFltTypeID,CString& strSQL) const;
		bool IsLogicRunwayInRunwayList(int RwyID, RUNWAY_MARK RwyMark);
		
	protected:
		mutable int m_nFltTypeID;
	};

	 class INPUTAIRSIDE_API CFlightTypeItem :public DBElement
	 {
	 public:
		 enum TriggerType
		 {
			 Independently,
			 Concurrently,
		 };

		 CFlightTypeItem()
		 {
			m_pAirportDB = NULL;
			m_emType = Independently;
			m_tMaxDelayTime = 0L;
		 }
		 explicit CFlightTypeItem(CAirportDatabase* pAirportDatbase)
		 {
			m_pAirportDB = pAirportDatbase;
			m_emType = Independently;
			m_tMaxDelayTime = 0L;
		 }
		 ~CFlightTypeItem()
		 {
		 }
	 public:
		CString GetFlightTypeName();
		const FlightConstraint& GetFltType();

		void SetFlightConstraint(FlightConstraint& fltCons);

		CTimeRangeList& GetTimeRangeList();
		CRunwayMarkList& GetRunwayList();
		CTriggerConditionList& GetTriggerConditionList();
		CTriggerConditionList& GetEnRouteConditionList();

		void SetTriggerType(TriggerType emType) {m_emType = emType;}
		TriggerType GetTriggerType()const {return m_emType;}

		void AddItem(CTimeRange * pTimeRange);
		void AddItem(CRunwayMark *pRunwayMark);
		void AddItem(CTriggerCondition *pTriggerCondiion);

		void DelItem(CTimeRange * pTimeRange);
		void DelItem(CRunwayMark *pRunwayMark);
		void DelItem(CTriggerCondition *pTriggerCondiion);

		ElapsedTime GetMaxTime()const{ return m_tMaxDelayTime; }
		void SetMaxTime(const ElapsedTime& t){ m_tMaxDelayTime = t; }

	 public:
		 virtual void InitFromDBRecordset(CADORecordset& recordset);
		 virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
		 virtual void GetUpdateSQL(CString& strSQL) const;
		 virtual void GetDeleteSQL(CString& strSQL) const;
		 virtual void GetSelectSQL(int nID,CString& strSQL)const;

		 void SaveData(int nParentID);
	 protected:
		FlightConstraint m_fltCons;

		CTimeRangeList m_vTimeRange;

		CRunwayMarkList m_vRunwayMark;

		CTriggerConditionList m_vTriggerCondition;

		CTriggerConditionList m_vEnRouteCondition;

		CAirportDatabase *m_pAirportDB;

		TriggerType m_emType;

		ElapsedTime m_tMaxDelayTime;
	 private:
	 };

}
/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CAirsideArrivalDelayTrigger : public DBElementCollection<AirsideArrivalDelayTrigger::CFlightTypeItem >
{
public:
	CAirsideArrivalDelayTrigger(CAirportDatabase* pAirportDatbase);
	~CAirsideArrivalDelayTrigger();

public:
	//BOOL ReadData(int nProjID);
	//BOOL SaveData(int nProjID);

	//void setID(int nID){ m_nID = nID;}
	//int	 getID(){ return m_nID;}

	void setProjID(int nProjID);
	int getProjID()const;

	void setAirportDB(CAirportDatabase *pAirportDB);
	CAirportDatabase *getAirportDB() const;

	void AddNewItem(AirsideArrivalDelayTrigger::CFlightTypeItem * dbElement);
	
	//BOOL AddItem(AirsideArrivalDelayTrigger::CFlightTypeItem  *pArriFltType);
	//BOOL DelItem(AirsideArrivalDelayTrigger::CFlightTypeItem  *pArriFltType);

	//int GetItemCount();
	//AirsideArrivalDelayTrigger::CFlightTypeItem * GetItem(int nIndex);
	void GetSelectElementSQL( CString& strSQL ) const;
	void ReadData();
	void ReadData(int nProjectID);
private:
	int			m_nID;
	int			m_nProjID;
	CAirportDatabase *m_pAirportDB;

	//std::vector<AirsideArrivalDelayTrigger::CFlightTypeItem *> m_vArriDelayTriList;
	//std::vector<AirsideArrivalDelayTrigger::CFlightTypeItem *> m_vArriDelayTriListNeedDel;

public:
};
