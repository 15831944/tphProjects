#include "StdAfx.h"
#include ".\outboundrouteinsimlist.h"
#include "OutboundRouteInSim.h"
#include "../../InputAirside/outboundRouteAssignment.h"
#include "RunwayInSim.h"
#include "StandInSim.h"
#include "../../InputAirside/ALTObjectGroup.h"

COutboundRouteInSimList::COutboundRouteInSimList(void)
: m_pTaxiRes(NULL)
, m_enumOutboundRouteType(OUTBOUNDROUTEASSIGNMENT_SHORTPATHTOSTAND)
{
}

COutboundRouteInSimList::~COutboundRouteInSimList(void)
{
	vector<COutboundRouteInSim*>::iterator iter = m_OutboundRouteInSimList.begin();
	for (; iter!=m_OutboundRouteInSimList.end(); iter++)
	{
		COutboundRouteInSim *pOutboundRoute = *iter;
		delete pOutboundRoute;
	}
	m_OutboundRouteInSimList.clear();
}

BOOL COutboundRouteInSimList::Init(int nPorjID, int nAirPortID, TaxiwayResourceManager * pTaxiwayRes)
{
	m_pTaxiRes = pTaxiwayRes;

	COutboundRouteAssignment outboundRouteAssignment;
	outboundRouteAssignment.ReadData(nAirPortID);

	m_enumOutboundRouteType = outboundRouteAssignment.GetRouteAssignmentStatus();

	int nOutboundRouteCount = outboundRouteAssignment.GetElementCount();
	for (int i=0; i<nOutboundRouteCount; i++)
	{
		COutboundRoute  *pOutboundRoute = outboundRouteAssignment.GetItem(i);

		COutboundRouteInSim *pOutboundRouteInSim = new COutboundRouteInSim(m_pTaxiRes, pOutboundRoute);

		m_OutboundRouteInSimList.push_back(pOutboundRouteInSim);
	}

	return TRUE;
}

OUTBOUNDROUTEASSIGNMENTSTATUS COutboundRouteInSimList::GetOutboundRouteType()
{
	return m_enumOutboundRouteType;
}

BOOL COutboundRouteInSimList::GetOutboundRoute(RunwayExitInSim *pRWExitInSim, StandInSim* pStand, TaxiwayDirectSegList& taxiwayDirectSegList)
{
	ASSERT(pRWExitInSim);
	ASSERT(pStand);


	COutboundRouteInSim *pBestFitOutboundRoute = NULL;
	BOOL bExist = FindBestFitOutBoundRoute(&pBestFitOutboundRoute, pStand, pRWExitInSim);

	//if there isn't route fit
	if (!bExist)
	{
		return FALSE;
	}

	bExist = pBestFitOutboundRoute->GetBranch(pStand, taxiwayDirectSegList);

	//not exist the branch
	if (!bExist)
	{
		return FALSE;
	}

	return TRUE;;
}

struct SortOutboundRoute
{
	bool operator()(COutboundRouteInSim* pFirst, COutboundRouteInSim* pSecond) const
	{
		int nFirstGroupID = pFirst->GetStandGroupID();
		ALTObjectGroup firstStandGroupID;
		firstStandGroupID.ReadData(nFirstGroupID);
		ALTObjectID firstStandGroupName = firstStandGroupID.getName();

		int nSecondGroupID = pSecond->GetStandGroupID();
		ALTObjectGroup secondStandGroupID;
		secondStandGroupID.ReadData(nSecondGroupID);
		ALTObjectID secondStandGroupName = secondStandGroupID.getName();

		//firststandGroupName is more detail than secondStandGroupName
		if (0 < firstStandGroupName.GetIDString().CompareNoCase(secondStandGroupName.GetIDString()))
		{
			return true;
		}

		return false;
	}
};

BOOL COutboundRouteInSimList::FindBestFitOutBoundRoute(COutboundRouteInSim **ppBestFitOutBoundRouteInSim, 
													   StandInSim *pStandInSim, RunwayExitInSim *pRwExitInSim)
{
	ASSERT(pStandInSim != NULL);
	ASSERT(pRwExitInSim != NULL);
	ASSERT(ppBestFitOutBoundRouteInSim != NULL);

	int nRwExitID = pRwExitInSim->GetID();
	Stand *pStand = pStandInSim->GetStandInput();
	ASSERT(pStand);
	ALTObjectID standName = pStand->GetObjectName();
	int nStandID  = pStand->getID();

	//pRWExitInSim->GetTaxiwaySeg()

	vector<COutboundRouteInSim*> newOutboundRouteInSim;
	for (vector<COutboundRouteInSim*>::iterator iter = m_OutboundRouteInSimList.begin(); iter!=m_OutboundRouteInSimList.end(); iter++)
	{
		//if the runwayexitID isn't the same
		COutboundRouteInSim *pOutboundRouteInSim = *iter;
		if (nRwExitID != (*iter)->GetRunwayExitID())
		{
			continue;
		}

		int nStandGroupID = (*iter)->GetStandGroupID();
		ALTObjectGroup standGroup;
		standGroup.ReadData(nStandGroupID);

		ASSERT(standGroup.getType() == ALT_STAND);

		ALTObjectID standGroupName = standGroup.getName();

		//standName is in standGroupName
		if (standGroupName.idFits(standName))
		{
			newOutboundRouteInSim.push_back(*iter);
		}

		//standName is equal with standGroupname
		if (standName.idFits(standGroupName))
		{
			*ppBestFitOutBoundRouteInSim = *iter;

			return TRUE;
		}
	}

	if (0 == (int)newOutboundRouteInSim.size())
	{
		return FALSE;
	}

	if (1 == (int)newOutboundRouteInSim.size())
	{
		*ppBestFitOutBoundRouteInSim = newOutboundRouteInSim.at(0);
		return TRUE;
	}

	std::sort(newOutboundRouteInSim.begin(), newOutboundRouteInSim.end(), SortOutboundRoute());

	*ppBestFitOutBoundRouteInSim = newOutboundRouteInSim.at(0);

	return TRUE;
}
