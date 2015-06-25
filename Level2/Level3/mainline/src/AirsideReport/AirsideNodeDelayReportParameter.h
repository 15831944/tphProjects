#pragma once
#include "parameters.h"
#include "AirsideReportNode.h"


class CXTPPropertyGrid;
class AIRSIDEREPORT_API CAirsideNodeDelayReportParameter :
	public CParameters
{
public:
	CAirsideNodeDelayReportParameter(void);
	~CAirsideNodeDelayReportParameter(void);


public:
	std::vector<CAirsideReportNode>& getReportNode(){ return m_vReportNode;}

	bool IsReportNodeExist(CAirsideReportNode& rpNode);
	void AddReportNode(CAirsideReportNode& rpNode);
	bool DelReportNode(size_t nIndex);
	size_t getReportNodeCount();
	void ClearReportNode();
	CAirsideReportNode getReportNode(size_t nIndex);

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

public:
	virtual void LoadParameter();
	//virtual void InitParameter(CXTPPropertyGrid * pGrid);
	virtual void UpdateParameter();


protected:
	std::vector<CAirsideReportNode> m_vReportNode;
	int                             m_nSubType;
public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;	
};
