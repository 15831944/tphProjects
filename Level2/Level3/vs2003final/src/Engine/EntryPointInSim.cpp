#include "stdafx.h"
#include "EntryPointInSim.h"
#include "OnBoardCorridorInSim.h"
#include "InputOnBoard/OnboardCorridor.h"
#include "OnboardSeatInSim.h"
#include "GroundInSim.h"
#include "PaxOnboardBaseBehavior.h"
#include "OnboardCellSeatGivePlaceLogic.h"
#include "OnboardStorageInSim.h"
#include "OnboardFlightInSim.h"



EntryPointInSim::EntryPointInSim(OnboardCorridorInSim* pCorridorInSim,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pCorridorInSim(pCorridorInSim)
,m_dDistFromStart(0.0)
,m_emType(Straight_Walk)
,m_pSeatInSim(NULL)
,m_pCellInSim(NULL)
{

	m_pGivePlaceLogic = NULL;
}

EntryPointInSim::EntryPointInSim(OnboardCellInSim* pCellInSim,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_emType(Straight_Walk)
,m_pSeatInSim(NULL)
,m_pCellInSim(pCellInSim)
{
	m_pGivePlaceLogic = NULL;
}

EntryPointInSim::~EntryPointInSim()
{

}

void EntryPointInSim::SetCreateSeat(OnboardSeatInSim *pSeat)
{
	ASSERT(pSeat);
	m_pSeatInSim = pSeat;

	CreateEntryPosition();
}
void EntryPointInSim::SetWalkType(WalkThroughtType emWalk)
{
	m_emType = emWalk;
}

EntryPointInSim::WalkThroughtType EntryPointInSim::GetWalkType()const
{
	return m_emType;
}

void EntryPointInSim::SetDistance(double dDist)
{
	m_dDistFromStart =dDist;
}

double EntryPointInSim::GetDistance()const
{
	return m_dDistFromStart;
}

void EntryPointInSim::SetLocation(const ARCVector3& location)
{
	m_location = location;
}

bool EntryPointInSim::GetLocation(ARCVector3& location)const
{
	if(m_pCellInSim)
	{
		location = m_pCellInSim->getLocation();
		return true;
	}

	return false;
}

OnboardCorridorInSim* EntryPointInSim::GetCorridor()const
{
	return m_pCorridorInSim;
}

BOOL EntryPointInSim::IsAvailable() const
{
	return TRUE;
}

void EntryPointInSim::CreateEntryPosition()
{
	GroundInSim *pGround = m_pSeatInSim->GetGround();

	double dPaxLength = 50.0;
	double dPaxWidth = 50.0;

	//get the seat direction
	CPoint2008 ptSeatLocation;
	m_pSeatInSim->GetLocation(ptSeatLocation);
	//seat front position
	ARCVector3 seatFrontVec;
	m_pSeatInSim->GetFrontPosition(seatFrontVec);
	CPoint2008 ptSeatFrontPosition(seatFrontVec[VX],seatFrontVec[VY],seatFrontVec[VZ]);
	CPoint2008 seatDirection = CPoint2008(ptSeatFrontPosition - ptSeatLocation);

	seatDirection.Normalize();
	seatDirection.length(PAXRADIUS);

	pGround->CalculateLocation(m_pCellInSim->getLocation(), seatDirection, dPaxWidth, dPaxWidth, m_entryLocation);
}
//
std::vector<PaxCellLocation > EntryPointInSim::GetCheckingPoints()
{
	if(m_vCheckingPoint.size())//calculated
		return m_vCheckingPoint;

// 	if(Straight_Walk == m_emType)
// 	{//straight walk, move to entry point directly
//         
// 		PaxCellLocation cellLocation;
// 		cellLocation.setPathCell(m_pCellInSim);
// 		cellLocation.setCellCenter(m_pCellInSim);
// 	
// 		m_vCheckingPoint.push_back(cellLocation);
// 
// 		return m_vCheckingPoint;
// 	}
	
	//side walk rows

	//create checking points
	ASSERT(m_pCellInSim != NULL);
	if(m_pCellInSim == NULL)
		return m_vCheckingPoint;

	//entry point location
	CPoint2008 ptEntryPoint = m_pCellInSim->getLocation();

	//get the seat direction
	CPoint2008 ptSeatLocation;
	m_pSeatInSim->GetLocation(ptSeatLocation);

	//seat front position
	ARCVector3 seatFrontVec;
	m_pSeatInSim->GetFrontPosition(seatFrontVec);
	CPoint2008 ptSeatFrontPosition(seatFrontVec[VX],seatFrontVec[VY],seatFrontVec[VZ]);
	CPoint2008 seatDirection = CPoint2008(ptSeatFrontPosition - ptSeatLocation);

	seatDirection.Normalize();
	seatDirection.length(PAXRADIUS);

	double dPaxLength = 40.0;
	double dPaxWidth = 40.0;
	
	GroundInSim *pGround = m_pSeatInSim->GetGround();
	//front
	{
		CPoint2008 ptFronCheck = ptEntryPoint + seatDirection;
		PaxCellLocation paxCheckLocation;

		OnboardCellInSim *pFrontCheckCell =  pGround->getCell(ptFronCheck);
		pGround->CalculateLocation(ptFronCheck,seatDirection,dPaxLength, dPaxWidth,paxCheckLocation);

		paxCheckLocation.setCellCenter(pFrontCheckCell);
		paxCheckLocation.setPathCell(pFrontCheckCell);
		m_vCheckingPoint.push_back(paxCheckLocation);

	}
	
	//back
	{
		CPoint2008 reverseDirection = seatDirection*(-1);

		CPoint2008 ptBackCheck = ptEntryPoint + reverseDirection;
		PaxCellLocation paxCheckLocation;
		OnboardCellInSim *pBackCheckCell =  pGround->getCell(ptBackCheck);
		pGround->CalculateLocation(ptBackCheck,seatDirection,dPaxLength, dPaxWidth,paxCheckLocation);

		paxCheckLocation.setCellCenter(pBackCheckCell);
		paxCheckLocation.setPathCell(pBackCheckCell);
		m_vCheckingPoint.push_back(paxCheckLocation);
	}

	//calculate entry location
	pGround->CalculateLocation(m_pCellInSim->getLocation(), seatDirection, dPaxWidth, dPaxWidth, m_entryLocation);


	return m_vCheckingPoint;
}

OnboardCellSeatGivePlaceLogic * EntryPointInSim::GetGivePlaceLogic()
{
	if(m_pGivePlaceLogic == NULL)
		m_pGivePlaceLogic = new OnboardCellSeatGivePlaceLogic(this);

	return m_pGivePlaceLogic;
}

bool EntryPointInSim::CalculateTempLocation( PaxCellLocation& checkingLocation, PaxCellLocation & tmpPaxLocation ) const
{	
	double dPaxLength = 40.0;
	double dPaxWidth = 40.0;

	//the another checking location
	OnboardCellInSim *pAnotherCeheckingCell  = NULL;
	PaxCellLocation anotherLocation;
	if(!GetAnotherCheckingLocation(checkingLocation, anotherLocation))
		return false;
	pAnotherCeheckingCell = anotherLocation.getCellCenter();
	if(pAnotherCeheckingCell == NULL)
		return false;

	//offset a passnger's width
	CPoint2008 ptDirection = CPoint2008(pAnotherCeheckingCell->getLocation() - checkingLocation.getCellCenter()->getLocation());

	ptDirection.Normalize();
	ptDirection.length(PAXRADIUS);
	CPoint2008 ptLocation = pAnotherCeheckingCell->getLocation() + ptDirection;// get the new location

	GroundInSim *pGround = m_pSeatInSim->GetGround();
	if(pGround == NULL)
		return false;

	OnboardCellInSim *pNextCell = pGround->GetPointCell(ptLocation);//out of range
	if(pNextCell == NULL || !pNextCell->IsValid())//or the cell is not valid
		ptLocation = pAnotherCeheckingCell->getLocation();


	//find the cells
	return pGround->CalculateLocation(ptLocation, ptDirection, dPaxLength, dPaxWidth, tmpPaxLocation);

}

bool EntryPointInSim::GetAnotherCheckingLocation( PaxCellLocation& curLocation, PaxCellLocation& nextCellLocation ) const
{
	for (int nCheckLocation = 0; nCheckLocation < static_cast<int>(m_vCheckingPoint.size()); ++ nCheckLocation)
	{
		if(m_vCheckingPoint[nCheckLocation].getCellCenter() != curLocation.getCellCenter())
		{
			nextCellLocation= m_vCheckingPoint[nCheckLocation];
			return true;
		}
	}
	return false;
}

int EntryPointInSim::GetResourceID() const
{
	return -1;
}

//bool EntryPointInSim::Reserve( PaxOnboardBaseBehavior *pCurPax )
//{
//	//CheckLocation
//	//lock the checking position
//	pCurPax->
//
//	for (int nCheckLocation = 0; nCheckLocation < static_cast<int>(m_vCheckingPoint.size()); ++ nCheckLocation)
//	{
//		m_vCheckingPoint[nCheckLocation].PaxReserve(pCurPax);
//	}
//
//	//entry location
//	m_entryLocation.PaxReserve(pCurPax);
//
//	return true;
//}
//
//void EntryPointInSim::CancelReserve( PaxOnboardBaseBehavior *pCurPax )
//{
//
//}



























