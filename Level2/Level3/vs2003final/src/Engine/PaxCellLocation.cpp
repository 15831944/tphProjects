#include "StdAfx.h"
#include ".\paxcelllocation.h"
#include "PaxOnboardBaseBehavior.h"


PaxCellLocation::PaxCellLocation(void)
:m_pCenterCell(NULL)
,m_pPathCell(NULL)
{

}

PaxCellLocation::~PaxCellLocation(void)
{
}

OnboardCellInSim * PaxCellLocation::getCellCenter() const
{
	return m_pCenterCell;
}

void PaxCellLocation::setCellCenter( OnboardCellInSim * pCell )
{
	m_pCenterCell = pCell;
}

void PaxCellLocation::Clear()
{
	OnboardCellPath::Clear();
	m_pCenterCell = NULL;
	m_pPathCell = NULL;
	m_paxRect.clear();
}

OnboardCellInSim * PaxCellLocation::getPathCell() const
{
	return m_pPathCell;
}

void PaxCellLocation::setPathCell( OnboardCellInSim * pCell )
{
	m_pPathCell = pCell;
}

void PaxCellLocation::PaxOccupy( PaxOnboardBaseBehavior *pBehavior )
{
	if(m_pCenterCell)
		m_pCenterCell->setBehavior(pBehavior);
	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell)
			pCell->setBehavior(pBehavior);
	}

}
void PaxCellLocation::PaxClear(PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime)
{
	if(m_pCenterCell)
		m_pCenterCell->PaxLeave(pBehavior, eTime);

	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell)
			pCell->PaxLeave(pBehavior, eTime);
	}
}

bool PaxCellLocation::IsAvailable( PaxOnboardBaseBehavior *pBehavior ) const
{
	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell)
		{
			if(!pCell->IsAvailable(pBehavior))
				return false;
		}
	}

	return m_pCenterCell->IsAvailable(pBehavior);
}

bool PaxCellLocation::PaxReserve( PaxOnboardBaseBehavior *pBehavior )
{
	//ASSERT(CanReserve(pBehavior));

    if(m_pCenterCell != NULL)
	   m_pCenterCell->PaxReserve(pBehavior);

	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell == NULL)
			continue;
		pCell->PaxReserve(pBehavior);
	}

	return true;

}

void PaxCellLocation::PaxCancelReserve( PaxOnboardBaseBehavior *pBehavior)
{
	if(m_pCenterCell)
		m_pCenterCell->PaxReserve(NULL);
	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell)
			pCell->PaxReserve(NULL);
	}
}

bool PaxCellLocation::CanReserve( PaxOnboardBaseBehavior *pBehavior )
{
	if(m_pCenterCell == NULL)
		return false;

	if(m_pCenterCell->getReservePax() != NULL && //checking exist
		m_pCenterCell->getReservePax() != pBehavior && //check is the same person
		m_pCenterCell->getReservePax()->TheReseveCouldOverWrite() == false)//checking could be overwrite
		return false;

	for (int nCell = 0; nCell <getCellCount(); ++nCell )
	{
		OnboardCellInSim *pCell = getCell(nCell);
		if(pCell == NULL)
			continue;

		if(pCell->getReservePax() != NULL &&
			pCell->getReservePax() != pBehavior &&
			pCell->getReservePax()->TheReseveCouldOverWrite() == false)
			return false;
	}

	return true;

}

void PaxCellLocation::setPaxRect( CPath2008 &posRect )
{
	m_paxRect.init(posRect);
}

CPath2008 PaxCellLocation::getPaxRect() const
{
	return m_paxRect;
}