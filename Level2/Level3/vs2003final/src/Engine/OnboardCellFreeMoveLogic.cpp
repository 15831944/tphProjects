#include "StdAfx.h"
#include ".\onboardcellfreemovelogic.h"
#include "PaxOnboardBaseBehavior.h"
#include "OnboardPaxCellStepEvent.h"
#include "person.h"
#include "OnboardDeckGroundInSim.h"
#include "OnboardFlightInSim.h"
#include "OnboardPaxFreeMovingConflict.h"
#include "OnboardDoorInSim.h"
#include "OnboardCellGraph.h"



OnboardCellFreeMoveLogic::OnboardCellFreeMoveLogic(PaxOnboardBaseBehavior *pBehavior )
:m_pBehavior(pBehavior)
,m_pFlight(NULL)
,m_pOrigin(NULL)
,m_pDest(NULL)
,m_nEndPaxState(NULL)
// ,m_dCurDirection(0.0)
,m_curState(LogicMove)
,m_bIsOutOfDoor(true)
,m_bNeedToWriteLog(false)
{
	mbSideWalk = false;
}
OnboardCellFreeMoveLogic::~OnboardCellFreeMoveLogic(void)
{
}

OnboardFlightInSim * OnboardCellFreeMoveLogic::getFlight()
{
	if(m_pFlight)
		return m_pFlight;

	if(m_pBehavior)
	{
		m_pFlight = m_pBehavior->GetOnBoardFlight();
	}
	ASSERT(m_pFlight != NULL);
	return m_pFlight;
}

void OnboardCellFreeMoveLogic::StartMove(const ElapsedTime& eTime)
{
	m_eCurrentTime = eTime;
	m_eLastMoveTime = eTime;
	m_curState = Moving;

	m_pBehavior->setState(OnboardFreeMoving);

	//have no path assigned
	if(m_cellPath.getCellCount() == 0)
	{
		m_curLocation.PaxClear(m_pBehavior, eTime);
		BackToFlow(eTime);
		return;
	}
// 	for (int nTmpCell = 0; nTmpCell < m_cellPath.getCellCount(); ++ nTmpCell)
// 	{
// 		TRACE(_T("\r\n cellPath:paxID %d,   %d"), m_pBehavior->m_pPerson->getID(),
// 			m_cellPath.getCell(nTmpCell)->getTileIndex());
// 	}


	//check situation
	ASSERT(m_nEndPaxState >= 0);

	//the current location is the first cell, 
	//m_pOrigin
	m_curLocation.setPathCell(m_pOrigin);
	if(!m_curLocation.Contains(m_pOrigin))
	{//need to calculate the passenger's location

		//get the current location
		m_curLocation.setCellCenter(m_pOrigin);
		m_pOrigin->PaxOccupy(m_pBehavior);//add take the cell
	}

	//generate step event
	OnboardPaxCellStepEvent *pEvent =  new OnboardPaxCellStepEvent(this,eTime);
	pEvent->addEvent();
}

static bool bDelay = true;

//when the event arrival, process it
int OnboardCellFreeMoveLogic::Step(ElapsedTime& eTime)
{
	m_eCurrentTime = eTime;

	//write log at the current location
	m_curState = Moving;

	if(m_curLocation.getPathCell())
		m_pBehavior->m_lCellIndex = m_curLocation.getPathCell()->getTileIndex();

	int nPaxID = m_pBehavior->m_pPerson->getID();

	if(m_pBehavior->m_pPerson->getID() == 92)
	{
		TRACE(_T("\r\n x: %f, y: %f, z: %f"),m_curLocation.getCellCenter()->getLocation().getX(),
			m_curLocation.getCellCenter()->getLocation().getY(),
			m_curLocation.getCellCenter()->getLocation().getZ());

	}



	if(m_curLocation.Contains(m_pDest))
	{	
		WriteLog(m_curLocation.getCellCenter(),eTime);
		//arrive at destination
		//back to flow
		BackToFlow(eTime);
		return TRUE;
	}
	//if(bDelay)
	//{
	//	bDelay = false;

	//	OnboardPaxCellStepEvent *pEvent =  new OnboardPaxCellStepEvent(this,eTime + ElapsedTime(60L));
	//	pEvent->addEvent();
	//	return TRUE;
	//}

	OnboardCellInSim *pCurCell = m_curLocation.getCellCenter();
	//need to move on
	//get next location
	OnboardCellInSim *pNextCell = m_cellPath.GetNextCell(m_curLocation.getPathCell());
	if(pNextCell)
	{
		//calculate time
		OnboardCellInSim *pCurCell = m_curLocation.getCellCenter();
		ElapsedTime moveTime = ElapsedTime(0L);

		//OnboardCellInSim *pNearestCellOccupied = NULL;
		//set location
		PaxCellLocation curLocation = m_curLocation;
//		PaxCellLocation nextLocation = m_nextLocation;

		std::vector<PaxCellLocation > vNextLocation;

		int nCellCountPerStep = 5;
		for (int nStepCell = 0; nStepCell < 5; ++ nStepCell)
		{
			if(curLocation.Contains(m_pDest))
				break;
			
			pNextCell = m_cellPath.GetNextCell(curLocation.getPathCell());
			
			PaxCellLocation tmpPaxLocation;
			GetPaxLocation(curLocation.getPathCell(),pNextCell, tmpPaxLocation);

			vNextLocation.push_back(tmpPaxLocation);
			OnboardPaxFreeMovingConflict conflict(m_pBehavior);//conflict 
			Person* bConflict
				= CheckLocationConflict(getGroundInSim(pNextCell),tmpPaxLocation, curLocation.getCellCenter(),conflict);
			if(!bConflict) //no conflict
			{	
				if(pCurCell && tmpPaxLocation.getCellCenter())
				{
					double dLen = pCurCell->GetDistanceTo(tmpPaxLocation.getCellCenter());
					moveTime += ElapsedTime(dLen/getSpeed());
					//move to next point
					pCurCell = tmpPaxLocation.getCellCenter();
				}
				curLocation = tmpPaxLocation;
				m_bNeedToWriteLog = true;

				continue;
			}
			else if(nStepCell < 4)// handle the conflict with that passenger
			{//if the passenger cannot move forward
				conflict.m_eCurrentTime = eTime;
				conflict.HandleConflict(curLocation ,tmpPaxLocation);

				m_pBehavior->SetWaitPax(bConflict->getOnboardBehavior());

				return FALSE;
			}
			else//has a conflict, 
				break;
		}
		m_pBehavior->SetWaitPax(NULL);

		//if the passenger could move, write the log
 		if(m_bNeedToWriteLog)
			m_pBehavior->writeLog(eTime,false);
 			//WriteLog(m_curLocation.getCellCenter(),eTime);
 		m_bNeedToWriteLog = false;

	//	m_pBehavior->writeLog(eTime,false);

		ElapsedTime eNextTime = eTime + moveTime;
		m_eLastMoveTime = eNextTime;

		//move to next position
		m_curLocation.PaxClear(m_pBehavior,eNextTime);
		//only move to this cell
		//ASSERT(vNextLocation.size() > 1);
		if(vNextLocation.size() > 1)
		{
			setCurLocation(vNextLocation[vNextLocation.size() -2]);
			//m_curLocation = vNextLocation[vNextLocation.size() -2];
			//the next cell location
			m_nextLocation = vNextLocation[vNextLocation.size() -1];

		}
		else
		{
			setCurLocation(curLocation);
			//the next cell location
			m_nextLocation = curLocation;
		}

		m_curLocation.PaxOccupy(m_pBehavior);
		WriteLog(m_curLocation.getCellCenter(),eNextTime);

		if(moveTime <= ElapsedTime(0L))
		{
			ASSERT(0);
			moveTime =ElapsedTime(1L);
		}
		//wake up the passenger at door
		//if(!m_bIsOutOfDoor)
			m_bIsOutOfDoor = m_pBehavior->m_pDoor->CheckDoorClear(m_pBehavior,eNextTime);
		
		//wake up the waiting passenger
		//WakeupWaitingPax(m_pBehavior,eNextTime);

		//generate next event
		//OnboardPaxCellStepEvent *pEvent =  new OnboardPaxCellStepEvent(this,eNextTime);
		//pEvent->addEvent();
		GenerateStepEvent(eNextTime);
		
		return TRUE;
	}	
	else
	{
		//error, cannot find the next location

		//when come to this
		//it will come back to Flow
		//and clean the logic temporary data
		BackToFlow(eTime);
	}

	return TRUE;
}
void OnboardCellFreeMoveLogic::WakeupWaitingPax( PaxOnboardBaseBehavior *pBehavior ,ElapsedTime& etime )
{
	if(pBehavior == NULL)
		return;

	//wait up the waiting passengers
	std::vector< PaxOnboardBaseBehavior *>::iterator iterPax = pBehavior->m_vWaitingPax.begin();
	for (; iterPax != pBehavior->m_vWaitingPax.end(); ++iterPax)
	{
		if(*iterPax)
		{
			OnboardPaxCellStepEvent *pEvent =  new OnboardPaxCellStepEvent((*iterPax)->m_pCellFreeMoveLogic,etime);
			pEvent->addEvent();
		}
	}
	pBehavior->m_vWaitingPax.clear();
}


//pCell, the cell where the passenger locate at now
//pNextCell, the cell where the passenger would move to
bool OnboardCellFreeMoveLogic::GetPaxLocation( OnboardCellInSim *pCurPathCell, OnboardCellInSim *pNextPathCell, PaxCellLocation& paxLocation) const
{
	if(pNextPathCell)
	{
		TRACE(_T("\r\n curCell:paxID %d,   %d"), m_pBehavior->m_pPerson->getID(),
			pNextPathCell->getTileIndex());
	}



	GroundInSim *pGround = getGroundInSim(pCurPathCell);

	 if(pGround == NULL)
	 {	
		paxLocation.setCellCenter(pNextPathCell);
		return false;
	 }


	paxLocation.setCellCenter(pNextPathCell);
	paxLocation.setPathCell(pNextPathCell);

	//this cell is used to determine the passenger's direction
	OnboardCellInSim *pDirCell = m_cellPath.GetNextCell(pNextPathCell);
	//passenger's figure
	//20* 40
	//				20
	//(-10,20,Z)|-------|(10,20,Z)
	//			| \	   /|
	//			|  \  / |
	//		40	|	\/  |---> face direction
	//			|0,0/\	|
	//			|  /  \ |
	//			| /    \|
//   (-10,-20,Z)|-------|(10,-20,Z)

	//double PaxOnboardBaseBehavior::PAXLENGTH = 20.0;
	//double PaxOnboardBaseBehavior::PAXWIDTH = 40.0;
	if(pDirCell == NULL)
		pDirCell = pCurPathCell;
	if(pDirCell)
	{
		//the passenger has more cell to move
		//
		//get passenger's location
		//
		double zPos = pCurPathCell->getLocation().getZ();
		std::vector<CPoint2008> vPaxSpace;

		CPoint2008 ptAngle = CPoint2008(pDirCell->getLocation() - pNextPathCell->getLocation());
		ptAngle.Normalize();
		
		CPoint2008 ptFront = ptAngle;
		ptFront.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

		CPoint2008 ptLeft  = ptAngle.perpendicular();
		ptLeft.length(PaxOnboardBaseBehavior::PAXWIDTH/2);

		CPoint2008 ptBack  = ptAngle * (-1);
		ptBack.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

		CPoint2008 ptRight = ptBack.perpendicular();
		ptRight.length(PaxOnboardBaseBehavior::PAXWIDTH/2);



		CPoint2008 ptLeftFront = ptFront + ptLeft;
		CPoint2008 ptLeftBack  = ptLeft + ptBack;
		CPoint2008 ptRightBack = ptBack + ptRight;
		CPoint2008 ptRightFront = ptRight + ptFront;


		//vPaxSpace.push_back(CPoint2008(0.5 * PaxOnboardBaseBehavior::PAXLENGTH, 0.5 * PaxOnboardBaseBehavior::PAXWIDTH, zPos));
		//vPaxSpace.push_back(CPoint2008(-0.5 *PaxOnboardBaseBehavior::PAXLENGTH,0.5 *PaxOnboardBaseBehavior::PAXWIDTH, zPos));
		//vPaxSpace.push_back(CPoint2008(-0.5 *PaxOnboardBaseBehavior::PAXLENGTH,-0.5 *PaxOnboardBaseBehavior::PAXWIDTH,zPos));
		//vPaxSpace.push_back(CPoint2008(0.5 * PaxOnboardBaseBehavior::PAXLENGTH,-0.5 *PaxOnboardBaseBehavior::PAXWIDTH,zPos));
		
		vPaxSpace.push_back(ptLeftFront);
		vPaxSpace.push_back(ptLeftBack);
		vPaxSpace.push_back(ptRightBack);
		vPaxSpace.push_back(ptRightFront);


		CPath2008 paxSpace;
		paxSpace.init(4,&vPaxSpace[0]);

		////rotate
		////get the angel, the direction

		//paxSpace.Rotate(ptAngle.getHeading());
		//

		//move to the cell
		paxSpace.DoOffset(pNextPathCell->getLocation().getX(),
			pNextPathCell->getLocation().getY(),
			0);



		//the passenger Center
		CPoint2008 ptPaxCenter = pNextPathCell->getLocation();
		//get all the cells in the Regine

		//start cell

		OnboardCellInSim* pStartCell = pGround->GetPointCell(paxSpace[0]);
		OnboardCellInSim* pEndCell = pGround->GetPointCell(paxSpace[3]);
		if(pStartCell == NULL || !pStartCell->IsValid())
		{
			//off set the center
			//get the point offset the direction, 
			//the offset length cannot larger than0.5*Passenger Width

			CPath2008  tmpPaxSpace = paxSpace;
			CPoint2008 tmpNewCenter=	ptPaxCenter;
			OnboardCellInSim* tmpStartCell = pStartCell;
			OnboardCellInSim* tmpEndCell = pEndCell;
			for (double dOffset = GRID_WIDTH; dOffset < 0.5 *PaxOnboardBaseBehavior::PAXWIDTH+ GRID_WIDTH; dOffset+=GRID_WIDTH )
			{
				tmpPaxSpace = paxSpace;

				CPoint2008 ptPerpendicular = ptAngle.perpendicular()*(-1)/*ptAngle*/;
// 				ptPerpendicular.rotate(-45.0);
// 				ptPerpendicular.Normalize();
				ptPerpendicular.length(dOffset);

				tmpNewCenter = pNextPathCell->getLocation();
				tmpNewCenter += ptPerpendicular;

				//get the regain
				tmpPaxSpace.DoOffset(ptPerpendicular.getX(), ptPerpendicular.getY(),0);

				tmpStartCell = pGround->GetPointCell(tmpPaxSpace[0]);
				tmpEndCell = pGround->GetPointCell(tmpPaxSpace[3]);				
				if(tmpStartCell != NULL && tmpStartCell->IsValid()
					&&tmpEndCell != NULL && tmpEndCell->IsValid()
					&& pGround->GetPointCell(tmpNewCenter))
				{//the cell exists and no furnish
					ptPaxCenter = tmpNewCenter;
					paxSpace = tmpPaxSpace;
					pStartCell = tmpStartCell;
					pEndCell = tmpEndCell;
					break;

				}
			 }
         }

		//check end cell
		if(pEndCell == NULL || !pEndCell->IsValid())
		{
			//off set the center
			//get the point offset the direction, 
			//the offset length cannot larger than0.5*Passenger Width

			CPath2008  tmpPaxSpace = paxSpace;
			CPoint2008 tmpNewCenter=	ptPaxCenter;
			OnboardCellInSim* tmpStartCell = pStartCell;
			OnboardCellInSim* tmpEndCell = pEndCell;

			for (double dOffset = GRID_WIDTH; dOffset < 0.5 *PaxOnboardBaseBehavior::PAXWIDTH + GRID_WIDTH; dOffset+=GRID_WIDTH )
			{
				tmpPaxSpace = paxSpace;

				CPoint2008 ptPerpendicular = ptAngle.perpendicular()/*ptAngle*/;
// 				ptPerpendicular.rotate(45.0);
// 				ptPerpendicular.Normalize();
				ptPerpendicular.length(dOffset);

				tmpNewCenter = pNextPathCell->getLocation();
				tmpNewCenter += ptPerpendicular;

				//get the regain
				tmpPaxSpace.DoOffset(ptPerpendicular.getX(), ptPerpendicular.getY(),0);

				tmpStartCell = pGround->GetPointCell(tmpPaxSpace[0]);
				tmpEndCell = pGround->GetPointCell(tmpPaxSpace[3]);				
				if(tmpStartCell != NULL && tmpStartCell->IsValid()
					&&tmpEndCell != NULL && tmpEndCell->IsValid()
					&& pGround->GetPointCell(tmpNewCenter))
				{//the cell exists and no furnish
					ptPaxCenter = tmpNewCenter;
					paxSpace = tmpPaxSpace;
					pStartCell = tmpStartCell;
					pEndCell = tmpEndCell;
					break;

				}
			 }
         }
		OnboardCellInSim *pCenterCell = pGround->getCell(ptPaxCenter);

		if(pStartCell == NULL)
			pStartCell = pGround->getCell(paxSpace[0]);

		if(pEndCell == NULL)
			pEndCell = pGround->getCell(paxSpace[3]);

		//start
		int nStartCell = -1;
		if(pStartCell)
			nStartCell = pStartCell->getTileIndex();
		
		//right top cell
		int nRightTopCell = -1;
		OnboardCellInSim* pRightTopCell = pGround->getCell(paxSpace[1]);
		if(pRightTopCell)
			nRightTopCell= pRightTopCell->getTileIndex();

		//left bottom cell
		int nLeftBottomCell = -1;
		OnboardCellInSim* pLeftBottomCell = pGround->getCell(paxSpace[2]);
		if(pLeftBottomCell)
			nLeftBottomCell= pLeftBottomCell->getTileIndex();
		//end
		int nEndCell = -1;
		if(pEndCell)
			nEndCell = pEndCell->getTileIndex();



		if(nStartCell >= 0 &&
			nRightTopCell >=0 &&
			nLeftBottomCell >=0 &&
			nEndCell >= 0
			)
		{

			//find cells at the direction
			//front
			std::vector<OnboardCellInSim *> vCellsOccupied;			
			std::vector<OnboardCellInSim *> vCellsNeedToCheck;
			std::vector<OnboardCellInSim *> vCellsChecked;
			
			CPollygon2008 paxPollygon;
			paxPollygon.init(paxSpace.getCount(),paxSpace.getPointList());
			double dDistanceMax = sqrt((0.5 * PaxOnboardBaseBehavior::PAXLENGTH) * (0.5 * PaxOnboardBaseBehavior::PAXLENGTH) + (0.5 * PaxOnboardBaseBehavior::PAXWIDTH) *(0.5 *PaxOnboardBaseBehavior::PAXWIDTH));
			{
				//center cell
				vCellsOccupied.push_back(pCenterCell);
				
				//get all neighborhood cells
				vCellsNeedToCheck = pGround->GetNeighbourCells(pCenterCell);

				while (vCellsNeedToCheck.size() > 0)
				{	
					std::vector<OnboardCellInSim *> vNeighbourCells;

					std::vector<OnboardCellInSim *>::iterator iterCheck = vCellsNeedToCheck.begin();
					for (; iterCheck != vCellsNeedToCheck.end(); ++iterCheck)
					{
						//check the cells
						OnboardCellInSim *pTmpCell = *iterCheck;
						if(pTmpCell)
						{
							if(std::find(vCellsChecked.begin(), vCellsChecked.end(), pTmpCell) != vCellsChecked.end())
								continue;

							vCellsChecked.push_back(pTmpCell);
													
							if(paxPollygon.contains2( pTmpCell->getLocation()))
							{
								paxLocation.addCell(pTmpCell);
								std::vector<OnboardCellInSim *> tempCells = pGround->GetNeighbourCells(pTmpCell);
								vNeighbourCells.insert(vNeighbourCells.end(), tempCells.begin(), tempCells.end());
							}
						}
					}
					vCellsNeedToCheck = vNeighbourCells;
				}
			}

			//center cell 
			paxLocation.setCellCenter(pCenterCell);
			//set cell in the path
			paxLocation.setPathCell(pNextPathCell);
			//passenger position rect
			paxLocation.setPaxRect(paxSpace);

		}
		else
		{
			//cannot be here
			//ASSERT(0);
			return false;
		}
	}

	return true;
}

bool OnboardCellFreeMoveLogic::GetPaxLocation( CPoint2008 ptLocation, CPoint2008 ptDirection, PaxCellLocation & paxLocation, GroundInSim *pGroundInSim) const
{
	//the passenger has more cell to move
	//
	GroundInSim *pGround = pGroundInSim;
	
	if(pGround == NULL)
		pGround = getGroundInSim(NULL);
	//get passenger's location
	//
//	double PaxOnboardBaseBehavior::PAXLENGTH = 20.0;
//	double PaxOnboardBaseBehavior::PAXWIDTH = 40.0;

	double zPos = ptLocation.getZ();
	std::vector<CPoint2008> vPaxSpace;

	CPoint2008 ptAngle =  ptDirection;
	ptAngle.Normalize();

	CPoint2008 ptFront = ptAngle;
	ptFront.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

	CPoint2008 ptLeft  = ptAngle.perpendicular();
	ptLeft.length(PaxOnboardBaseBehavior::PAXWIDTH/2);

	CPoint2008 ptBack  = ptAngle * (-1);
	ptBack.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

	CPoint2008 ptRight = ptBack.perpendicular();
	ptRight.length(PaxOnboardBaseBehavior::PAXWIDTH/2);

	//get the passenger' rect
	CPoint2008 ptLeftFront = ptFront + ptLeft;
	CPoint2008 ptLeftBack  = ptLeft + ptBack;
	CPoint2008 ptRightBack = ptBack + ptRight;
	CPoint2008 ptRightFront = ptRight + ptFront;

	vPaxSpace.push_back(ptLeftFront);
	vPaxSpace.push_back(ptLeftBack);
	vPaxSpace.push_back(ptRightBack);
	vPaxSpace.push_back(ptRightFront);


	CPath2008 paxSpace;
	paxSpace.init(4,&vPaxSpace[0]);

	//move to the cell
	paxSpace.DoOffset(ptLocation.getX(), ptLocation.getY(),0);

	//check the Rect is valid
	OnboardCellInSim *pStartCell = pGround->getCell(paxSpace[0]);
	OnboardCellInSim *pEndCell = pGround->getCell(paxSpace[3]);

	//center cell
	OnboardCellInSim *pCenterCell = pGround->getCell(ptLocation);

	//start
	int nStartCell = -1;
	if(pStartCell)
		nStartCell = pStartCell->getTileIndex();

	//right top cell
	int nRightTopCell = -1;
	OnboardCellInSim* pRightTopCell = pGround->getCell(paxSpace[1]);
	if(pRightTopCell)
		nRightTopCell= pRightTopCell->getTileIndex();

	//left bottom cell
	int nLeftBottomCell = -1;
	OnboardCellInSim* pLeftBottomCell = pGround->getCell(paxSpace[2]);
	if(pLeftBottomCell)
		nLeftBottomCell= pLeftBottomCell->getTileIndex();

	//end
	int nEndCell = -1;
	if(pEndCell)
		nEndCell = pEndCell->getTileIndex();

	if(nStartCell >= 0 &&
		nRightTopCell >=0 &&
		nLeftBottomCell >=0 &&
		nEndCell >= 0
		)
	{
		// return this passenger location
		std::vector<OnboardCellInSim *> vCellsOccupied;			
		std::vector<OnboardCellInSim *> vCellsNeedToCheck;
		std::vector<OnboardCellInSim *> vCellsChecked;

		CPollygon2008 paxPollygon;
		paxPollygon.init(paxSpace.getCount(),paxSpace.getPointList());
		double dDistanceMax = sqrt((0.5 * PaxOnboardBaseBehavior::PAXLENGTH) * (0.5 * PaxOnboardBaseBehavior::PAXLENGTH) + (0.5 * PaxOnboardBaseBehavior::PAXWIDTH) *(0.5 *PaxOnboardBaseBehavior::PAXWIDTH));
		{
			//center cell
			vCellsOccupied.push_back(pCenterCell);

			//get all neighborhood cells
			vCellsNeedToCheck = pGround->GetNeighbourCells(pCenterCell);

			while (vCellsNeedToCheck.size() > 0)
			{	
				std::vector<OnboardCellInSim *> vNeighbourCells;

				std::vector<OnboardCellInSim *>::iterator iterCheck = vCellsNeedToCheck.begin();
				for (; iterCheck != vCellsNeedToCheck.end(); ++iterCheck)
				{
					//check the cells
					OnboardCellInSim *pTmpCell = *iterCheck;
					if(pTmpCell)
					{
						if(std::find(vCellsChecked.begin(), vCellsChecked.end(), pTmpCell) != vCellsChecked.end())
							continue;

						vCellsChecked.push_back(pTmpCell);

						if(paxPollygon.contains2( pTmpCell->getLocation()))
						{
							paxLocation.addCell(pTmpCell);
							std::vector<OnboardCellInSim *> tempCells = pGround->GetNeighbourCells(pTmpCell);
							vNeighbourCells.insert(vNeighbourCells.end(), tempCells.begin(), tempCells.end());
						}
					}
				}
				vCellsNeedToCheck = vNeighbourCells;
			}
		}

		paxLocation.setCellCenter(pCenterCell);

		//set path 
		paxLocation.setPathCell(pCenterCell);

		//passenger position rect
		paxLocation.setPaxRect(paxSpace);

		return true;

	}
	else
		return false;
}




PaxCellLocation OnboardCellFreeMoveLogic::SearchSpace(GroundInSim *pGround, int iStartCell,int iRightTopCell,int iLeftBottomCell,int iEndCell ) const
{
	PaxCellLocation paxLocation;
	//calculate min/max col and row
	RectMap& _Gndmap = pGround->getLogicMap();
	int nCount = _Gndmap.mCols;
	int iStart = min(iStartCell,iLeftBottomCell);

	int nRowCount = ABS(iStartCell-iLeftBottomCell)/nCount + 1;
	int nColCount = ABS(iRightTopCell - iStartCell) + 1;

	for (int nRow = 0; nRow < nRowCount; nRow++)
	{
		for (int nCol = 0; nCol < nColCount; nCol++)
		{
			int titleIndex = nCol + iStart + nRow*nCount;
			OnboardCellInSim* pCellInSim = pGround->getCell(titleIndex);
			paxLocation.addCell(pCellInSim);
		}
	}
	return paxLocation;

}

void OnboardCellFreeMoveLogic::setCellPath( OnboardCellPath& cellPath )
{
	m_cellPath = cellPath;
	
	if(cellPath.getCellCount() > 0)
	{
		m_pOrigin = cellPath.getCell(0);
		m_pDest = cellPath.getCell(cellPath.getCellCount() -1 );

	}

}
double OnboardCellFreeMoveLogic::getSpeed() const
{
	ASSERT(m_pBehavior != NULL);
	ASSERT(m_pBehavior->m_pPerson != NULL);
	if(m_pBehavior && m_pBehavior->m_pPerson)
		return m_pBehavior->m_pPerson->getSpeed();
	return 100.0;
}
void OnboardCellFreeMoveLogic::WriteLog( OnboardCellInSim *pCell, const ElapsedTime& eTime )
{
	if(m_pBehavior && pCell)
	{

	//	m_pBehavior->setLocation(pCell->getLocation());
		m_pBehavior->setDestination(pCell->getLocation());
		m_pBehavior->writeLog(eTime,false);		

	}
}

void OnboardCellFreeMoveLogic::Clean()
{
	m_curState = LogicMove;
	m_cellPath.Clear();
	m_curLocation.Clear();
	m_nextLocation.Clear();
	m_pOrigin = NULL;
	m_pDest = NULL;
	m_nEndPaxState = -1;

}

void OnboardCellFreeMoveLogic::BackToFlow(ElapsedTime eTime)
{
	Person *pPerson = m_pBehavior->m_pPerson;
	ASSERT(pPerson != NULL);
	if(pPerson)
	{
		m_pBehavior->m_curLocation = m_curLocation;
		pPerson->setState(m_nEndPaxState);
		m_pBehavior->GenerateEvent(eTime);
	}

	//clean the logic
	Clean();
}
bool OnboardCellFreeMoveLogic::CheckLocationAvailable( PaxCellLocation& paxLocation )
{
	return false;
}

Person* OnboardCellFreeMoveLogic::CheckLocationConflict( GroundInSim *pGround,
																		PaxCellLocation& paxLocation ,
																		OnboardCellInSim *pCurCenterCell,
																		OnboardPaxFreeMovingConflict& conflict) const
{
	if(pCurCenterCell == NULL || paxLocation.getCellCount() == 0)
		return false;

	//get passenger ahead
	std::vector<Person*> paxOnFlight;
	m_pBehavior->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);

	//get the nearest passenger at the direction
	//current passenger's direction
	CPoint2008 vecDirection = CPoint2008(paxLocation.getCellCenter()->getLocation() - pCurCenterCell->getLocation());
    
	Person *pNearestPerson = NULL;
	double dNearestDistance = 1000.0;

	std::vector<PaxOnboardBaseBehavior *> vPaxNeedConcern;
	for(int i=0;i<(int)paxOnFlight.size();i++)
	{
		Person* pPax = paxOnFlight.at(i);
		if(pPax==m_pBehavior->m_pPerson)
			continue;

		ARCVector3 concernlocation;
		if( pPax &&  pPax->getOnboardPoint(concernlocation) )
		{
			//if(pPax->getState() == SitOnSeat)
			//	continue;
			//if(pPax->getState() == ArriveAtSeat)
			//	continue;

			if(paxLocation.getCellCenter() == NULL)
				continue;

			if(pPax->getOnboardBehavior() == NULL)
				continue;

			OnboardCellInSim *pCellCenern = pPax->getOnboardBehavior()->getCurLocation().getCellCenter();
			if(pCellCenern == NULL)
				continue;

			CPoint2008 ptConcern = pCellCenern->getLocation();

			//check the passenger is under concern
			//CPoint2008 ptConcern = CPoint2008(concernlocation[VX], concernlocation[VY], concernlocation[VZ]);

			double dDistance = ptConcern.getDistanceTo(paxLocation.getCellCenter()->getLocation());
			if(dDistance > 100)//2 meters
				continue;

			
			//the pax's direction relative passenger
			CPoint2008 vecConcernPax = CPoint2008(ptConcern - pCurCenterCell->getLocation());/*paxLocation.getCellCenter()->getLocation()*/;
			
			double dAngle = vecDirection.GetCosOfTwoVector(vecConcernPax);
			if(dAngle < cos(ARCMath::DegreesToRadians(80.0))) //(+-80)degree
				continue;//out of concern

			// -80 << 0 << 80 degree
			//get direction
			OnboardPaxFreeMovingConflict::ConflictPax insightPax;
			insightPax.m_dAngle = dAngle;
			insightPax.m_dDistance = dDistance;
			insightPax.m_pPax = pPax->getOnboardBehavior();
			conflict.m_vInsightPax.push_back(insightPax);

			if(!m_pBehavior->bCanWaitFor(pPax->getOnboardBehavior()))
				continue;

			if(dDistance< dNearestDistance)
			{
				pNearestPerson = pPax;
				dNearestDistance = dDistance;
			}			
        }
	}
	if(pNearestPerson == NULL)
		return NULL; //have no conflict
	
	if(dNearestDistance < 60)
	{
		return pNearestPerson;
	}
	else
		return NULL;	

}

GroundInSim * OnboardCellFreeMoveLogic::getGroundInSim(OnboardCellInSim *pCell) const
{
	//ASSERT(m_pFlight != NULL);
	if(pCell == NULL)
		pCell = m_curLocation.getCellCenter();
	
	ASSERT(pCell != NULL);

	OnboardDeckGroundInSim *pDeckGround = pCell->GetDeckGround();
	ASSERT(pDeckGround!= NULL);
	if(pDeckGround)
	{
		return pDeckGround->GetGround();
	}
	
	return NULL;
}

bool OnboardCellFreeMoveLogic::getCurrentPaxRect( CPath2008& pathRect ) const
{
	pathRect = m_pBehavior->getCurLocation().getPaxRect();

	//double PaxOnboardBaseBehavior::PAXLENGTH = 20.0;
	//double PaxOnboardBaseBehavior::PAXWIDTH = 40.0;
	//OnboardCellInSim* pDirCell = m_nextLocation.getCellCenter();
	//OnboardCellInSim* pCurCell = m_curLocation.getCellCenter();
	//CPoint2008 ptAngle;
	//if( m_curState == LogicMove )//the passenger is not in free moving state
	//{
	//	pathRect = m_pBehavior->getCurLocation().getPaxRect();
	//	//ptAngle = m_pBehavior->m_vecDirction;//use the logic direction
	//}
	//else// if(pDirCell && pCurCell)
	//{
	//	//ptAngle =  pDirCell->getLocation() - pCurCell->getLocation();
	//	pathRect = m_curLocation.getPaxRect();
	//}
	//else
	//{
	//	ASSERT(0);
	//	return false;
	//}

	//if(m_curLocation.getCellCenter() == NULL)
	//	return false;



	////the passenger has more cell to move
	////
	////get passenger's location
	////
	//double zPos = m_curLocation.getCellCenter()->getLocation().getZ();
	//std::vector<CPoint2008> vPaxSpace;

	////CPoint2008 ptAngle =  pDirCell->getLocation() - pCurCell->getLocation();
	//ptAngle.Normalize();

	//CPoint2008 ptFront = ptAngle;
	//ptFront.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

	//CPoint2008 ptLeft  = ptAngle.perpendicular();
	//ptLeft.length(PaxOnboardBaseBehavior::PAXWIDTH/2);

	//CPoint2008 ptBack  = ptAngle * (-1);
	//ptBack.length(PaxOnboardBaseBehavior::PAXLENGTH/2);

	//CPoint2008 ptRight = ptBack.perpendicular();
	//ptRight.length(PaxOnboardBaseBehavior::PAXWIDTH/2);



	//CPoint2008 ptLeftFront = ptFront + ptLeft;
	//CPoint2008 ptLeftBack  = ptLeft + ptBack;
	//CPoint2008 ptRightBack = ptBack + ptRight;
	//CPoint2008 ptRightFront = ptRight + ptFront;

	//vPaxSpace.push_back(ptLeftFront);
	//vPaxSpace.push_back(ptLeftBack);
	//vPaxSpace.push_back(ptRightBack);
	//vPaxSpace.push_back(ptRightFront);


	//CPath2008 paxSpace;
	//paxSpace.init(4,&vPaxSpace[0]);

	////move to the cell
	//paxSpace.DoOffset(pCurCell->getLocation().getX(), pCurCell->getLocation().getY(), 0);
	//
	//pathRect = paxSpace;

	return true;
}


bool OnboardCellFreeMoveLogic::getCurrentPaxDirection(CPoint2008& ptAngle) const
{
	OnboardCellInSim* pDirCell = m_nextLocation.getCellCenter();
	OnboardCellInSim* pCurCell = m_curLocation.getCellCenter();
	if(pDirCell && pCurCell)
	{
		//the passenger has more cell to move
		//
		//get passenger's location
		//
		double zPos = m_curLocation.getCellCenter()->getLocation().getZ();
		std::vector<CPoint2008> vPaxSpace;

		ptAngle =  CPoint2008(pDirCell->getLocation() - pCurCell->getLocation());
		ptAngle.Normalize();
		return true;
	}
	return false;
}

bool OnboardCellFreeMoveLogic::IsStopped( const ElapsedTime& eCurrentTime ) const
{
	if(m_curState == LogicMove)
		return true;

	if(/*m_curState == Waiting && */eCurrentTime - m_eLastMoveTime> ElapsedTime(5L))//waiting than 20s
		return true;

	return false;
}

bool OnboardCellFreeMoveLogic::ResetLocation( PaxCellLocation& newLocation )
{
	//get the new center
	OnboardCellInSim *pNewStart = newLocation.getCellCenter();
	if(pNewStart == NULL || m_pDest == NULL)
		return false;


	//get the path
	GroundInSim *pGround = getGroundInSim(pNewStart);
	if(pGround == NULL)
		return false;

	OnboardCellPath newPath;
	m_pBehavior->GetOnBoardFlight()->getCellGraph()->FindPath(pNewStart, m_pDest,newPath);
	//pGround->GetPath(pNewStart, m_pDest,newPath);

	//ASSERT(newPath.getCellCount() > 0);
	if(newPath.getCellCount() == 0)
	{
		GenerateStepEvent(m_eCurrentTime + ElapsedTime(1L));
		return false;
	}

	//write log
	double dLen = m_curLocation.getCellCenter()->GetDistanceTo(pNewStart);
	ElapsedTime moveTime = ElapsedTime(dLen/getSpeed());
	ElapsedTime eNextTime = m_eCurrentTime + moveTime;

	m_curLocation.PaxClear(m_pBehavior,eNextTime);
	setCurLocation(newLocation);
	m_curLocation.PaxOccupy(m_pBehavior);

	m_nextLocation.Clear();
	m_nextLocation.setPathCell(newPath.GetNextCell(pNewStart));
	m_nextLocation.setCellCenter(newPath.GetNextCell(pNewStart));
	m_cellPath = newPath;

	WriteLog(pNewStart,eNextTime);
	m_curState = Moving;


	//generate next event
	GenerateStepEvent(eNextTime);
	return true;
}
bool OnboardCellFreeMoveLogic::StepToLocation(const PaxCellLocation& newLocation )
{
	//get the new center
	OnboardCellInSim *pNewStart = newLocation.getCellCenter();
	if(pNewStart == NULL || m_pDest == NULL)
		return false;

	if(!newLocation.IsAvailable(m_pBehavior))
	{
		GenerateStepEvent(m_eCurrentTime + ElapsedTime(1L));
		return true;
	}

	
	WriteLog(m_curLocation.getCellCenter(),m_eCurrentTime);
	//write log
	double dLen = m_curLocation.getCellCenter()->GetDistanceTo(pNewStart);
	ElapsedTime moveTime = ElapsedTime(dLen/getSpeed());
	ElapsedTime eNextTime = m_eCurrentTime + moveTime;

	m_curLocation.PaxClear(m_pBehavior,eNextTime);
	setCurLocation(newLocation);
	m_curLocation.PaxOccupy(m_pBehavior);

	m_nextLocation.Clear();
	m_nextLocation.setPathCell(m_cellPath.GetNextCell(newLocation.getPathCell()));
	m_nextLocation.setCellCenter(m_cellPath.GetNextCell(newLocation.getPathCell()));
	WriteLog(pNewStart,m_eCurrentTime + moveTime);
	m_curState = Moving;

	//if(!m_bIsOutOfDoor)
		m_bIsOutOfDoor = m_pBehavior->m_pDoor->CheckDoorClear(m_pBehavior, eNextTime);

	WakeupWaitingPax(m_pBehavior,eNextTime);

	GenerateStepEvent(eNextTime);
	//generate next event

	return true;
}

void OnboardCellFreeMoveLogic::GenerateStepEvent( const ElapsedTime& eTime )
{
	ElapsedTime eEventTime = eTime;
	if(m_eCurrentTime >= eEventTime)// the next time must be larger than the current event time
		eEventTime = m_eCurrentTime + ElapsedTime(1L);

	OnboardPaxCellStepEvent *pEvent =  new OnboardPaxCellStepEvent(this,eEventTime);
	pEvent->addEvent();

}

#define MIN_STEP_DIST 10
#define MAX_STEP_DIST 20

int OnboardCellFreeMoveLogic::Step2( const ElapsedTime& eTime )
{
	
	SensorEnviroment();

	int bestSensor = mSensor.GetBest();
	if(bestSensor <0)
	{
		m_pBehavior->SetMovale(false);	

		return 0;
	}
	

	ARCVector3 mCurDir = mSensor.GetSensor(bestSensor).dir;	

	double dDist  = mSensor.GetSensor(bestSensor).dDistToBlock;
	if(dDist <= MIN_STEP_DIST )
	{
		PaxOnboardBaseBehavior* pWaitPax = mSensor.GetSensor(bestSensor).pOtherPax;		
		m_pBehavior->SetWaitPax(pWaitPax);		
		dDist = 0;
	}
	else
	{
		m_pBehavior->SetWaitPax(NULL);
	}


	dDist = MIN(dDist,MAX_STEP_DIST);
	ElapsedTime moveTime = dDist/getSpeed();

	//WriteLog(m_curLocation.getCellCenter(),eTime);
	ElapsedTime eNextTime = eTime + moveTime;
	m_eLastMoveTime = eNextTime;
	GenerateStepEvent(eNextTime);
	return TRUE;
}
	

void OnboardCellFreeMoveLogic::SensorEnviroment()
{
	mSensor.Reset();
	//get passenger ahead
	std::vector<Person*> paxOnFlight;
	m_pBehavior->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);
	
	for(int i=0;i<(int)paxOnFlight.size();i++)
	{
		Person* pPax = paxOnFlight.at(i);
		if(pPax==m_pBehavior->m_pPerson)
			continue;

		ARCVector3 concernlocation;
		if( pPax &&  pPax->getOnboardPoint(concernlocation) )
		{
			//check the passenger is under concern
			//ARCVector3
			
			//if(dDistance > 100)//2 meters
				//continue;
		}
	}

	mSensor.SensorMap();
	
}

void OnboardCellFreeMoveLogic::setCurLocation(const PaxCellLocation& paxLocation )
{
	m_curLocation = paxLocation;
	m_pBehavior->SetPaxLocation(paxLocation);
	if(paxLocation.getPathCell())
		m_pBehavior->m_lCellIndex = paxLocation.getPathCell()->getTileIndex();
}
//////////////////////////////////////////////////////////////////////////
#define MSTEP 0

PaxOnboardBaseBehavior*
OnboardCellFreeMoveLogic::findDirConflict(DistBlock& dbahead)
{
	for(size_t i=0;i<dbahead.vPaxList.size();i++)
	{
		PaxOnboardBaseBehavior* otherPax = dbahead.vPaxList[i];
		if(otherPax->m_pPerson->getState() == OnboardFreeMoving )
		{
			if( otherPax->m_pCellFreeMoveLogic->mlastAround.dpHead.HavePax(this->m_pBehavior) )
				return otherPax;
		}
		
	}
	return NULL;
}

//#define RUNTIME_DEBUG
#include <common/RunTimeDebug.h>

int OnboardCellFreeMoveLogic::Step3( const ElapsedTime& eTime )
{
	m_eCurrentTime = eTime;
	LOG_LINE("");
	GroundInSim *pGround = getGroundInSim(m_curLocation.getPathCell());
	
	CPoint2008 curPos = m_curLocation.getCellCenter()->getLocation();

	double distToNextCell = 0;
	//update curDir;
	OnboardCellInSim* pToCell= NULL;
	if(pToCell = m_cellPath.GetNextCell(m_curLocation.getCellCenter(),4))
	{
		mCurDir = pToCell->getLocation() - m_curLocation.getCellCenter()->getLocation();
		distToNextCell = mCurDir.length();
		mCurDir.Normalize();
	}
	else
	{
		BackToFlow(eTime);
		return TRUE;
	}
	LOG_LINE("");
	
	//
	if(mbSideWalk)
	{
		LOG_LINE("");
		SensorEnv(curPos,mCurDir,false, mlastAround);
		DistBlockAround paxAround;
		if( mlastAround.dpRight.dist + mlastAround.dpLeft.dist >0 )
		{
			CPoint2008 vOffset(0,0,0);
			if(mlastAround.dpLeft.dist<0)
			{
				vOffset = mCurDir.perpendicular() * mlastAround.dpLeft.dist;
			}
			if(mlastAround.dpRight.dist<0)
			{
				vOffset = mCurDir.perpendicular() * (-mlastAround.dpRight.dist);
			}
			CPoint2008 nextPos = curPos + vOffset;
			DistBlockAround paxAround;
			SensorEnv(nextPos,mCurDir, false, paxAround);

			OnboardCellInSim *pNextCell = pGround->getCell(nextPos);
			if(!findDirConflict(paxAround.dpHead) &&  pNextCell && pNextCell!=m_curLocation.getCellCenter() )
			{
				mbSideWalk = false;
				//do move to next pos
				double dLen = m_curLocation.getCellCenter()->GetDistanceTo(pNextCell);
				ElapsedTime moveTime = ElapsedTime(dLen/getSpeed());
				StepToCell(pNextCell, eTime + moveTime);
				return TRUE;
			}
		}
	}


	SensorEnv(curPos,mCurDir, mbSideWalk, mlastAround);
	//1. find if there is a dir conflict pax ahead
	LOG_LINE("");
	if(PaxOnboardBaseBehavior* pDirConfclitPax = NULL /*findDirConflict(mlastAround.dpHead)*/ )
	{
		LOG_LINE("");
		CPoint2008 otherPos = pDirConfclitPax->getCurLocation().getCellCenter()->getLocation();
		//try move left or right
		double dMoveStep = 0;
		CPoint2008 moveDir(0,0,0);
		if( mCurDir.crossProduct( CPoint2008(otherPos-curPos) ).z >0) //head pax in the left move right
		{
			if( mlastAround.dpRight.dist > 0 )
			{
				dMoveStep = mlastAround.dpRight.dist;
				moveDir =  -mCurDir.perpendicular();					
			}
		}
		if(dMoveStep <=0 && mlastAround.dpLeft.dist >0)
		{
			dMoveStep = mlastAround.dpLeft.dist;
			moveDir = mCurDir.perpendicular();
		}

		//do move 
		CPoint2008 nextPos = curPos +  moveDir*dMoveStep;
		OnboardCellInSim *pNextCell = pGround->getCell(nextPos);
		if(pNextCell && pNextCell != m_curLocation.getCellCenter())
		{
			double dLen = m_curLocation.getCellCenter()->GetDistanceTo(pNextCell);
			ElapsedTime moveTime = ElapsedTime(dLen/getSpeed());
			StepToCell(pNextCell, eTime + moveTime);
			return TRUE;
		}		
		else if(!mbSideWalk)// do side walk
		{
			mbSideWalk = true;
			WaitForNextRound(eTime);
			return TRUE;;
		}		
		else if(mlastAround.dpHead.pPax && mlastAround.dpHead.pPax!=pDirConfclitPax) //if there is head pax and the head pax is not the conflict one wait
		{
			WaitForNextRound(eTime);
			return TRUE;
		}
		else //move at low speed
		{		
			CPoint2008 nextPos = curPos +  moveDir*dMoveStep;
			OnboardCellInSim *pNextCell = pGround->getCell(nextPos);
			if(pNextCell)
			{
				double dLen = m_curLocation.getCellCenter()->GetDistanceTo(pNextCell);
				ElapsedTime moveTime = ElapsedTime(dLen*3/getSpeed());
				StepToCell(pNextCell, eTime + moveTime);
				return TRUE;
			}		
			ASSERT(FALSE);
		}
	}
	LOG_LINE("");
	//OnboardPaxFreeMovingConflict conflict(m_pBehavior); !CheckLocationConflict(pGround,m_curLocation, pToCell,conflict)

	PaxCellLocation tmpPaxLocation;
	GetPaxLocation(pToCell->getLocation(), mCurDir, tmpPaxLocation, getGroundInSim(pToCell) );
	if(mlastAround.dpHead.dist > distToNextCell  && tmpPaxLocation.IsAvailable(m_pBehavior))	
	{		
		m_pBehavior->SetWaitPax(NULL);
		ElapsedTime moveTime = ElapsedTime(distToNextCell/getSpeed());
		StepToCell(pToCell, eTime + moveTime);
		return TRUE;
	}	
	//wait
	if( m_pBehavior->bCanWaitFor(mlastAround.dpHead.pPax) )	{
		//ASSERT(FALSE);
		m_pBehavior->SetWaitPax(mlastAround.dpHead.pPax);
		WaitForNextRound(eTime);		
		return TRUE;;
	}
	ElapsedTime moveTime = ElapsedTime(distToNextCell/getSpeed());
	StepToCell(pToCell, eTime + moveTime);
	return TRUE;
}


bool distRayCircle( const ARCVector3& raypos, const ARCVector3& rayDir, const ARCVector3& cPos, double cRad, double& dist )
{
	ARCVector3 vOffset = cPos - raypos;

	double b = rayDir.dot(vOffset);
	double c = vOffset.Magnitude2() - cRad*cRad;

	double dt  = b*b -c;
	if(dt>0 && b >0)
	{
		dist = b- sqrt(dt);
		return true;
	}

	return false;
}

void OnboardCellFreeMoveLogic::SensorEnv(const CPoint2008& pos, const CPoint2008& dir, bool bSideSense, DistBlockAround& reslt )
{
	reslt.dpHead = reslt.dpRight = reslt.dpLeft = DistBlock();
	std::vector<Person*> paxOnFlight;
	m_pBehavior->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);
	CPoint2008 dirleft = dir.perpendicular();
	CPoint2008 dirright(-dirleft.x, -dirleft.y,dirleft.z);


	for(size_t i=0;i<paxOnFlight.size();i++)
	{
		Person* pOther = paxOnFlight[i];		

		if(pOther->getOnboardBehavior() == NULL)
			continue;

		OnboardCellInSim *pCellCenern = pOther->getOnboardBehavior()->getCurLocation().getCellCenter();
		if(pCellCenern == NULL)
			continue;

		
		if(pOther==this->m_pBehavior->getPerson())
			continue;

		PaxOnboardBaseBehavior* pOtherBehavir = pOther->getOnboardBehavior();
		PaxOnboardBaseBehavior* pThisBehavir = this->m_pBehavior;

		ARCVector3 otherloc;
		if( !pOther->getOnboardPoint(otherloc) )
			continue;

		if( otherloc.DistanceTo(pos) > 500  )
			continue;
		double dWithrad = (pOtherBehavir->GetWidth() + pThisBehavir->GetWidth() )*0.5;
		double dsiderad = (pOtherBehavir->GetWidth() + pThisBehavir->GetThick() )*0.5;

		double dist;
		if( distRayCircle(pos,dir,otherloc,dWithrad,dist) ) //head
		{
			if(dist < reslt.dpHead.dist)
			{
				reslt.dpHead.dist = dist;
				reslt.dpHead.pPax = pOtherBehavir;
			}
			if(dist  < dWithrad)
			{
				reslt.dpHead.vPaxList.push_back(pOtherBehavir);
			}
		}
		if(  distRayCircle(pos,dirleft,otherloc,!bSideSense?dWithrad:dsiderad,dist) )
		{
			if(dist < reslt.dpLeft.dist)
			{
				reslt.dpLeft.dist = dist;
				reslt.dpLeft.pPax = pOtherBehavir;
			}
			if(dist < dWithrad)
			{
				reslt.dpLeft.vPaxList.push_back(pOtherBehavir);
			}
		}
		if(  distRayCircle(pos,dirright,otherloc,!bSideSense?dWithrad:dsiderad,dist) )
		{
			if(dist < reslt.dpRight.dist)
			{
				reslt.dpRight.dist = dist;
				reslt.dpRight.pPax = pOtherBehavir;
			}
			if(dist < dWithrad)
			{
				reslt.dpRight.vPaxList.push_back(pOtherBehavir);
			}
		}
	}	
	
	//
	GroundInSim *pGround = getGroundInSim(m_curLocation.getPathCell());
	double dConcernRad = m_pBehavior->GetRadiusOfConcern();
	double dSideSpan = bSideSense?m_pBehavior->GetThick()*0.5:m_pBehavior->GetWidth()*0.5;
	//left bound
	double dLeftFreeDist = pGround->GetDirValidDist(m_pBehavior,pos, dirleft,dConcernRad ) - dSideSpan;
	if(dLeftFreeDist < reslt.dpLeft.dist )
	{
		reslt.dpLeft.dist = dLeftFreeDist;
		reslt.dpLeft.bStaticBlock = true;
	}
	//right bound
	double dRightFreeDist = pGround->GetDirValidDist(m_pBehavior,pos, dirright, dConcernRad ) - dSideSpan;;
	if(dRightFreeDist < reslt.dpRight.dist )
	{
		reslt.dpRight.dist = dRightFreeDist;
		reslt.dpRight.bStaticBlock = true;
	}


}

void OnboardCellFreeMoveLogic::StepToCell( OnboardCellInSim* pNextCell ,const ElapsedTime& eNextTime)
{
	if(m_bNeedToWriteLog)
	{
		m_pBehavior->writeLog(m_eCurrentTime);
		m_bNeedToWriteLog = false;
	}

	PaxCellLocation tmpPaxLocation;
	GetPaxLocation(pNextCell->getLocation(), mCurDir, tmpPaxLocation, getGroundInSim(pNextCell) );
	m_curLocation.PaxClear(m_pBehavior,eNextTime);
	m_curLocation = tmpPaxLocation;
	m_curLocation.PaxOccupy(m_pBehavior);
	WriteLog(m_curLocation.getCellCenter(),eNextTime);
	
	GenerateStepEvent(eNextTime);
}

void OnboardCellFreeMoveLogic::WaitForNextRound(const ElapsedTime& eNextTime)
{
	GenerateStepEvent(eNextTime+ElapsedTime(1L));
	m_bNeedToWriteLog = true;
}
bool OnboardCellFreeMoveLogic::DistBlock::HavePax( PaxOnboardBaseBehavior* pPax ) const
{
	return std::find(vPaxList.begin(), vPaxList.end(),pPax)!=vPaxList.end();
}

OnboardCellFreeMoveLogic::DistBlock::DistBlock()
{
	dist = 100; pPax = 0;bStaticBlock = false;
}