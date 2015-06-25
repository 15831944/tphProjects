#pragma once
#include "Airside\resourcemanager.h"
#include "..\InputAirside\AirsideBagCartParkSpot.h"
#include "bagcartsparkingspotinsim.h"
#include "..\InputAirside\BaggageCartParkingLinkage.h"


class BaggageCartParkingLinkageInSimManager;

class CBagCartsParkingSpotResourceManager :
	public ResourceManager
{
public:
	CBagCartsParkingSpotResourceManager(void);
	~CBagCartsParkingSpotResourceManager(void);


public:
	void initialize(int nProjID,int nAirportID);

	size_t GetSpotCount();
	CBagCartsParkingSpotInSim *GetSpotByIndex(size_t nIndex);

	CBagCartsParkingSpotInSim *GetBestMatch();

	
	CBagCartsParkingSpotInSim *GetSpotLinked(ALTObjectID terminalSpot);

protected:
	AirsideBagCartParkSpotList m_lstBagSpot;
	std::vector<CBagCartsParkingSpotInSim::RefPtr> m_vBagSpotInSim;

	//Baggage Cart Spot and Loader/Pusher linkage
	BaggageCartParkingLinkageInSimManager *m_pLinkageManager;




};
