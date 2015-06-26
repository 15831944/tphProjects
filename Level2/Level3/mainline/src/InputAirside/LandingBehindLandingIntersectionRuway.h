#pragma once
#include "ApproachSeparationType.h"
#include "ApproachSeparationItem.h"
#include ".\runway.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API LandingBehindLandingIntersectionRuwayNONLAHSO : public AircraftClassificationBasisApproachSeparationItem
{
public:
	LandingBehindLandingIntersectionRuwayNONLAHSO(int nProjID);
	~LandingBehindLandingIntersectionRuwayNONLAHSO(void);
};

class INPUTAIRSIDE_API LandingBehindLandingIntersectionRuwayLAHSO : public ApproachSeparationItem
{
public:
	LandingBehindLandingIntersectionRuwayLAHSO(int nProjID);
	~LandingBehindLandingIntersectionRuwayLAHSO(void);
public:
	typedef struct MatchIntersectRunway //mir
	{
		int nRunwayID; //id 
		RUNWAY_MARK markLogicRunway;//port
		bool bHadMatch;// if had been assigned nRunwayID with nIntersectRunwayID,bHadMatch is true,else false;
		int nIntersectRunwayID;
		RUNWAY_MARK markIntersectLogicRunway;
	}MatchIntersectRunway;

	void ReadData();
	void SaveData();
	void UpdateData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	std::vector <MatchIntersectRunway *> * GetMatchData(void){return(&m_vectSelected);}

	static CString GetRunwayName(int nRunwayID);
	static CString GetRunwayFirstPort(int nRunwayID);
	static CString GetRunwaySecondPort(int nRunwayID);
protected:
	std::vector <MatchIntersectRunway *> m_vectSelected;
protected:
	void Clear();
};

