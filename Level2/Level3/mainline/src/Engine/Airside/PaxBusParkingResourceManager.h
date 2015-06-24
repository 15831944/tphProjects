#pragma once
#include "resourcemanager.h"
#include "PaxBusParkingInSim.h"
#include "../../InputAirside/PaxBusParking.h"

class CPaxBusParkingResourceManager :
	public ResourceManager
{
public:
	CPaxBusParkingResourceManager(void);
	~CPaxBusParkingResourceManager(void);

public:
	void initialize(int nProjID,int nAirportID);

	size_t GetPaxBusParkingCount();
	CPaxBusParkingInSim *GetPaxBusParkingByIndex(size_t nIndex);

	CPaxBusParkingInSim *GetBestMatch(const CString& strGateID,Gatetype gateType);
protected:
	CPaxBusParkingList m_paxParkingList;
	std::vector<CPaxBusParkingInSim::RefPtr> m_vParkingInSim;
};
