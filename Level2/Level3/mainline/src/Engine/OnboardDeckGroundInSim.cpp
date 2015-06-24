#include "stdafx.h"
#include "../InputOnboard/Deck.h"
#include "OnboardDeckGroundInSim.h"
#include "OnBoard/PathFinding/OBRect.h"
#include "OnBoard/PathFinding/RectMap.h"
#include "../Common/pollygon.h"
#include "../Common/point.h"

OnboardDeckGroundInSim::OnboardDeckGroundInSim(CDeck* pDeck)
:mpDeck(pDeck)
{

}

OnboardDeckGroundInSim::~OnboardDeckGroundInSim()
{

}

bool OnboardDeckGroundInSim::BuildDeckGround()
{
	if(mpDeck->UseVisibleRegions())
	{
		CreateVisibleRegion();
	}
	else if(mpDeck->IsMapValid())
	{
		CreateMapRegion();
	}
	else
	{
		CreateGridRegion();
	}

	Point ptBottomLeft, ptRightTop;
	m_pollygon.GetBoundsPoint(ptBottomLeft,ptRightTop);
	double deckMaxLength = ptRightTop.getX() - ptBottomLeft.getX();
	double deckMaxWidth  =  ptRightTop.getY() - ptBottomLeft.getY();
	OBGeo::FRect _outBoundRect(ptBottomLeft.getX(),  ptBottomLeft.getY(), deckMaxLength, deckMaxWidth);
	mGround.setHeight( mpDeck->RealAltitude() );
	mGround.generateGround( _outBoundRect, this);

	POLLYGONVECTOR pollygonList;
	//set occupied cell state
	if (mpDeck->UseVisibleRegions())
	{
		for(int i=0;i<(int)mpDeck->m_vVisibleRegions.size();i++)
		{
			Pollygon& poly = mpDeck->m_vVisibleRegions[i]->polygon;

			pollygonList.push_back(poly);
		}

		//invisible
		for(int i=0;i<(int)mpDeck->m_vInVisibleRegions.size();i++)
		{
			Pollygon& poly = mpDeck->m_vInVisibleRegions[i]->polygon;
			mGround.setInVisibleAsBarriers(poly);
		}
	}
	else
	{
		pollygonList.push_back(m_pollygon);
	}
	mGround.setOutsideLocatorSitesAsBarriers(pollygonList);
	return true;
}

void OnboardDeckGroundInSim::CreateGridRegion()
{
	//use grid size
	CGrid* pGridData = mpDeck->GetGrid();
	double dHfSizeX = pGridData->dSizeX;
	double dHfSizeY = pGridData->dSizeY;
	//top

	//
	Point ptX(1.0,0.0,0.0);
	Point ptY(0.0,1.0,0.0);

	Point ptList[4];
	ptList[0] = ptX*(-dHfSizeX) - ptY*dHfSizeY;
	ptList[1] = ptX*dHfSizeX - ptY*dHfSizeY;
	ptList[2] = ptX*dHfSizeX + ptY*dHfSizeY;
	ptList[3] = ptX*(-dHfSizeX) + ptY*dHfSizeY;

	m_pollygon.init(4,ptList);
}

void OnboardDeckGroundInSim::CreateMapRegion()
{
	//map size
	ARCVector2 vSize =  mpDeck->m_vMapExtentsMax - mpDeck->m_vMapExtentsMin;
	double dHfSizeX = vSize[VX]*0.5;
	double dHfSizeY = vSize[VY]*0.5;
	//
	Point ptX(1.0,0.0,0.0);
	Point ptY(0.0,1.0,0.0);

	Point ptList[4];
	ptList[0] = ptX*(-dHfSizeX) - ptY*dHfSizeY;
	ptList[1] = ptX*dHfSizeX - ptY*dHfSizeY;
	ptList[2] = ptX*dHfSizeX + ptY*dHfSizeY;
	ptList[3] = ptX*(-dHfSizeX) + ptY*dHfSizeY;

	double dScale = mpDeck->m_CadFile.dScale;
	double dRotate = mpDeck->m_CadFile.dRotation;
	ARCVector2 voffset = mpDeck->m_vOffset;

	ptList[0].DoOffset(voffset[VX],voffset[VY],0);
	ptList[0].scale(dScale,dScale);
	ptList[0].rotate(dRotate);

	ptList[1].DoOffset(voffset[VX],voffset[VY],0);
	ptList[1].scale(dScale,dScale);
	ptList[1].rotate(dRotate);

	ptList[2].DoOffset(voffset[VX],voffset[VY],0);
	ptList[2].scale(dScale,dScale);
	ptList[2].rotate(dRotate);

	ptList[3].DoOffset(voffset[VX],voffset[VY],0);
	ptList[3].scale(dScale,dScale);
	ptList[3].rotate(dRotate);

	m_pollygon.init(4,ptList);
}

void OnboardDeckGroundInSim::CreateVisibleRegion()
{
	Point ptBottomLeft, ptRightTop;

	if (mpDeck->m_vVisibleRegions.empty())
		return;

	Pollygon& frontPoly = mpDeck->m_vVisibleRegions.front()->polygon;
	frontPoly.GetBoundsPoint(ptBottomLeft,ptRightTop);

	for(int i=1;i<(int)mpDeck->m_vVisibleRegions.size();i++)
	{
		Point pt1,pt2;
		Pollygon& poly = mpDeck->m_vVisibleRegions[i]->polygon;
		poly.GetBoundsPoint(pt1,pt1);

		ptBottomLeft = ptBottomLeft.getMinPoint(pt1);
		ptRightTop = ptRightTop.getMaxPoint(pt2);
	}

	Point ptList[4];
	ptList[0] = Point(ptBottomLeft.getX(),ptBottomLeft.getY(),0);
	ptList[1] = Point(ptBottomLeft.getX(),ptRightTop.getY(),0);;
	ptList[2] = Point(ptRightTop.getX(),ptRightTop.getY(),0);
	ptList[3] = Point(ptRightTop.getX(),ptBottomLeft.getY(),0);;

	m_pollygon.init(4,ptList);
}

void OnboardDeckGroundInSim::initCellHoldMobile()
{
	mGround.initCellHoldMobile();
}

void OnboardDeckGroundInSim::GetPath( OnboardCellInSim* pStartCell, OnboardCellInSim* pEndCell, OnboardCellPath& _movePath )
{
	return mGround.GetPath(pStartCell, pEndCell,_movePath);
}
