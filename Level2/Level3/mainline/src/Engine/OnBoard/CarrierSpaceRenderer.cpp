#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "CarrierSpaceRenderer.h"
#include "carrierSpaceDiagnosis.h"
#include "LocatorSite.h"
#include "carrierSpace/carrierDefs.h"
#include "Services/SeatAssignmentService.h"
#include "PathFinding/OBRect.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "InputOnBoard/Deck.h"
#include "InputOnboard/AircraftPlacements.h"
#include "InputOnBoard/Seat.h"
#include "InputOnBoard/Door.h"


CarrierSpaceRenderer::CarrierSpaceRenderer(CAircaftLayOut* pLayout, carrierSpace* pNewCarrierSpace)
{
	_placements = pLayout->GetPlacements();
	ASSERT(_placements != NULL);

	buildCarrierSpace(pLayout, pNewCarrierSpace);
}

CarrierSpaceRenderer::~CarrierSpaceRenderer(void)
{
}

void CarrierSpaceRenderer::buildCarrierSpace(CAircaftLayOut* pLayout, carrierSpace* pNewCarrierSpace)
{
	CDeckManager* pDecks = pLayout->GetDeckManager();
	if(pDecks->getCount() < 1)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("aircraft mode:%s .no deck inside."), pLayout->GetName() );
		throw new OnBoardSimEngineConfigException( new OnBoardDiagnose(strErrorMsg) );
	}
	
	for (int i = 0; i < pDecks->getCount(); i++)
	{
		CDeck* pDeckLayout = pDecks->getItem( i );
		
		carrierDeck* pNewCarrierDeck = new carrierDeck;
		buildCarrierDeck( pDeckLayout, pNewCarrierDeck);

		if( !pNewCarrierDeck->isValid())
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("aircraft mode:%s deck:%s .layout define invalid."), pLayout->GetName(), 
				pDeckLayout->GetName());

			throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
		}

		carrierSpaceDiagnosis()->traceDeckGroundLayout( pNewCarrierDeck );

        pNewCarrierSpace->addCarrierDeck( pNewCarrierDeck );
	}

}

/*
** 
step 1: get deck's out bound rect, then tiled with grid.

		 ____^_____					_
		|/        \| Cols: '|'      ^
		/          \				|
  /----|| CARRIER  ||----\			|
 /-----||          ||-----\			|
		|          |			  length(X axis)
		\          /				|
		|\________/|				_
           /__|__\   Rows: '-'		
									
		|<-width-->|(Y axis)				

step 2: treat cell on grid as barrier object which is outside carrier body.

step 3: build non-moveable carrier elements (seats, doors, stairs, misc device, etc) on deck.
		3.1	locate carrier elements on carrier ground.
		3.2	treat each non-moveable carrier element as barrier objects,
			according to element's physical location and out bound rect.


		_____^_____
		|x|x|x|x|x|
		|x|o|o|x|x|
  /----||x|x|o|x|x||----\
 /-----||x|x|o|x|x||-----\
		|x|x|o|o|x|
		|o|o|o|o|o|
		-----------
		  /__|__\

**
*/
void CarrierSpaceRenderer::buildCarrierDeck(CDeck* pDeckLayout, carrierDeck* pCarrierDeck)
{
	
	//---------------------------------------------------------------
	// step 1: get deck's out bound rect, then tiled with grid cell.

	ARCVector2 mRectMin = pDeckLayout->mRectMin;
	ARCVector2 mRectMax = pDeckLayout->mRectMax;
	double carrierMaxLength = mRectMax[VX] - mRectMin[VX];
	double carrierMaxWidth  =  mRectMax[VY] - mRectMin[VY];

	// minimal carrier deck dimension in OnBoard simEngine: 5m * 2m.
	if( carrierMaxLength < 500 || carrierMaxWidth < 200)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("deck:%s. carrier deck space is too small, please check deck definition"), pDeckLayout->GetName() );
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}

	OBGeo::FRect _outBoundRect(mRectMin[VX],  mRectMin[VY], carrierMaxLength, carrierMaxWidth);
	carrierGround& _carrierGnd = pCarrierDeck->getCarrierGround();
	_carrierGnd.setHeight( pDeckLayout->RealAltitude() );
	_carrierGnd.generateGround( _outBoundRect );

	//---------------------------------------------------------------
	// step 2: treat cell on grid as barrier object which is outside carrier body.
	ARCPath& deckOutLine = pDeckLayout->GetOutline();
	if( deckOutLine.size() < 3 )
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("deck:%s. deck outline is invalid"), pDeckLayout->GetName() );
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}
	////_carrierGnd.setOutsideLocatorSitesAsBarriers( deckOutLine );

	//---------------------------------------------------------------
	// step 3: build non-moveable carrier elements (seats, doors, stairs, misc device, etc) on deck.
	//		3.1	locate carrier elements on carrier ground.
	//		3.2	treat each non-moveable carrier element as barrier objects,
	//		according to element's physical location and out bound rect.
 
	buildCarrierDoors(pDeckLayout, _carrierGnd);
	buildCarrierCabins(pDeckLayout, _carrierGnd);
	buildCarrierSeats(pDeckLayout, _carrierGnd);
	buildCarrierMiscDevices(pDeckLayout, _carrierGnd);
	
}

//	1. init seat locating site on carrier ground
//  2. init seat relationship: same row, same group.
//  3. seat characteristic: near by emergency door, etc.
void CarrierSpaceRenderer::buildCarrierSeats(CDeck* pDeckLayout, carrierGround& ground)
{
	//CAircraftSeatsManager* pSeatMananger = _placements->GetSeatManager();
	//ASSERT( pSeatMananger != NULL );

	//pSeatMananger->ReadData();

	CSeatDataSet* pSeatsLayout = _placements->GetSeatData();
	ASSERT(pSeatsLayout != NULL);
	int nSeatCounts = pSeatsLayout->GetItemCount();
	if( nSeatCounts <= 0 )
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: no seat defined on deck"), pDeckLayout->GetName() );
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}

	for (int i = 0; i < nSeatCounts; i++)
	{
		CSeat* pSeatLayout = pSeatsLayout->GetItem( i );
		ASSERT( pSeatLayout != NULL );

		// just load seats on current deck
		if( pSeatLayout->GetDeck()->GetID() != pDeckLayout->GetID())
			continue;

		ARCVector3 _location = pSeatLayout->GetPosition();
		CPoint2008 _position;
		_position.setPoint( _location[VX], _location[VY], _location[VZ]);
		LocatorSite* pSeatLocator = ground.getLocatorSite( _position );
		
		ARCVector3 _entryPos = pSeatLayout->GetEntryPos();
		CPoint2008 _EntryPos;
		_EntryPos.setPoint( _entryPos[VX], _entryPos[VY], _entryPos[VZ]);
		LocatorSite* pSeatEntryLocator = ground.getLocatorSite( _EntryPos );

		if(NULL == pSeatLocator || NULL == pSeatEntryLocator)
		{
			CString strErrorMsg;
			ARCVector3 location = pSeatLayout->GetPosition();
			
			strErrorMsg.Format(_T("Seat: %s [%.2f,%.2f](Deck:%s) is invalid"), 
				pSeatLayout->GetIDName().GetIDString(),
				location[VX],location[VY],
				pDeckLayout->GetName() );
			throw new OnBoardSimEngineConfigException(
				new OnBoardDiagnose(strErrorMsg) );
		}

		carrierSeat * pNewSeat = new carrierSeat( pSeatLocator, ground, pSeatLayout);
		pNewSeat->setEntryLocatorSite( pSeatEntryLocator );
		ground.addCarrierSeat( pNewSeat );

		seatAssignService()->registerSeat( pNewSeat );

	}
/*	
	CSeatGroupDataSet* pSeatGroupArray = pSeatMananger->GetSeatGroupData();


	int nSeatGroupCount = pSeatGroupArray->GetItemCount();
	if( nSeatGroupCount == 0)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("%s: no seat defined on deck"), pDeckLayout->GetName() );
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}

	for (int i = 0 ; i < nSeatGroupCount; i++)
	{//seat group
		CSeatGroup* pSeatGroup = pSeatGroupArray->GetItem( i );
		ASSERT(pSeatGroup != NULL);
		
		int nSeatRowCount = pSeatGroup->GetRowCount();
		if( nSeatRowCount == 0)
			continue;

		for (int j = 0; j < nSeatRowCount; j++)
		{//seat row
			CSeatRow* pSeatRow = pSeatGroup->GetRow( j );
			ASSERT( pSeatGroup != NULL );

			int nSeatCount = pSeatRow->GetSeatCount();
			if(nSeatCount == 0)
				continue;

			for (int k = 0 ; k < nSeatCount; k++)
			{// seat
				CSeat* pSeatLayout = pSeatRow->GetSeat( k );
				ASSERT( pSeatLayout != NULL );
				//
				// seat space renderer.
				ARCVector3 _location = pSeatLayout->GetPosition();
				CPoint2008 _position;
				_position.setPoint( _location[VX], _location[VY], ground.getHeight());

				LocatorSite* pSeatLocator = ground.getLocatorSite( _position );
				if(NULL == pSeatLocator )
				{
					CString strErrorMsg;
					strErrorMsg.Format(_T("Seat%s: (Deck:%s) %s Seat: %s invalid"), 
						pSeatLayout->GetIDName().GetIDString(), pDeckLayout->GetName() );
					throw new OnBoardSimEngineConfigException(
						new OnBoardDiagnose(strErrorMsg) );
				}

				carrierSeat * pNewSeat = new carrierSeat( pSeatLocator, pSeatLayout);
				ground.addSeat( pNewSeat );

				seatAssignService()->registerSeats();

			}// seat


		}// seat row
	}// seat group
*/


}


void CarrierSpaceRenderer::buildCarrierDoors(CDeck* pDeckLayout, carrierGround& ground)
{
	CDoorDataSet& _doors = *_placements->GetDoorData();

	int nDoorCount = _doors.GetItemCount();
	CDoor *pEntryDoorLayout = NULL;
	CDoor *pExitDoorLayout = NULL;
	// well, by now, just set first door as entry door,
	// second door as exit door.
	for (int i = 0 ; i < nDoorCount; i++)
	{
		CDoor *pCurDoor = _doors.GetItem( i );
		if(pCurDoor->GetDeck()->GetID() != pDeckLayout->GetID())
			continue;
		
		if( pEntryDoorLayout == NULL)
			pEntryDoorLayout = _doors.GetItem( i );

		if(pExitDoorLayout == NULL)
			pExitDoorLayout = _doors.GetItem( i );

		if(pExitDoorLayout != NULL && pEntryDoorLayout != NULL )
			break;
	}
	if( pExitDoorLayout == NULL || pEntryDoorLayout == NULL )
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("entry door and exit door invalide on deck: %s."), pDeckLayout->GetName() );
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}

	ARCVector3 _location = pEntryDoorLayout->GetPosition();
	CPoint2008 _positionEntry, _positionExit;
	_positionEntry.setPoint( _location[VX], _location[VY], _location[VZ]);
	LocatorSite* pEntryLocator = ground.getLocatorSite( _positionEntry );

	_location = pExitDoorLayout->GetPosition();
	_positionExit.setPoint( _location[VX], _location[VY], _location[VZ]);
	LocatorSite* pExitLocator = ground.getLocatorSite( _positionExit );
	
	if(pEntryLocator == NULL || pExitLocator == NULL)
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("entry and exit door invalide on deck: %s."),pDeckLayout->GetName());
		throw new OnBoardSimEngineConfigException(
			new OnBoardDiagnose(strErrorMsg) );
	}

	carrierEntryDoor * pNewEntryDoor  = new carrierEntryDoor( pEntryLocator, ground);
	carrierExitDoor * pNewExitDoor = new carrierExitDoor(pExitLocator, ground);

	ground.addCarrierEntryDoors( pNewEntryDoor );
	ground.addCarrierExitDoors( pNewExitDoor );
	//ground.addCarrierEmergencyDoors();
}

void CarrierSpaceRenderer::buildCarrierCabins(CDeck* pDeckLayout, carrierGround& ground)
{
	//ground.addCarrierCabin();
}

void CarrierSpaceRenderer::buildCarrierStorageDevices(CDeck* pDeckLayout, carrierGround& ground)
{
	//ground.addCarrierStorageDevice();
}

// misc elements.
void CarrierSpaceRenderer::buildCarrierMiscDevices(CDeck* pDeckLayout, carrierGround& ground)
{
	//ground.addMiscElements
}


