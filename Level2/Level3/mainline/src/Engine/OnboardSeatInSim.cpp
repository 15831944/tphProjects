#include "StdAfx.h"
#include ".\onboardseatinsim.h"
#include "InputOnBoard\Seat.h"
#include <inputonboard\Deck.h>
#include "EntryPointInSim.h"
#include "OnboardSeatRowInSim.h"
#include "OnboardSeatGroupInSim.h"
#include "person.h"
#include "visitor.h"
#include "OnboardFlightInSim.h"
#include "GroundInSim.h"
#include "OnboardElementSpace.h"
#include "Common/RayIntersectPath.h"
#include "PaxOnboardBaseBehavior.h"
#include "OnboardDoorInSim.h"

OnboardSeatInSim::OnboardSeatInSim(CSeat *pSeat,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pSeat(pSeat)
,m_pPerson(NULL)
,m_pEntryPointInSim(NULL)
,m_pElementSpace(NULL)
,m_pSeatGroupInSim(NULL)
{
	m_pAssigndPerson = NULL;
	m_pSeatRowInSim = NULL;
	m_dCarryonLoad = 0.0;
	CalculateSpace();
	CreateEntryPoint();
}

OnboardSeatInSim::~OnboardSeatInSim(void)
{
	ClearSpace();
	if (m_pEntryPointInSim && m_pSeatRowInSim == NULL)
	{
		//delete m_pEntryPointInSim;
		m_pEntryPointInSim = NULL;
	}

	m_vSeatType.clear();
}

BOOL OnboardSeatInSim::AssignToPerson( Person *pPerson )
{
	if (m_pPerson == NULL)
	{
		m_pPerson = pPerson;
		m_pAssigndPerson = pPerson;

		return TRUE;
	}

	if(m_pPerson != pPerson)
		return FALSE;

	return TRUE;
}

BOOL OnboardSeatInSim::ClearPaxFromSeat()
{
	m_pPerson = NULL;
	m_pAssigndPerson = NULL;
	return TRUE;
}
BOOL OnboardSeatInSim::GetPosition( ARCVector3& seatPos ) const
{
	ASSERT(m_pSeat != NULL);
	if(m_pSeat == NULL)
		return FALSE;
	seatPos = m_pSeat->GetPosition();	
	seatPos[VZ] = m_pSeat->GetDeck()->RealAltitude();

	return TRUE;
}

bool OnboardSeatInSim::GetLocation(CPoint2008& location)const
{
	ASSERT(m_pSeat != NULL);
	if(m_pSeat == NULL)
		return false;

	location.init( m_pSeat->GetPosition().n[VX],m_pSeat->GetPosition().n[VY],m_pSeat->GetDeck()->RealAltitude());
	return true;
}

BOOL OnboardSeatInSim::IsAssigned() const
{
	return m_pPerson == NULL?FALSE:TRUE;
}

Person* OnboardSeatInSim::GetSittingPerson()
{
	return m_pPerson;
}

void OnboardSeatInSim::PersonLeave()
{	
	m_pPerson = NULL;
}

CString OnboardSeatInSim::GetType() const
{
	return m_pSeat->GetType();
}

BOOL OnboardSeatInSim::GetFrontPosition( ARCVector3& entryPos ) const
{
	ARCVector3 seatPos;
	GetPosition(seatPos);
	Point ptTo(m_pSeat->GetLength() * 0.75,0,0);
	seatPos += ptTo;

	entryPos = seatPos;

	return TRUE;

}

OnboardCellInSim* OnboardSeatInSim::GetOnboardCellInSim()
{
	ARCVector3 location;
	GetFrontPosition(location);


	return GetGround()->getCell(CPoint2008(location.n[VX],location.n[VY],location.n[VZ]));
}

EntryPointInSim* OnboardSeatInSim::GetEntryPoint()const
{
	if(m_pEntryPointInSim )
		return m_pEntryPointInSim;

	if (m_pSeatRowInSim == NULL)//does not assign row
		return m_pEntryPointInSim;

	m_pEntryPointInSim = m_pSeatRowInSim->GetEntryPoint(this);

	return m_pEntryPointInSim;
}

BOOL OnboardSeatInSim::IsPaxSit()
{
	if(m_pPerson && 
		(m_pPerson->getState() == SitOnSeat ||
		m_pPerson->getState() == ArriveAtSeat ||
		m_pPerson->getState() == StrideOverSeat)
		)
		return TRUE;
	else
		return FALSE;
}

void OnboardSeatInSim::CalculateSpace()
{
	if (m_pSeat->getRow() || m_pSeat->getGroup())
		return;
	
	ClearSpace();
	m_pElementSpace = new OnboardElementSpace(GetGround());

	m_pElementSpace->CreatePollygon(this);

	m_pElementSpace->CalculateSpace();
}

void OnboardSeatInSim::ClearSpace()
{
	if (m_pElementSpace)
	{
		delete m_pElementSpace;
		m_pElementSpace = NULL;
	}
}

GroundInSim* OnboardSeatInSim::GetGround()
{
	return m_pFlightInSim->GetGroundInSim(m_pSeat->GetDeck());
}

void OnboardSeatInSim::CreatePollygon()
{
	CPath2008 path;
	m_pSeat->GetFrontSeatLeftRightOfRowPath(path);
	CPollygon2008 pollygon;
	pollygon.init(path.getCount(),path.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}

EntryPointInSim* OnboardSeatInSim::FindEntryPoint()
{
	ARCVector3 seatPt;
	GetFrontPosition(seatPt);

	CPoint2008 entryPt(seatPt.n[VX],seatPt.n[VY],0.0);
	GroundInSim* pGroundInSim = GetGround();

	if (pGroundInSim == NULL)
		return NULL;

	OnboardCellInSim* pCellInSim = pGroundInSim->getCell(entryPt);
	EntryPointInSim* pEntryPointInSim = new EntryPointInSim(pCellInSim,m_pFlightInSim);
	pEntryPointInSim->SetCreateSeat(this);
	pEntryPointInSim->SetWalkType(EntryPointInSim::Straight_Walk);
	return pEntryPointInSim;
}

void OnboardSeatInSim::CreateEntryPoint()
{
	//seat without seat row and seat group that need create entry point
	if (m_pSeat->getRow() || m_pSeat->getGroup())
		return;
	
	m_pEntryPointInSim = FindEntryPoint();
	m_pEntryPointInSim->GetOnboardCell()->entryPoint(true);
}

EntryPointInSim* OnboardSeatInSim::GetEntryPointWithSeatRowAndSeatGroup()
{
	if (m_pSeatGroupInSim == NULL && m_pSeatRowInSim == NULL)
		return NULL;
	
	return FindEntryPoint();
}

EntryPointInSim* OnboardSeatInSim::GetfrontRowEntryPoint(const CPoint2008& dir)
{
	CPoint2008 rotateDir = dir;
	if (m_pSeatGroupInSim == NULL)
		return NULL;
	const CPollygon2008& polygon = m_pSeatGroupInSim->GetPolygon();

	EntryPointInSim* pEntryPointInSim = NULL;
	ARCVector3 entryPt;
	GetFrontPosition(entryPt);
	CPoint2008 seatPt(entryPt.n[VX],entryPt.n[VY],0.0);

	CPoint2008 cellPt(GRID_WIDTH,GRID_HEIGHT,0.0);
	double dDist = PAXRADIUS + GRID_WIDTH;//offset make be able to next cell
	double dOffset = m_pSeat->GetWidth()/2;

	CPoint2008 ptOffset = seatPt + rotateDir*(m_pSeat->GetWidth()/2);
	OnboardCellInSim* pCellInSim = GetGround()->getCell(ptOffset);
	CPoint2008 cellInSimPt = pCellInSim->getLocation();
	cellInSimPt += rotateDir * dDist;
	OnboardCellInSim* pEntryCellInSim = GetGround()->GetPointCell(cellInSimPt);
	if (pEntryCellInSim)
	{
		pEntryCellInSim->SetState(OnboardCellInSim::Idle);
		GetGround()->getLogicMap().setTileAsBarrier(pEntryCellInSim->getTileIndex(), false);
		pEntryPointInSim = new EntryPointInSim(pEntryCellInSim,m_pFlightInSim);
		pEntryPointInSim->SetWalkType(EntryPointInSim::TurnDirection_Walk);
		pEntryPointInSim->SetCreateSeat(this);
	}
	
	return pEntryPointInSim;
}

EntryPointInSim* OnboardSeatInSim::GetIntersectionNodeWithSeatGroupPolygon()
{
	if (m_pSeatGroupInSim == NULL)
		return NULL;
	const CPollygon2008& polygon = m_pSeatGroupInSim->GetPolygon();

	EntryPointInSim* pEntryPointInSim = NULL;
	ARCVector3 entryPt;
	GetFrontPosition(entryPt);
	CPoint2008 seatPt(entryPt.n[VX],entryPt.n[VY],0.0);
	CPoint2008 locationPt;
	GetLocation(locationPt);
	locationPt.setZ(0.0);
	
	CPoint2008 vDir(locationPt,seatPt);
	vDir.Normalize();
	
	CPoint2008 cellPt(GRID_WIDTH,GRID_HEIGHT,0.0);
	double dDist = PAXRADIUS/2 + GRID_WIDTH;//cellPt.length()*0.5 + 20;//offset make be able to next cell
	//rotate 90
	double dRayLeft = (std::numeric_limits<double>::max)();
	CPoint2008 ptRayLeft;
	CPoint2008 vDirLeft(vDir);
	vDirLeft.rotate(90.0);
	vDirLeft.Normalize();
	CRayIntersectPath2D rayPathLeft(seatPt,vDirLeft,polygon);
	if (rayPathLeft.Get())
	{
		ptRayLeft = polygon.GetIndexPoint((float)rayPathLeft.m_Out.begin()->m_indexInpath);
		CPoint2008 rayDirLeft(seatPt,ptRayLeft);
		rayDirLeft.Normalize();
		OnboardCellInSim* pLeftCellInSim = GetGround()->getCell(ptRayLeft);
		CPoint2008 leftCellPt = pLeftCellInSim->getLocation();
		leftCellPt += rayDirLeft * dDist;
		OnboardCellInSim* pLeftEntryCellInSim = GetGround()->GetPointCell(leftCellPt);
		if (pLeftEntryCellInSim)
		{
			pLeftEntryCellInSim->SetState(OnboardCellInSim::Idle);
			GetGround()->getLogicMap().setTileAsBarrier(pLeftEntryCellInSim->getTileIndex(), false);
			dRayLeft = rayPathLeft.m_Out.front().m_dist;
			pEntryPointInSim = new EntryPointInSim(pLeftEntryCellInSim,m_pFlightInSim);
			pEntryPointInSim->SetWalkType(EntryPointInSim::TurnDirection_Walk);
			pEntryPointInSim->SetCreateSeat(this);
		}
	}
	//rotate -90
	double dRayRight = (std::numeric_limits<double>::max)();
	CPoint2008 ptRayRight;
	CPoint2008 vDirRight(vDir);
	vDirRight.rotate(-90.0);
	vDirRight.Normalize();
	CRayIntersectPath2D rayPathRight(seatPt,vDirRight,polygon);
	if (rayPathRight.Get())
	{
		ptRayRight = polygon.GetIndexPoint((float)rayPathRight.m_Out.begin()->m_indexInpath);
		OnboardCellInSim* pRightCellInSim = GetGround()->getCell(ptRayRight);
		CPoint2008 rayDirRight(seatPt,ptRayRight);
		rayDirRight.Normalize();
		CPoint2008 rightCellPt = pRightCellInSim->getLocation();
		rightCellPt += rayDirRight * dDist;
		OnboardCellInSim* pRightEntryCellInSim = GetGround()->GetPointCell(rightCellPt);

		if (pRightEntryCellInSim)
		{
			pRightEntryCellInSim->SetState(OnboardCellInSim::Idle);
			GetGround()->getLogicMap().setTileAsBarrier(pRightEntryCellInSim->getTileIndex(), false);
			dRayRight = rayPathRight.m_Out.front().m_dist;
			if (dRayRight < dRayLeft)
			{
				if (pEntryPointInSim)
				{
					delete pEntryPointInSim;
					pEntryPointInSim = NULL;
				}
				pEntryPointInSim = new EntryPointInSim(pRightEntryCellInSim,m_pFlightInSim);
				pEntryPointInSim->SetWalkType(EntryPointInSim::TurnDirection_Walk);
				pEntryPointInSim->SetCreateSeat(this);
			}
		}
		
	}

	return pEntryPointInSim;
}

double OnboardSeatInSim::GetRotation() const
{
	return m_pSeat->GetRotation();
}

OnboardCellInSim * OnboardSeatInSim::GetEntryCell() const
{
	return	m_pEntryPointInSim->GetEntryCell();
}

void OnboardSeatInSim::addVisitor(PaxVisitor* pVisitor)
{
	ASSERT(pVisitor);
	if (pVisitor)
	{
		m_pVisitorList.push_back(pVisitor);
		m_dCarryonLoad += pVisitor->getOnboardBehavior()->GetVolume(NULL);
	}
}

void OnboardSeatInSim::visitorWriteLog(const ElapsedTime& eTime)
{
	std::vector<PaxVisitor*>::iterator iter = m_pVisitorList.begin();
	for (; iter != m_pVisitorList.end(); ++iter)
	{
		PaxVisitor* pVisitor = *iter;
		PaxOnboardBaseBehavior* pOnboardBehavior = pVisitor->getOnboardBehavior();
		ASSERT(pOnboardBehavior);
		if (pOnboardBehavior)
		{
			pOnboardBehavior->writeLog(eTime,false);
		}
	}
}

void OnboardSeatInSim::visitorFlushLog(const ElapsedTime& eTime)
{
	std::vector<PaxVisitor*>::iterator iter = m_pVisitorList.begin();
	for (; iter != m_pVisitorList.end(); ++iter)
	{
		PaxVisitor* pVisitor = *iter;
		pVisitor->flushLog(eTime,true);
	}
}

ARCVector3 OnboardSeatInSim::GetRandomPoint()const
{
	ARCVector3 seatPos;
	GetPosition(seatPos);
	Point ptTo(m_pSeat->GetLength() * 0.25,0,0);
	seatPos += ptTo;
	
	double xProb, yProb;

	double nRadius = min(m_pSeat->GetLength()/2,m_pSeat->GetWidth())/2;
	ARCVector3 randomPt;
	do {
		// get 3 random numbers between 0 and 1
		xProb = (double)rand() / RAND_MAX;
		yProb = (double)rand() / RAND_MAX;

		// create random point
		randomPt.n[VX] = seatPos.n[VX] + (xProb * nRadius);
		randomPt.n[VY] = seatPos.n[VY] + (yProb * nRadius);
		randomPt.n[VZ] = seatPos.n[VZ];

		// if new point is not within polygon area, repeat
	} while (seatPos.DistanceTo(randomPt) > nRadius);

	return randomPt;
}

void OnboardSeatInSim::clearVisitor()
{
	m_pVisitorList.clear();
}

int OnboardSeatInSim::GetResourceID() const
{
	if(m_pSeat)
		return m_pSeat->GetID();

	return -1;
}

void OnboardSeatInSim::AddSeatType( OnboardSeatType emType )
{
	ASSERT(emType >= ST_Bulkhead && emType < ST_SeatTypeNum);
	m_vSeatType.push_back(emType);
}

bool OnboardSeatInSim::fitSeatType( OnboardSeatType emType ) const
{
	if (std::find(m_vSeatType.begin(),m_vSeatType.end(),emType) != m_vSeatType.end())
	{
		return true;
	}

	return false;
}

double OnboardSeatInSim::GetAvailableSpace() const
{
	return m_pSeat->GetCapacity() - m_dCarryonLoad;
}
