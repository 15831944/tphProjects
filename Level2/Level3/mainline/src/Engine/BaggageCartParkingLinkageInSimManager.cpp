#include "StdAfx.h"
#include ".\baggagecartparkinglinkageinsimmanager.h"
#include "BagCartsParkingSpotInSim.h"
#include "InputAirside\AirsideBagCartParkSpot.h"



//////////////////////////////////////////////////////////////////////////
//BaggageCartParkingLinkageInSim
BaggageCartParkingLinkageInSim::BaggageCartParkingLinkageInSim(BaggageParkingPlace* pSpotLinakge)
:m_pBagSpotLinkage(pSpotLinakge)
{
	if(m_pBagSpotLinkage != NULL)
	{
	  m_procID.FromString(m_pBagSpotLinkage->GetParkingProcessor());
	}
	
}

BaggageCartParkingLinkageInSim::~BaggageCartParkingLinkageInSim()
{

}

void BaggageCartParkingLinkageInSim::Initialize( std::vector<CBagCartsParkingSpotInSim::RefPtr>& vBagSpotInSim )
{
	int nCount = static_cast<int>(vBagSpotInSim.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CBagCartsParkingSpotInSim *pSpotInSim = vBagSpotInSim.at(nItem).get();
		if(pSpotInSim == NULL)
			continue;

		AirsideBagCartParkSpot *pSpot = pSpotInSim->GetBagCartsSpotInput();
		if(pSpot == NULL)
			continue;

		if(pSpot->getName().idFits(m_pBagSpotLinkage->GetBagTrainSpot()))
		{
			m_vBagSpotInSim.push_back(pSpotInSim);
		}
	}
}

const ALTObjectID& BaggageCartParkingLinkageInSim::getTermProcID() const
{
	return m_procID;
}
CBagCartsParkingSpotInSim * BaggageCartParkingLinkageInSim::GetSpotLinked(const ALTObjectID& terminalSpot )
{
	if(m_vBagSpotInSim.size() == 0)
		return NULL;

	if(m_pBagSpotLinkage->GetParkingRelate() == BaggageParkingPlace::Random_Type)
	{
		int nValue = random(m_vBagSpotInSim.size() -1);
		return m_vBagSpotInSim.at(nValue);
	}
	else if (m_pBagSpotLinkage->GetParkingRelate() == BaggageParkingPlace::OneToOne_Type)
	{
		return GetOneToOneSpotInSIm(terminalSpot,m_pBagSpotLinkage->GetBagTrainSpot().idLength() );
	}

	return NULL;
}

CBagCartsParkingSpotInSim *  BaggageCartParkingLinkageInSim::GetOneToOneSpotInSIm( const ALTObjectID&  terminalSpot,int _nDestIdLength )
{

	TRACE("\r\n Terminal Proc: %s \r\n", terminalSpot.GetIDString());
	std::vector<CBagCartsParkingSpotInSim *> existsSpotList = m_vBagSpotInSim;

	int nDeltaFromLeaf = 0;

	std::vector<CBagCartsParkingSpotInSim *>  arrayResult;
	while( existsSpotList.size() > 0 )
	{ 
		int nCount = (int)existsSpotList.size();
		if( nCount == 1 )
		{
			CBagCartsParkingSpotInSim *resultObject = existsSpotList.at( 0 );
			return resultObject;
		}

		std::vector<CBagCartsParkingSpotInSim *> arrayTemp = existsSpotList;
		existsSpotList.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = terminalSpot.idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	

		strcpy(szSourceLevelName,terminalSpot.at( nSourceIdLength ).c_str());

		nCount = (int)arrayTemp.size();
		for( int i=0; i<nCount; i++ )
		{
			CBagCartsParkingSpotInSim *pSpotInSim = arrayTemp.at( i );
			ALTObjectID objID = pSpotInSim->GetBagCartsSpotInput()->getName();

			TRACE("\r\n Parking Spot: %s \r\n", objID.GetIDString());

			int nThisIdLenght = objID.idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= _nDestIdLength )
			{
				arrayResult.push_back( pSpotInSim );
			}
			else
			{
				char szLevelName[128];
				strcpy(szLevelName,objID.at(nThisIdLenght-1 ).c_str());
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					existsSpotList.push_back( pSpotInSim );
				}
			}
		}

		//if( existsSpotList.size() == 0 )
		//{
		//	return NULL;
		//}
		nDeltaFromLeaf++;
	}
	int nCount = (int)arrayResult.size();
	if( nCount == 0 || nCount > 1 )
		return NULL;

	CBagCartsParkingSpotInSim *resultObject = arrayResult.at(0);
	return resultObject;

}

//////////////////////////////////////////////////////////////////////////
//BaggageCartParkingLinkageInSimManager
BaggageCartParkingLinkageInSimManager::BaggageCartParkingLinkageInSimManager(void)
{
}

BaggageCartParkingLinkageInSimManager::~BaggageCartParkingLinkageInSimManager(void)
{
	int nCount = static_cast<int>(m_vBagCartLinkageInSim.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		delete m_vBagCartLinkageInSim[nItem];
	}
	m_vBagCartLinkageInSim.clear();
}

void BaggageCartParkingLinkageInSimManager::Initialize( int nAirportID, std::vector<CBagCartsParkingSpotInSim::RefPtr>& vBagSpotInSim )
{
	m_BaggageCartSpec.ReadData(nAirportID);
	int nCount = m_BaggageCartSpec.GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		BaggageParkingPlace* pSpotLinakge = m_BaggageCartSpec.GetItem(nItem);
		BaggageCartParkingLinkageInSim *pSpotLinkageInSim =	new BaggageCartParkingLinkageInSim(pSpotLinakge);
		pSpotLinkageInSim->Initialize(vBagSpotInSim);
		m_vBagCartLinkageInSim.push_back(pSpotLinkageInSim);
	}
}

CBagCartsParkingSpotInSim * BaggageCartParkingLinkageInSimManager::GetSpotLinked( const ALTObjectID&  terminalSpot )
{
	int nCount = static_cast<int>(m_vBagCartLinkageInSim.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		BaggageCartParkingLinkageInSim *pBagCartSpotLinkage = m_vBagCartLinkageInSim.at(nItem);
		if(pBagCartSpotLinkage == NULL)
			continue;
		//contained by terminal processor
		if (terminalSpot.idFits(pBagCartSpotLinkage->getTermProcID()))
		{
			//get the parking spot in sim
			CBagCartsParkingSpotInSim * pLinkedParkingSpot = pBagCartSpotLinkage->GetSpotLinked(terminalSpot);
			if(pLinkedParkingSpot)
				return pLinkedParkingSpot;

		}
	}

	return NULL;
}	


