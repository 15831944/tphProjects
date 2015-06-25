#pragma once
#include "parameters.h"

class AIRSIDEREPORT_API  CFlightActivityParam :
	public CParameters
{
public:
	CFlightActivityParam(void);
	~CFlightActivityParam(void);

public:
	virtual void LoadParameter();
//	virtual void InitParameter(CXTPPropertyGrid * pGrid);
	virtual void UpdateParameter();
	virtual CString GetReportParamName();
	

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}


protected:
	int m_nSubType;
public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;	
};
