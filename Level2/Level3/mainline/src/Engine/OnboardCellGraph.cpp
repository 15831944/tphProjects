#include "StdAfx.h"
#include ".\onboardcellgraph.h"
#include "..\InputOnBoard\Deck.h"
#include "OnboardDeckGroundInSim.h"
#include "OnboardCellInSim.h"
#include "OnboardFlightInSim.h"
#include "..\InputOnboard\Door.h"
#include "..\InputOnboard\Seat.h"
#include "OnboardSeatInSim.h"
#include "OnboardDoorInSim.h"
#include "Common\csv_file.h"


OnboardCellGraph::OnboardCellGraph( OnboardFlightInSim *pFlight )
:m_pFlightInSim(pFlight)
,m_bLoadFile(false)
{

}

OnboardCellGraph::~OnboardCellGraph(void)
{
}
void OnboardCellGraph::Reset()
{
	for (int iDeck = 0; iDeck < (int)m_vGroundInSimList.size(); iDeck++)
	{
		delete m_vGroundInSimList[iDeck];
	}
	m_vGroundInSimList.clear();
	m_mapCellPath.clear();

}

void OnboardCellGraph::AddDeckGroundInSim( OnboardDeckGroundInSim* pDeckInSim)
{
	m_vGroundInSimList.push_back(pDeckInSim);
}

void OnboardCellGraph::InitGraph( CDeckManager* pDeckManger )
{
	int nDeckCount = pDeckManger->getCount();
	for (int iDeck = 0; iDeck < nDeckCount; iDeck++)
	{
		CDeck* pDeck = pDeckManger->getItem(iDeck);
		OnboardDeckGroundInSim* pDeckGroundInSim = new OnboardDeckGroundInSim(pDeck);
		m_vGroundInSimList.push_back(pDeckGroundInSim);
		pDeckGroundInSim->BuildDeckGround();
	}
}

OnboardDeckGroundInSim* OnboardCellGraph::GetDeckInSim( CDeck* pDeck )
{
	for (int iDeck = 0; iDeck < (int)m_vGroundInSimList.size(); iDeck++)
	{
		OnboardDeckGroundInSim* pDeckGroundInSim = m_vGroundInSimList[iDeck];

		if (pDeck == pDeckGroundInSim->GetDeck())
		{
			return pDeckGroundInSim;
		}
	}
	return NULL;
}

OnboardDeckGroundInSim* OnboardCellGraph::GetDeckInSim( int nDeck )
{
	ASSERT(nDeck >= 0 && nDeck < (int)m_vGroundInSimList.size());
	if(nDeck >= 0 && nDeck < (int)m_vGroundInSimList.size())
		return m_vGroundInSimList[nDeck];
	
	return NULL;

}
void OnboardCellGraph::setOnboardCellBarrier()
{
	for (int iDeck = 0; iDeck < (int)m_vGroundInSimList.size(); iDeck++)
	{
		OnboardDeckGroundInSim* pDeckGroundInSim = m_vGroundInSimList[iDeck];

		pDeckGroundInSim->initCellHoldMobile();
	}
}

GroundInSim* OnboardCellGraph::GetGroundInSim( CDeck* pDeck )
{
	OnboardDeckGroundInSim* pDeckGroundInSim = GetDeckInSim(pDeck);
	if (pDeckGroundInSim)
	{
		return pDeckGroundInSim->GetGround();
	}
	return NULL;
}

OnboardCellInSim* OnboardCellGraph::GetElementCellInDeck( const OnboardAircraftElementInSim* pElementInSim )
 {
	if(pElementInSim == NULL)
		return NULL;

	if(pElementInSim->GetType() == CDoor::TypeString)
	{
		OnboardDoorInSim *pDoor = (OnboardDoorInSim *)pElementInSim;
		return pDoor->GetEntryCell();

	}
	if(pElementInSim->GetType() == CSeat::TypeString)
	{
		OnboardSeatInSim *pSeat = (OnboardSeatInSim *)pElementInSim;
		if(pSeat && pSeat->GetEntryPoint())
			return pSeat->GetEntryPoint()->GetOnboardCell();
	}



	return NULL;
}

bool OnboardCellGraph::FindPath( const OnboardAircraftElementInSim* pEntryElementInSim, const OnboardAircraftElementInSim* pExitElementInSim, OnboardCellPath& _movePath )
{
	//check the entry and exit is valid
	if(pEntryElementInSim == NULL || pExitElementInSim == NULL)
		return false;

	OnboardCellInSim *pOriginCell =  GetElementCellInDeck(pEntryElementInSim);
	OnboardCellInSim *pDestCell = GetElementCellInDeck(pExitElementInSim);

	if(pOriginCell == NULL || pDestCell == NULL)
	{
		TRACE("'\r\n OnboardFlightInSim::FindPath(), Parameters passed have problem.");
	}

	return FindPath(pOriginCell,pDestCell,_movePath);
}
bool OnboardCellGraph::FindPath( const ARCVector3& sourcePoint, const ARCVector3& destPoint, OnboardCellPath& _movePath )
{
	return false;
}

bool OnboardCellGraph::FindPath( const OnboardAircraftElementInSim* pEntryElementInSim, const ARCVector3& destPoint, OnboardCellPath& _movePath )
{
	return false;
}

bool OnboardCellGraph::FindPath( const ARCVector3& sourcePoint, const OnboardAircraftElementInSim* pExitElementInSim, OnboardCellPath& _movePath )
{
	return false;
}

bool OnboardCellGraph::FindPath( OnboardCellInSim *pOriginCell, OnboardCellInSim *pDestCell, OnboardCellPath& _movePath )
{
//	loadFile();
	
	std::map< std::pair<OnboardCellInSim*, OnboardCellInSim *>, OnboardCellPath >::iterator iterFind = 
		m_mapCellPath.find(std::make_pair(pOriginCell, pDestCell));

	if(m_mapCellPath.end() != iterFind)
	{
		_movePath = (*iterFind).second;
		return true;
	}
	//not find

	pOriginCell->GetDeckGround()->GetPath(pOriginCell,pDestCell,_movePath);

	m_mapCellPath[std::make_pair(pOriginCell, pDestCell)] = _movePath;

	//{
	//	CSVFile csvFile;
	//	csvFile.init(_T("C:\\groundPath.txt"),APPEND);
	//	{
	//
	//		csvFile.writeInt(pOriginCell->getTileIndex());
	//		csvFile.writeInt(pDestCell->getTileIndex());
	//		csvFile.writeInt(_movePath.getCellCount());

	//		for (int nCell = 0; nCell < _movePath.getCellCount(); ++nCell)
	//		{
	//			OnboardCellInSim *pCell = _movePath.getCell(nCell);
	//			csvFile.writeInt(pCell->getTileIndex());
	//		}
	//		csvFile.writeLine();
	//	}
	//	csvFile.closeOut();
	//}
	////get the element ground
	return true;
}

void OnboardCellGraph::loadFile()
{
	if(!m_bLoadFile)
	{
		OnboardCellInSim *pOriginCell = NULL;
		OnboardCellInSim *pDestCell = NULL;

		m_bLoadFile = true;
		//load from file
		CSVFile csvFile;
		csvFile.init(_T("C:\\groundPath.txt"), READ);

		while (!csvFile.isBlank())
		{
			OnboardCellInSim *tmpOriginCell;
			OnboardCellInSim *tmpDestCell;
			OnboardCellPath tmpmovePath;
			//original
			long lIndex;
			csvFile.getInteger(lIndex);
			tmpOriginCell = GetDeckInSim(0)->GetGround()->getCell(lIndex);
			//dest
			csvFile.getInteger(lIndex);
			tmpDestCell = GetDeckInSim(0)->GetGround()->getCell(lIndex);

			//path
			int nCount;
			csvFile.getInteger(nCount);
			for (int nCell = 0; nCell < nCount; ++nCell)
			{
				OnboardCellInSim *tmpCell = NULL;
				csvFile.getInteger(lIndex);
				tmpCell = GetDeckInSim(0)->GetGround()->getCell(lIndex);
				if(tmpCell != NULL)
				{
					tmpmovePath.addCell(tmpCell);
				}
			}
			if(tmpOriginCell != NULL && tmpDestCell != NULL && tmpmovePath.getCellCount() > 0)
				m_mapCellPath[std::make_pair(tmpOriginCell, tmpDestCell)] = tmpmovePath;

			if(!csvFile.getLine())
				break;
		}
		csvFile.closeIn();

	}
}






