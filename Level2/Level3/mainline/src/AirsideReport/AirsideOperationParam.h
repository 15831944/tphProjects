#pragma once
#include "parameters.h" 

class AIRSIDEREPORT_API CAirsideOperationParam :
	public CParameters
{
public:
	CAirsideOperationParam(void);
	virtual ~CAirsideOperationParam(void);
	//operation
public:
	virtual void LoadParameter();
	//virtual void InitParameter(CXTPPropertyGrid * pGrid);
	virtual void UpdateParameter();

	//write and load parameter from file
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	virtual CString GetReportParamName();
public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;	
};
