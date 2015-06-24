#include "StdAfx.h"
#include ".\onboardpaxfreemovingconflict.h"
#include "PaxOnboardBaseBehavior.h"
#include "OnboardCellFreeMoveLogic.h"
#include "common\RayIntersectPath.h"


OnboardPaxFreeMovingConflict::OnboardPaxFreeMovingConflict( PaxOnboardBaseBehavior *pCurPax )
:m_pCurPax(pCurPax)
{

}
OnboardPaxFreeMovingConflict::~OnboardPaxFreeMovingConflict(void)
{
}

void OnboardPaxFreeMovingConflict::HandleConflict( const PaxCellLocation& curLocation, const PaxCellLocation& nextLocation )
{
	//get passenger right ahead

	//if(pNearestPaxOccupied)
	//{
	//	m_curState = Waiting;
	//	if(!pNearestPaxOccupied->AddWaitingPax(m_pBehavior))
	//	{//there would be cycle id add the behavior
	//		
	//		//pNearestCellOccupied->getBehavior()->GenerateEvent(time);
	//	}
	//}

	////analysis the passengers ahead
	//std::vector<ConflictPax > vInLogic;
	//std::vector<ConflictPax > vMoving;
	//std::vector<ConflictPax > vWaiting;

	//int nCountInsight = m_vInsightPax.size();
	//for (int n = 0; n < nCountInsight; ++n)
	//{
	//	ConflictPax insightPax = m_vInsightPax[n];
	//	if(insightPax.m_pPax && insightPax.m_pPax->m_pCellFreeMoveLogic)
	//	{
	//		if(insightPax.m_pPax->m_pCellFreeMoveLogic->m_curState == OnboardCellFreeMoveLogic::LogicMove)
	//		{
	//			vInLogic.push_back(insightPax);
	//		}
	//		else if(insightPax.m_pPax->m_pCellFreeMoveLogic->m_curState == OnboardCellFreeMoveLogic::Moving)
	//		{
	//			vMoving.push_back(insightPax);
	//		}
	//		else if (insightPax.m_pPax->m_pCellFreeMoveLogic->m_curState == OnboardCellFreeMoveLogic::Waiting)
	//		{
	//			vWaiting.push_back(insightPax);
	//		}
	//	}
	//}

	std::vector<PaxOnboardBaseBehavior *> vConflictPax = GetPassengerRightAhead(curLocation,nextLocation, m_pCurPax);
	if(vConflictPax.size()  == 0)//conflict in next location
	{
		m_pCurPax->m_pCellFreeMoveLogic->StepToLocation(nextLocation);
		return; //no conflict
	}

	std::vector<PaxOnboardBaseBehavior *> vSameDirection;
	std::vector<PaxOnboardBaseBehavior *> vRelativeDirection;
	std::vector<PaxOnboardBaseBehavior *> vStopped;
	
	CPoint2008 ptThisPaxDirection = CPoint2008(nextLocation.getCellCenter()->getLocation() -
		curLocation.getCellCenter()->getLocation());
	{
		//must make a decision
		//wait for that passenger if having same direction and not stopped
		//if not same direction, find a resolution
		//return
 
		//get the direction
		for(int nConflictPax = 0; nConflictPax < static_cast<int>(vConflictPax.size());++nConflictPax)
		{
			 PaxOnboardBaseBehavior *pPax = vConflictPax.at(nConflictPax);
			 if(pPax == NULL)
				 continue;
			 
			 //this passenger is stopped
			 if(pPax->m_pCellFreeMoveLogic->m_curState == OnboardCellFreeMoveLogic::LogicMove)
			 {
				 vStopped.push_back(pPax);
				 continue;
			 }

			 CPoint2008 conflictDirection;
			 if(!pPax->m_pCellFreeMoveLogic->getCurrentPaxDirection(conflictDirection))
			 {
				 vStopped.push_back(pPax);
				 continue;
			 }

			 //check the direction
			 //same direction or relative direction
			 if(ptThisPaxDirection.GetCosOfTwoVector(conflictDirection) >= 0)
				 //same direction
			 {

				vSameDirection.push_back(pPax);
			 }
			 else
				//relative direction
			{
				vRelativeDirection.push_back(pPax);
			}
		}
	}

	//check same direction
	if(vSameDirection.size() > 0  )
	{
		//check relative direction

		if(vRelativeDirection.size() > 0 )//have relative direction passenger ahead
		{
			//follow the same direction and give way to relative direction

			PaxOnboardBaseBehavior *pSameDirPax = vSameDirection[0];
			//PaxOnboardBaseBehavior *pRelativeDirPax = vRelativeDirection[0];
			
			//get the direction to Same Direction Passenger
			PaxCellLocation& sameDirLocation = pSameDirPax->getCurLocation();
			
			//get vector to the Same Direction Pax
			//the direction form the current passenger to the same direction passenger
			CPoint2008 vecSameDirection = CPoint2008(sameDirLocation.getCellCenter()->getLocation() - curLocation.getCellCenter()->getLocation());
			
			CPoint2008 vecPerpendicular = ptThisPaxDirection.perpendicular();
			if(vecSameDirection.GetCosOfTwoVector(vecPerpendicular) < 0)
				vecPerpendicular = ptThisPaxDirection.perpendicular() * (-1);

			//get the new direction
			//vecSameDirection.Normalize();
			vecPerpendicular.Normalize();
			CPoint2008 ptNewDirection = vecPerpendicular; //+ vecSameDirection;


			//check the next location is valid
			bool bNewNextLocation = false;
			PaxCellLocation newNextLocation;
			double dOffeset = 5;//5cm
			for (; dOffeset <= 20 ; dOffeset += 5 )
			{
				//get the new location
				ptNewDirection.length(5);
				CPoint2008 ptNextLocation = curLocation.getCellCenter()->getLocation();
				ptNextLocation += ptNewDirection;

				if(ptNextLocation.getDistanceTo(sameDirLocation.getCellCenter()->getLocation()) < 30) //lease than safe distance
					break;

				PaxCellLocation tmpNextLocation;
				if(!m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptNextLocation, vecSameDirection, tmpNextLocation))
					break;
                
				//check the location is occupied or not
				if(!tmpNextLocation.IsAvailable(m_pCurPax))
					break;

				//check the location
				newNextLocation = tmpNextLocation;
				bNewNextLocation = true;
			}

			if(bNewNextLocation)//
			{

				//passenger move to new location, 
				//find new path
				//generate next event
				m_pCurPax->m_pCellFreeMoveLogic->ResetLocation(newNextLocation);

				return;
			}
			else
			{
				//waiting for same direction passenger
				//pSameDirPax->AddWaitingPax(m_pCurPax);
				m_pCurPax->m_pCellFreeMoveLogic->GenerateStepEvent(m_eCurrentTime + ElapsedTime(1L));
				return;
			}
		}

		//if(vStopped.size() > 0)
		//{
		//	//get the direction
		//
		//	//follow the same direction and give way to relative direction

		//	PaxOnboardBaseBehavior *pSameDirPax = vSameDirection[0];
		//	//PaxOnboardBaseBehavior *pRelativeDirPax = vRelativeDirection[0];

		//	//get the direction to Same Direction Passenger
		//	PaxCellLocation& sameDirLocation = pSameDirPax->m_pCellFreeMoveLogic->m_curLocation;

		//	//get vector to the Same Direction Pax
		//	//the direction form the current passenger to the same direction passenger
		//	CPoint2008 vecSameDirection = sameDirLocation.getCellCenter()->getLocation() - curLocation.getCellCenter()->getLocation();

		//	CPoint2008 vecPerpendicular = ptThisPaxDirection.perpendicular();
		//	if(vecSameDirection.GetCosOfTwoVector(vecPerpendicular) < 0)
		//		vecPerpendicular = ptThisPaxDirection.perpendicular() * (-1);

		//	//get the new direction
		//	//vecSameDirection.Normalize();
		//	vecPerpendicular.Normalize();
		//	CPoint2008 ptNewDirection = vecPerpendicular; //+ vecSameDirection;


		//	//check the next location is valid
		//	bool bNewNextLocation = false;
		//	PaxCellLocation newNextLocation;
		//	double dOffeset = 5;//5cm
		//	for (; dOffeset <= 40 ; dOffeset += 5 )
		//	{
		//		//get the new location
		//		ptNewDirection.length(5);
		//		CPoint2008 ptNextLocation = curLocation.getCellCenter()->getLocation();
		//		ptNextLocation += ptNewDirection;

		//		if(ptNextLocation.getDistanceTo(sameDirLocation.getCellCenter()->getLocation()) < 30) //lease than safe distance
		//			break;

		//		PaxCellLocation tmpNextLocation;
		//		if(!m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptNextLocation, vecSameDirection, tmpNextLocation))
		//			break;

		//		//check the location is occupied or not
		//		if(!tmpNextLocation.IsAvailable(m_pCurPax))
		//			break;

		//		//check the location
		//		newNextLocation = tmpNextLocation;
		//		bNewNextLocation = true;
		//	}

		//	if(bNewNextLocation)//
		//	{

		//		//passenger move to new location, 
		//		//find new path
		//		//generate next event
		//		m_pCurPax->m_pCellFreeMoveLogic->ResetLocation(newNextLocation);

		//		return;
		//	}
		//	else
		//	{
		//		//waiting for same direction passenger
		//		pSameDirPax->AddWaitingPax(m_pCurPax);
		//		return;
		//	}



		//}
		//return;


		//only same direction
		if(vSameDirection.size() > 0 || vStopped.size() > 0)
		{
			PaxOnboardBaseBehavior *pSameDirPax = vSameDirection[0];

			////if(pSameDirPax->m_pCellFreeMoveLogic->IsStopped(m_eCurrentTime))
			////{
			////	//get the direction to Same Direction Passenger
			////	PaxCellLocation& sameDirLocation = pSameDirPax->getCurLocation();
			////	
			////	////try to move around
			////	//check the available next location
			////	CPoint2008 vecPerpendicular = ptThisPaxDirection.perpendicular();
			////	vecPerpendicular.length(40);

			////	
			////	//left side
			////	bool bLeftSide = false;
			////	CPoint2008 ptAssumeLocation = sameDirLocation.getCellCenter()->getLocation() + vecPerpendicular;
			////	PaxCellLocation newNextLocation;
			////	if(m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptAssumeLocation,ptThisPaxDirection,newNextLocation))
			////		bLeftSide = true;

			////	//RIGHT SIDE
			////	bool bRightSide = false;
			////	if(!bLeftSide)
			////	{
			////		vecPerpendicular = vecPerpendicular * (-1);
			////		ptAssumeLocation = sameDirLocation.getCellCenter()->getLocation() + vecPerpendicular;
			////		if(m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptAssumeLocation,ptThisPaxDirection,newNextLocation))
			////			bRightSide = true;
			////	}

			////	if(bLeftSide || bRightSide)// move to that direction
			////	{
			////		if(newNextLocation.IsAvailable(m_pCurPax))
			////		{
			////			//get the new direction
			////			CPoint2008 ptNewDirection = vecPerpendicular;

			////			bool bNewNextLocation = false;
			////			PaxCellLocation newNextLocation;
			////			double dOffeset = 5;//5cm
			////			for (; dOffeset <= 20 ; dOffeset += 5 )
			////			{
			////				//get the new location
			////				ptNewDirection.length(5);
			////				CPoint2008 ptNextLocation = curLocation.getCellCenter()->getLocation();
			////				ptNextLocation += ptNewDirection;

			////				if(ptNextLocation.getDistanceTo(sameDirLocation.getCellCenter()->getLocation()) < 50) //lease than safe distance
			////					break;

			////				PaxCellLocation tmpNextLocation;
			////				if(!m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptNextLocation, ptThisPaxDirection, tmpNextLocation))
			////					break;

			////				//check the location is occupied or not
			////				if(!tmpNextLocation.IsAvailable(m_pCurPax))
			////					break;

			////				//check the location
			////				newNextLocation = tmpNextLocation;
			////				bNewNextLocation = true;
			////			}

			////			if(bNewNextLocation)//
			////			{

			////				//passenger move to new location, 
			////				//find new path
			////				//generate next event
			////				m_pCurPax->m_pCellFreeMoveLogic->ResetLocation(newNextLocation);

			////				return;
			////			}
			////		}
			////	}
			////}

			//wait at the passenger
			{
				//waiting for same direction passenger
				//pSameDirPax->AddWaitingPax(m_pCurPax);
				m_pCurPax->m_pCellFreeMoveLogic->GenerateStepEvent(m_eCurrentTime + ElapsedTime(2L));
				return;
			}
		}
	}

	//check relative direction
	if(vRelativeDirection.size() > 0)
	{

		//get the direction
		PaxOnboardBaseBehavior *pRelativeDirPax = vRelativeDirection[0];
		//PaxOnboardBaseBehavior *pRelativeDirPax = vRelativeDirection[0];

		//get the direction to Same Direction Passenger
		PaxCellLocation& relativeDirLocation = pRelativeDirPax->getCurLocation();

		//get vector to the Same Direction Pax
		//the direction form the current passenger to the same direction passenger
		CPoint2008 vecRelativeDirection = CPoint2008(relativeDirLocation.getCellCenter()->getLocation() - curLocation.getCellCenter()->getLocation());

		CPoint2008 vecPerpendicular = ptThisPaxDirection.perpendicular();
		if(vecRelativeDirection.GetCosOfTwoVector(vecPerpendicular) > 0)
			vecPerpendicular = ptThisPaxDirection.perpendicular() * (-1);

		//passenger move to perpendicular direction
		//get the new direction
		CPoint2008 ptNewDirection = vecPerpendicular;

		bool bNewNextLocation = false;
		PaxCellLocation newNextLocation;
		double dOffeset = 5;//5cm
		for (; dOffeset <= 20 ; dOffeset += 5 )
		{
			//get the new location
			ptNewDirection.length(dOffeset);
			CPoint2008 ptNextLocation = curLocation.getCellCenter()->getLocation();
			ptNextLocation += ptNewDirection;

			PaxCellLocation tmpNextLocation;
			if(!m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptNextLocation, ptThisPaxDirection, tmpNextLocation))
				break;

			//check the location is occupied or not
			if(!tmpNextLocation.IsAvailable(m_pCurPax))
				break;

			//check the location
			newNextLocation = tmpNextLocation;
			bNewNextLocation = true;
		}

		if(bNewNextLocation)//
		{

			//passenger move to new location, 
			//find new path
			//generate next event
			m_pCurPax->m_pCellFreeMoveLogic->ResetLocation(newNextLocation);

			return;
		}	
		//wait at the passenger
		{
			//waiting for relative direction passenger
			//pRelativeDirPax->AddWaitingPax(m_pCurPax);
			m_pCurPax->m_pCellFreeMoveLogic->GenerateStepEvent(m_eCurrentTime + ElapsedTime(1L));
			return;
		}

		//if(vStopped.size() > 0)
		//{
		//}
		//return;

	}

	if(vStopped.size() > 0)
	{
		//get the direction to Same Direction Passenger
		PaxOnboardBaseBehavior *pStopPax = vStopped[0];
		PaxCellLocation& stopPaxLocation = pStopPax->getCurLocation();

		////////try to move around
		//////check the available next location
		////CPoint2008 vecPerpendicular = ptThisPaxDirection.perpendicular();
		////vecPerpendicular.length(40);

		//////left side
		////bool bLeftSide = false;
		////CPoint2008 ptAssumeLocation = stopPaxLocation.getCellCenter()->getLocation() + vecPerpendicular;
		////PaxCellLocation newNextLocation;
		////if(m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptAssumeLocation,ptThisPaxDirection,newNextLocation))
		////	bLeftSide = true;

		//////RIGHT SIDE
		////bool bRightSide = false;
		////if(!bLeftSide)
		////{
		////	vecPerpendicular = vecPerpendicular * (-1);
		////	ptAssumeLocation = stopPaxLocation.getCellCenter()->getLocation() + vecPerpendicular;
		////	if(m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptAssumeLocation,ptThisPaxDirection,newNextLocation))
		////		bRightSide = true;
		////}

		////if(bLeftSide || bRightSide)// move to that direction
		////{
		////	if(newNextLocation.IsAvailable(m_pCurPax))
		////	{
		////		//get the new direction
		////		CPoint2008 ptNewDirection = vecPerpendicular;

		////		bool bNewNextLocation = false;
		////		PaxCellLocation newNextLocation;
		////		double dOffeset = 5;//5cm
		////		for (; dOffeset <= 20 ; dOffeset += 5 )
		////		{
		////			//get the new location
		////			ptNewDirection.length(5);
		////			CPoint2008 ptNextLocation = curLocation.getCellCenter()->getLocation();
		////			ptNextLocation += ptNewDirection;

		////			if(ptNextLocation.getDistanceTo(stopPaxLocation.getCellCenter()->getLocation()) < 30) //lease than safe distance
		////				break;

		////			PaxCellLocation tmpNextLocation;
		////			if(!m_pCurPax->m_pCellFreeMoveLogic->GetPaxLocation(ptNextLocation, ptThisPaxDirection, tmpNextLocation))
		////				break;

		////			//check the location is occupied or not
		////			if(!tmpNextLocation.IsAvailable(m_pCurPax))
		////				break;

		////			//check the location
		////			newNextLocation = tmpNextLocation;
		////			bNewNextLocation = true;
		////		}

		////		if(bNewNextLocation)//
		////		{

		////			//passenger move to new location, 
		////			//find new path
		////			//generate next event
		////			m_pCurPax->m_pCellFreeMoveLogic->ResetLocation(newNextLocation);

		////			return;
		////		}
		////	}
		//////}

		m_pCurPax->m_pCellFreeMoveLogic->GenerateStepEvent(m_eCurrentTime + ElapsedTime(1L));
		return;
		
	}

	//have no conflict
	//move ahead
	
	m_pCurPax->m_pCellFreeMoveLogic->StepToLocation(nextLocation);


	




	//have no cell conflict with next passenger
	//check the passenger on his way, have a intersection with the passenger ahead((0,-20) (0,20),, 
	//on current passenger's direction

	//try to move around if the passenger ahead is waiting or not in moving logic

	//following the heading passenger 

	//first 
	//handle the moving passenger
		//same direction
		//follow the passenger

		//relative direction
		//side walk if necessary

	//handle the waiting passenger

	//handle the in logic procession passengers


}

std::vector<PaxOnboardBaseBehavior *> OnboardPaxFreeMovingConflict::GetPassengerConflict(const PaxCellLocation& nextLocation, 
																				PaxOnboardBaseBehavior *pBehavior) const
{
	
	std::vector<PaxOnboardBaseBehavior *> vConflictPax;

	//get the latest waiting passenger need to wait
	 int nCellCount = nextLocation.getCellCount();
	 for (int nCell = 0; nCell < nCellCount; ++ nCell)
	 {
		OnboardCellInSim *pCell =  nextLocation.getCell(nCell);
		if(pCell && pCell->getBehavior() && pCell->getBehavior() != pBehavior)
		{
			vConflictPax.push_back(pCell->getBehavior());

		}
	 }

	return vConflictPax;
}

std::vector<PaxOnboardBaseBehavior *> OnboardPaxFreeMovingConflict::GetPassengerRightAhead(const PaxCellLocation& curLocation, 
																						   const PaxCellLocation& nextLocation, 
																						   PaxOnboardBaseBehavior *pCurBehavior) const
{
	std::vector<PaxOnboardBaseBehavior *> vConflictPax;
	//current passenger
	//
	if(pCurBehavior == NULL)
		return vConflictPax;

	CPath2008 curPaxPath;
	if(!pCurBehavior->m_pCellFreeMoveLogic->getCurrentPaxRect(curPaxPath))
		return vConflictPax;

	if(!curLocation.getCellCenter() || !nextLocation.getCellCenter() )
		return vConflictPax;

	//Center Direction
	CPoint2008 curCenterDirection =CPoint2008(nextLocation.getCellCenter()->getLocation() - curLocation.getCellCenter()->getLocation());
	//1 - 2 = 2 -> 1
	//4 - 3 = 3 -> 4

	
	std::vector< PaxOnboardBaseBehavior * > vTotalPax;
// 	std::vector< PaxOnboardBaseBehavior * > centerPax;
// 	std::vector< PaxOnboardBaseBehavior * > rightPax;

	double dLeftDistance	= 10000;
	double dCenterDistance	= 10000;
	double dRightDistance	= 10000;

	for (int nInsightPax = 0; nInsightPax < static_cast<int >(m_vInsightPax.size()); ++ nInsightPax)
	{
		ConflictPax insightPax = m_vInsightPax[nInsightPax];
		if(insightPax.m_pPax && insightPax.m_pPax->m_pCellFreeMoveLogic)
		{
			CPath2008 insightPaxRect;
			if(!insightPax.m_pPax->m_pCellFreeMoveLogic->getCurrentPaxRect(insightPaxRect))
				continue;
            
			//center
			CRayIntersectPath2D centerInterseciton(curLocation.getCellCenter()->getLocation(), curCenterDirection,insightPaxRect );
			if(centerInterseciton.Get())
			{
				if(centerInterseciton.m_Out.size())
				{
					if(dCenterDistance > centerInterseciton.m_Out[0].m_dist)
					{
						dCenterDistance = centerInterseciton.m_Out[0].m_dist;
						if (std::find(vTotalPax.begin(),vTotalPax.end(),insightPax.m_pPax) == vTotalPax.end())
						{
							vTotalPax.push_back(insightPax.m_pPax);
						}
                    //    centerPax.push_back(insightPax.m_pPax);
					}
				}
			}

			//left
			CRayIntersectPath2D leftInterseciton(curPaxPath[0], curCenterDirection,insightPaxRect );
			if(leftInterseciton.Get())
			{
				if(leftInterseciton.m_Out.size())
				{
					if(dLeftDistance > leftInterseciton.m_Out[0].m_dist)
					{
						dLeftDistance = leftInterseciton.m_Out[0].m_dist;
						if (std::find(vTotalPax.begin(),vTotalPax.end(),insightPax.m_pPax) == vTotalPax.end())
						{
							vTotalPax.push_back(insightPax.m_pPax);
						}
					//	leftPax.push_back(insightPax.m_pPax);
					}
				}
			}

			//right
			CRayIntersectPath2D rightInterseciton(curPaxPath[3], curCenterDirection,insightPaxRect );
			if(rightInterseciton.Get())
			{
				if(rightInterseciton.m_Out.size())
				{
					if(dRightDistance > rightInterseciton.m_Out[0].m_dist)
					{
						dRightDistance = rightInterseciton.m_Out[0].m_dist;
						if (std::find(vTotalPax.begin(),vTotalPax.end(),insightPax.m_pPax) == vTotalPax.end())
						{
							vTotalPax.push_back(insightPax.m_pPax);
						}
					//	rightPax.push_back(insightPax.m_pPax);
					}
				}
			}
		}
	}

	vConflictPax.clear();

	vConflictPax = vTotalPax;
// 	vConflictPax.insert(vConflictPax.end() ,centerPax.begin() ,centerPax.end());
// 	vConflictPax.insert(vConflictPax.end() ,leftPax.begin() ,leftPax.end());
// 	vConflictPax.insert(vConflictPax.end() ,rightPax.begin() ,rightPax.end());

	return vConflictPax;
}























