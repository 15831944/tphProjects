#pragma once

#include "../InputAirside/ALTObjectGroup.h"
#include "../Common/termfile.h"
#include "..\InputAirside\ALT_BIN.h"
#include "AirsideReportAPI.h"

class AIRSIDEREPORT_API  CAirsideReportNode
{
public:

	enum ReportNodeType
	{
		ReportNodeType_All = 0,
		ReportNodeType_WayPointStand,
		ReportNodeType_Intersection,
		ReportNodeType_Taxiway,

		//add new type before this line
		ReportNodeType_Count
	};

	const static CString AirsideReportNodeTypeName[];
public:
	CAirsideReportNode(void);
	virtual ~CAirsideReportNode(void);

	
	void SetNodeType(ReportNodeType nodeType);
	void SetNodeID(int nID);

	ReportNodeType GetNodeType();
	int GetNodeID();

	CString GetNodeName();
	void SetNodeName(const CString& strName);
	
	
	const ALTObjectGroup& GetObjGroup() const { return m_objGroup; }
	void SetObjGroup(ALTObjectGroup val) { m_objGroup = val; }
public:
	//int node, get the object id list
	void InitNode();

	bool IsDefault();

	bool EqualTo(CAirsideReportNode& rpNode);

	void AddObj(int nObjID);
	//check the node has object or not
	bool IncludeObj(int nObjID);

	void SetObjIDList(std::vector<int>& vObjID);
protected:
	ReportNodeType m_objType;
	CString m_strName;

	//ReportNodeType_Intersection
	int m_nObjID;//intersection node id

	//ReportNodeType_WayPointStand
	//ReportNodeType_Taxiway
	ALTObjectGroup m_objGroup;//group

	//
private:
	// the object id list
	std::vector<int> m_vObjID;
public:
	void ExportFile(ArctermFile& _file) ;
	void ImportFile(ArctermFile& _file) ;
};



class AIRSIDEREPORT_API CAirsideReportRunwayMark
{
public:
	CAirsideReportRunwayMark();
	~CAirsideReportRunwayMark();


	void SetRunway(int nRunwayID,RUNWAY_MARK runwayMark,const CString& strMarkName);
    bool operator==(const CAirsideReportRunwayMark& other) const;
public:
	int m_nRunwayID; 
	RUNWAY_MARK m_enumRunwayMark;
	CString m_strMarkName;

};




