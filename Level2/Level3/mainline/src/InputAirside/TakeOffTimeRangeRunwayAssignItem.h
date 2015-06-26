#pragma once
#include "../Common/TimeRange.h"
#include "StrategyType.h"
#include "RunwayExitDiscription.h"
#include "InputAirsideAPI.h"
#include "IntersectionNode.h"

class INPUTAIRSIDE_API CTakeOffPosition
{
public:
	CString m_strName; //do not use any more 
	int m_TakeoffID ;//runway exit id for old code version use
	RunwayExitDescription m_runwayExitDescription;
	double m_dPercent;
	int m_ID ;
public:
	CTakeOffPosition(); ;
protected:
	static CString GetSelectSQL(int _Parid) ;
	static CString GetInsertSQL(CTakeOffPosition* _position ,int _Parid) ;
	static CString GetUpdateSQL(CTakeOffPosition* _position,int _Parid) ;
	static CString GetDeleteSQL(CTakeOffPosition* _position) ;
public:
	static void ReadData(std::vector<CTakeOffPosition*>& _data ,int _Parid) ;
	static void SaveData(std::vector<CTakeOffPosition*>& _data ,int _Parid) ;
	static void DeleteDate(std::vector<CTakeOffPosition*>& _data) ;

};
enum TakeOffDecisionPointType
{
	DP_DepStand,
	DP_Intersection
};
enum TakeOffBackTrack
{
	BT_NoBacktrack,
	BT_MinBacktrack,
	BT_MaxBacktrack,
	BT_BacktrackNUM
};
class INPUTAIRSIDE_API CTakeOffPriorityRunwayItem
{
public:
	CTakeOffPriorityRunwayItem(); ;
	~CTakeOffPriorityRunwayItem();;
	bool HasIntersecion(int intID)const;
public:
	CString m_strRwyName ;
	int m_RunwayID ;
	int m_nRwyMark ;
	std::vector<CTakeOffPosition*> m_TakeOffPosition ;
	std::vector<CTakeOffPosition*> m_DelTakeOffPosition ;
 	std::vector<IntersectionNode> m_vIntersection;
 	std::vector<IntersectionNode> m_vDelIntersection;
	int m_ID ;

	TakeOffDecisionPointType m_eDecision;
	bool m_bTakeOffInSlotTime;
	bool m_bNoFactorForTrailingACSep;
	TakeOffBackTrack m_eBackTrackOption;
public:
	void RemoveAllData() ;
	void RemoveDelData() ;
public:
	void ReadData(int _parid) ;
	void SaveData(int _parid) ;
	void DeleteData() ;

public:
	CTakeOffPosition* AddTakeoffPosition(CString _Name,const RunwayExit& exit) ;
	void RemoveTakeoffPosition(CTakeOffPosition* _posstion) ;

	void SetRunwayName(CString _runway) { m_strRwyName = _runway ;} ;
	CString GetRunwayName() { return m_strRwyName ;} ;

	CTakeOffPosition* FindTakeoffPosition(const RunwayExit& exit);

	TakeOffDecisionPointType GetDecisionPointType(){return m_eDecision;}
	void SetDecisionPointType(TakeOffDecisionPointType pointType){ m_eDecision=pointType;}

	TakeOffBackTrack GetTakeOffBackTrack(){return m_eBackTrackOption;}
	void SetTakeOffBackTrack(TakeOffBackTrack backTrack) {m_eBackTrackOption=backTrack;}

	bool CanTakeOffInSlotTime(){return m_bTakeOffInSlotTime;}
	void SetCanTakeOffInSlotTime(bool bCanTakeoff){ m_bTakeOffInSlotTime=bCanTakeoff;}

	bool NoFactorForTrailingACSep(){return m_bNoFactorForTrailingACSep;}
	void SetNoFactorForTrailingACSep(bool bNoFactor){m_bNoFactorForTrailingACSep=bNoFactor;}

	int AddIntersection(int nID);
	void DelIntersection(int nID);
	int  GetIntersectionNum(){return (int)m_vIntersection.size();}
	CString GetIntersectionName(int nIndex);
	int GetIntersectionID(int nIndex);
	bool IntersectionExist(int nID);
protected:
	CString GetSelectSQL( int _parid) ;
	CString GetInsertSQL(int _parID) ;
	CString GetUpdateSQL() ;

};

class INPUTAIRSIDE_API CManageAssignPriorityData
{
public:
	CManageAssignPriorityData(); ;
	~CManageAssignPriorityData(); ;
public:
	int m_ID ;
	int m_AircraftNum ;
	int m_time ; 
	CString m_Name ;
	CTakeOffPriorityRunwayItem* m_pRunwayItem ;
	std::vector<CTakeOffPriorityRunwayItem*> m_DelItem ;
	bool m_bHasChangeCondition;//the last priority data doesn't have change condition
protected:
	static CString GetSelectSQL(int _ParID) ;
	static CString GetInsertSQL(CManageAssignPriorityData* _data ,int _ParID) ;
	static CString GetUpdateSQL(CManageAssignPriorityData* _data , int _parID) ;
	static CString GetDeleteSQL(CManageAssignPriorityData* _data) ;
public:
	static void ReadData(std::vector<CManageAssignPriorityData*>& _data ,int _parID) ;
	static void SaveData(std::vector<CManageAssignPriorityData*>& _data ,int _parID) ;
	static void DeleteData(std::vector<CManageAssignPriorityData*> _data) ;
public:
	void SetRunwayItem(CTakeOffPriorityRunwayItem* pItem);
	CTakeOffPriorityRunwayItem* GetRunwayItem();
	void RemoveRunwayItem(CTakeOffPriorityRunwayItem* _item) ;

	void GetAssignRunway(int& nRwyID, int& nMark );
	bool HasChangeCondition(){return m_bHasChangeCondition;}
	void SetHasChangeCondition(bool bHasCondition);
	void SetDefaultChangeCondition();
public:
	void RemoveData() ;
	void RemoveDelData() ;
};

class INPUTAIRSIDE_API CTakeOffTimeRangeRunwayAssignItem
{
public:
	CTakeOffTimeRangeRunwayAssignItem(void);
	~CTakeOffTimeRangeRunwayAssignItem(void);

protected:
	int m_ID ;
	RunwayAssignmentStrategyType m_emStrategyType;
	TimeRange m_dtTimeRange;
public:
	void ReadPriorityData() ;
	void WritePriorityData() ;
	void RemoveAll() ;
	void RemoveDeleteData() ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;

	RunwayAssignmentStrategyType GetTimeRangeType() { return m_emStrategyType ;} ;
	void SetTimeRangeType(RunwayAssignmentStrategyType _type) { m_emStrategyType = _type ;} ;

	void SetStartTime(ElapsedTime _range) { m_dtTimeRange.SetStartTime(_range)   ;} ;
	ElapsedTime GetStartTime() { return m_dtTimeRange.GetStartTime() ;} ;

	void SetEndTime(ElapsedTime _time) { m_dtTimeRange.SetEndTime(_time) ; } ;
	ElapsedTime GetEndTime() { return m_dtTimeRange.GetEndTime() ;} ;
public:
	std::vector<CManageAssignPriorityData*> m_Data ;
	std::vector<CManageAssignPriorityData*> m_DelData ;
public:
	CManageAssignPriorityData* AddPriorityRwyData(const CString& _strPriority,int nRwyID, int nRwyMark) ;
	void RemovePriorityRwyData(CManageAssignPriorityData* _PData) ;
	CManageAssignPriorityData* FindPriorityRwyData(int nRwyID, int nRwyMark) ;
	
	bool IsAssignedRunway(int nRwyID, int nRwyMark);
protected:
	static CString GetSelectSQL(int _parID) ;
	static CString GetInsertSQL(CTakeOffTimeRangeRunwayAssignItem* _item ,int _parID) ;
	static CString GetUpdateSQL(CTakeOffTimeRangeRunwayAssignItem* _item ,int _parID) ;
	static CString GetDeleteSQL(CTakeOffTimeRangeRunwayAssignItem* _item) ;
public:
	static void ReadData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data,int _ParID) ;
	static void WriteData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data ,int _parID) ;
	static void DeleteData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data) ;
};
