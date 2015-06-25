#include "StdAfx.h"
#include ".\bridgeconnector.h"
#include "Main/TermPlanDoc.h"
#include "Inputs\miscproc.h"
#include "terminal.h"
#include "../Engine/Airside/AirsideFlightInSim.h"
#include "../Results/BridgeConnectorLog.h"
#include "Common/ARCTracker.h"
#include "../Engine/OnboardFlightInSim.h"
#include "OnboardDoorInSim.h"
#include "OnboardSimulation.h"
#include "Common/FloorChangeMap.h"
#include "FIXEDQ.H"
#include "Airside/FlightOpenDoors.h"
#include "InputAirside/InputAirside.h"
#include "Airside/StandInSim.h"
#include "BridgeQueue.h"
#include "Inputs/BridgeConnectorPaxData.h"


BridgeConnector::BridgeConnector(void)
{
	//m_vConnectPoints.clear();

	m_connectPoint.m_dWidth = 3*SCALE_FACTOR;
	m_connectPoint.m_dLength = 10*SCALE_FACTOR;
	Point pos;
	m_location.init(1);
	m_location[0] = pos;	
	m_pConnectFlight = NULL;
	m_pOnboardConnectFlight = NULL;
}

BridgeConnector::~BridgeConnector(void)
{
}

int BridgeConnector::readSpecialField( ArctermFile& procFile )
{

	if (procFile.getVersion() > 2.60f)
	{
		Point ctPoint;
		procFile.getPoint(ctPoint);
		m_connectPoint.m_Location = ctPoint;

		procFile.getFloat(m_connectPoint.m_dWidth);
		procFile.getFloat(m_connectPoint.m_dLength);

		procFile.getPoint(ctPoint);
		m_connectPoint.m_dirFrom = ctPoint;
		procFile.getPoint(ctPoint);
		m_connectPoint.m_dirTo = ctPoint;
	}
	else
	{
		int m_nConnectPointCount=0;
		procFile.getInteger( m_nConnectPointCount );

		for (int i =0; i < m_nConnectPointCount; i++)
		{
			BridgeConnector::ConnectPoint conPoint;

			Point ctPoint;
			procFile.getPoint(ctPoint);
			conPoint.m_Location = ctPoint;

			procFile.getFloat(conPoint.m_dWidth);
			procFile.getFloat(conPoint.m_dLength);

			procFile.getPoint(ctPoint);
			conPoint.m_dirFrom = ctPoint;
			procFile.getPoint(ctPoint);
			conPoint.m_dirTo = ctPoint;

			m_vConnectPoints.push_back(conPoint);

		}
	}
	

	return TRUE;
}

int BridgeConnector::writeSpecialField( ArctermFile& procFile ) const
{
	//procFile.writeInt(m_nConnectPointCount);

	//for (int i =0; i < m_nConnectPointCount; i++)
	//{
	//	BridgeConnector::ConnectPoint conPoint = m_vConnectPoints.at(i);

	//	procFile.writePoint(conPoint.m_Location);
	//	procFile.writeDouble(conPoint.m_dWidth);
	//	procFile.writeDouble(conPoint.m_dLength);
	//	procFile.writePoint(conPoint.m_dirFrom);
	//	procFile.writePoint(conPoint.m_dirTo);
	//}

	procFile.writePoint(m_connectPoint.m_Location);
	procFile.writeDouble(m_connectPoint.m_dWidth);
	procFile.writeDouble(m_connectPoint.m_dLength);
	procFile.writePoint(m_connectPoint.m_dirFrom);
	procFile.writePoint(m_connectPoint.m_dirTo);

	return TRUE;
}



//void BridgeConnector::AddConnectPoint( ConnectPoint& conPoint )
//{
//	m_vConnectPoints.push_back(conPoint);
//}

void BridgeConnector::SetConnectPoint( const ConnectPoint& conPoint )
{
	m_connectPoint = conPoint;
}

void BridgeConnector::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	Processor::DoOffset(_xOffset,_yOffset);
	//for(int i=0;i<(int)m_vConnectPoints.size();i++)
	{
		ConnectPoint& cp = m_connectPoint;//m_vConnectPoints[i];
		Point offset(_xOffset,_yOffset,0);
		cp.m_dirFrom+=offset;
		cp.m_dirTo += offset;
		cp.m_Location+=offset;
	}
}

void BridgeConnector::Rotate( DistanceUnit _degree )
{
	Processor::Rotate(_degree);
	//for (int i =0; i < m_nConnectPointCount; i++)
	{
		ConnectPoint* pPoint = &m_connectPoint;//m_vConnectPoints.at(i);
		pPoint->m_Location.rotate(_degree);
		pPoint->m_dirFrom.rotate(_degree);

	}
}

void BridgeConnector::Mirror( const Path* _p )
{
	Processor::Mirror(_p);
}

//BridgeConnector::ConnectPointStatus& BridgeConnector::GetConnectPointStatus( int idx )
//{
//	return m_vStatus[idx];
//}

bool BridgeConnector::_connectFlight(const CPoint2008& ptDoorPos, const ElapsedTime& t )
{

	{
		BridgeConnector::ConnectPointStatus& connectStatus = m_Status; //(idxDoor);

		if(!connectStatus.mbConnected)
		{
			//write the current position log
			ElapsedTime eCurTime = t;
			BridgeConnectorEventStruct bridgeBeforeEventDesc;

			bridgeBeforeEventDesc.time = (long)eCurTime;
			bridgeBeforeEventDesc.state = 'S'; // start connect

			bridgeBeforeEventDesc.x = connectStatus.m_EndPoint.getX();
			bridgeBeforeEventDesc.y = connectStatus.m_EndPoint.getY();
			bridgeBeforeEventDesc.z = connectStatus.m_EndPoint.getZ();

			connectStatus.m_logEntryOfBridge.addEvent(bridgeBeforeEventDesc);
			connectStatus.mbConnected = true;


			//write after the current position log
		
			ElapsedTime eAfterTime = t + ElapsedTime(60L);//add 60 seconds move time
			connectStatus.m_tLastTime = eAfterTime;

			BridgeConnectorEventStruct bridgeAfterEventDesc;
			bridgeAfterEventDesc.time = (long)eAfterTime;
			bridgeAfterEventDesc.state = 'C'; // connect

			//end position
			connectStatus.m_EndPoint.setX(ptDoorPos.getX());
			connectStatus.m_EndPoint.setY(ptDoorPos.getY());
			connectStatus.m_EndPoint.setZ(ptDoorPos.getZ());

			bridgeAfterEventDesc.x = ptDoorPos.getX();
			bridgeAfterEventDesc.y = ptDoorPos.getY();
			bridgeAfterEventDesc.z = ptDoorPos.getZ();

			connectStatus.m_logEntryOfBridge.addEvent(bridgeAfterEventDesc);

			//generate queue for bridge
			GenerateQueue();
			return true;
		}
	}

	return false;
}

void BridgeConnector::ConnectFlight( AirsideFlightInSim* pFlight,const ElapsedTime& t )
{
	if(pFlight == NULL)
		return;

	if (pFlight->GetEngine()->IsOnboardSel())
	{
		OnboardSimulation* pOnboardSim = pFlight->GetEngine()->GetOnboardSimulation();
		if (pOnboardSim)
		{
			bool bArrival = pFlight->IsArrivingOperation() ? true : false;
			OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(pFlight,bArrival);
			if (pOnboardFlightInSim)
			{
				ConnectOnboardFlight(pOnboardFlightInSim, t);
			}			
		}
	}

	//try connect to flight
	if(!IsBridgeConnectToFlight())
		ConnectAirsideFlight(pFlight,t);

	if(IsBridgeConnectToFlight())
		GenerateQueue();
}

void BridgeConnector::ConnectAirsideFlight( AirsideFlightInSim* pFlight,const ElapsedTime& t )
{
	if(pFlight == NULL)
		return;

	CFlightOpenDoors* openDoors = pFlight->OpenDoors(t);
	if(!openDoors)
	{
		ASSERT(FALSE);
		return;
	}
	StandInSim* pStand = pFlight->GetOperationParkingStand();
	if(!pStand)
	{
		ASSERT(FALSE);
		return ;
	}


	MiscBridgeIDWithDoor::DoorPriorityList doorPriority =  _getLinkDoorPriority(pStand->GetStandInput()->getName());

	for(size_t i =0; i < doorPriority.size();++i)
	{
		const MiscBridgeIDWithDoor::DoorPriority& p = doorPriority[i];
		int doorIdx = openDoors->getDoorIndex(p.m_iHandType,p.m_iIndex-1);
		if( doorIdx >= 0)
		{
			COpenDoorInfo& doorInfo = openDoors->getDoor(doorIdx);
			if(doorInfo.m_bConnected)
				continue;
			if(_connectFlight(doorInfo.mDoorPos,t))
			{
			
				m_pConnectFlight = pFlight;
				m_iDoorIndex = doorIdx;
				doorInfo.m_bConnected = true;
				return;
			}
		}
	}
	//use the first one
	for(int i=0;i< openDoors->getCount();i++)
	{
		COpenDoorInfo& doorInfo = openDoors->getDoor(i);
		if(doorInfo.m_bConnected)
			continue;
		if(_connectFlight(doorInfo.mDoorPos,t))
		{
			
			m_pConnectFlight = pFlight;
			m_iDoorIndex = 0;
			doorInfo.m_bConnected = true;
			return;
		}
	}
	
}

void BridgeConnector::ConnectOnboardFlight( OnboardFlightInSim* pFlight,const ElapsedTime& t )
{
	if(pFlight == NULL)
		return;

	std::vector<OnboardDoorInSim *>* pDoorSet = pFlight->GetOnboardDoorList();

	//int idxDoor = 0;
	for (size_t i = 0; i < pDoorSet->size(); i++)
	{
		OnboardDoorInSim* pDoorInSim = pDoorSet->at(i);
		if (pDoorInSim->ConnectValid())
		{
			//int nConnnectCount = GetConnectPointCount();

			CPoint2008 ptDoorPos;
			ptDoorPos = pDoorInSim->GetConnectionPos();
			
			if (_connectFlight(ptDoorPos,t))
			{
				m_pOnboardConnectFlight = pFlight;
				return;
			}
		}
	}
}

void BridgeConnector::DisAirsideConnect( const ElapsedTime& t )
{
	cpputil::autoPtrReset(m_pQueue);
	DisConnect(t);
	m_pConnectFlight = NULL;
	m_iDoorIndex = -1;
}

void BridgeConnector::DisOnboardConnect( const ElapsedTime& t )
{
	cpputil::autoPtrReset(m_pQueue);

	if (m_pOnboardConnectFlight == NULL)
		return;
	
	if (occupants.IsEmpty())
	{
		m_pOnboardConnectFlight->ProcessCloseDoor(t);
		DisConnect(t);
		m_pOnboardConnectFlight = NULL;
		m_iDoorIndex = -1;
	}
}

void BridgeConnector::DisConnect(const ElapsedTime& t)
{
	BridgeConnector::ConnectPointStatus& connectStatus = m_Status;//(idxDoor);

	if (m_pConnectFlight == NULL && m_pOnboardConnectFlight == NULL)
		return;
	
	if(connectStatus.mbConnected)
	{
		connectStatus.mbConnected = false;
		//write after the current position log
		ElapsedTime eAfterTime = t;//add 60 seconds move time	

		BridgeConnectorEventStruct bridgeAfterEventDesc;
		bridgeAfterEventDesc.time = (long)eAfterTime;
		bridgeAfterEventDesc.state = 'D'; // disconnect

		bridgeAfterEventDesc.x = connectStatus.m_EndPoint.getX();
		bridgeAfterEventDesc.y = connectStatus.m_EndPoint.getY();
		bridgeAfterEventDesc.z = connectStatus.m_EndPoint.getZ();
		connectStatus.m_logEntryOfBridge.addEvent(bridgeAfterEventDesc);


		const ConnectPoint& cp = m_connectPoint;//m_vConnectPoints[idxDoor];
// 		Point endPoint = (cp.m_dirTo - cp.m_dirFrom);
// 		endPoint.length(cp.m_dLength);
// 		endPoint+=cp.m_dirFrom;
// 		onnectStatus.m_EndPoint = cp.m_dirFrom;
		Point pVector;
		pVector.initVector3D( cp.m_Location, cp.m_dirFrom );
		pVector.length3D(cp.m_dLength);
		pVector.plus(cp.m_Location);
		connectStatus.m_EndPoint = pVector;


		eAfterTime = (t+ElapsedTime(60L));	
		connectStatus.m_tLastTime = eAfterTime;

		bridgeAfterEventDesc.time = (long)eAfterTime;
		bridgeAfterEventDesc.state = 'I'; // stop, bridge return to original state

		bridgeAfterEventDesc.x = connectStatus.m_EndPoint.getX();
		bridgeAfterEventDesc.y = connectStatus.m_EndPoint.getY();
		bridgeAfterEventDesc.z = connectStatus.m_EndPoint.getZ();

		connectStatus.m_logEntryOfBridge.addEvent(bridgeAfterEventDesc);		
		
	}
	m_pConnectFlight = NULL;
	m_pOnboardConnectFlight = NULL;
}

void BridgeConnector::initMiscData( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates )
{
	m_bDisallowGroup = _bDisallowGroup;
	visitorFlag = visitors;
}

void BridgeConnector::initSpecificMisc (const MiscData *p_miscData)
{
	const ConnectPoint& cp = m_connectPoint;
	Point pVector;
	pVector.initVector3D( cp.m_Location, cp.m_dirFrom );
	pVector.length3D(cp.m_dLength);
	pVector.plus(cp.m_Location);
	m_Status.m_EndPoint = pVector;
	m_pConnectFlight = NULL;

	if( p_miscData != NULL )
	{
		std::vector<int> vAirports;
		InputAirside::GetAirportList(GetTerminal()->m_nProjID,vAirports);
		if (vAirports.empty())
		{
			return;
		}
		int airportID = vAirports.front();
		
		ALTObjectIDList tempList;
		ALTObject::GetObjectNameList(ALT_STAND,airportID,tempList);
		SetAllStandList(tempList);

		MiscBridgeConnectorData *data = (MiscBridgeConnectorData *)p_miscData;
		MiscBridgeIDListWithDoor* misIdList = data->GetBridgeConnectorLinkedProcList();
		appendLinkedStandProcs(misIdList);
	}
	

}

void BridgeConnector::SetAllStandList(const ALTObjectIDList& allStandIDList)
{
	m_AllStandIDList = allStandIDList;
}

void BridgeConnector::appendLinkedStandProcs(const MiscBridgeIDListWithDoor* _pLinkedStandIdList)
{
	m_standIDList.clear();
	m_doorPriorityMap.clear();

	if (_pLinkedStandIdList==NULL) return;

	const MiscBridgeIDListWithDoor* pMiscLinkedBridgeIDList = _pLinkedStandIdList;
	assert( m_pTerm );	


	for (int i = 0; i < pMiscLinkedBridgeIDList->getCount(); i++)
	{
		const MiscBridgeIDWithDoor* pIDWithFlag = (const MiscBridgeIDWithDoor*) pMiscLinkedBridgeIDList->getID( i ) ;
		if( pIDWithFlag )
		{
			ALTObjectIDList tempArray;
			ALTObjectID standID(pIDWithFlag->GetIDString());
			m_doorPriorityMap[standID] = pIDWithFlag->m_vDoorPriority;

			for (int j = 0; j < (int)m_AllStandIDList.size(); j++)
			{
				if (m_AllStandIDList.at(j).idFits(standID))
				{
					tempArray.push_back(m_AllStandIDList.at(j));
				}
			}

			if( pIDWithFlag->getOne2OneFlag() )//is one to one 
			{
				//get 1:to:1
				ALTObjectID DestStandID = GetOne2OneStand( *this, tempArray, pMiscLinkedBridgeIDList->getID( i )->idLength());
				if( !DestStandID.IsBlank())
				{
					m_standIDList.push_back(DestStandID);
				}
			}
			else
			{
				m_standIDList.insert(m_standIDList.end(),tempArray.begin(),tempArray.end());
			}
		}

	}	
}

ALTObjectID BridgeConnector::GetOne2OneStand(const Processor& _pSourceProcessor, const ALTObjectIDList& _destStandArray,int _nDestIdLength)
{
	// TRACE( "-%s-=\n", _pSourceProcessor.getID()->GetIDString());
	ALTObjectIDList arrayResult;
	ALTObjectIDList arrayCandidated( _destStandArray );
	int nDeltaFromLeaf = 0;
	ALTObjectID blankID;

	while( arrayCandidated.size() > 0 )
	{
		// move 
		int nCount = arrayCandidated.size();
		if( nCount == 1 )
			return arrayCandidated.at( 0 );

		ALTObjectIDList arrayTemp( arrayCandidated );
		arrayCandidated.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = _pSourceProcessor.getID()->idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	// stop on the first level.
		_pSourceProcessor.getID()->getNameAtLevel( szSourceLevelName, nSourceIdLength );
		nCount = arrayTemp.size();
		for( int i=0; i<nCount; i++ )
		{
			ALTObjectID standID = arrayTemp.at( i );

			int nThisIdLenght = standID.idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= _nDestIdLength )
			{
				arrayResult.Add( standID );
			}
			else
			{
				if( strcmp( standID.at(nThisIdLenght-1).c_str(), szSourceLevelName ) == 0 )
				{
					arrayCandidated.Add( standID );
				}
			}
		}

		if( arrayCandidated.size() == 0 )
		{
			return blankID;
		}
		nDeltaFromLeaf++;
	}
	int nCount = arrayResult.size();
	if( nCount == 0 || nCount > 1 )
		return blankID;
	return arrayResult.at(0);	
}

bool BridgeConnector::IsBridgeConnectToStand(const ALTObjectID& standID)
{
	ALTObjectIDList::iterator iter = std::find(m_standIDList.begin(),m_standIDList.end(),standID);
	if (iter != m_standIDList.end())
	{
		return true;
	}
	return false;
}

bool BridgeConnector::IsBridgeConnectToFlight(int nFlightIndex)
{
	if (m_Status.mbConnected == false)
	{
		return false;
	}

	if (m_pConnectFlight)
	{
		if (nFlightIndex != m_pConnectFlight->GetFlightInput()->getFlightIndex())
			return false;
	}

	if (m_pOnboardConnectFlight)
	{
		if(nFlightIndex != m_pOnboardConnectFlight->GetFlightInput()->getFlightIndex())
			return false;
	}

	return true;
}

bool BridgeConnector::IsBridgeConnectToFlight()
{
	return m_pConnectFlight || m_pOnboardConnectFlight;
}

bool BridgeConnector::GetStartEndPoint( Point& pStart, Point& pEnd )
{
	//if (nBridgeIndex == -1)
	//	return false;

	//if (nBridgeIndex >= GetConnectPointCount())
	//	return false;

	ConnectPointStatus& conPtStatus = m_Status;//(nBridgeIndex);
	ConnectPoint conPt = m_connectPoint;//(nBridgeIndex);
	if (conPtStatus.mbConnected)
	{
		pStart = conPt.m_Location;
		pEnd = conPtStatus.m_EndPoint;
		return true;
	}

	return false;
}

int BridgeConnector::GetRandomPoint(Point& pStart, Point& pEnd,Person* pPerson)
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pOnboardConnectFlight)
	{
		CString strError;
		OnboardSeatInSim* pSeatInSim = m_pOnboardConnectFlight->GetSeat(pPerson,strError);
		if (pSeatInSim == NULL)
			return -1;

		OnboardDoorInSim* pDoorInSim = m_pOnboardConnectFlight->GetDoor(pPerson,pSeatInSim);
		if (pDoorInSim == NULL)
			return -1;
		
		//for (int i = 0; i < GetConnectPointCount(); i++)
		{
			ConnectPointStatus& conPtStatus = m_Status;//GetConnectPointStatus(i);
			ConnectPoint& conPt = m_connectPoint; //(i);
			CPoint2008 ptEnd;
			ptEnd.init(conPtStatus.m_EndPoint.getX(),conPtStatus.m_EndPoint.getY(),conPtStatus.m_EndPoint.getZ());
			if (conPtStatus.mbConnected && ptEnd == pDoorInSim->GetConnectionPos())
			{
				pStart = conPt.m_Location;
				pEnd = conPtStatus.m_EndPoint;
				return 0;
			}
		}

	}
	else
	{
		std::vector<ConnectPointStatus> vStatus;
		std::vector<ConnectPoint>vConPt;
		//for (int i = 0; i < GetConnectPointCount(); i++)
		{
			ConnectPointStatus& conPtStatus = m_Status;//GetConnectPointStatus(i);
			ConnectPoint conPt = m_connectPoint; //(i);
			if (conPtStatus.mbConnected)
			{
				vStatus.push_back(conPtStatus);
				vConPt.push_back(conPt);
			}
		}
		if (vStatus.size() > 0)
		{
			int nRadomIndex = random(vStatus.size());
			pStart = vConPt[nRadomIndex].m_Location;
			pEnd = vStatus[nRadomIndex].m_EndPoint;
			return nRadomIndex;
		}
	}
	
	return -1;
}

void BridgeConnector::FlushLog(const ElapsedTime& t)
{
	cpputil::autoPtrReset(m_pQueue);
	m_Status.FlushLog(t,m_pTerm);
	m_pConnectFlight = NULL;
	m_pOnboardConnectFlight = NULL;
}

void BridgeConnector::CopyDataToProc( BridgeConnector* pCopyToProc )
{
	//pCopyToProc->m_nConnectPointCount = m_nConnectPointCount;
	//pCopyToProc->m_vConnectPoints.assign(m_vConnectPoints.begin(),m_vConnectPoints.end());
	pCopyToProc->m_connectPoint = m_connectPoint ;
}

void BridgeConnector::AddOccupy( Person* pPerson )
{
	occupants.addItem(pPerson);
}

void BridgeConnector::Release( Person* pPerson )
{
	int nPos = occupants.Find(pPerson);
	if (nPos != INT_MAX)
	{
		occupants.removeItem(nPos);
	}
}

void BridgeConnector::UpdateFloorIndex( const FloorChangeMap& changMap )
{
	__super::UpdateFloorIndex(changMap);
	//for (int i =0; i < m_nConnectPointCount; i++)
	{
		BridgeConnector::ConnectPoint conPoint = m_connectPoint; //m_vConnectPoints.at(i);
		changMap.ChangePointFloor(conPoint.m_Location);
		m_connectPoint = conPoint;
		//m_vConnectPoints.at(i) = conPoint;
		//conPoint.m_Location.setZ( changMap.getNewFloor(conPoint.m_Location.getZ()) );
	}
}

MiscBridgeIDWithDoor::DoorPriorityList BridgeConnector::_getLinkDoorPriority( const ALTObjectID& standname ) const
{
	for(StandDoorPriorityMap::const_iterator itr = m_doorPriorityMap.begin();itr!=m_doorPriorityMap.end();++itr)
	{
		if( standname.idFits(itr->first))
		{
			return itr->second;
		}
	}
	return MiscBridgeIDWithDoor::DoorPriorityList();
}

const BridgeConnector::ConnectPoint& BridgeConnector::GetConnectPoint() const
{
	return m_connectPoint;
}

void BridgeConnector::beginService( Person *person, ElapsedTime curTime )
{
	ArrDepBridgeState BridgeState  = person->getTerminalBehavior()->getBridgeState();
	if( BridgeState == DepBridge)
	{
		CMobileElemConstraintDatabase* pDataBase = person->GetTerminal()->bcPaxData->getPaxData(EntryFlightTimeDatabase);
		const ProbabilityDistribution* dist = pDataBase->FindFit(person->getType());
		ElapsedTime serviceT(0L);
		if(dist)
		{
			serviceT = ElapsedTime(dist->getRandomValue());
		}
		//
		person->SetWalkOnBridge(TRUE);
		/*	TerminalMobElementBehavior* spTerminalBehavior = person->getTerminalBehavior();
		if (spTerminalBehavior != NULL)
		spTerminalBehavior->SetWalkOnBridge(TRUE);*/
		//person->setState();
		person->generateEvent (curTime + serviceT,false);
		return;
	}
	
	if(BridgeState == ArrBridge)
	{
		removePerson(person);
		Point startPs, endPs;

		TerminalMobElementBehavior* spTerminalBehavior = person->getTerminalBehavior();
		if (spTerminalBehavior == NULL)
			return;

		GetStartEndPoint(startPs,endPs);
		person->setTerminalDestination(startPs);
		person->setTerminalLocation(endPs);

		//cal person move from flight to gate
		ElapsedTime t;
		double dLxy = startPs.distance(endPs);
		double dLz = 0.0;
		double dL = dLxy;
		dLz = fabs(endPs.getZ() - startPs.getZ());
		Point pt(dLxy, dLz, 0.0);
		dL = pt.length();
		double time = dL;
		t = (float) (time / (double)person->getSpeed() );
		
		person->SetWalkOnBridge(TRUE);//spTerminalBehavior->SetWalkOnBridge(TRUE);

		person->generateEvent (curTime + t,false);
		return;
	}

	ASSERT(FALSE);
	
}

//Queue for departure passenger to leave
void BridgeConnector::GenerateQueue()
{
	if (m_pQueue)
	{
		delete m_pQueue;
		m_pQueue = NULL;
	}
	
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	assert( pDoc );
	double dLz = pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(m_connectPoint.m_Location.getZ());//GetFloor2(nFloorFrom)->RealAltitude();
	Point ptConnect= m_connectPoint.m_Location;
	ptConnect.z = dLz;

	Path tempPath;
	tempPath.init(2);
	tempPath[1]= ptConnect;
	tempPath[0] = m_Status.m_EndPoint;

	m_pQueue = new BridgeQueue(m_connectPoint.m_Location, tempPath);
}

//arrival passenger doesn't have queue
void BridgeConnector::getNextState( int& state,Person* _pPerson ) const
{
	assert( _pPerson );
	
	if (_pPerson->getLogEntry().isArrival())
	{
		if (state == FreeMoving && In_Constr.getCount())
			state = MoveAlongInConstraint;
		else if (state == LeaveServer && Out_Constr.getCount())
			state = MoveAlongOutConstraint;
		else if (state == LeaveServer)
			state = FreeMoving;
		else
			state = ArriveAtServer;
	}
	else//departure part
	{
		if (state == FreeMoving && In_Constr.getCount())
			state = MoveAlongInConstraint;
		else if (state == LeaveServer && Out_Constr.getCount())
			state = MoveAlongOutConstraint;
		else if (state == LeaveServer)
			state = FreeMoving;
		else if (m_pQueue != NULL)
			state = MoveToQueue;
		else
			state = ArriveAtServer;
	}
}

Point BridgeConnector::AcquireServiceLocation( Person* _pPerson )
{
	if (_pPerson->getLogEntry().isArrival())
	{
		return m_Status.m_EndPoint;
	}
	else
	{
		return m_connectPoint.m_Location;
	}
}

void BridgeConnector::getNextLocation( Person *aPerson )
{
	if(aPerson->getState()==ArriveAtServer )
	{
		aPerson->setTerminalDestination(m_Status.m_EndPoint);
		return;
	}
	return __super::getNextLocation(aPerson);	
}


void BridgeConnector::ConnectPointStatus::SetLogEntryOfBridge( const CBridgeConnectorLogEntry& _logEntry )
{
	m_logEntryOfBridge = _logEntry;
}

void BridgeConnector::ConnectPointStatus::FlushLog(const ElapsedTime& t,Terminal* pTerm)
{
	if(!pTerm)
		return;

	if(t>m_tLastTime)
	{
		BridgeConnectorEventStruct bridgeAfterEventDesc;
		bridgeAfterEventDesc.time = t;
		bridgeAfterEventDesc.state = mbConnected?'C':'I'; // stop, bridge return to original state

		bridgeAfterEventDesc.x = m_EndPoint.getX();
		bridgeAfterEventDesc.y = m_EndPoint.getY();
		bridgeAfterEventDesc.z = m_EndPoint.getZ();

		m_logEntryOfBridge.addEvent(bridgeAfterEventDesc);
	}	

	long trackCount = m_logEntryOfBridge.getCurrentCount();
	BridgeConnectorEventStruct *log = NULL;
	m_logEntryOfBridge.getLog (log);
	// write first member of group
	m_logEntryOfBridge.setEventList (log, trackCount);
	delete [] log;
	log = NULL;
	pTerm->m_pBridgeConnectorLog->updateItem (m_logEntryOfBridge, m_logEntryOfBridge.GetIndex());
	m_logEntryOfBridge.clearLog();

	mbConnected = false;
}