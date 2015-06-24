#pragma once

/**
	Class to provide conversion functionality from aircraft physical layout to 
	logistic carrier space. Such as: deck to deck ground gird map. non-moveable elements on deck to barriers. 
*/

class carrierElement;
class carrierSpace;
class carrierDeck;
class carrierGround;

class CAircaftLayOut;
class CDeck;
class CAircraftPlacements;

class CarrierSpaceRenderer
{
public:
	//CarrierSpaceRenderer(CAircraftPlacements* pPlacements, carrierSpace* pNewCarrierSpace);
	CarrierSpaceRenderer(CAircaftLayOut* pLayout, carrierSpace* pNewCarrierSpace);
	~CarrierSpaceRenderer(void);


private:

	void buildCarrierSpace( CAircaftLayOut* pLayout, carrierSpace* pNewCarrierSpace);
	void buildCarrierDeck(CDeck* pDeckLayout, carrierDeck* pCarrierDeck);
	void buildCarrierDoors(CDeck* pDeckLayout, carrierGround& ground);
	void buildCarrierSeats(CDeck* pDeckLayout, carrierGround& ground);
	void buildCarrierCabins(CDeck* pDeckLayout, carrierGround& ground);
	void buildCarrierStorageDevices(CDeck* pDeckLayout, carrierGround& ground);
	void buildCarrierMiscDevices(CDeck* pDeckLayout, carrierGround& ground);


	CAircraftPlacements* _placements;
};
