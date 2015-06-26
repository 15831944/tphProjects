#pragma once

#include "IntersectionNode.h"
#include "../Database/DBElement.h"
#include "Common/path2008.h"
#include "Common/Point2008.h"
#include "InputAirsideAPI.h"

class IntersectionNodesInAirport;
/************************************************************************/
/* CPoint2008 of Fillet Taxiway

*/
/************************************************************************/
class INPUTAIRSIDE_API FilletPoint 
{
friend class FilletTaxiway;
public:
	FilletPoint();
	FilletPoint(int nitemID,const DistanceUnit& distToIntersect);
	~FilletPoint();

	int GetIntersectItemID()const{ return m_nIntersectItemID; }
	DistanceUnit GetDistToIntersect()const;//auto calculate dist
	DistanceUnit GetUsrDist()const;

	DistanceUnit GetRealDist()const;

	void SetIntersectItemID(int itemID){ m_nIntersectItemID = itemID; }
	void SetDistToInterect(const DistanceUnit& dist); //auto calculate dist
	void SetUsrDist(const DistanceUnit& dist); //dist offset auto calculate dist
	

	bool IsAtSameSide(const FilletPoint& otherFillet)const;
	

protected:
	int m_nIntersectItemID;
	DistanceUnit m_dDistToIntersect;
	DistanceUnit m_dUserDist;
};

/************************************************************************/
/* Fillet Taxiway
*/
/************************************************************************/
class INPUTAIRSIDE_API FilletTaxiway //: public DBElement
{
public:
	FilletTaxiway();
	explicit FilletTaxiway(const IntersectionNode& node);
	~FilletTaxiway();
	
	void SetFilletPoint1(const FilletPoint& filtPt);
	void SetFilletPoint2(const FilletPoint& filtPt);

	IntersectionNode& GetIntersectNode();
	const IntersectionNode& GetIntersectNode()const{ return m_IntersectNode; }
	FilletPoint& GetFilletPoint1();
	FilletPoint& GetFilletPoint2();
	void SetActive(bool b);
	bool IsActive()const;

	bool IsOverLapWith(const FilletTaxiway& otherWay)const;
	
	CPoint2008 GetFilletPoint1Pos();
	CPoint2008 GetFilletPoint2Pos();
	CPoint2008 GetFilletPoint1Ext();
	CPoint2008 GetFilletPoint2Ext();
	ARCVector3 GetFilletPoint1Dir();
	ARCVector3 GetFilletPoint2Dir();

	CPath2008 GetFilletPt1AtPath();
	CPath2008 GetFilletPt2AtPath();
	

	DistanceUnit GetFilletPt1Dist()const;
	DistanceUnit GetFilletPt2Dist()const;

	DistanceUnit GetWidth();

	void SetID(int nID);
	int GetID()const;

	void SaveDataToDB();
	
	void GetControlPoints(std::vector<CPoint2008>& vPts);

	
	CPath2008 GetRoutePath(bool needReverse);

	int GetObject1ID()const;
	int GetObject2ID()const;

	void SetIntersectNode(const IntersectionNode& node){ m_IntersectNode = node; }

protected:
	int m_nID;

	IntersectionNode m_IntersectNode;
	
	FilletPoint m_filetPt1;
	FilletPoint m_filetPt2;
	bool m_bActive;

	void CalculateRoutePath();
	CPath2008 m_routePath;

public:


	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nARPID,CString& strSQL)const;

	void GetSelectSQLByIntersectNode(int nInterctNode, CString& strSQL)const;

	void ReadData(CADORecordset& recordset,const IntersectionNodesInAirport& vnodes );
	void DeleteData();
	void SaveData(int nParentID);
public:
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	static void ReadAirportFillets(int nARPid ,	const IntersectionNodesInAirport& vnodes,  std::vector<FilletTaxiway>& vFillets);
	static void ReadIntersectionFillets(const IntersectionNode& intersectNode, std::vector<FilletTaxiway>& vFillets );
};

/************************************************************************/
/* all the fillet taxiways in the airport */
/************************************************************************/
class ALTObject;
class Runway;
class Taxiway;
class IntersectionNodesInAirport;

class INPUTAIRSIDE_API FilletTaxiwayList 
{
public:
	void AddItem(const FilletTaxiway& filtTaxiway); //do not add overlap items
	int FindOverLapFillet(const FilletTaxiway& fillet);	
	int GetCount()const;
	bool Remove(int idx);
	FilletTaxiway& FilletAt(int idx);
	const FilletTaxiway& FilletAt(int idx)const;
public:
	std::vector<FilletTaxiway> m_vData;
};

/************************************************************************/
/*      Airport fillets                                                 */
/************************************************************************/


class INPUTAIRSIDE_API FilletTaxiwaysInAirport 
{
public:
	int GetCount()const;
	FilletTaxiway& ItemAt(int idx);
	const FilletTaxiway& ItemAt(int idx)const;


public:
	
	void UpdateFillets(int nARPID,const IntersectionNodesInAirport& vNodes, const std::vector<int>& vChangeNodeIndexs);
	//
	void SaveData(int nARPID);
	void ReadData(int nARPID,const IntersectionNodesInAirport& vnodes );
	void ReadData(int nARPID);

	static void ExportData(CAirsideExportFile& exportFile);
	static void ImportData(CAirsideImportFile& importFile);
	
protected:
	FilletTaxiwayList m_vFilletWays;

};