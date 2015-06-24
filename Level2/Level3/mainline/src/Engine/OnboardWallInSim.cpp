#include "stdafx.h"
#include "OnboardWallInSim.h"
#include "../InputOnboard/OnBoardWall.h"
#include "GroundInSim.h"
#include "OnboardFlightInSim.h"
#include "../Common/ARCPipe.h"
#include "OnboardElementSpace.h"

OnboardWallInSim::OnboardWallInSim(COnBoardWall *pWall,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pOnboardWall(pWall)
,m_pElementSpace(NULL)
{
	CalculateSpace();
}

OnboardWallInSim::~OnboardWallInSim(void)
{
	ClearSpace();
}

bool OnboardWallInSim::GetLocation(CPoint2008& location)const
{
	CPath2008& path = m_pOnboardWall->getPath();
	if (path.getCount() == 0)	
		return false;
	
	location = path.getPoint(0);
	return true;
}

CString OnboardWallInSim::GetType() const
{
	return m_pOnboardWall->GetType();
}

COnBoardWall* OnboardWallInSim::GetWallInput()const
{
	return m_pOnboardWall;
}

GroundInSim* OnboardWallInSim::GetGround()
{
	return m_pFlightInSim->GetGroundInSim(m_pOnboardWall->GetDeck());
}

void OnboardWallInSim::ClearSpace()
{
	if (m_pElementSpace)
	{
		delete m_pElementSpace;
		m_pElementSpace = NULL;
	}
}

void OnboardWallInSim::CreatePollygon()
{
	CPath2008& path = m_pOnboardWall->getPath();

	ARCPipe arcPath(path,m_pOnboardWall->getWidth());
	CPath2008& sidePath1 = arcPath.m_sidePath1.getPath2008();
	CPath2008& sidePath2 = arcPath.m_sidePath2.getPath2008();

	CPath2008 wallPath;
	wallPath.ConnectPath(sidePath1);
	sidePath2.invertList();
	wallPath.ConnectPath(sidePath2);

	CPollygon2008 pollygon;
	pollygon.init(wallPath.getCount(),wallPath.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}



void OnboardWallInSim::CalculateSpace()
{
	ClearSpace();
	m_pElementSpace = new OnboardElementSpace(GetGround());
	CreatePollygon();
	m_pElementSpace->CalculateSpace();
}

int OnboardWallInSim::GetResourceID() const
{
	if(m_pOnboardWall)
		return m_pOnboardWall->GetID();

	return -1;
}