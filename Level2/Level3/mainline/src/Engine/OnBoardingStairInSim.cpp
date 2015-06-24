#include "stdafx.h"
#include "OnBoardingStairInSim.h"
#include "InputOnBoard/OnBoardBaseVerticalDevice.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "Common/ARCPipe.h"
#include "GroundInSim.h"
#include "OnboardFlightInSim.h"
#include "OnboardElementSpace.h"

OnboardStairInSim::OnboardStairInSim(COnBoardStair *pStair,OnboardFlightInSim* pFlightInSim)
:OnboardDeviceInSim(pStair,pFlightInSim)
,m_pElementSpace(NULL)
,m_pBackElementSpace(NULL)
{
	m_realpath = pStair->GetLayOut()->GetDeckManager()->GetRealPath(pStair->GetPathService());
	CalculateSpace();
}

OnboardStairInSim::~OnboardStairInSim()
{
	ClearSpace();
}

GroundInSim* OnboardStairInSim::GetGround(CDeck* pDeck)
{
// 	DeckPoint deckPt = m_pDevice->GetDeckPoint().front();
// 	return m_pFlightInSim->GetGroundInSim(deckPt.first);
	return m_pFlightInSim->GetGroundInSim(pDeck);
}

bool OnboardStairInSim::GetLocation(CPoint2008& location)const
{
	if(m_realpath.getCount() == 0)
		return false;

	location = m_realpath.getPoint(0);
	return true;
}

void OnboardStairInSim::ClearSpace()
{
	if (m_pElementSpace)
	{
		delete m_pElementSpace;
		m_pElementSpace = NULL;
	}

	if (m_pBackElementSpace)
	{
		delete m_pBackElementSpace;
		m_pBackElementSpace = NULL;
	}
}
void OnboardStairInSim::CreatePollygon()
{
	CPollygon2008 pollygon;
	CPath2008 path;
	int nPointCount = m_realpath.getCount();
	CPoint2008* ptList = new CPoint2008[nPointCount];
	for (int iPoint = 0; iPoint < m_realpath.getCount(); iPoint++)
	{
		CPoint2008 point = m_realpath.getPoint(iPoint);
		point.setZ(0.0);
		ptList[iPoint] = point;
	}

	path.init(nPointCount,ptList);

	delete []ptList;

	COnBoardStair* pStair = (COnBoardStair*)m_pDevice;
	ARCPipe arcPath(path,pStair->GetWidth());

	CPath2008& sidePath1 = arcPath.m_sidePath1.getPath2008();
	CPath2008& sidePath2 = arcPath.m_sidePath2.getPath2008();

	CPath2008 wallPath;
	wallPath.ConnectPath(sidePath1);
	sidePath2.invertList();
	wallPath.ConnectPath(sidePath2);

	pollygon.init(wallPath.getCount(),wallPath.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}

void OnboardStairInSim::CreateBackElementSpace()
{
	int nPointCount = m_realpath.getCount();
	if (nPointCount < 2)
		return;

	DeckPoint deckBackPt = m_pDevice->GetDeckPoint().back();
	CPoint2008 backPt = m_realpath.getPoint(nPointCount - 1);
	CPoint2008 prePt = m_realpath.getPoint(nPointCount - 2);
	CPoint2008 backDir(backPt - prePt);
	backDir.setZ(0.0);
	backDir.Normalize();

	CPoint2008 cellPt(GRID_WIDTH,GRID_HEIGHT,0.0);
	double dOffsetDist = cellPt.length();//offset make be able to next cell

	COnBoardStair* pStair = (COnBoardStair*)m_pDevice;
	double dDist = pStair->GetWidth()/2;
	//rotate 90
	CPoint2008 leftDir = backDir;
	leftDir.rotate(90);
	leftDir.Normalize();
	CPoint2008 leftTop = backPt + leftDir*dDist;
	CPoint2008 rightTop = leftTop + backDir*dOffsetDist;

	//rotate -90
	CPoint2008 rightDir = backDir;
	rightDir.rotate(-90);
	rightDir.Normalize();
	CPoint2008 leftBottom = backPt + rightDir*dDist;
	CPoint2008 rightBottom = leftBottom + backDir*dOffsetDist;

	CPollygon2008 pollygon;
	CPoint2008 ptList[4];
	ptList[0] = leftTop;
	ptList[1] = rightTop;
	ptList[2] = rightBottom;
	ptList[3] = leftBottom;

	pollygon.init(4,ptList);

	if (m_pBackElementSpace)
	{
		delete m_pBackElementSpace;
		m_pBackElementSpace = NULL;
	}
	
	m_pBackElementSpace = new OnboardElementSpace(GetGround(deckBackPt.first));
	m_pBackElementSpace->setPollygon(pollygon);
	m_pBackElementSpace->CalculateSpace();
}

void OnboardStairInSim::CreateEntryPoint()
{
	int nDeckCount = (int)m_pDevice->GetDeckPoint().size();
	if (nDeckCount < 2) //less than two point can not calculate entry point
		return;
	
	int nPointCount = m_realpath.getCount();
	if (nPointCount < 2)
		return;

	CPoint2008 cellPt(GRID_WIDTH,GRID_HEIGHT,0.0);
	double dDist = cellPt.length();//offset make be able to next cell

	//first entry point
	DeckPoint deckFirstPt = m_pDevice->GetDeckPoint().front();
	CPoint2008 firstDir(m_realpath.getPoint(0) - m_realpath.getPoint(1));
	firstDir.setZ(0.0);
	firstDir.Normalize();
	OnboardCellInSim* pFirstCell = GetGround(deckFirstPt.first)->getCell(m_realpath.getPoint(0));
	CPoint2008 firstLocate = pFirstCell->getLocation();
	CPoint2008 firstEntryPoint = firstLocate + firstDir*dDist;
	OnboardCellInSim* pFirstEntryCell = GetGround(deckFirstPt.first)->getCell(firstEntryPoint);
	m_entryPointList.push_back(pFirstEntryCell);

	//back entry point
	DeckPoint deckBackPt = m_pDevice->GetDeckPoint().back();
	CPoint2008 backPt = m_realpath.getPoint(nPointCount - 1);
	CPoint2008 prePt = m_realpath.getPoint(nPointCount - 2);
	CPoint2008 backDir(backPt - prePt);
	backDir.setZ(0.0);
	backDir.Normalize();
	backPt += backDir*dDist;
	OnboardCellInSim* pBackCell = GetGround(deckBackPt.first)->getCell(backPt);
	CPoint2008 backLocate = pBackCell->getLocation();
	CPoint2008 backEntryPoint = backLocate + backDir*(dDist/2);
	OnboardCellInSim* pBackEntryCell = GetGround(deckBackPt.first)->getCell(backEntryPoint);
	m_entryPointList.push_back(pBackEntryCell);
}

void OnboardStairInSim::CalculateSpace()
{
	ClearSpace();
	CreateEntryPoint();
	DeckPoint deckPt = m_pDevice->GetDeckPoint().front();
	m_pElementSpace = new OnboardElementSpace(GetGround(deckPt.first));
	m_pElementSpace->CreatePollygon(this);
	m_pElementSpace->CalculateSpace();

	CreateBackElementSpace();
}
