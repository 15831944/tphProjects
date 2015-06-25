#pragma once
#include "parameters.h"
#include "AirsideReportNode.h"

class AIRSIDEREPORT_API CFlightOperationalParam :
	public CParameters
{
public:
	CFlightOperationalParam(void);
	virtual ~CFlightOperationalParam(void);

public:
	virtual void LoadParameter();
	virtual void InitParameter(CXTPPropertyGrid * pGrid);
	virtual void UpdateParameter();
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	virtual CString GetReportParamName();

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	CAirsideReportNode GetStartObject();
	CAirsideReportNode GetEndObject();

	void SetStartNodeType(CAirsideReportNode::ReportNodeType nType){ m_startNode.SetNodeType(nType);}
	void SetEndNodeType(CAirsideReportNode::ReportNodeType nType){ m_endNode.SetNodeType(nType);}

	void SetStartNode(CAirsideReportNode& node){ m_startNode = node;}
	void SetEndNode(CAirsideReportNode& node){ m_endNode = node;}

protected:
	int m_nSubType;
	CAirsideReportNode m_startNode;
	CAirsideReportNode m_endNode;
public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;	
};
