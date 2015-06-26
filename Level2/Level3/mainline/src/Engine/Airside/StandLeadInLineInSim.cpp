#include "StdAfx.h"
#include "StandLeadInLineInSim.h"
#include "StandInSim.h"
#include "AirsideFlightInSim.h"
#include "../../InputAirside/StandLeadInLine.h"
#include "../../InputAirside/DeicePad.h"
#include "SimulationErrorShow.h"

CPoint2008 StandLeadInLineInSim::GetDistancePoint( double dist ) const
{
	return m_linePath.GetDistPoint(dist);
}


CString StandLeadInLineInSim::PrintResource() const
{
	CString str;
	str.Format("%s-%s", m_pStand->PrintResource(), m_pLeadInLine->GetName() );
	return str;
}

StandLeadInLineInSim::StandLeadInLineInSim( StandInSim* pStand, int inLineidx )
{
	m_pStand = pStand;
	m_pLeadInLine = &pStand->GetStandInput()->GetLeadInLineList().ItemAt(inLineidx);
	m_linePath = m_pLeadInLine->GetPath();
	if (m_pLeadInLine->GetPath().getCount() != 0)
	{
		m_linePath.insertPointAfterAt(pStand->GetStandInput()->GetServicePoint(), m_linePath.getCount()-1);
	}
	else
	{
		m_linePath.init(m_pLeadInLine->getPath().getCount()+1);
		m_linePath[0] = pStand->GetStandInput()->GetServicePoint();
	}
	m_pDeiceInCons = NULL;
}

StandLeadInLineInSim::StandLeadInLineInSim(DeiceStandInSim* pDecie)
{
	m_pStand = pDecie;
	DeicePad* pDeiceInput = (DeicePad*)pDecie->GetStandInput();
	m_pDeiceInCons = new StandLeadInLine();
	m_pDeiceInCons->SetID(pDecie->GetID());
	m_pDeiceInCons->SetName("Lead In Line");
	m_pDeiceInCons->SetPath(pDeiceInput->GetInContrain());
	m_pLeadInLine = m_pDeiceInCons;

	m_linePath = m_pLeadInLine->GetPath();
	if (m_pLeadInLine->GetPath().getCount() != 0)
	{
		m_linePath.insertPointAfterAt(pDeiceInput->GetServicePoint(), m_linePath.getCount()-1);
	}
	else
	{
		m_linePath.init(m_pLeadInLine->getPath().getCount()+1);
		m_linePath[0] = pDeiceInput->GetServicePoint();
	}
}

StandLeadInLineInSim::~StandLeadInLineInSim()
{
	if (m_pDeiceInCons)
	{
		delete m_pDeiceInCons;
		m_pDeiceInCons = NULL;
	}
	m_pLeadInLine = NULL;
}

void StandLeadInLineInSim::InitRelate( IntersectionNodeInSimList& nodelist )
{
	m_vInNodes.clear();
	for(int i=0;i< nodelist.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNode = nodelist.GetNodeByIndex(i);
		if (m_pStand->GetStandInput()->GetType() == ALT_STAND)
		{
			if( pNode->GetNodeInput().HasLeadInLine( m_pLeadInLine->GetID() ) )
			{
				m_vInNodes.push_back(pNode);
			}
		}
		else
		{
			if( pNode->GetNodeInput().HasDeiceInContrain(m_pLeadInLine->GetID()) )
			{
				m_vInNodes.push_back(pNode);
			}
		}
	}

	if (0 == m_vInNodes.size())
	{
		AirsideSimErrorShown::SimpleSimWarning(
			 m_pLeadInLine->GetName() + _T("(") + m_pStand->GetStandInput()->GetObjectName().GetIDString() + _T(")"),
			_T("The stand lead-in line has no entry node. Please adapt the layout."),
			_T("Stand Lead-in Line Floating")
			);
	}
}

DistanceUnit StandLeadInLineInSim::GetEndDist()
{
	return m_linePath.GetTotalLength();
}

void StandLeadInLineInSim::SetEnterTime( CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode, double dSpd )
{
	m_pStand->SetEnterTime(pFlight,enterT, fltMode, dSpd);
	for(int i=0;i< (int)m_vInNodes.size();i++)
	{
		IntersectionNodeInSim* pNode = m_vInNodes.at(i);
		pNode->SetExitTime(pFlight,enterT);
	}
}

void StandLeadInLineInSim::ReleaseLock( AirsideFlightInSim * pFlight, const ElapsedTime& tTime )
{
	if(m_pStand && !pFlight->IsDeparture())
		m_pStand->ReleaseLock(pFlight,tTime);
}

bool StandLeadInLineInSim::GetLock( AirsideFlightInSim * pFlight )
{
	if(m_pStand)
		return m_pStand->GetLock(pFlight);
	return false;
}

bool StandLeadInLineInSim::TryLock( AirsideFlightInSim * pFlight )
{
	if(m_pStand)
		return m_pStand->TryLock(pFlight);
	return false;
}

AirsideFlightInSim * StandLeadInLineInSim::GetLockedFlight()
{
	if(m_pStand)
		return m_pStand->GetLockedFlight();
	return false;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void StandLeadInLineInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetStandInSim()->GetStandInput()->getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

int StandLeadInLineInSim::GetID() const
{
	return m_pLeadInLine->GetID();
}

RouteResource* StandLeadInLineInSim::getRouteResource() const
{
	return GetStandInSim();
}

IntersectionNodeInSim* StandLeadInLineInSim::GetEndNode()
{
	if (m_vInNodes.empty())
		return NULL;

	std::map<double,IntersectionNodeInSim*> mapNode;
	for (int i = 0; i < (int)m_vInNodes.size(); i++)
	{
		IntersectionNodeInSim* pNode = m_vInNodes.at(i);
		double dDist = m_linePath.GetPointDist(pNode->GetDistancePoint(0));
		mapNode.insert(std::make_pair(dDist,pNode));
	}

	return (*mapNode.begin()).second;
}

StandInSim* StandLeadInLineInSim::IsStandResource() const
{
	return m_pStand;
}
