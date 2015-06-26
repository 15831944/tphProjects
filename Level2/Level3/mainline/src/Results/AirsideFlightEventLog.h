#pragma once
#include ".\ARCEventLog.h"
#include "Engine\Airside\AirsideResource.h"

//------------------------------------------------------------
// 
class ResourceDesc
{
public:
	ResourceDesc();
	enum DIR{ POSITIVE, NEGATIVE, }; //for runway and taxiway the direction
	AirsideResource::ResourceType resType;
	int resid; //id of the resource
	DIR resdir;

	int fromnode;  //node from id for taxiway dir seg
	int tonode;		//node to id for taxiway dir seg
	std::string strRes; //res format string
	
	//DECLARE_SERIALIZE
	friend class boost::serialization::access;
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
	{
		ar & resType;
		ar & resid;
		ar & resdir;
	
		ar & fromnode;
		ar & tonode;
		ar & strRes;
	}
	ResourceDesc& operator = (const ResourceDesc& _resource)
	{
		resType = _resource.resType ;
		resid = _resource.resid ;
		resdir = _resource.resdir ;
		fromnode = _resource.fromnode ;
		tonode = _resource.tonode ;
		strRes = _resource.strRes ;
		return *this ;
	}
};



//Indicate the delay of the log
class ARCMobileDesc // the delay reason the other flight and the other vehicle
{
public:
	ARCMobileDesc();
	enum emMobileTYPE{ FLIGHT, VEHICLE, };
	int nMobileID;
	std::string strName;
	emMobileTYPE mType;
	bool operator ==(const ARCMobileDesc& other)const;

	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
	{
		ar & mType;
		ar & nMobileID;
		ar & mType;
		ar & strName;
	}

};
//------------------------------------------------------------


class AirsideFlightEventLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideFlightEventLog,ARCEventLog)
public:
	enum STATE{ NONE, ARR,DEP,INT,}; //arrival , departure, intermediate

	AirsideFlightEventLog(void);
	~AirsideFlightEventLog(void);

	double x,y,z;     //position
	AirsideMobileElementMode mMode;       //
	STATE state;	

	ResourceDesc resDesc;
	double distInRes;

	double dSpd;
	int m_ExitId ;
	//DECLARE_SERIALIZE
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ARCEventLog>(*this);
		ar & x;
		ar & y;
		ar & z;
		
		ar & mMode;
		ar & state;
	
		ar & resDesc;
		ar & distInRes;	
		ar & dSpd;	
		ar & m_ExitId ;

	}
};




#include <Engine/airside/CommonInSim.h>
class AirsideConflictionDelayLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideConflictionDelayLog,ARCEventLog)
public:
	
	AirsideConflictionDelayLog();
	

	ARCMobileDesc motherMobileDesc;
	FlightConflict::ActionType mAction;
	FlightConflict::LocationType mConflictLocation;
	ResourceDesc mAtResource;
	DistanceUnit distInRes;
	int mAreaID;
	std::string sAreaName;
	AirsideFlightEventLog::STATE state;
	AirsideMobileElementMode mMode;
	double mdSpd;
	long mDelayTime;

	FltDelayReason m_emFlightDelayReason; // for flight delay report delay reason
	std::string m_sDetailReason; //for description delay reason for flight delay report
	bool bTheSameDelay(const ARCMobileDesc& conflictMobilDesc,const FlightConflict::ActionType& action, const FlightConflict::LocationType& conflictLocation)const;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ARCEventLog>(*this);
		ar & motherMobileDesc;
		ar & mAction;
		ar & mConflictLocation;
		ar & mAtResource;
		ar & distInRes;
		ar & mAreaID;
		ar & sAreaName;
		ar & state;
		ar & mMode;
		ar & mdSpd;
		ar & mDelayTime;
		ar & state;
		ar & mAreaID;
		ar & sAreaName;
		ar & m_emFlightDelayReason;
		ar & m_sDetailReason;
	}
	static CString GetDelayReasonByCode();
};

class AirsideRunwayCrossigsLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideRunwayCrossigsLog,ARCEventLog)
public:
	enum { OnWaitingCrossRunway,OnEnterRunway,OnExitRunway };

	AirsideRunwayCrossigsLog(void);
	~AirsideRunwayCrossigsLog(void);

	std::string sRunway;
	std::string sTaxiway;
	std::string sNodeName;
	int state;
	double speed;

	//DECLARE_SERIALIZE
	friend class boost::serialization::access;
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ARCEventLog>(*this);
		ar & sRunway;
		ar & sTaxiway;
		ar & sNodeName;
		ar & state;
		ar & speed;
	}
};

enum FLIGHT_PARKINGOP_TYPE;
class AirsideFlightInSim;
class AirsideFlightStandOperationLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideFlightStandOperationLog,ARCEventLog)
public:
	enum StandOperationType
	{
		EnteringStand,
		StandDelay,
		EndDelay,
		LeavingStand
	};

	enum StandDelayReason
	{
		NoDelay = 0, // no delay

		StandOccupied,
		GSEDelay,
		PushbackClearance,
		TaxiwayOccupiedDelay
	};

	AirsideFlightStandOperationLog();

	AirsideFlightStandOperationLog(long lTime, AirsideFlightInSim* pFlight,int nStandID, ALTObjectID altStandID, StandOperationType eOpType);
	~AirsideFlightStandOperationLog(void);


	int m_nStandID; // used to match and compare, 
	ALTObjectID m_altStandID; // Individual ALTObject Name of the Stand
	StandOperationType m_eOpType; // 
	StandDelayReason   m_eDelayReason; // if with delay, m_eDelayReason is the delay reason
	FLIGHT_PARKINGOP_TYPE m_eParkingOpType; // 


	DECLARE_SERIALIZE
};

class AirsideTakeoffProcessLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideTakeoffProcessLog,ARCEventLog)

public:
	AirsideTakeoffProcessLog();
	~AirsideTakeoffProcessLog();

	std::string m_sTakeoffPoistion;
	int m_state;
	long m_lCount;//same queue
	long m_lTotalCount;//all queue
	double m_dDist;
	std::string m_sStartNode;
	std::string m_sEndNode;
	double m_speed;
	double x,y,z;     //position

	friend class boost::serialization::access;
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ARCEventLog>(*this);
		ar & m_sTakeoffPoistion;
		ar & m_state;
		ar & m_lCount;
		ar & m_lTotalCount;
		ar & m_dDist;
		ar & m_sStartNode;
		ar & m_sEndNode;
		ar & m_speed;
		ar & x;
		ar & y;
		ar & z;
	}
};