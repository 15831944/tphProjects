#pragma once
#include "GroundInSim.h"
#include "../Common/pollygon.h"
class CDeck;
class OnboardDeckGroundInSim
{
public:
	OnboardDeckGroundInSim(CDeck* pDeck);
	~OnboardDeckGroundInSim();

	CDeck* GetDeck()const {return mpDeck;}
	GroundInSim* GetGround(){return &mGround;}

	bool BuildDeckGround();
	void initCellHoldMobile();


public:
	//give origin cell, Destination Cell, 
	//return a path
	void GetPath(OnboardCellInSim* pStartCell, OnboardCellInSim* pEndCell, 
		OnboardCellPath& _movePath);

protected:
	void CreateVisibleRegion();
	void CreateGridRegion();
	void CreateMapRegion();


protected:
	GroundInSim mGround;
	CDeck*   mpDeck;
	Pollygon m_pollygon;
};