#pragma once
#include "ARCEventLog.h"
#include <string>
#define SERIALIZE_CODE	friend class boost::serialization::access;\
			template<class Archive>\
			void serialize(Archive & ar, const unsigned int version)
#define SERIALIZE_PARENT(x) ar & boost::serialization::base_object<x>(*this)

class AirsideFlightNodeLog : public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideFlightNodeLog,ARCEventLog)	
public:
	typedef std::string String;
	int mNodeId;
	String mNodeName;
	char mFlightMode; //arr dep 
	


	SERIALIZE_CODE
	{
		SERIALIZE_PARENT(ARCEventLog);
		ar & mFlightMode;
		ar & mNodeId;
		ar & mNodeName;
	}
};


class FlightStartNodeDelayLog : public AirsideFlightNodeLog
{
	DynamicClass
	RegisterInherit(FlightStartNodeDelayLog,AirsideFlightNodeLog)
public:
	int miConflictFlightID;
	String msConflictFlight;
	String msConflictFlightType;
	
	int miConflictFltFromNodeID;
	String msConflictFltFromNode;

	int miConflictFltToNodeID;
	String msConflictFltToNode;

	String msConflictFltOrig;
	String msConflictFltDest;
	SERIALIZE_CODE
	{
		SERIALIZE_PARENT(AirsideFlightNodeLog);
		ar & miConflictFlightID;
		ar & msConflictFlight;
		ar & msConflictFlightType;
		ar & miConflictFltFromNodeID;
		ar & msConflictFltFromNode;
		ar & miConflictFltToNodeID;
		ar & msConflictFltToNode;
		ar & msConflictFltOrig;
		ar & msConflictFltDest;
	}

};

class FlightCrossNodeLog : public AirsideFlightNodeLog
{
	DynamicClass
	RegisterInherit(FlightCrossNodeLog,AirsideFlightNodeLog)

public:
	int mNodeFromId;
	String mNodeFromName;

	int mNodeToId;
	String mNodeToName;

	double mdDist;
	int mEnterStand;//enter stand or not

	String mGlobalOrigin;
	String mGlobalDest;

	enum eventType{ IN_NODE , OUT_NODE , OUT_NODEBUFFER };
	FlightCrossNodeLog(){ meType = IN_NODE; };
	FlightCrossNodeLog(eventType emType){ meType = emType; }
	eventType meType;
	
	SERIALIZE_CODE
	{
		SERIALIZE_PARENT(AirsideFlightNodeLog);
		ar & meType;
		ar & mNodeFromId;
		ar & mNodeFromName;
		ar & mNodeToId;
		ar & mNodeToName;
		ar & mGlobalOrigin;
		ar & mGlobalDest;
		ar & mdDist;
		ar & mEnterStand;
	}
};

