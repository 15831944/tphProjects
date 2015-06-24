#include "stdafx.h"
#include "EntryPointInSim.h"
#include "InputOnBoard/Seat.h"
#include "OnboardSeatRowInSim.h"
#include "OnboardSeatInSim.h"
#include "OnboardFlightInSim.h"
#include "GroundInSim.h"
#include "OnboardElementSpace.h"
#include "OnboardSeatGroupInSim.h"
#include "PaxOnboardBaseBehavior.h"
#include "person.h"
#include "PaxDeplaneBehavior.h"

OnboardSeatRowInSim::OnboardSeatRowInSim(CSeatRow* pSeatRow,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pSeatRow(pSeatRow)
,m_pElementSpace(NULL)
,m_pSeatGroupInSim(NULL)
{
	
}

OnboardSeatRowInSim::~OnboardSeatRowInSim()
{
	for (size_t i = 0; i < m_vEntryPoint.size(); i++)
	{
		delete m_vEntryPoint[i];
	}
	m_vEntryPoint.clear();

	for (size_t i = 0; i< m_vSeatInSim.size(); i++)
	{
		delete m_vSeatInSim[i];
	}
	m_vSeatInSim.clear();

	ClearSpace();
}

//find shortest distance from seat entry point
EntryPointInSim* OnboardSeatRowInSim::GetEntryPoint(const OnboardSeatInSim* pSeatInSim)const
{
	ASSERT(pSeatInSim);
	if (m_vEntryPoint.empty())
		return NULL;
	
	std::map<double,EntryPointInSim*> mapEntry;
	ARCVector3 location;
	pSeatInSim->GetFrontPosition(location);
	CPoint2008 ptLocation(location.n[VX],location.n[VY],location.n[VZ]);

	for (size_t i = 0; i < m_vEntryPoint.size(); i++)
	{
		EntryPointInSim* pEntryPointInSim = m_vEntryPoint[i];
		ARCVector3 entryPoint;
		OnboardCellInSim* pCellInSim = pEntryPointInSim->GetOnboardCell();
		if (pCellInSim->entryPointValid())
		{
			CPoint2008 cellPt = pCellInSim->getLocation();
			double dDist = ptLocation.distance(cellPt);

			mapEntry.insert(std::make_pair(dDist,pEntryPointInSim));
		}
	}

	if (mapEntry.empty())
		return NULL;
	
	return (*mapEntry.begin()).second;
}

CSeatRow* OnboardSeatRowInSim::GetSeatRowInput()const
{
	return m_pSeatRow;
}

int OnboardSeatRowInSim::GetCount()const
{
	return (int)m_vSeatInSim.size();
}

OnboardSeatInSim* OnboardSeatRowInSim::front()const
{
	if (m_vSeatInSim.empty())
		return NULL;
	
	return m_vSeatInSim.front();
}

OnboardSeatInSim* OnboardSeatRowInSim::back()const
{
	if (m_vSeatInSim.empty())
		return NULL;
	
	return m_vSeatInSim.back();
}

OnboardSeatInSim* OnboardSeatRowInSim::GetSeatInSim(int idx)const
{
	ASSERT(idx >= 0 && idx < (int)m_vSeatInSim.size());
	return m_vSeatInSim[idx];
}

bool OnboardSeatRowInSim::empty()const
{
	return m_vEntryPoint.size() > 0 ? false:true;
}
void OnboardSeatRowInSim::AddSeatInSim(OnboardSeatInSim* pSeatInSim)
{
	ASSERT(pSeatInSim);
	m_vSeatInSim.push_back(pSeatInSim);
}

void OnboardSeatRowInSim::AddEntryPoint(EntryPointInSim* pEntryInSim)
{
	if (pEntryInSim)
	{
		m_vEntryPoint.push_back(pEntryInSim);
	}
}

std::vector<OnboardSeatInSim *> OnboardSeatRowInSim::GetSeatNeedGivePlace( OnboardSeatInSim* pSeat, EntryPointInSim *pEntryPoint )
{
	std::vector<OnboardSeatInSim *> vStrideSeat;
	if(pSeat == NULL || pEntryPoint == NULL)
		return vStrideSeat;


	int nSeatCount =  m_vSeatInSim.size();
	if(nSeatCount == 1)// only 1 seat in the row
		return vStrideSeat;

	ASSERT(pEntryPoint != NULL);

	OnboardSeatInSim *pSeatEntryPoint = pEntryPoint->GetCreateSeat();
	if(pSeatEntryPoint == pSeat)//the seat where the seat entry created 
		return vStrideSeat;

	if(m_vSeatInSim[0] == pSeatEntryPoint)
	{
		for (int nSeat = 0; nSeat < nSeatCount; ++ nSeat)
		{
			if(m_vSeatInSim[nSeat] != pSeat)
				vStrideSeat.push_back(m_vSeatInSim[nSeat]);
			else
				break;
		}

	}
	else if(m_vSeatInSim[nSeatCount -1] == pSeatEntryPoint)
	{
		for (int nSeat = nSeatCount -1; nSeat >= 0 ; -- nSeat)
		{
			if(m_vSeatInSim[nSeat] != pSeat)
				vStrideSeat.push_back(m_vSeatInSim[nSeat]);
			else
				break;
		}

	}


	return vStrideSeat;
}

std::vector<OnboardSeatInSim *> OnboardSeatRowInSim::GetSequenceOfSeat( EntryPointInSim *pEntryPoint )
{
	std::vector<OnboardSeatInSim *> vSequenceSeat;
	
	if(pEntryPoint == NULL)
		return vSequenceSeat;
// 	if(pEntryPoint->GetWalkType() == EntryPointInSim::Straight_Walk)
// 		return vSequenceSeat;

	int nSeatCount =  m_vSeatInSim.size();
	OnboardSeatInSim *pSeatEntryPoint = pEntryPoint->GetCreateSeat();

	if(m_vSeatInSim[0] == pSeatEntryPoint)
	{
		for (int nSeat = 0; nSeat < nSeatCount; ++ nSeat)
		{
			vSequenceSeat.push_back(m_vSeatInSim[nSeat]);
		}

	}
	else if(m_vSeatInSim[nSeatCount -1] == pSeatEntryPoint)
	{
		for (int nSeat = nSeatCount -1; nSeat >= 0 ; -- nSeat)
		{
			vSequenceSeat.push_back(m_vSeatInSim[nSeat]);
		}
	}
	
	std::reverse(vSequenceSeat.begin(), vSequenceSeat.end());

	return vSequenceSeat;
}

void OnboardSeatRowInSim::CreatePollygon()
{
	CPollygon2008 pollygon;
	OnboardSeatInSim* pSeatInSim = m_vSeatInSim.front();
	CSeat* pFrontSeat = pSeatInSim->m_pSeat;
	CSeat* pBackSeat = m_vSeatInSim.back()->m_pSeat;
	CPath2008 path;
	if ((int)m_vSeatInSim.size() == 1)//only one seat in seat row
	{
		pFrontSeat->GetFrontSeatLeftRightOfRowPath(path);
		pollygon.init(path.getCount(),path.getPointList());
		m_pElementSpace->setPollygon(pollygon);
		return;
	}

	CPath2008 frontPath,backPath;
	pFrontSeat->GetFrontSeatLeftRightOfRowPath(frontPath);
	pBackSeat->GetBackSeatLeftRightOfRowPath(backPath);

	//calculate middle seat path
	CPath2008 topPath,bottomPath;
	for (int iSeat = 1; iSeat < (int)m_vSeatInSim.size() - 1; iSeat++)
	{
		CSeat* pSeat = m_vSeatInSim[iSeat]->m_pSeat;
		CPath2008 topPath1,bottomPath1;
		pSeat->GetTopBottomPath(topPath1,bottomPath1);

		topPath.ConnectPath(topPath1);
		bottomPath.ConnectPath(bottomPath1);
	}

	//connect front,back and middle seat to pollygon
	CPath2008 resultPath;
	resultPath.ConnectPath(frontPath);
	resultPath.ConnectPath(topPath);
	resultPath.ConnectPath(backPath);
	bottomPath.invertList();
	resultPath.ConnectPath(bottomPath);

	pollygon.init(resultPath.getCount(),resultPath.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}

void OnboardSeatRowInSim::ClearSpace()
{
	if (m_pElementSpace)
	{
		delete m_pElementSpace;
		m_pElementSpace = NULL;
	}
}

bool OnboardSeatRowInSim::GetLocation(CPoint2008& location)const
{
	m_pElementSpace->GetCenterPoint(location);
	return true;
}

void OnboardSeatRowInSim::CalculateSpace()
{
	if(m_vSeatInSim.empty())
		return;
	
	OnboardSeatInSim* pSeatInSim = m_vSeatInSim.front();
	CSeat* pSeat = pSeatInSim->m_pSeat;

	if (pSeat->getGroup())
		return;

	ClearSpace();
	m_pElementSpace = new OnboardElementSpace(GetGround());

	m_pElementSpace->CreatePollygon(this);

	m_pElementSpace->CalculateSpace();
}


GroundInSim* OnboardSeatRowInSim::GetGround()
{
	if (m_vSeatInSim.empty())
		return NULL;

	OnboardSeatInSim* pSeatInSim = m_vSeatInSim.front();
	CSeat* pSeat = pSeatInSim->m_pSeat;

	return m_pFlightInSim->GetGroundInSim(pSeat->GetDeck());
}

void OnboardSeatRowInSim::CreatFrontRowEntryPoint()
{
	if (m_vSeatInSim.empty())
		return;

	OnboardSeatInSim* pFrontSeatInSim = front();
	OnboardSeatInSim* pBackSeatInSim = back();

	ARCVector3 frontPt;
	pFrontSeatInSim->GetFrontPosition(frontPt);
	CPoint2008 seatFrontPt(frontPt.n[VX],frontPt.n[VY],0.0);

	ARCVector3 backPt;
	pBackSeatInSim->GetFrontPosition(backPt);
	CPoint2008 seatBackPt(backPt.n[VX],backPt.n[VY],0.0);

	CPoint2008 dir(seatBackPt,seatFrontPt);
	dir.Normalize();
	EntryPointInSim* pFrontEntryPointInSim = pFrontSeatInSim->GetfrontRowEntryPoint(dir);
	if (pFrontEntryPointInSim && pFrontEntryPointInSim->GetOnboardCell()->entryPointValid())
	{
		pFrontEntryPointInSim->GetOnboardCell()->entryPoint(true);
		m_vEntryPoint.push_back(pFrontEntryPointInSim);
	}
	else
	{
		delete pFrontEntryPointInSim;
		pFrontEntryPointInSim = NULL;
	}

	//seat row total count is single
	if (pFrontSeatInSim == pBackSeatInSim)
	{
		return;
	}

	dir.rotate(180);
	dir.Normalize();
	EntryPointInSim* pBackEntryPointInSim = pBackSeatInSim->GetfrontRowEntryPoint(dir);
	if (pBackEntryPointInSim && pBackEntryPointInSim->GetOnboardCell()->entryPointValid())
	{
		pBackEntryPointInSim->GetOnboardCell()->entryPoint(true);
		m_vEntryPoint.push_back(pBackEntryPointInSim);
	}
	else
	{
		delete pBackEntryPointInSim;
		pBackEntryPointInSim = NULL;
	}
}

void OnboardSeatRowInSim::CreateOtherRowEntryPoint()
{
	if (m_vSeatInSim.empty())
		return;
	
	OnboardSeatInSim* pFrontSeatInSim = front();
	OnboardSeatInSim* pBackSeatInSim = back();
	EntryPointInSim* pFrontEntryPointInSim = pFrontSeatInSim->GetIntersectionNodeWithSeatGroupPolygon();
	if (pFrontEntryPointInSim && pFrontEntryPointInSim->GetOnboardCell()->entryPointValid())
	{
		pFrontEntryPointInSim->GetOnboardCell()->entryPoint(true);
		m_vEntryPoint.push_back(pFrontEntryPointInSim);
	}
	else
	{
		delete pFrontEntryPointInSim;
		pFrontEntryPointInSim = NULL;
	}

	if (pFrontSeatInSim == pBackSeatInSim)
	{
		return;
	}
	EntryPointInSim* pBackEntryPointInSim = pBackSeatInSim->GetIntersectionNodeWithSeatGroupPolygon();
	if (pBackEntryPointInSim && pBackEntryPointInSim->GetOnboardCell()->entryPointValid())
	{
		if (!m_vEntryPoint.empty())//compare the distance from seat to two entry point
		{
			CPoint2008 point = pFrontEntryPointInSim->GetOnboardCell()->getLocation();
			
			ARCVector3 frontPt;
			ARCVector3 backPt;
			pFrontSeatInSim->GetFrontPosition(frontPt);
			pBackSeatInSim->GetFrontPosition(backPt);
			CPoint2008 seatFrontPt(frontPt.n[VX],frontPt.n[VY],0.0);
			CPoint2008 seatBackPt(backPt.n[VX],backPt.n[VY],0.0);
			double dFrontDist = seatFrontPt.distance(point);
			double dBackDist = seatBackPt.distance(point);

			if (dFrontDist > dBackDist)
			{
				delete pBackEntryPointInSim;
				pBackEntryPointInSim = NULL;

				pFrontEntryPointInSim->SetCreateSeat(pBackSeatInSim);
				return;
			}
		}
		pBackEntryPointInSim->GetOnboardCell()->entryPoint(true);
		m_vEntryPoint.push_back(pBackEntryPointInSim);
	}
	else
	{
		delete pBackEntryPointInSim;
		pBackEntryPointInSim = NULL;
	}
}

void OnboardSeatRowInSim::CreateEntryPoint()
{
	if (m_pSeatGroupInSim)
	{
		CreateEntryPointWithSeatGroup();
	}
	else
	{
		CreateEntryPointWithoutSeatGroup();
	}
}

void OnboardSeatRowInSim::CreateEntryPointWithoutSeatGroup()
{
	CreatFrontRowEntryPoint();
}

void OnboardSeatRowInSim::CreateEntryPointWithSeatGroup()
{
	if (m_pSeatGroupInSim->frontSeatRow(this))
	{
		CreatFrontRowEntryPoint();
	}
	else
	{
		CreateOtherRowEntryPoint();
	}
}

void OnboardSeatRowInSim::SetSeatGroupInSim(OnboardSeatGroupInSim* pSeatGroupInSim)
{
	m_pSeatGroupInSim = pSeatGroupInSim;
}

OnboardSeatGroupInSim* OnboardSeatRowInSim::GetSeatGroupInSim()const
{
	return m_pSeatGroupInSim;
}

//check from seat to entry point seat whether has pax occupy
bool OnboardSeatRowInSim::CanLeadToEntryPoint(OnboardSeatInSim* pSeat,EntryPointInSim *pEntryPoint)
{
	std::vector<OnboardSeatInSim *> vSeatInSim = GetSeatNeedGivePlace(pSeat,pEntryPoint);

	std::vector<OnboardSeatInSim *>::iterator iter = vSeatInSim.begin();
	for (; iter != vSeatInSim.end(); ++iter)
	{
		OnboardSeatInSim* pSeatInSim = (*iter);
		if(pSeatInSim->GetSittingPerson())//the direction has other occupy seat, cannot let passenger go
			return false;
	}
	return true;
}

void OnboardSeatRowInSim::wakeupPax(const ElapsedTime& p_time,EntryPointInSim *pEntryPoint)
{
	std::vector<OnboardSeatInSim *> vSeatInSim = GetSequenceOfSeat(pEntryPoint);
	std::reverse(vSeatInSim.begin(), vSeatInSim.end());

	std::vector<OnboardSeatInSim *>::iterator iter = vSeatInSim.begin();
	for (; iter != vSeatInSim.end(); ++iter)
	{
		OnboardSeatInSim* pSeatInSim = (*iter);
		if(pSeatInSim->GetEntryPoint() != pEntryPoint)
			continue;
		Person* pPerson = pSeatInSim->GetSittingPerson();
		if(pPerson == NULL)
			continue;

		MobElementBehavior* pBehavior = pPerson->getCurrentBehavior();
		if (pBehavior == NULL || pBehavior->getBehaviorType() != MobElementBehavior::OnboardBehavior)
			continue;

		PaxDeplaneBehavior* pOnboardBehavior = (PaxDeplaneBehavior*)pBehavior;
		if (pPerson->getState() == SitOnSeat && pOnboardBehavior->m_bGenerateStandupTime)
		{
			if(p_time > pOnboardBehavior->m_eStandupPlaneTime)
				return pOnboardBehavior->GenerateEvent(p_time);
			else
				return pOnboardBehavior->GenerateEvent(pOnboardBehavior->m_eStandupPlaneTime);
		}
	}
}

int OnboardSeatRowInSim::GetResourceID() const
{
	return -1;
}