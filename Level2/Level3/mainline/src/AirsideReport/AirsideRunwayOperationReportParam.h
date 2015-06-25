#pragma once
#include "parameters.h"
#include "../InputAirside\ApproachSeparationType.h"

#include "AirsideReportNode.h"
class AIRSIDEREPORT_API AirsideRunwayOperationReportParam :
	public CParameters
{
public:
	AirsideRunwayOperationReportParam(void);
	~AirsideRunwayOperationReportParam(void);

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();

	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);


public:
	void ClearRunwayMark();
	void AddRunwayMark(CAirsideReportRunwayMark& reportRunwayMark);

	//==
	bool IsRunwayMarkExist(CAirsideReportRunwayMark& reportRunwayMark);
	bool IsRunwayMarkFit(CAirsideReportRunwayMark& reportRunwayMark);


	//classification
	void ClearClassification();
	bool IsClassificationExist(int nClass);
	void AddClassification(int nClass);

	virtual CString GetReportParamName();
public:
	//the parameter shows in the GUI
	//has "All"
	std::vector<CAirsideReportRunwayMark> m_vRunwayParameter;
	//concrete runway
	std::vector<CAirsideReportRunwayMark> m_vRunway;
	std::vector<FlightClassificationBasisType> m_vTypes;

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

protected:
	int   m_nSubType;

protected:
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
};
