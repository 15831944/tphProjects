#include "StdAfx.h"
#include ".\bridgeconnector.h"
#include "Inputs\miscproc.h"
#include "terminal.h"
#include "../Engine/Airside/AirsideFlightInSim.h"
#include "../Results/BridgeConnectorLog.h"
#include "Common/ARCTracker.h"
#include "../Engine/OnboardFlightInSim.h"
#include "OnboardDoorInSim.h"
#include "OnboardSimulation.h"
#include "Common/FloorChangeMap.h"

BridgeConnector::BridgeConnector(void)
{
	m_vConnectPoints.clear();

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

	return TRUE;
}

int BridgeConnector::writeSpecialField( ArctermFile& procFile ) const
{
	procFile.writeInt(m_nConnectPointCount);

	for (int i =0; i < m_nConnectPointCount; i++)
	{
		BridgeConnector::ConnectPoint conPoint = m_vConnectPoints.at(i);

		procFile.writePoint(conPoint.m_Location);
		procFile.writeDouble(conPoint.m_dWidth);
		procFile.writeDouble(conPoint.m_dLength);
		procFile.writePoint(conPoint.m_dirFrom);
		procFile.writePoint(conPoint.m_dirTo);
	}

	return TRUE;
}

const BridgeConnector::ConnectPoint& BridgeConnector::GetConnectPointByIdx( int idx )
{
	return m_vConnectPoints.at(idx);
}

void BridgeConnector::AddConnectPoint( ConnectPoint& conPoint )
{
	m_vConnectPoints.push_back(conPoint);
}

void BridgeConnector::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	Processor::DoOffset(_xOffset,_yOffset);
	for(int i=0;i<(int)m_vConnectPoints.size();i++)
	{
		ConnectPoint& cp = m_vConnectPoints[i];
		Point offset(_xOffset,_yOffset,0);
		cp.m_dirFrom+=offset;
		cp.m_dirTo += offset;
		cp.m_Location+=offset;
	}
}

void BridgeConnector::Rotate( DistanceUnit _degree )
{
	Processor::Rotate(_degree);
	for (int i =0; i < m_nConnectPointCount; i++)
	{
		ConnectPoint* pPoint = &m_vConnectPoints.at(i);
		pPoint->m_Location.rotate(_degree);
		pPoint->m_dirFrom.rotate(_degree);

	}
}

void BridgeConnector::Mirror( const Path* _p )
{
	Processor::Mirror(_p);
}

BridgeConnector::ConnectPointStatus& BridgeConnector::GetConnectPointStatus( int idx )
{
	return m_vStatus[idx];
}

bool BridgeConnector::ConnectFlight(const CPoint2008& ptDoorPos, int idxDoor,const ElapsedTime& t )
{
	int nConnnectCount = GetConnectPointCount();

	//for (int nConnectPoint = idxDoor; nConnectPoint < nConnnectCount; ++nConnectPoint )
	if(idxDoor<nConnnectCount)
	{
		BridgeConnector::ConnectPointStatus& connectStatus = GetConnectPointStatus(idxDoor);

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
				return ConnectOnboardFlight(pOnboardFlightInSim, t);
			}
			
		}
	}

	ConnectAirsideFlight(pFlight,t);
}

void BridgeConnector::ConnectAirsideFlight( AirsideFlightInSim* pFlight,const ElapsedTime& t )
{
	if(pFlight == NULL)
		return;

	pFlight->OpenDoors(t);
	int nCountOpenDoors = pFlight->m_vOpenDoors.size();

	for (int idxDoor =0;idxDoor<nCountOpenDoors;idxDoor++)
	{		
		CPoint2008 ptDoorPos = pFlight->m_vOpenDoors[idxDoor].mDoorPos;

		if(ConnectFlight(ptDoorPos,idxDoor,t))
			m_pConnectFlight = pFlight;
	}
}
void BridgeConnector::ConnectOnboardFlight( OnboardFlightInSim* pFlight,const ElapsedTime& t )
{
	if(pFlight == NULL)
		return;

	std::vector<OnboardDoorInSim *>* pDoorSet = pFlight->GetOnboardDoorList();

	int idxDoor = 0;
	for (size_t i = 0; i < pDoorSet->size(); i++)
	{
		OnboardDoorInSim* pDoorInSim = pDoorSet->at(i);
		if (pDoorInSim->ConnectValid())
		{
			int nConnnectCount = GetConnectPointCount();

			CPoint2008 ptDoorPos;
			ptDoorPos = pDoorInSim->GetConnectionPos();
			
			if (ConnectFlight(ptDoorPos,idxDoor,t))
			{
				m_pOnboardConnectFlight = pFlight;
				idxDoor++;
			}
		}
	}
}

void BridgeConnector::DisAirsideConnect( const ElapsedTime& t )
{
	int nConnnectCount = GetConnectPointCount();

	for (int nConnectPoint = 0; nConnectPoint < nConnnectCount; ++nConnectPoint )
	{
		DisConnect(nConnectPoint,t);
	}
	m_pConnectFlight = NULL;
}

void BridgeConnector::DisOnboardConnect( const ElapsedTime& t )
{
	if (m_pOnboardConnectFlight == NULL)
		return;
	
	if (occupants.IsEmpty())
	{
		m_pOnboardConnectFlight->ProcessCloseDoor(t);
		int nConnnectCount = GetConnectPointCount();

		for (int nConnectPoint = 0; nConnectPoint < nConnnectCount; ++nConnectPoint )
		{
			DisConnect(nConnectPoint,t);
		}
		m_pOnboardConnectFlight = NULL;
	}
}

void BridgeConnector::DisConnect(int idxDoor, const ElapsedTime& t)
{
	BridgeConnector::ConnectPointStatus& connectStatus = GetConnectPointStatus(idxDoor);

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


		const ConnectPoint& cp = m_vConnectPoints[idxDoor];
// 		Point endPoint = (cp.m_dirTo - cp.m_dirFrom);
// 		endPoint.length(cp.m_dLength);
// 		endPoint+=cp.m_dirFrom;
// 		onnectStatus.m_EndPoint = cp.m_dirFrom;
		Point pVector;
		pVector.initVector3D( cp.m_Location, cp.m_dirFrom );
		pVector.length3D(cp.m_dLength);
		pVector.plus(cp.m_Location);
		connectStatus.m_EndPoint = pVector;


		bridgeAfterEventDesc.time = (long)(t+ElapsedTime(60L));
		bridgeAfterEventDesc.state = 'I'; // stop, bridge return to original state

		bridgeAfterEventDesc.x = connectStatus.m_EndPoint.getX();
		bridgeAfterEventDesc.y = connectStatus.m_EndPoint.getY();
		bridgeAfterEventDesc.z = connectStatus.m_EndPoint.getZ();
		connectStatus.m_logEntryOfBridge.addEvent(bridgeAfterEventDesc);		
		
	}
	
}

void BridgeConnector::initMiscData( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates )
{
	m_bDisallowGroup = _bDisallowGroup;
	visitorFlag = visitors;
}

void BridgeConnector::initSpecificMisc (const MiscData *p_miscData)
{

	m_vStatus.clear();
	m_vStatus.resize(m_vConnectPoints.size());
	for(int i=0;i<(int)m_vConnectPoints.size();i++)
	{
		const ConnectPoint& cp = m_vConnectPoints[i];
		/*Point endPoint = (cp.m_dirFrom - cp.m_Location);
		endPoint.length(cp.m_dLength);
		endPoint+=cp.m_Location;*/
		Point pVector;
		pVector.initVector3D( cp.m_Location, cp.m_dirFrom );
		pVector.length3D(cp.m_dLength);
		pVector.plus(cp.m_Location);
		m_vStatus[i].m_EndPoint = pVector;
	}

	if( p_miscData != NULL )
	{
		MiscBridgeConnectorData *data = (MiscBridgeConnectorData *)p_miscData;

		MiscProcessorIDList* misIdList = data->GetBridgeConnectorLinkedProcList();
		appendLinkedStandProcs(misIdList);
	}
	m_pConnectFlight = NULL;

}

void BridgeConnector::SetAllStandList(const ALTObjectIDList& allStandIDList)
{
	m_AllStandIDList = allStandIDList;
}

void BridgeConnector::appendLinkedStandProcs(const MiscProcessorIDList* _pLinkedStandIdList)
{
	m_standIDList.clear();
	if (_pLinkedStandIdList==NULL) return;

	const MiscProcessorIDList* pMiscLinkedBridgeIDList = _pLinkedStandIdList;
	assert( m_pTerm );		

	for (int i = 0; i < pMiscLinkedBridgeIDList->getCount(); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pIDWithFlag = (const MiscProcessorIDWithOne2OneFlag*) pMiscLinkedBridgeIDList->getID( i ) ;
		if( pIDWithFlag )
		{
			ALTObjectIDList tempArray;
			ALTObjectID standID(pMiscLinkedBridgeIDList->getID( i )->GetIDString());
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
	if (m_pConnectFlight)
	{
		if (nFlightIndex == m_pConnectFlight->GetFlightInput()->getFlightIndex())
			return true;
	}

	if (m_pOnboardConnectFlight)
	{
		if(nFlightIndex == m_pOnboardConnectFlight->GetFlightInput()->getFlightIndex())
			return true;
	}

	return false;
}

bool BridgeConnector::GetStartEndPoint( int nBridgeIndex,Point& pStart, Point& pEnd )
{
	if (nBridgeIndex == -1)
		return false;
	
	if (nBridgeIndex >= GetConnectPointCount())
		return false;
	
	ConnectPointStatus& conPtStatus = GetConnectPointStatus(nBridgeIndex);
	ConnectPoint conPt = GetConnectPointByIdx(nBridgeIndex);
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
		
		for (int i = 0; i < GetConnectPointCount(); i++)
		{
			ConnectPointStatus& conPtStatus = GetConnectPointStatus(i);
			ConnectPoint conPt = GetConnectPointByIdx(i);
			CPoint2008 ptEnd;
			ptEnd.init(conPtStatus.m_EndPoint.getX(),conPtStatus.m_EndPoint.getY(),conPtStatus.m_EndPoint.getZ());
			if (conPtStatus.mbConnected && ptEnd == pDoorInSim->GetConnectionPos())
			{
				pStart = conPt.m_Location;
				pEnd = conPtStatus.m_EndPoint;
				return i;
			}
		}

	}
	else
	{
		std::vector<ConnectPointStatus> vStatus;
		std::vector<ConnectPoint>vConPt;
		for (int i = 0; i < GetConnectPointCount(); i++)
		{
			ConnectPointStatus& conPtStatus = GetConnectPointStatus(i);
			ConnectPoint conPt = GetConnectPointByIdx(i);
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

bool BridgeConnector::IsBridgeConnectToFlight()
{
	if(m_pConnectFlight == NULL && m_pOnboardConnectFlight == NULL)
		return false;

	return true;

}

void BridgeConnector::FlushLog()
{
	for(int i=0;i<(int)m_vStatus.size();i++)
	{
		m_vStatus[i].FlushLog(m_pTerm);
	}
}

void BridgeConnector::CopyDataToProc( BridgeConnector* pCopyToProc )
{
	pCopyToProc->m_nConnectPointCount = m_nConnectPointCount;
	pCopyToProc->m_vConnectPoints.assign(m_vConnectPoints.begin(),m_vConnectPoints.end());
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
	for (int i =0; i < m_nConnectPointCount; i++)
	{
		BridgeConnector::ConnectPoint conPoint = m_vConnectPoints.at(i);
		changMap.ChangePointFloor(conPoint.m_Location);
		//conPoint.m_Location.setZ( changMap.getNewFloor(conPoint.m_Location.getZ()) );
	}
}


void BridgeConnector::ConnectPointStatus::SetLogEntryOfBridge( const CBridgeConnectorLogEntry& _logEntry )
{
	m_logEntryOfBridge = _logEntry;
}

void BridgeConnector::ConnectPointStatus::FlushLog(Terminal* pTerm)
{
	if(!pTerm)return;

	long trackCount = m_logEntryOfBridge.getCurrentCount();
	BridgeConnectorEventStruct *log = NULL;
	m_logEntryOfBridge.getLog (log);
	// write first member of group
	m_logEntryOfBridge.setEventList (log, trackCount);
	delete [] log;
	log = NULL;
	pTerm->m_pBridgeConnectorLog->updateItem (m_logEntryOfBridge, m_logEntryOfBridge.GetIndex());
	m_logEntryOfBridge.clearLog();
}