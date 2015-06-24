#include "stdafx.h"
#include "DeplaneAgent.h"
#include "DeplaneAgentBehavior.h"
#include "LocatorSite.h"
#include "OnBoardCarrierSpaces.h"
#include "OnBoardSimEngineConfig.h"
#include "carrierSpace/carrierDefs.h"
#include "../terminal.h"
#include "Engine/ARCportEngine.h"


DeplaneAgent::DeplaneAgent(MobLogEntry& p_entry, CARCportEngine* _pEngine )
:MobileAgent(p_entry, _pEngine)
{
//	MobileElement::setBehavior( new DeplaneAgentBehavior (this) );
	MobileElement::setState( EnterOnBoardMode );

	assignParticularConfig();
}

DeplaneAgent::~DeplaneAgent()
{

}

void DeplaneAgent::assignParticularConfig()
{
	// assign carrier space.
	int nArrFltIndex = getLogEntry().getArrFlight();
	if(nArrFltIndex < 0 )
	{
		CString strAgentMsg = this->getPersonErrorMsg();
		CString strErrorMsg;
		strErrorMsg.Format(_T("Deplane Passenger:%s, cannot find arrival plane"), screenMobAgent());
		throw new OnBoardSimEngineFatalError( new OnBoardDiagnose( strErrorMsg) );
	}

	Flight *pFlight = getEngine()->m_simFlightSchedule.getItem( nArrFltIndex );
	char lpszACType[1024] = {0};
	pFlight->getACType( lpszACType );

	carrierSpace* pCarrierSpace = 
		getConfig()->m_pCarrierSpaces->getCarrierSpace( lpszACType );
	if(pCarrierSpace == NULL)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("Deplane Passenger:%s, cannot find carrier"), screenMobAgent());
		throw new OnBoardSimEngineFatalError( new OnBoardDiagnose( strErrorMsg) );
	}

	setCarrierSpace( pCarrierSpace );


	carrierDeck* pCurrDeck = m_pCarrierSpace->getCarrierDeck( 0 );
	ASSERT(pCurrDeck != NULL);
	pCurrDeck->getCarrierGround().setMobAgentIntelligence( this );

}

bool DeplaneAgent::findMovementPath()
{
	int nLevel = 0; //getCurrentLevel();

	// first, set my seat's location as non barrier objects.
	carrierDeck* pCurrDeck = m_pCarrierSpace->getCarrierDeck( 0 );
	carrierGround& ground = pCurrDeck->getCarrierGround();
	ground.swapCarrierSeatBarrierState( getAssignedSeat(), false);

	// then, find movement path to seat location.
	MobAgentMovementPath _movePath;
	pCurrDeck->getMovementPath(this, _movePath);

	m_pMovePath->clear();
	for (MobAgentMovementPath::iterator iter = _movePath.begin();
		iter != _movePath.end(); iter++)
		m_pMovePath->addLocatorSite( *iter );

	// at last, reset my seat as barrier objects.
	ground.swapCarrierSeatBarrierState( getAssignedSeat(), true);
	return m_pMovePath->getCount() > 0;

}

carrierExitDoor* DeplaneAgent::getExitDoor()
{
	// debug purpose.

	carrierGround& _ground = m_pCarrierSpace->getCarrierDeck(0)->getCarrierGround();
	return _ground.getCarrierExitDoor();
}

carrierEmergencyDoor* DeplaneAgent::getEmergencyDoor()
{
	return NULL;
}

carrierStair* DeplaneAgent::getStair()
{
	return NULL;
}

carrierStorage* DeplaneAgent::getStorage()
{
	return NULL;
}



