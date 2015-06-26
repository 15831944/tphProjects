#pragma once

#include "ApproachSeparationType.h"
#include "ApproachSeparationItem.h"
#include "InputAirsideAPI.h"



class INPUTAIRSIDE_API LandingBehindTakeoffSameRunway : public ApproachSeparationItem
{
public:
	LandingBehindTakeoffSameRunway(int nProjID);
	~LandingBehindTakeoffSameRunway(void);;
public:
	double  GetLeadDisFromThreshold(){	return m_dLeadDisFromThreshold; }
	void SetLeadDisFromThreshold(double dNewValue){	m_dLeadDisFromThreshold = dNewValue; }
	void ReadData();
	void SaveData();
	void UpdateData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
private:
	int m_nID;
	double m_dLeadDisFromThreshold;
};

class INPUTAIRSIDE_API LandingBehindTakeoffIntersectionRunway : public AircraftClassificationBasisApproachSeparationItem
{
public:
	LandingBehindTakeoffIntersectionRunway(int nProjID);
	~LandingBehindTakeoffIntersectionRunway();    
};