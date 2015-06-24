#pragma once
#include "onboardcellinsim.h"

class PaxCellLocation: public OnboardCellPath
{
public:
	PaxCellLocation();
	~PaxCellLocation();

public:

	//clear the occupied operation
	virtual void Clear();

	//set the flag that passenger  took the place
	void PaxOccupy(PaxOnboardBaseBehavior *pBehavior);
	//remove the flag, and make the location available for other passengers
	void PaxClear(PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime);
	

	//check the location has been reserved or not
	bool CanReserve(PaxOnboardBaseBehavior *pBehavior);
	//reserve the location
	bool PaxReserve(PaxOnboardBaseBehavior *pBehavior);
	//remove the flag that the place has been taken by other passenger
	void PaxCancelReserve(PaxOnboardBaseBehavior *pBehavior);

	//date operation
public:
	OnboardCellInSim * getCellCenter() const;
	void setCellCenter(OnboardCellInSim * pCell);

	OnboardCellInSim * getPathCell() const;
	void setPathCell(OnboardCellInSim * pCell);

	//passenger's position rect, four points, vertex
	void setPaxRect(CPath2008 &posRect);
	CPath2008 getPaxRect() const;

	bool IsAvailable(PaxOnboardBaseBehavior *pBehavior) const;
protected:
	//the passenger's center path
	OnboardCellInSim *m_pCenterCell;

	//the cell which at Path
	OnboardCellInSim *m_pPathCell;

	//passenger  position rect
	CPath2008 m_paxRect;



};