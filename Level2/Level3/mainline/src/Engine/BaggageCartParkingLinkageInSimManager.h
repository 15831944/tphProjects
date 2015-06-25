#pragma once
#include "..\InputAirside\BaggageCartParkingLinkage.h"
#include "..\Common\ALTObjectID.h"
#include "BagCartsParkingSpotInSim.h"

class CBagCartsParkingSpotInSim;

class BaggageCartParkingLinkageInSim
{
public:
	BaggageCartParkingLinkageInSim(BaggageParkingPlace* pSpotLinakge);
	~BaggageCartParkingLinkageInSim();

public:
	void Initialize( std::vector<CBagCartsParkingSpotInSim::RefPtr>& vBagSpotInSim );

	const ALTObjectID& getTermProcID() const;
	CBagCartsParkingSpotInSim * GetSpotLinked( const ALTObjectID&  terminalSpot );
	CBagCartsParkingSpotInSim * GetOneToOneSpotInSIm( const ALTObjectID& terminalSpot,int _nDestIdLength );
protected:
	BaggageParkingPlace* m_pBagSpotLinkage;

	ALTObjectID m_procID;

	//OneToOneProcess

	//the items contained by this BaggageParkingPlace
	std::vector<CBagCartsParkingSpotInSim *> m_vBagSpotInSim;

};



class BaggageCartParkingLinkageInSimManager
{
public:
	BaggageCartParkingLinkageInSimManager(void);
	~BaggageCartParkingLinkageInSimManager(void);

public:
	void Initialize(int nAirportID,std::vector<CBagCartsParkingSpotInSim::RefPtr>& vBagSpotInSim);

	CBagCartsParkingSpotInSim *GetSpotLinked( const ALTObjectID&  terminalSpot );

protected:
	BaggageCartParkingLinkage m_BaggageCartSpec;
	std::vector<BaggageCartParkingLinkageInSim *> m_vBagCartLinkageInSim;
};
