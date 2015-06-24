#pragma once
#include "../../Common/elaptime.h"
#include "../EngineDll.h"
#include "CommonInSim.h"

class ArctermFile;

class  ENGINE_TRANSFER AirsideFlightDelay
{
public:
	AirsideFlightDelay();
	virtual ~AirsideFlightDelay();

	enum DelayType
	{
		enumDelayType_Unknown = 0,
		enumDelayType_Runway,
		enumArrivalDelayTriggers,
		enumFlightConflictAtWayPoint,
		enumPushbackDelay,
		enumTaxiTempParkDelay,
		enumTaxiDirLockDelay,
		enumTaxiLeadFlightDelay,
		enumTaxiIntersectNodeDelay,
		enumVehicleServiceDelay
	};
	
public:

	long GetUniqueID(){ return m_lUniqueID;}
	void SetUniqueID(long lID){ m_lUniqueID = lID;}

	//set at resource object id
	void SetFlightID(int nFltID){ m_nFltId = nFltID; }
	void SetStartTime(const ElapsedTime& startTime){ m_startTime = startTime; }
	void SetDelayTime(const ElapsedTime& delayTime){ m_delayTime = delayTime; }
	//void SetAtObjectID(int nObjID){ m_nAtObjID = nObjID; }
	//void SetAtIntersectionNodeID(int nIntersectNode){ m_nAtNodeID = nIntersectNode; }
	void SetDelayResult(const FltDelayReason& result){ m_result = result; }
	//
	int GetFlightID()const{ return m_nFltId; }
	ElapsedTime GetStartTime()const{ return m_startTime; }
	ElapsedTime GetDelayTime()const { return m_delayTime; }
	FltDelayReason GetResult()const { return m_result; }
	//int GetAtObjectID()const{ return m_nAtObjID; }
	//int GetAtNodeID()const{ return  m_nAtNodeID; }

	//void SetMode(AirsideMobileElementMode mode){ m_FltMode = mode;}
	//AirsideMobileElementMode GetMode(){ return m_FltMode;}

	//
	virtual DelayType GetType()const = 0;
	virtual void WriteDelayReason(ArctermFile& outFile ) =0 ;
	virtual void ReadDelayReason(ArctermFile& inFile) = 0;
	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const =0;
	
	//string detail
	void SetDetailString(const CString& str){ m_strDelayDetail = str; }
	virtual CString GetStringDelayReason(){ return m_strDelayDetail; }

	void WriteLog(ArctermFile& outFile);


	static AirsideFlightDelay* ReadLog(ArctermFile& inFile);

protected:
	long m_lUniqueID;
	//who
	int m_nFltId;
	//when
	ElapsedTime m_startTime;
	ElapsedTime m_delayTime;
	
	//where
	//int m_nAtObjID;
	//int m_nAtNodeID;
	
	//result , stop hold
	FltDelayReason m_result;
	CString m_strDelayDetail;


	//AirsideMobileElementMode m_FltMode;

};



