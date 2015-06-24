#include "StdAfx.h"
#include ".\airsideresourcemanager.h"
#include "../../InputAirside/InputAirside.h"
#include "AirsideSimConfig.h"
#include "AirsideReportingAreaInSim.h"
#include "../../InputAirside/ALTAirspace.h"
#include "../../InputAirside/AirSector.h"
#include "../../Common/pollygon2008.h"


bool AirsideResourceManager::Init( int nPrjID,CAirportDatabase* pAirportDB, AircraftClassificationManager* pAircraftClassification, const AirsideSimConfig& simconf )
{
	std::vector<int> vAirportID;
	InputAirside:: GetAirportList(nPrjID,vAirportID);	

	for(int i=0;i< (int)vAirportID.size(); ++i)
	{
		AirportResourceManager * pNewAirport = new AirportResourceManager;
		pNewAirport->Init(nPrjID,vAirportID.at(i), pAirportDB, pAircraftClassification, simconf);

		m_vAirportRes.push_back(pNewAirport);
	}

	InitReportingArea(nPrjID, vAirportID.at(0));

	if(!simconf.bOnlySimStand)
		if( !m_AirspaceRes.Init(nPrjID,this) )
			return false;

	ALTAirspace::GetSectorObjList(nPrjID,vSectors);
	return true;
}
AirportResourceManager * AirsideResourceManager::GetAirportResource( int nAirportID )
{

	for(int i=0;i<(int)m_vAirportRes.size();i++)
	{
		AirportResourceManager  *pAirport = m_vAirportRes.at(i);
		if(pAirport->GetAirportInfo().getID() == nAirportID )
			return pAirport;
	}
	return NULL;
}

AirportResourceManager * AirsideResourceManager::GetAirportResource()
{
	if(m_vAirportRes.size())
	{
		return *m_vAirportRes.begin();
	}
	return NULL;
}

AirRouteNetworkInSim * AirsideResourceManager::GetAirspaceResource()
{
	return & m_AirspaceRes;
}

AirsideResourceManager::~AirsideResourceManager()
{
	for(int i=0;i<(int)m_vAirportRes.size();i++)
	{
		delete m_vAirportRes.at(i);
	}

	int nCount = m_vReportingArea.size();
	for (int i =0; i < nCount; i++)
	{
		delete m_vReportingArea.at(i);
	}
	m_vReportingArea.clear();
}

bool AirsideResourceManager::InitReportingArea(int nPrjID, int nAirportID)
{

	ALTObjectList vAreas;
	vAreas.clear();

	ALTAirport::GetReportingAreaList(nAirportID,vAreas);
	int nCount = (int)vAreas.size();
	for(int i = 0;i< nCount; ++i)
	{
		CReportingArea * pArea =(CReportingArea*) vAreas.at(i).get();

		m_vReportingArea.push_back( new AirsideReportingAreaInSim(pArea));
	}

	return true;
}

AirsideReportingAreaInSim* AirsideResourceManager::GetInReportingArea(const CPoint2008& point)
{
	int nCount = m_vReportingArea.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideReportingAreaInSim* pArea = m_vReportingArea.at(i);
		if (pArea->IsInReportingArea(point))
		{
			return pArea;
		}
	}

	return NULL;
}

CString AirsideResourceManager::GetInSectorNameAndID(const CPoint2008& point,double dFltAlt, int& nSectorID)
{
	int nCount =  vSectors.size();
	for (int i = 0; i < nCount; ++i )
	{
		AirSector* pSector = (AirSector*)vSectors[i].get();

		double dLowAltitude = pSector->GetLowAltitude();
		double dHeighAltitude = pSector->GetHighAltitude();

		//check if altitude is invalid
		if (dFltAlt-dLowAltitude <= 0.000001 && dFltAlt-dHeighAltitude >= 0.000001)
			continue;

		//check if flight is in bound
		CPath2008 sectorPath = pSector->GetPath();
		CPath2008 meterUnitPath;
		meterUnitPath.init(sectorPath.getCount());
		CPoint2008* pointList = sectorPath.getPointList();
		for (int i = 0; i < sectorPath.getCount(); i++)
		{
			CPoint2008 _point = pointList[i];
			_point.setX(_point.getX()/100);
			_point.setY(_point.getY()/100);
			meterUnitPath[i] = _point;
		}

		CPollygon2008 sectorPollygon;
		sectorPollygon.init(meterUnitPath.getCount(), meterUnitPath.getPointList());
		CPoint2008 testPoint;
		testPoint.setX(point.getX()/100);
		testPoint.setY(point.getY()/100);
		testPoint.setZ(0);

		if (sectorPollygon.contains(testPoint))
		{
			nSectorID = pSector->getID();
			return pSector->GetObjNameString();
		}


	}
	nSectorID = -1;
	return "All";
}