#include "stdafx.h"
#include "EnplaneAgent.h"
#include "EnplaneAgentBehavior.h"
#include "LocatorSite.h"
#include "OnBoardCarrierSpaces.h"
#include "OnBoardSimEngineConfig.h"
#include "carrierSpace/carrierDefs.h"
#include "../terminal.h"
#include "Engine/ARCportEngine.h"


EnplaneAgent::EnplaneAgent(MobLogEntry& p_entry, CARCportEngine *pEngine )
:MobileAgent(p_entry, pEngine)
{
//	MobileElement::setBehavior( new EnplaneAgentBehavior( this) );
	MobileElement::setState( EnterOnBoardMode );

	assignParticularConfig();

}

EnplaneAgent::~EnplaneAgent()
{

}

void EnplaneAgent::assignParticularConfig()
{
	// assign carrier space.
	int nDepFltIndex = getLogEntry().getDepFlight();
	if(nDepFltIndex < 0 )
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("Enplane Pax:%s, cannot find departure plane"), screenMobAgent());
		throw new OnBoardSimEngineFatalError( new OnBoardDiagnose( strErrorMsg) );
	}

	Flight *pFlight = getEngine()->m_simFlightSchedule.getItem( nDepFltIndex );
	char lpszACType[1024] = {0};
	pFlight->getACType( lpszACType );

	carrierSpace* pCarrierSpace = 
		getConfig()->m_pCarrierSpaces->getCarrierSpace( lpszACType );
	if(pCarrierSpace == NULL)
	{
		CString strAgentMsg = this->getPersonErrorMsg();
		CString strErrorMsg;
		strErrorMsg.Format(_T("Enplane Passenger:%s, cannot find carrier"), strAgentMsg);
		throw new OnBoardSimEngineFatalError( new OnBoardDiagnose( strErrorMsg) );
	}

	setCarrierSpace( pCarrierSpace );

}

ElapsedTime EnplaneAgent::moveTime (void) const
{
	return MobileAgent::moveTime();
}

bool EnplaneAgent::findMovementPath()
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

carrierEntryDoor* EnplaneAgent::getEntryDoor()
{
	// debug purpose.

	carrierGround& _ground = m_pCarrierSpace->getCarrierDeck(0)->getCarrierGround();
    return _ground.getCarrierEntryDoor();

}

carrierEmergencyDoor* EnplaneAgent::getEmergencyDoor()
{
	return NULL;
}

carrierStair* EnplaneAgent::getStair()
{
	return NULL;
}

carrierStorage* EnplaneAgent::getStorage()
{
	return NULL;
}



