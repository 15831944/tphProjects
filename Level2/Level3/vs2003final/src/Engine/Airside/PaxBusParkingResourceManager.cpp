#include "StdAfx.h"
#include ".\paxbusparkingresourcemanager.h"
#include "../../InputAirside/AirsidePaxBusParkSpot.h"
#include "../../InputAirside/ALTAirport.h"
#include "AirsidePaxBusParkSpotInSim.h"

CPaxBusParkingResourceManager::CPaxBusParkingResourceManager(void)
{
}

CPaxBusParkingResourceManager::~CPaxBusParkingResourceManager(void)
{
	std::vector<AirsidePaxBusParkLinkageInSim*>::iterator iterLinkage = m_vLinkageInSim.begin();
	for (;iterLinkage != m_vLinkageInSim.end(); ++iterLinkage)
	{
		delete *iterLinkage;
	}

	std::vector<AirsidePaxBusParkSpotInSim *>::iterator iterSpot = m_vParkSpotInSim.begin();
	for (;iterSpot != m_vParkSpotInSim.end(); ++iterSpot)
	{
		delete *iterSpot;
	}
}

void CPaxBusParkingResourceManager::initialize(int nProjID,int nAirportID)
{

	//Init parking spot
	ALTObjectList vPaxBusParkSpot;

	ALTAirport::GetPaxBusParkSpotList(nAirportID,vPaxBusParkSpot);

	//Read linkage with gate
	m_paxBusParkLinkageList.ReadData(-1);
	int nCount = m_paxBusParkLinkageList.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		PaxBusParkingPlace* pParkLinkage = m_paxBusParkLinkageList.GetItem(i);
		AirsidePaxBusParkLinkageInSim* pLinkageInSim = new AirsidePaxBusParkLinkageInSim(pParkLinkage);
		pLinkageInSim->InitDefaultData(vPaxBusParkSpot);
		m_vLinkageInSim.push_back(pLinkageInSim);
	}

	for(int i = 0;i< (int)vPaxBusParkSpot.size(); ++i)
	{
		AirsidePaxBusParkSpot * pParkSpot =(AirsidePaxBusParkSpot*) vPaxBusParkSpot.at(i).get();
		m_vParkSpotInSim.push_back( new AirsidePaxBusParkSpotInSim(pParkSpot));
	}

}
size_t CPaxBusParkingResourceManager::GetPaxBusParkingCount()
{
	return m_vParkSpotInSim.size();
}

AirsidePaxBusParkSpotInSim *CPaxBusParkingResourceManager::GetPaxBusParkingByIndex(size_t nIndex)
{	
	if(nIndex < m_vParkSpotInSim.size())
		return m_vParkSpotInSim.at(nIndex);

	return NULL;
}


AirsidePaxBusParkSpotInSim* CPaxBusParkingResourceManager::GetBestMatch( const CString& strGateID )
{
	ALTObjectID gateID;
	gateID.FromString(strGateID);
	for (int i = 0; i < (int)m_vLinkageInSim.size(); i++)
	{
		AirsidePaxBusParkLinkageInSim* pLinkageInSim = m_vLinkageInSim.at(i);
		ALTObjectID resultObject;
		if (pLinkageInSim->GetBestMatchObject(gateID,resultObject) == true)
		{
			return GetParkSpotInSim(resultObject);
		}
		
	}
	return NULL;
}

AirsidePaxBusParkSpotInSim* CPaxBusParkingResourceManager::GetParkSpotInSim( const ALTObjectID& spotID )
{
	int nCount = (int)m_vParkSpotInSim.size();
	for (int i = 0; i < nCount; i++)
	{	
		AirsidePaxBusParkSpotInSim* pParkSpotInSim = m_vParkSpotInSim[i];
		if (pParkSpotInSim->GetParkSpot()->getName() == spotID)
		{
			return pParkSpotInSim;
		}
	}
	return NULL;
}


AirsidePaxBusParkLinkageInSim::AirsidePaxBusParkLinkageInSim( PaxBusParkingPlace* pParkLinkage )
:m_pParkLinkage(pParkLinkage)
{

}

AirsidePaxBusParkLinkageInSim::~AirsidePaxBusParkLinkageInSim()
{

}

bool AirsidePaxBusParkLinkageInSim::GetOneToOneObject( const ALTObjectID& gateID,ALTObjectID& resultObject )
{
	int nDestIdLength = m_pParkLinkage->GetTerminalSpot().idLength();
	ALTObjectIDList arrayResult;
	ALTObjectIDList arrayCandidated( m_vObjectList );
	int nDeltaFromLeaf = 0;

	while( arrayCandidated.size() > 0 )
	{
		int nCount = (int)arrayCandidated.size();
		if( nCount == 1 )
		{
			resultObject = arrayCandidated.at( 0 );
			return true;
		}

		ALTObjectIDList arrayTemp( arrayCandidated );
		arrayCandidated.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = gateID.idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	// stop on the first level.

		strcpy(szSourceLevelName,gateID.at(nSourceIdLength).c_str());
		nCount = (int)arrayTemp.size();
		for( int i=0; i<nCount; i++ )
		{
			ALTObjectID destObject = arrayTemp.at( i );

			int nThisIdLenght = destObject.idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= nDestIdLength )
			{
				arrayResult.push_back( destObject );
			}
			else
			{
				char szLevelName[128];
				strcpy(szLevelName,destObject.at(nThisIdLenght-1).c_str());
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					arrayCandidated.push_back( destObject );
				}
			}
		}
		nDeltaFromLeaf++;
	}
	int nCount = (int)arrayResult.size();
	if( nCount == 0 )
		return false;

	resultObject = arrayResult.at( random(nCount) );
	return true;
}

bool AirsidePaxBusParkLinkageInSim::GetRandomObject(const ALTObjectID& gateID,ALTObjectID& resultObject )
{
	int nCount = (int)m_vObjectList.size();
	if( nCount == 0 )
		return false;

	resultObject = m_vObjectList.at( random(nCount) );
	return true;
}

bool AirsidePaxBusParkLinkageInSim::GetBestMatchObject( const ALTObjectID& gateID,ALTObjectID& resultObject )
{
	ALTObjectID sourcID;
	sourcID.FromString(m_pParkLinkage->GetGate());
	
	if (gateID.idFits(sourcID) == FALSE)
		return false;

	if (m_pParkLinkage->GetParkingRelate() == PaxBusParkingPlace::OneToOne_Type)
	{
		return GetOneToOneObject(gateID,resultObject);
	}

	return GetRandomObject(gateID,resultObject);
}

void AirsidePaxBusParkLinkageInSim::InitDefaultData( ALTObjectList vPaxBusParkSpot )
{
	for(unsigned i = 0; i < vPaxBusParkSpot.size(); i++)
	{
		const ALTObjectID& parkSpot = vPaxBusParkSpot.at(i)->getName();

		if (parkSpot.idFits(m_pParkLinkage->GetTerminalSpot()) == TRUE)
			m_vObjectList.Add(parkSpot);
	}
}
