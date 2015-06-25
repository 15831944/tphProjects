#pragma once
#include "PROCESS.H"
#include ".\Common\ALTObjectID.h"
#include "../Results/BridgeConnectorLogEntry.h"
#include <CommonData/ExtraProcProp.h>
#include "Inputs/MISCPROC.H"
class MiscBridgeIDListWithDoor;
class OnboardFlightInSim;

class AirsideFlightInSim;
class StandDoorPriortyMap;

class Terminal;
class BridgeConnector : public Processor, public BridgeConnectorProp
{
public:
	BridgeConnector(void);
	~BridgeConnector(void);

	void FlushLog();

public:
	// Returns ProcessorClassType corresponding to proc type.
	virtual int getProcessorType (void) const { return BridgeConnectorProc; };

	virtual ExtraProcProp* GetExtraProp() { return this; }

	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );
	virtual void Rotate( DistanceUnit _degree );
	virtual void Mirror( const Path* _p );
	void CopyDataToProc(BridgeConnector* pCopyToProc);


	//Returns processor name. It will be override by subclasses.
	//Without special specific, define a processor name value as "Point". 
	virtual const char *getProcessorName (void) const { return "BridgeConnector"; };

	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;
	void initSpecificMisc (const MiscData *p_miscData);
	void initMiscData( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates );
	void appendLinkedStandProcs(const MiscBridgeIDListWithDoor* _pLinkedStandIdList);

	//void setConnectPointCount(int nCount){ m_nConnectPointCount = nCount; }
	
	//void AddConnectPoint(ConnectPoint& conPoint);
	void SetConnectPoint(const ConnectPoint& conPoint);
	const ConnectPoint& GetConnectPoint()const;
	//void ClearConnectPointData(){ m_vConnectPoints.clear(); }
	int GetRandomPoint(Point& pStart, Point& pEnd,Person* pPerson);
	bool GetStartEndPoint( Point& pStart, Point& pEnd);

	void SetAllStandList(const ALTObjectIDList& allStandIDList);
	//status of the connect point
	class ConnectPointStatus
	{
	public:
		ConnectPointStatus(){ mbConnected = false;	}
		bool mbConnected;
		Point m_EndPoint;  //end point of the connect point
		
		void SetLogEntryOfBridge( const CBridgeConnectorLogEntry& _logEntry );
	
		void FlushLog(Terminal* pTerm);
		//log entry
		CBridgeConnectorLogEntry m_logEntryOfBridge;

	};
	//engine part
	ConnectPointStatus m_Status;
	//std::vector<ConnectPointStatus> m_vStatus;
	//ConnectPointStatus& GetConnectPointStatus(int idx);
	AirsideFlightInSim* m_pConnectFlight; //connected flight, null if no
	OnboardFlightInSim* m_pOnboardConnectFlight;
	int m_iDoorIndex;   //connected door index;

	//connect the Flight
	void ConnectFlight(AirsideFlightInSim* pFlight,const ElapsedTime& t);
	void ConnectAirsideFlight(AirsideFlightInSim* pFlight,const ElapsedTime& t);
	void ConnectOnboardFlight(OnboardFlightInSim* pFlight,const ElapsedTime& t);

	void DisAirsideConnect(const ElapsedTime& t);
	void DisOnboardConnect(const ElapsedTime& t);

	bool IsBridgeConnectToFlight();
	bool IsBridgeConnectToFlight(int nFlightIndex);
	bool IsBridgeConnectToStand(const ALTObjectID& standID);

	void AddOccupy(Person* pPerson);
	void Release(Person* pPerson);

	void UpdateFloorIndex(const FloorChangeMap& changMap);

	
	 
private:
	ALTObjectID GetOne2OneStand(const Processor& _pSourceProcessor, const ALTObjectIDList& _destStandArray,int _nDestIdLength);
	bool _connectFlight(const CPoint2008& ptDoorPos, const ElapsedTime& t );
	void DisConnect(const ElapsedTime& t);
	
	typedef std::map<ALTObjectID, MiscBridgeIDWithDoor::DoorPriorityList> StandDoorPriorityMap;
	StandDoorPriorityMap m_doorPriorityMap;

	MiscBridgeIDWithDoor::DoorPriorityList _getLinkDoorPriority(const ALTObjectID& standname)const;
	
};
