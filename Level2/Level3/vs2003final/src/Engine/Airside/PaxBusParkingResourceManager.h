#pragma once
#include "resourcemanager.h"
#include "PaxBusParkingInSim.h"
#include "../../InputAirside/PaxBusParking.h"
#include "../../InputAirside/PaxBusParkingPlaceList.h"

class AirsidePaxBusParkSpotInSim;
class AirsidePaxBusParkLinkageInSim;
class CPaxBusParkingResourceManager :
	public ResourceManager
{
public:
	CPaxBusParkingResourceManager(void);
	~CPaxBusParkingResourceManager(void);

public:
	void initialize(int nProjID,int nAirportID);

	size_t GetPaxBusParkingCount();
	AirsidePaxBusParkSpotInSim *GetPaxBusParkingByIndex(size_t nIndex);

	AirsidePaxBusParkSpotInSim* GetBestMatch(const CString& strGateID);

private:
	AirsidePaxBusParkSpotInSim* GetParkSpotInSim(const ALTObjectID& spotID);
protected:
// 	CPaxBusParkingList m_paxParkingList;
// 	std::vector<CPaxBusParkingInSim::RefPtr> m_vParkingInSim;

	PaxBusParkingPlaceList m_paxBusParkLinkageList;
	std::vector<AirsidePaxBusParkSpotInSim*> m_vParkSpotInSim;
	std::vector<AirsidePaxBusParkLinkageInSim*> m_vLinkageInSim;
};

class AirsidePaxBusParkLinkageInSim
{
public:
	AirsidePaxBusParkLinkageInSim(PaxBusParkingPlace* pParkLinkage);
	~AirsidePaxBusParkLinkageInSim();

	void InitDefaultData(ALTObjectList vPaxBusParkSpot);
	bool GetBestMatchObject(const ALTObjectID& gateID,ALTObjectID& resultObject);
private:
	bool GetOneToOneObject(const ALTObjectID& gateID,ALTObjectID& resultObject);
	bool GetRandomObject(const ALTObjectID& gateID,ALTObjectID& resultObject);
private:
	PaxBusParkingPlace* m_pParkLinkage;
	ALTObjectIDList m_vObjectList;
};