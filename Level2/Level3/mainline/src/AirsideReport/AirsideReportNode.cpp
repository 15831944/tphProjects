#include "StdAfx.h"
#include ".\airsidereportnode.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/IntersectionNode.h"
#include <algorithm>


//ReportNodeType_Unknown = 0,
//ReportNodeType_WayPoint,
//ReportNodeType_Stand,
//ReportNodeType_Intersection,
//ReportNodeType_Taxiway,
//
//
//
//ReportNodeType_Count
const CString CAirsideReportNode::AirsideReportNodeTypeName[] = 
{
	_T("Unkown"),
	_T("WayPoint"),
	_T("Stand"),
	_T("Intersection"),
	_T("Taxiway"),



	//add new before this line
	_T("Count")

};
CAirsideReportNode::CAirsideReportNode(void)
:m_objType(ReportNodeType_All)
,m_nObjID(-1)
,m_strName("All")
{
}

CAirsideReportNode::~CAirsideReportNode(void)
{
}
void CAirsideReportNode::SetNodeType(ReportNodeType nodeType)
{
	m_objType = nodeType;

}
void CAirsideReportNode::SetNodeID(int nID)
{
	m_nObjID = nID;
}
bool CAirsideReportNode::IsDefault()
{
	if (m_objType == ReportNodeType_All)
		return true;

	return false;
}
bool CAirsideReportNode::EqualTo(CAirsideReportNode& rpNode)
{
	if(m_objType == ReportNodeType_WayPointStand || m_objType == ReportNodeType_Taxiway)
	{
		return m_objGroup.getName() == rpNode.GetObjGroup().getName() && m_objType == rpNode.GetNodeType();
	}
	else if(m_objType == ReportNodeType_Intersection)
	{
		return m_nObjID == m_nObjID && m_objType == rpNode.GetNodeType();
	}
	else if(m_objType == ReportNodeType_All)
	{
		return m_objType == rpNode.GetNodeType();
	}



	return false;
}
int CAirsideReportNode::GetNodeID()
{
	return m_nObjID;
}

CAirsideReportNode::ReportNodeType CAirsideReportNode::GetNodeType()
{
	return m_objType;
}

void CAirsideReportNode::SetNodeName( const CString& strName )
{
	m_strName = strName;
}

CString CAirsideReportNode::GetNodeName()
{

	//CString strObjName = _T("");
	//if (m_objType == ReportNodeType_WayPointStand)
	//{
	//	ALTObject *pObject = ALTObject::ReadObjectByID(m_nObjID);
	//	if (pObject)
	//	{
	//		strObjName = pObject->GetObjNameString();
	//	}
	//}
	//else
	//{

	//	IntersectionNode intersecNode;
	//	intersecNode.ReadData(m_nObjID);
	//	strObjName = intersecNode.GetName();

	//}
	return m_strName;
}

bool CAirsideReportNode::IncludeObj( int nObjID )
{
	if(std::find(m_vObjID.begin(), m_vObjID.end(), nObjID) == m_vObjID.end()) // not find
		return false;

	return true;
}

void CAirsideReportNode::InitNode()
{
	
}

void CAirsideReportNode::AddObj( int nObjID )
{
	m_vObjID.push_back(nObjID);
}

void CAirsideReportNode::SetObjIDList( std::vector<int>& vObjID )
{
	m_vObjID.clear();
	m_vObjID = vObjID;
}
void CAirsideReportNode::ExportFile(ArctermFile& _file)
{
	_file.writeField(m_strName);
	_file.writeInt((int)m_objType) ;
	_file.writeInt(m_nObjID);
	int nCount = (int)m_vObjID.size();
	_file.writeInt(nCount);
	for (int i = 0; i < (int)nCount; i++)
	{
		_file.writeInt(m_vObjID[i]);
	}
	_file.writeLine() ;
}
void CAirsideReportNode::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = { 0} ;
	_file.getField(th,1024) ;
	m_strName.Format(_T("%s"),th) ;
	int type =0 ;
	_file.getInteger(type) ;
	m_objType = (ReportNodeType)type ;
	_file.getInteger(m_nObjID) ;

	int nCount = 0;
	_file.getInteger(nCount);
	for (int i = 0; i < (int)nCount; i++)
	{
		int nObject = -1;
		_file.getInteger(nObject);
		m_vObjID.push_back(nObject);
	}
}













//////////////////////////////////////////////////////////////////////////
//CAirsideReportRunwayMark
CAirsideReportRunwayMark::CAirsideReportRunwayMark()
{
	m_nRunwayID = -1; 
	m_enumRunwayMark = RUNWAYMARK_FIRST;
	m_strMarkName = _T("");
}

CAirsideReportRunwayMark::~CAirsideReportRunwayMark()
{

}

void CAirsideReportRunwayMark::SetRunway( int nRunwayID,RUNWAY_MARK runwayMark,const CString& strMarkName )
{
	m_nRunwayID = nRunwayID;
	m_enumRunwayMark = runwayMark;
	m_strMarkName = strMarkName;
}


















