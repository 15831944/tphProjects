#include "StdAfx.h"

#include "StandLeadOutLineInSim.h"
#include "StandInSim.h"
#include "../../InputAirside/DeicePad.h"

#include "../../InputAirside/StandLeadOutLine.h"
#include "SimulationErrorShow.h"

StandLeadOutLineInSim::StandLeadOutLineInSim( StandInSim* pStand, int inLineidx )
{
	m_pStand = pStand;
	m_pLeadOutLine = &pStand->GetStandInput()->GetLeadOutLineList().ItemAt(inLineidx);
	m_linePath.init(m_pLeadOutLine->getPath().getCount()+1);
	m_linePath[0] = pStand->GetStandInput()->GetServicePoint();
	for(int i=0;i<m_pLeadOutLine->getPath().getCount();++i)
		m_linePath[i+1] = m_pLeadOutLine->getPath()[i];
	m_pDeiceOutCons = NULL;
}

StandLeadOutLineInSim::StandLeadOutLineInSim(StandInSim* pStand,StandLeadInLine* pLeadInLine)
{
	m_pStand = pStand;
	m_pDeiceOutCons = new StandLeadOutLine();
	m_pDeiceOutCons->SetID(pLeadInLine->GetID());
	m_pDeiceOutCons->SetName(pLeadInLine->GetName());
	m_pDeiceOutCons->SetPushBack(TRUE);

	m_pLeadOutLine = m_pDeiceOutCons;

	CPath2008 path(pLeadInLine->getPath());
	path.invertList();
	m_pLeadOutLine->SetPath(path);
	m_linePath.init(m_pLeadOutLine->getPath().getCount()+1);
	m_linePath[0] = pStand->GetStandInput()->GetServicePoint();
	for(int i=0;i<m_pLeadOutLine->getPath().getCount();++i)
		m_linePath[i+1] = m_pLeadOutLine->getPath()[i];
}

StandLeadOutLineInSim::StandLeadOutLineInSim( DeiceStandInSim* pDecie )
{
	m_pStand = pDecie;
	DeicePad* pDeiceInput = (DeicePad*)pDecie->GetStandInput();
	m_pDeiceOutCons = new StandLeadOutLine();
	m_pDeiceOutCons->SetID(pDecie->GetID());
	m_pDeiceOutCons->SetName("Lead Out Line");
	m_pDeiceOutCons->SetPath(pDeiceInput->GetOutConstrain());
	m_pLeadOutLine = m_pDeiceOutCons;

	m_linePath.init(m_pLeadOutLine->getPath().getCount()+1);
	m_linePath[0] = pDeiceInput->GetServicePoint();
	for(int i=0;i<m_pLeadOutLine->getPath().getCount();++i)
		m_linePath[i+1] = m_pLeadOutLine->getPath()[i];
}

StandLeadOutLineInSim::~StandLeadOutLineInSim()
{
	if (m_pDeiceOutCons)
	{
		delete m_pDeiceOutCons;
		m_pDeiceOutCons = NULL;
	}
	m_pLeadOutLine = NULL;
}

CString StandLeadOutLineInSim::PrintResource()const 
{
	CString str;
	str.Format("%s-%s", m_pStand->PrintResource(), m_pLeadOutLine->GetName() );
	return str;

}

CPoint2008 StandLeadOutLineInSim::GetDistancePoint( double dist ) const
{
	return GetPath().GetDistPoint(dist);
}

void StandLeadOutLineInSim::InitRelate( IntersectionNodeInSimList& nodelist )
{
	m_vOutNodes.clear();
	for(int i=0;i< nodelist.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNode = nodelist.GetNodeByIndex(i);
		if (m_pStand->GetStandInput()->GetType() == ALT_STAND)
		{
			if( pNode->GetNodeInput().HasLeadOutLine( m_pLeadOutLine->GetID()))
			{
				m_vOutNodes.push_back(pNode);
			}
		}
		else
		{
			if( pNode->GetNodeInput().HasDeiceOutContrain(m_pLeadOutLine->GetID()) )
			{
				m_vOutNodes.push_back(pNode);
			}
		}
		
	}

	if (0 == m_vOutNodes.size())
	{
		AirsideSimErrorShown::SimpleSimWarning(
			m_pLeadOutLine->GetName() + _T("(") + m_pStand->GetStandInput()->GetObjectName().GetIDString() + _T(")"),
			_T("The stand lead-out line has no exit node. Please adapt the layout."),
			_T("Stand Lead-out Line Floating")
			);
	}

}

DistanceUnit StandLeadOutLineInSim::GetEndDist()
{
	return GetPath().GetTotalLength();
}

void StandLeadOutLineInSim::SetExitTime( CAirsideMobileElement * pFlight, const ElapsedTime& exitT )
{
	m_pStand->SetExitTime(pFlight,exitT);
}


void StandLeadOutLineInSim::ReleaseLock( AirsideFlightInSim * pFlight, const ElapsedTime& tTime )
{
	if(m_pStand)
		m_pStand->ReleaseLock(pFlight,tTime);
}

bool StandLeadOutLineInSim::GetLock( AirsideFlightInSim * pFlight )
{
	if(m_pStand)
		return m_pStand->GetLock(pFlight);
	return false;
}

bool StandLeadOutLineInSim::TryLock( AirsideFlightInSim * pFlight )
{
	if(m_pStand)
		return m_pStand->TryLock(pFlight);
	return false;
}

AirsideFlightInSim * StandLeadOutLineInSim::GetLockedFlight()
{
	if(m_pStand)
		return m_pStand->GetLockedFlight();
	return false;
}

bool StandLeadOutLineInSim::IsPushBack() const
{
	if(m_pLeadOutLine)
		return m_pLeadOutLine->IsPushBack() == TRUE;
	return false;
}

bool StandLeadOutLineInSim::IsReleasePoint()const
{
	if(m_pLeadOutLine)
		return m_pLeadOutLine->IsSetReleasePoint() == TRUE;
	return false;
}

DistanceUnit StandLeadOutLineInSim::GetReleaseDistance()const
{
	if (m_pLeadOutLine)
	{
		return m_pLeadOutLine->GetDistance();
	}
	return 0.0;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void StandLeadOutLineInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetStandInSim()->GetStandInput()->getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

int StandLeadOutLineInSim::GetID() const
{
	return m_pLeadOutLine->GetID();
}

RouteResource* StandLeadOutLineInSim::getRouteResource()const
{
	return GetStandInSim();
}

IntersectionNodeInSim* StandLeadOutLineInSim::GetEndNode()
{
	if (m_vOutNodes.empty())
		return NULL;

	std::map<double,IntersectionNodeInSim*> mapNode;
	for (int i = 0; i < (int)m_vOutNodes.size(); i++)
	{
		IntersectionNodeInSim* pNode = m_vOutNodes.at(i);
		double dDist = m_linePath.GetPointDist(pNode->GetDistancePoint(0));
		mapNode.insert(std::make_pair(dDist,pNode));
	}

	return (*mapNode.rbegin()).second;
}

StandInSim* StandLeadOutLineInSim::IsStandResource() const
{
	return m_pStand;
}

