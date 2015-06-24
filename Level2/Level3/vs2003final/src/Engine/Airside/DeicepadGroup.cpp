#include "StdAfx.h"

#include <Common/ALTObjectID.h>
#include "FlightRouteSeginSim.h"

#include "DeicepadGroup.h"

#include "AirsideFlightInSim.h"
#include "DeiceResourceManager.h"
#include "FlightGetClearanceEvent.h"

DeicepadGroupEntry::DeicepadGroupEntry( DeicepadGroup* pGroup, IntersectionNodeInSim* pNode )
	: m_pGroup(pGroup)
	, m_pNode(pNode)
	, m_pFlightWaiting(NULL)
{
	ASSERT(m_pNode);
}
DeicepadGroupEntry::~DeicepadGroupEntry()
{

}

AirsideFlightInSim* DeicepadGroupEntry::WakeFlight( DeicePadInSim* pDeicepad, const ElapsedTime& exitT )
{
// 	if (m_vFlightQ.size())
// 	{
// 		AirsideFlightList::iterator ite = m_vFlightQ.begin();
// 		AirsideFlightInSim* pFlight = *ite;
// 		m_vFlightQ.erase(ite);
// 
// 		MakeFlightGotoDeicepad(pFlight, pDeicepad);
// 		return pFlight;
// 	}
// 	return NULL;
	if (m_pFlightWaiting)
	{
// 		m_pGroup->RemoveApproachingFlight(m_pFlightWaiting);
		FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(m_pFlightWaiting);
		pNextEvent->setTime(exitT);
		pNextEvent->addEvent();
		return m_pFlightWaiting;
	}
	return NULL;
}

DeicepadGroup::DeicepadGroup(ALTObjectID groupID)
	: m_nLastEntry(0)
	, m_groupID(groupID)
{

}

DeicepadGroup::~DeicepadGroup()
{
	for (EntryList::iterator ite = m_vEntries.begin();ite!=m_vEntries.end();ite++)
	{
		delete *ite;
	}
	m_vEntries.clear();
}

AirsideFlightInSim* DeicepadGroup::WakeFlight( DeicePadInSim* pDeicepad, const ElapsedTime& exitT )
{
	size_t nCount = m_vEntries.size();
	for(size_t i=0;i<nCount;i++)
	{
		size_t nIndex = (m_nLastEntry + 1 + i)%nCount;
		AirsideFlightInSim* pFlight = m_vEntries[nIndex]->WakeFlight(pDeicepad, exitT);
		if (pFlight)
		{
			m_nLastEntry = nIndex;
			return pFlight;
		}
	}
	return NULL;
}

DeicePadInSim* DeicepadGroup::AssignDeicepad( AirsideFlightInSim* pFlight )
{
	for (DeicepadList::iterator ite = m_vDeicepads.begin();ite!=m_vDeicepads.end();ite++)
	{
		DeicePadInSim* pDeicepad = *ite;
		if (!pDeicepad->GetFlightOnto())
		{
			return pDeicepad;
		}
	}
	return NULL;
}

void DeicepadGroup::AddDeicepad( DeicePadInSim* pDeicepad )
{
	m_vDeicepads.push_back(pDeicepad);
	pDeicepad->SetGroup(this);
}

void DeicepadGroup::InitEntryPoint()
{
	// Remove invalid deicepad
	DeicepadList::iterator ite =  std::remove_if(m_vDeicepads.begin(), m_vDeicepads.end(),
		std::not1(std::mem_fun(&DeicePadInSim::IsEntryExitNodeValid)));
	m_vDeicepads.erase(ite, m_vDeicepads.end());

	size_t nSize = m_vDeicepads.size();
	if (nSize)
	{
		std::sort(m_vDeicepads.begin(), m_vDeicepads.end(), DeicePadIDSorter());
		m_vEntries.push_back(new DeicepadGroupEntry(this, m_vDeicepads[0]->m_pEntryNode));
		if (nSize>1)
		{
			m_vEntries.push_back(new DeicepadGroupEntry(this, m_vDeicepads.back()->m_pEntryNode));
		}
	}
}

void DeicepadGroup::AddApproachingFlight( AirsideFlightInSim* pFlight )
{
	if (m_vFlightApproaching.end() == std::find(m_vFlightApproaching.begin(), m_vFlightApproaching.end(), pFlight))
	{
#ifdef _DEBUG
		CStdioFile f;
		f.Open(_T("C:\\added_flight.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
		f.SeekToEnd();
		char cTmp[1024];
		pFlight->GetFlightInput()->getAirline(cTmp);
		pFlight->GetFlightInput()->getDepID(cTmp + strlen(cTmp));
		f.WriteString(cTmp);
		f.WriteString(_T("\n"));
#endif
		m_vFlightApproaching.push_back(pFlight);
	}
}

void DeicepadGroup::RemoveApproachingFlight( AirsideFlightInSim* pFlight )
{
#ifdef _DEBUG
	CStdioFile f;
	f.Open(_T("C:\\removed_flight.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	f.SeekToEnd();
	char cTmp[1024];
	pFlight->GetFlightInput()->getAirline(cTmp);
	pFlight->GetFlightInput()->getDepID(cTmp + strlen(cTmp));
	f.WriteString(cTmp);
	f.WriteString(_T("\n"));
#endif
	AirsideFlightList::iterator ite = std::remove(m_vFlightApproaching.begin(), m_vFlightApproaching.end(), pFlight);
	ASSERT(ite != m_vFlightApproaching.end() && ite + 1 == m_vFlightApproaching.end());
	m_vFlightApproaching.erase(ite, m_vFlightApproaching.end());
}

int DeicepadGroup::GetApproachingFlightNum() const
{
	return m_vFlightApproaching.size();
}

bool DeicepadGroup::DeicePadIDSorter::operator()( DeicePadInSim* lhs, DeicePadInSim* rhs )
{
	ALTObjectID lAltID = lhs->GetInput()->GetObjectName();
	ALTObjectID rAltID = rhs->GetInput()->GetObjectName();
	int nLen = lAltID.idLength();
	ASSERT(nLen && rAltID.idLength() == nLen);
	const std::string& strL = lAltID.at(nLen - 1);
	const std::string& strR = rAltID.at(nLen - 1);
	int nL = atoi(strL.c_str());
	int nR = atoi(strR.c_str());
	return (nL && nR) ? nL<nR : strL<strR;
}

bool DeicepadGroup::IDEqual::operator()( const DeicepadGroup* pGroup ) const
{
	return pGroup->m_groupID == m_altID;
}