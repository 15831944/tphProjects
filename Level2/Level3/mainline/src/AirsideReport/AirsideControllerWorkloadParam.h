#pragma once
#include "parameters.h"
#include "Common\ALTObjectID.h"


class AIRSIDEREPORT_API CAirsideControllerWorkloadParam :
	public CParameters
{
public:
	enum enumWeight
	{
		weight_PushBacks = 0,
		weight_TaxiStart,
		weight_TakeOff_NoConflict,
		weight_TakeOff_Conflict,
		weight_Landing_NoConflict,
		weight_Landing_Conflict,
		weight_GoArround,
		weight_VacateRunway,
		weight_CrossActive,
		weight_HandOffGround,
		weight_HandOffAir,
		weight_AltitudeChange,
		weight_VectorAir,
		weight_Holding,
		weight_DirectRoute_Air,
		weight_RerouteGround,
		weight_Contribution,
		weight_ExtendedDownwind,




		//add new before this line
		weight_count

	};

	const static CString weightName[];

public:
	CAirsideControllerWorkloadParam(void);
	~CAirsideControllerWorkloadParam(void);

	virtual void LoadParameter();
	virtual void UpdateParameter() ;

	

public:
	std::vector<ALTObjectID > getSector() const;
	std::vector<ALTObjectID > getArea() const;

	int getSectorCout() const;
	ALTObjectID getSector(int nIndex) const;

	void AddSector(const ALTObjectID& sectorID);


	int getAreaCout() const;
	ALTObjectID getArea(int nIndex) const;

	void AddArea(const ALTObjectID& areaID);

	CString getWeightName(enumWeight weight) const;

public:
	void Clear();
protected:
	//sector
	std::vector<ALTObjectID > m_vSector;

	//reporting area
	std::vector<ALTObjectID > m_vArea;


};
