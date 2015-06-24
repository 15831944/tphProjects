#include "StdAfx.h"
#include ".\standassignmentregisterinsim.h"

#include "StandInSim.h"
#include "../../Common/ARCFlight.h"

CStandAssignmentRegisterInSim::CStandAssignmentRegisterInSim(StandInSim* pStand)
:m_pStand(pStand)
{
	m_vStandAssignRecords.clear();
}

CStandAssignmentRegisterInSim::~CStandAssignmentRegisterInSim(void)
{
}

bool CStandAssignmentRegisterInSim::IsStandIdle(ElapsedTime tStart, ElapsedTime tEnd)
{
	if (m_vStandAssignRecords.empty())
		return true;

	if (m_vStandAssignRecords[0].m_tStart > tEnd)		//before first record
		return true;

	int nCount = int(m_vStandAssignRecords.size());			//between records
	for (int i = 1; i < nCount;i++)
	{
		if ( m_vStandAssignRecords[i-1].m_tEnd < tStart && m_vStandAssignRecords[i].m_tStart > tEnd)
			return true;
	}

	if (tStart > m_vStandAssignRecords[nCount-1].m_tEnd)		// behind last record
		return true;

	return false;
}


void CStandAssignmentRegisterInSim::AddRecord(StandAssignRecord _Record)
{
	m_vStandAssignRecords.push_back( _Record);

	std::sort(m_vStandAssignRecords.begin(), m_vStandAssignRecords.end());
}

void CStandAssignmentRegisterInSim::DeleteRecord(int Idx)
{
	if(Idx < 0)
		return;
	m_vStandAssignRecords.erase( m_vStandAssignRecords.begin() + Idx);
}

int CStandAssignmentRegisterInSim::FindRecord(AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type)
{
	for (int i =0; i< int(m_vStandAssignRecords.size());i++)
	{
		if (m_vStandAssignRecords[i].m_pFlight == pFlight && m_vStandAssignRecords[i].m_OpType == _type)
		{
			return i;
		}
	}
	return -1;
}

//void CStandAssignmentRegisterInSim::GetFlightOccupancyTime(AirsideFlightInSim* pFlight, ElapsedTime& tStart, ElapsedTime& tEnd)
//{
//	ASSERT(pFlight);		//the flight shouldn't be null
//	
//	int nIdx = FindRecord(pFlight);
//	if (nIdx < 0)
//		return;
//
//	tStart = m_vStandAssignRecords[nIdx].m_tStart;
//	tEnd = m_vStandAssignRecords[nIdx].m_tEnd;
//}

void CStandAssignmentRegisterInSim::StandAssignmentRecordInit(AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type, ElapsedTime tStart, ElapsedTime tEnd)
{
	StandAssignRecord _record;

	_record.m_pFlight = pFlight;
	_record.m_tStart = tStart;
	_record.m_tEnd = tEnd;
	_record.m_OpType = _type;

	m_vStandAssignRecords.push_back(_record);
	std::sort(m_vStandAssignRecords.begin(), m_vStandAssignRecords.end());
}

void CStandAssignmentRegisterInSim::GetFlightInTimeRange(ElapsedTime tStart, ElapsedTime tEnd, std::vector<ARCFlight*>& vFlights)
{

	int nCount = int(m_vStandAssignRecords.size());	
	for (int i = 0; i < nCount;i++)
	{
		StandAssignRecord record = m_vStandAssignRecords[i];
		if ((record.m_tStart >=tStart && record.m_tStart <= tEnd)|| (record.m_tEnd >=tStart && record.m_tEnd <= tEnd)
			|| (tStart >= record.m_tStart && tStart <= record.m_tEnd) || (tEnd >= record.m_tStart && tEnd <= record.m_tEnd))
			vFlights.push_back(record.m_pFlight->GetFlightInput());
	}
}