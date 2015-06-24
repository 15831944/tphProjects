#include "StdAfx.h"
#include ".\assignedgate.h"
#include "../Common/ALTObjectID.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "flight.h"
#include "FlightForGateAssign.h"

/*
|<---(arrive time)AssignedDepartureFlight(departure time)--->| 
|<---buffer time--->| |<---(arrive time)unAssignedArriveFlight(departure time)--->|

*/
static bool standOccupancyConflict(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, CFlightOperationForGateAssign &assignedFlight, ParkingStandBufferList* pStandBufferList)
{
	ElapsedTime leftTime, rightTime;

	leftTime = assignedFlight.GetStartTime();
	rightTime = assignedFlight.GetEndTime();

	// note: Flights has been sorted by arriving time,  so, while check unAssignedFlight is arriving, 
	// just need check conflicts(append with stand buffer constraints) with assigned departing flights.

	ElapsedTime bufTime = 0L;

	if(pflight != assignedFlight.getFlight())		//same flight without stand buffer time
		bufTime = pStandBufferList->GetBufferTime((assignedFlight.getFlight())->getLogEntry() , pflight->getLogEntry());

	if((  (tStart >= (rightTime + bufTime) ) || ( tEnd <= (leftTime - bufTime)) ))
		return false;

	return true;
}

static bool standBufferConflict(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, CFlightOperationForGateAssign &assignedFlight, ParkingStandBufferList* pStandBufferList,CString& strError)
{
	if(pflight == assignedFlight.getFlight())
		return false;
	
	ElapsedTime leftTime, rightTime;

	leftTime = assignedFlight.GetStartTime();
	rightTime = assignedFlight.GetEndTime();

	ElapsedTime bufTime = 0L;

	if(pflight != assignedFlight.getFlight())		//same flight without stand buffer time
		bufTime = pStandBufferList->GetBufferTime((assignedFlight.getFlight())->getLogEntry() , pflight->getLogEntry());

	if((  (tStart >= (rightTime + bufTime) ) || ( tEnd <= (leftTime - bufTime)) ))
		return false;

	char buffer1[256];
	pflight->getFlightIDString(buffer1);
	char buffer2[256];
	memset(buffer2,0,256);
	assignedFlight.getFlight()->getFlightIDString(buffer2);
	
	strError.Format(_T("Flight: %s and Flight: %s have stand buffer conflict"),buffer1,buffer2);
	return true;
}

static bool standOverlapConflict(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, CFlightOperationForGateAssign &assignedFlight,CString& strError)
{
	if(pflight == assignedFlight.getFlight())
		return false;

	ElapsedTime leftTime, rightTime;

	leftTime = assignedFlight.GetStartTime();
	rightTime = assignedFlight.GetEndTime();

	if((  (tStart >= (rightTime) ) || ( tEnd <= (leftTime)) ))
		return false;


	char buffer1[256];
	pflight->getFlightIDString(buffer1);
	char buffer2[256];
	memset(buffer2,0,256);
	assignedFlight.getFlight()->getFlightIDString(buffer2);
	strError.Format(_T("Flight: %s and Flight: %s are overlap"),buffer1,buffer2);
	return true;
}

static bool GateOccupancyConflict(const ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, CFlightOperationForGateAssign &assignedFlight)
{
	ElapsedTime leftTime, rightTime;
	leftTime = assignedFlight.GetStartTime();
	rightTime = assignedFlight.GetEndTime();


	// note: Flights has been sorted by arriving time,  so, while check unAssignedFlight is arriving, 
	// just need check conflicts(append with stand buffer constraints) with assigned departing flights.


	if((  (tStart >= rightTime) || ( tEnd <= leftTime) ))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CAssignGate::CAssignGate()
{
}

CAssignGate::~CAssignGate()
{
}
CString CAssignGate::GetName(void)
{
	return _T("");
}

void CAssignGate::SetFlightSelected( int _nFlightIndex, bool _bSel )
{
	m_vectAssignedFlight[_nFlightIndex].SetSelected( _bSel );	
}

int CAssignGate::GetFlightCount()
{
	return m_vectAssignedFlight.size();
}

CFlightOperationForGateAssign& CAssignGate::GetFlight(int _nIndex)
{
	assert( _nIndex >= 0 && _nIndex < static_cast<int>(m_vectAssignedFlight.size()) );
	return m_vectAssignedFlight[_nIndex];
}

const CFlightOperationForGateAssign& CAssignGate::GetFlight( int _nIndex ) const
{
	assert( _nIndex>=0 && _nIndex < static_cast<int>(m_vectAssignedFlight.size()) ); 
	return m_vectAssignedFlight[_nIndex];
}

void CAssignGate::RemoveFlight( int _nIndex )
{
	assert( _nIndex>=0 && _nIndex < static_cast<int>(m_vectAssignedFlight.size()) );
	m_vectAssignedFlight.erase( m_vectAssignedFlight.begin() + _nIndex );
}

void CAssignGate::AddFlight(CFlightOperationForGateAssign& _flight)
{
	m_vectAssignedFlight.push_back( _flight );
}

int CAssignGate::GetFlightIdx(CFlightOperationForGateAssign& _flight)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=0; i< nCount; i++ )
	{
		if( m_vectAssignedFlight[i] == _flight)
			return i;
	}
	return -1;
}

void CAssignGate::SaveFlightAssignment()
{
	return;
}
void CAssignGate::RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		if( m_vectAssignedFlight[i].IsSelected() )
		{
			_vectFlight.push_back( m_vectAssignedFlight[i] );
			m_vectAssignedFlight.erase(m_vectAssignedFlight.begin() + i);
		}
	}
}

void CAssignGate::RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight)
{
	return;
}

bool CAssignGate::BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList)
{
	ASSERT(0);
	return false;
}

void CAssignGate::SetParent(CGateAssignmentMgr * _pParent)
{
	m_pParent = _pParent;
}

void CAssignGate::GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict)
{
	_csToolTips = _T("");
	return;
}

void CAssignGate::GetFlightsInTimeRange(ElapsedTime tStart, ElapsedTime tEnd, std::vector<ARCFlight*>& vFlights)
{
	int nCount = m_vectAssignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{
		CFlightOperationForGateAssign flight = m_vectAssignedFlight.at(i);

		//if ((flight.GetStartTime() >= tStart && flight.GetStartTime() <= tEnd)|| (flight.GetEndTime() >= tStart && flight.GetEndTime() <= tEnd))
		if (flight.GetEndTime() < tStart || flight.GetStartTime() > tEnd)
			continue;

		vFlights.push_back(flight.getFlight());
	}

}

bool CAssignGate::OverlapStandErrorMessage( ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError)
{
	ASSERT(0);
	return false;
}

bool CAssignGate::StandBufferErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList,CString& strError)
{
	ASSERT(0);
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CAssignStand::CAssignStand()
{
}

CAssignStand::~CAssignStand(void)
{
}

CString CAssignStand::GetName(void)
{
	char str[128];
	m_StandID.printID( str );
	return (CString( str ));
}

void CAssignStand::SaveFlightAssignment()
{
	int nCount = m_vectAssignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{
		CFlightOperationForGateAssign _flight = m_vectAssignedFlight.at(i);
		if (_flight.getOpType() == ARR_OP)
		{
			_flight.getFlight()->setArrStand(m_StandID);
			_flight.getFlight()->setArrParkingTime(_flight.GetEndTime() - _flight.GetStartTime());
		}
		if (_flight.getOpType() == DEP_OP)
		{
			_flight.getFlight()->setDepStand(m_StandID);
			_flight.getFlight()->setDepParkingTime(_flight.GetEndTime() - _flight.GetStartTime());

		}
		if (_flight.getOpType() == ARR_DEP_OP)
		{
			_flight.getFlight()->setArrStand(m_StandID);
			_flight.getFlight()->setDepStand(m_StandID);
			_flight.getFlight()->setArrParkingTime(_flight.GetEndTime() - _flight.GetStartTime());

		}
		if (_flight.getOpType() == INTPAKRING_OP)
		{
			_flight.getFlight()->setIntermediateStand(m_StandID);
			_flight.getFlight()->setIntParkingTime(_flight.GetEndTime() - _flight.GetStartTime());

		}
	}
}


void CAssignStand::RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		if( m_vectAssignedFlight[i].IsSelected() )
		{

			////no stand ,no arr dep gate
			//ProcessorID procid;
			//m_vectAssignedFlight[i].getFlight()->setArrGate( procid );
			//m_vectAssignedFlight[i].getFlight()->setDepGate( procid );

			_vectFlight.push_back( m_vectAssignedFlight[i] );
			m_vectAssignedFlight.erase(m_vectAssignedFlight.begin() + i);
		}
	}
}

void CAssignStand::RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight)
{

	int nCount = m_vectAssignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vectAssignedFlight[i] == pFlight)
		{
			m_vectAssignedFlight.erase(m_vectAssignedFlight.begin() + i);
			return;
		}
	}

}

bool CAssignStand::OverlapStandErrorMessage( ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError )
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=0; i<nCount; i++ )
	{
		if ( standOverlapConflict(pflight,tStart,tEnd, m_vectAssignedFlight[i],strError) )
			return false;
	}

	return true;
}

bool CAssignStand::StandBufferErrorMessage(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList,CString& strError)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=0; i<nCount; i++ )
	{
		if ( standBufferConflict(pflight,tStart,tEnd, m_vectAssignedFlight[i], pStandBufferList,strError) )
			return false;
	}

	return true;
}

bool CAssignStand::BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=0; i<nCount; i++ )
	{
		if ( standOccupancyConflict(pflight,tStart,tEnd, m_vectAssignedFlight[i], pStandBufferList) )
			return false;
	}

	return true;
}

void CAssignStand::GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict)
{
	int nFlightCount = m_vectAssignedFlight.size();
	if( _nIdx < 0 || _nIdx >= nFlightCount )
	{// give gate tooltips
		_csToolTips = GetName();
	}
	else
	{
		// give flight tooltips.
		char str[128];

		//Modify by adam 2007-04-24
		//m_vectAssignedFlight[_nIdx].printFlight( str, 0, _pStrDict, _psDate );
		if ((m_vectAssignedFlight[_nIdx].getFlight())->getFlightType() == TerminalFlight)
		{
			OpType eType = m_vectAssignedFlight[_nIdx].getOpType();
			if (eType == ARR_OP)
			{
				((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'A', 0, _pStrDict);
			}
			if (eType == DEP_OP)
			{
				((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'D', 0, _pStrDict);
			}
			if (eType == ARR_DEP_OP || eType == INTPAKRING_OP)
			{
				((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'T', 0, _pStrDict);
			}
			//End modify by adam 2007-04-24
			_csToolTips = str;
		}
	}
	return;
}

bool CAssignStand::operator > (const CAssignStand& _gate) const
{
	CString strProcA = m_StandID.GetIDString();
	CString strProcB = _gate.m_StandID.GetIDString();

	return strProcA > strProcB;
}

bool CAssignStand::operator<( const CAssignStand& _gate ) const
{
	CString strProcA = m_StandID.GetIDString();
	CString strProcB = _gate.m_StandID.GetIDString();

	return strProcA < strProcB; 
}


bool CAssignStand::isLess(CAssignGate* pObj)
{
	CAssignStand *pObjStand = (CAssignStand *)pObj;
	if(pObjStand == NULL)
		return false;
	return *this < *pObjStand;

}

int CAssignStand::GetStandIdx()
{
	return m_nStandIdx;
}

void CAssignStand::SetStandIdx( int idx )
{
	m_nStandIdx = idx;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

CAssignTerminalGate::CAssignTerminalGate()
{
}

CAssignTerminalGate::~CAssignTerminalGate(void)
{
}

CString CAssignTerminalGate::GetName(void)
{
	return (m_GateID.GetIDString());
}

void CAssignTerminalGate::SaveFlightAssignment()
{
	int nCount = m_vectAssignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{
		CFlightOperationForGateAssign _flight = m_vectAssignedFlight.at(i);
		if (_flight.getOpType() == ARR_OP)
		{
			((Flight*)_flight.getFlight())->setArrGate(m_GateID);
		}
		else if (_flight.getOpType() == DEP_OP)
		{
			((Flight*)_flight.getFlight())->setDepGate(m_GateID);
		}
	}
}

void  CAssignTerminalGate::RemoveSelectedFlight(std::vector<CFlightOperationForGateAssign>& _vectFlight)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		if( m_vectAssignedFlight[i].IsSelected() )
		{
			/*m_vectAssignedFlight[i].EmptyGate();*/
			_vectFlight.push_back( m_vectAssignedFlight[i] );
			m_vectAssignedFlight.erase(m_vectAssignedFlight.begin() + i);
		}
	}
}

void CAssignTerminalGate::RemoveAssignedFlight(CFlightOperationForGateAssign& pFlight)
{
	std::vector<CFlightOperationForGateAssign>::iterator iter = std::find(m_vectAssignedFlight.begin(),m_vectAssignedFlight.end(),pFlight);
	if (iter != m_vectAssignedFlight.end())
	{
		m_vectAssignedFlight.erase(iter);
	}

}

bool CAssignTerminalGate::BeAbletoTake(ARCFlight* pflight,const ElapsedTime& tStart, const ElapsedTime& tEnd, ParkingStandBufferList* pStandBufferList)
{
	int nCount = m_vectAssignedFlight.size();
	for( int i=0; i<nCount; i++ )
	{
		if ( GateOccupancyConflict(pflight,tStart,tEnd, m_vectAssignedFlight[i]) )
			return false;
	}

	return true;
}

void CAssignTerminalGate::GetToolTips(int _nIdx, CString& _csToolTips, StringDictionary* _pStrDict)
{
	int nFlightCount = m_vectAssignedFlight.size();
	if( _nIdx < 0 || _nIdx >= nFlightCount )
	{// give gate tooltips
		_csToolTips = GetName();
	}
	else
	{
		// give flight tooltips.
		char str[128];

		//Modify by adam 2007-04-24
		//m_vectAssignedFlight[_nIdx].printFlight( str, 0, _pStrDict, _psDate );
		OpType eType = m_vectAssignedFlight[_nIdx].getOpType();
		if (eType == ARR_OP)
		{
			((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'A', 0, _pStrDict);
		}
		if (eType == DEP_OP)
		{
			((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'D', 0, _pStrDict);
		}
		if (eType == ARR_DEP_OP)
		{
			((Flight*)m_vectAssignedFlight[_nIdx].getFlight())->printFlight( str,'T', 0, _pStrDict);
		}
		//End modify by adam 2007-04-24
		_csToolTips = str;
	}
	return;
}


bool CAssignTerminalGate::isLess(CAssignGate* pObj)
{
	CAssignTerminalGate *pObjGate = (CAssignTerminalGate *)pObj;
	if(pObjGate == NULL)
		return false;
	return GetName() < pObjGate->GetName();
}
