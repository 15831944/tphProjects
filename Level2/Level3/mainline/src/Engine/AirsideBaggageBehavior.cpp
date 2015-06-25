#include "StdAfx.h"
#include ".\airsidebaggagebehavior.h"
#include "Person.h"
#include "GroupLeaderInfo.h"
#include "..\Common\ARCTracker.h"
#include "AirsideBaggageCartInSim.h"



AirsideBaggageBehavior::AirsideBaggageBehavior( Person *pBaggage )
:AirsideMobElementBaseBehavior(pBaggage)
{

}
AirsideBaggageBehavior::AirsideBaggageBehavior(Person* pBaggage ,int curentstate)
:AirsideMobElementBaseBehavior(pBaggage)
{
	setState(curentstate) ;
}
AirsideBaggageBehavior::~AirsideBaggageBehavior(void)
{
}

void AirsideBaggageBehavior::SetFollowerDestination( const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation )
{
	PLACE_METHOD_TRACK_STRING();

	if (m_pMobileElemment->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pMobileElemment->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup())
		return;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		MobElementBehavior* spBehavior = _pFollower->getCurrentBehavior();
		if (spBehavior == NULL)
		{
			_pFollower->setBehavior(new AirsideBaggageBehavior(_pFollower,_pFollower->getState()));
		}
		else if (spBehavior->getBehaviorType()!= AirsideBehavior)
		{
			_pFollower->setBehavior(new AirsideBaggageBehavior(_pFollower,_pFollower->getState()));
		}

		CPoint2008 ptDestination(0.0, 0.0, 0.0);
		ptDestination = _ptDestination;
		float fDir = (i> (MAX_GROUP-1)) ? (float)0.0 : _pRotation[i+1];
		ptDestination.offsetCoords( _ptCurrent, (double)fDir, GROUP_OFFSET );
		if( _pFollower->getState() != Death )
		{
			//_pFollower->setAirsideDestination( ptDestination );
			MobElementBehavior* spBehavior = _pFollower->getCurrentBehavior();
			if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
			{
				AirsideMobElementBaseBehavior* spAirsideBehavior = _pFollower->getAirsideBehavior();
				if (spAirsideBehavior == NULL)
				{
					_pFollower->setBehavior(new AirsideBaggageBehavior(_pFollower,getState()));
				}

				spAirsideBehavior = _pFollower->getAirsideBehavior();
				spAirsideBehavior->setDestination(ptDestination);
			}
		}
	}
}

int AirsideBaggageBehavior::performanceMove( ElapsedTime p_time,bool bNoLog )
{
	ASSERT(0);
	//switch(getState())
	//{
	//}
	return 0;
}

//void AirsideBaggageBehavior::writeLog( ElapsedTime time, bool _bBackup, bool bOffset /*= true */ )
//{
//	ASSERT(0);
//}

void AirsideBaggageBehavior::MoveToCartFromPusher(AirsideBaggageCartInSim *pBagCart, ElapsedTime& eTime)
{
	ResetTerminalToAirsideLocation();
	
	setState(MOVETOBAGCART);
	WriteLog(eTime);

	CPoint2008 ptCart = pBagCart->GetPosition();
	CPoint2008 ptRandom = pBagCart->GetRanddomPoint();
	ptRandom.setZ(pBagCart->GetVehicleRandomZ());
	_offSetInBus = ptRandom;

	ptCart += ptRandom;
	ptCart.setZ(pBagCart->GetVehicleRandomZ());
	setDestination(ptCart);
	ElapsedTime eMoveTime = moveTime();
	setState(ARRIVEATBAGCART);

	eTime += eMoveTime;
	setLocation(ptCart);
	WriteLog(eTime);

	setState(STAYATCART);
	WriteLog(eTime);

}
