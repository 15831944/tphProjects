#pragma once 

#include "AirsideImportExportManager.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API  CGoAroundCriteriaDataProcess
{
public:
	CGoAroundCriteriaDataProcess(void);
	CGoAroundCriteriaDataProcess(int nProjID);
	~CGoAroundCriteriaDataProcess(void);
public:
	void ReadData(int nProjID);
	void SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ExportGoAroundCriteriaData(CAirsideExportFile& exportFile);
	static void ImportGoAroundCriteriaData(CAirsideImportFile& importFile);

	int GetProjID(void) const;
	void SetProjID(int nProjID);

	//Distance between Landed aircraft on runway and approach aircraft of threshold (m)
	double GetLBLSRWDIS(void) const;
	void SetLBLSRWDIS(double dLBLSRWDIS);

	//Approach separation distance (m)
	double GetLBLSRWADIS(void) const;
	void SetLBLSRWADIS(double dLBLSRWADIS);

	int GetLBLIRWFPTDIFF(void) const;
	void SetLBLIRWFPTDIFF(int nLBLIRWFPTDIFF);

	double GetLBLIRWFPDDIFF(void) const;
	void SetLBLIRWFPDDIFF(double dLBLIRWFPDDIFF);

	int GetLBLIRWRWTDIFF(void) const;
	void SetLBLIRWRWTDIFF(int nLBLIRWRWTDIFF);

	double GetLBLIRWRWDDIFF(void) const;
	void SetLBLIRWRWDDIFF(double dLBLIRWRWDDIFF);

	double GetLBTSRWDIS(void) const;
	void SetLBTSRWDIS(double dLBTSRWDIS);

	int GetLBTIRWTDIFF(void) const;
	void SetLBTIRWTDIFF(int nLBTIRWTDIFF);
protected:
	int m_nProjID;

	//Landing behind landing on same runway
	double m_dLBLSRWDIS;         //Distance between Landed aircraft on runway and approach aircraft of threshold (m)
	double m_dLBLSRWADIS;        //Approach separation distance (m)

	//on intersecting runway
	//At intersection in flight path
	int m_nLBLIRWFPTDIFF;       //Max time difference
	double m_dLBLIRWFPDDIFF;    //Max Distance difference

	//At intersection on runway
	int m_nLBLIRWRWTDIFF;       //Max time difference
	double m_dLBLIRWRWDDIFF;    //Max Distance difference

	//Landing behind takeoff 
	//on same runway
	double m_dLBTSRWDIS;   //Distance between taking off aircraft and landing threshold (m)
	//on intersecting runway
	int m_nLBTIRWTDIFF;    //Time difference (mins)
};