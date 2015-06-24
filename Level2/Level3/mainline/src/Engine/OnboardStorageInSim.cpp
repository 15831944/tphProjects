#include "StdAfx.h"
#include ".\onboardstorageinsim.h"
#include "InputOnBoard\Storage.h"
#include "InputOnBoard\Deck.h"
#include "PaxOnboardBaseBehavior.h"
#include "visitor.h"
#include "OnboardFlightInSim.h"

OnboardStorageInSim::OnboardStorageInSim(CStorage *pStorage,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pStorage(pStorage)
,m_pEntryPointInSim(NULL)
,m_dCarryonLoad(NULL)
{
	CreatePollygon();
}

OnboardStorageInSim::~OnboardStorageInSim(void)
{
	delete m_pEntryPointInSim;
	m_pEntryPointInSim = NULL;
}

CString OnboardStorageInSim::GetType()const
{
	return m_pStorage->GetType(); 
}

BOOL OnboardStorageInSim::GetPosition( ARCVector3& pos ) const
{
	ASSERT(m_pStorage != NULL);
	if(m_pStorage == NULL || m_pStorage->GetDeck() == NULL)
		return FALSE;
	pos = m_pStorage->GetPosition();	
	pos[VZ] = m_pStorage->GetDeck()->RealAltitude() + m_pStorage->GetDeckHeight();

	return TRUE;
}

void OnboardStorageInSim::CreatePollygon()
{
	ASSERT(m_pStorage);
	CPath2008 path;
	m_pStorage->GetStoragePath(path);
	m_pollygon.init(path.getCount(),path.getPointList());
}

bool OnboardStorageInSim::Contain(const CPoint2008& pt)const
{
	return m_pollygon.contains2(pt);
}

void OnboardStorageInSim::SetEntryPoint(EntryPointInSim* pEntryPointInSim)
{
	ASSERT(pEntryPointInSim);
	m_pEntryPointInSim = pEntryPointInSim;
}

EntryPointInSim* OnboardStorageInSim::GetEntryPoint()
{
	return m_pEntryPointInSim;
}


void OnboardStorageInSim::visitorWriteLog(const ElapsedTime& eTime)
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

void OnboardStorageInSim::addVisitor(PaxVisitor* pVisitor)
{
	ASSERT(pVisitor);
	if (pVisitor)
	{
		m_pVisitorList.push_back(pVisitor);

		m_dCarryonLoad += pVisitor->getOnboardBehavior()->GetVolume(NULL);
	}
}

void OnboardStorageInSim::visitorFlushLog(const ElapsedTime& eTime)
{
	std::vector<PaxVisitor*>::iterator iter = m_pVisitorList.begin();
	for (; iter != m_pVisitorList.end(); ++iter)
	{
		PaxVisitor* pVisitor = *iter;
		pVisitor->flushLog(eTime,true);
	}
}

void OnboardStorageInSim::clearVisitor()
{
	m_pVisitorList.clear();
}

bool OnboardStorageInSim::onboardState()
{
	std::vector<PaxVisitor*>::iterator iter = m_pVisitorList.begin();
	for (; iter != m_pVisitorList.end(); ++iter)
	{
		PaxVisitor* pVisitor = *iter;
		PaxOnboardBaseBehavior* pOnboardBehavior = pVisitor->getOnboardBehavior();
		ASSERT(pOnboardBehavior);
		if (pOnboardBehavior->getState() < EntryOnboard)
		{
			return false;
		}
	}
	return true;
}

GroundInSim* OnboardStorageInSim::GetGround()
{
	return m_pFlightInSim->GetGroundInSim(m_pStorage->GetDeck());
}

ARCVector3 OnboardStorageInSim::GetRandomPoint()const
{
	ARCVector3 centerPt;
	GetPosition(centerPt);
	double xProb, yProb;

	double nRadius = min(m_pStorage->GetLength(),m_pStorage->GetWidth())/2;

	ARCVector3 randomPt;
	do {
		// get 3 random numbers between 0 and 1
		xProb = (double)rand() / RAND_MAX;
		yProb = (double)rand() / RAND_MAX;
	
		// create random point
		randomPt.n[VX] = centerPt.n[VX] + (xProb * nRadius);
		randomPt.n[VY] = centerPt.n[VY] + (yProb * nRadius);
		randomPt.n[VZ] = centerPt.n[VZ];

		// if new point is not within polygon area, repeat
	} while (centerPt.DistanceTo(randomPt) > nRadius);

	return randomPt;
}

int OnboardStorageInSim::GetResourceID() const
{
	if(m_pStorage)
		return m_pStorage->GetID();

	return -1;
	
}

double OnboardStorageInSim::GetAvailableSpace() const
{
	return m_pStorage->getCapcity() - m_dCarryonLoad;
}

void OnboardStorageInSim::CreateEntryPoint()
{
	ARCVector3 centerPt;
	GetPosition(centerPt);

	//direction
	double dRotate = m_pStorage->GetRotation();

	ARCVector3 vecDirect(1,0,0);
	vecDirect.RotateXY(dRotate);
	vecDirect.Normalize();
	vecDirect.SetLength(m_pStorage->GetWidth()/2 + 20);
	
	centerPt += vecDirect;


	OnboardCellInSim *pEntryCell = GetGround()->GetPointCell(CPoint2008(centerPt[VX],centerPt[VY], centerPt[VZ]));
	delete m_pEntryPointInSim;
	m_pEntryPointInSim = new EntryPointInSim(pEntryCell,m_pFlightInSim);
	
}


























