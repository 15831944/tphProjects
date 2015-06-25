#pragma once
#include "parameters.h"
#include "../common/elaptime.h"
#include "AirsideReportNode.h"
class AIRSIDEREPORT_API CAirsideFlightDelayParam :
	public CParameters
{
public:
	CAirsideFlightDelayParam(void);
	virtual ~CAirsideFlightDelayParam(void);


	//operation
public:
	virtual void LoadParameter();
	//virtual void InitParameter(CXTPPropertyGrid * pGrid);
	virtual void UpdateParameter();



	CAirsideReportNode GetStartObject();
	CAirsideReportNode GetEndObject();

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	void SetStartNodeType(CAirsideReportNode::ReportNodeType nType){ m_startNode.SetNodeType(nType);}
	void SetEndNodeType(CAirsideReportNode::ReportNodeType nType){ m_endNode.SetNodeType(nType);}

	void SetStartNode(CAirsideReportNode& node){ m_startNode = node;}
	void SetEndNode(CAirsideReportNode& node){ m_endNode = node;}

protected:


	CAirsideReportNode m_startNode;
	CAirsideReportNode m_endNode;
	//int m_nStartObjectType;//0,alt object;1,intersection node

	//int m_nEndObjectType;//0,alt object;1,intersection node

	//int m_nStartObject;

	//int m_nEndObject;
	int m_nSubType;


public:
	 BOOL ExportFile(ArctermFile& _file) ;
	 BOOL ImportFile(ArctermFile& _file) ;	
};
