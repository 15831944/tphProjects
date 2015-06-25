#pragma once
#include "parameters.h"
#include "AirsideReportNode.h"
class CAirsideFlightFuelBurnParmater :
	public CParameters
{
public:
	CAirsideFlightFuelBurnParmater(){};
	~CAirsideFlightFuelBurnParmater(void){};
public:
	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

	virtual void LoadParameter() {} ;
	virtual void UpdateParameter() {} ;

	//write and load parameter from file
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	virtual CString GetReportParamName();
protected:
	CAirsideReportNode m_FromNode ;
	CAirsideReportNode m_ToNode ;
public:
	void AddFromNode(const CAirsideReportNode& _fromnode) { m_FromNode = _fromnode ;};
	void AddToNode(const CAirsideReportNode& _tonode) { m_ToNode = _tonode ;};

	CAirsideReportNode GetFromNode() {return m_FromNode ;} ;
	CAirsideReportNode GetToNode() { return m_ToNode ;} ;
};
