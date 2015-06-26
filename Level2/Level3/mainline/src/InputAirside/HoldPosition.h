#pragma once

#include "ALTObject.h"
#include "../Common/GetIntersection.h"
#include "IntersectionNode.h"
#include "InputAirsideAPI.h"


typedef std::vector<IntersectionNode> IntersectionNodeList;
class INPUTAIRSIDE_API HoldPosition{
public:
	HoldPosition();
	HoldPosition(const IntersectionNode& node , double dintersect,int nObjectID);
	HoldPosition(const HoldPosition& );

	HoldPosition& operator = (const HoldPosition& );
	~HoldPosition();


	CString m_strSign;
	double m_distToIntersect;    // pos to the intersect pos;   cm
	double m_intersectPos;       //real distance of path       cm
	IntersectionNode m_intersectNode;

	IntersectionNode& GetIntersectNode();
	const IntersectionNode& GetIntersectNode()const;

	bool operator < (const HoldPosition& other)const;
	bool operator == ( const HoldPosition& other)const;

	bool IsFirst()const;

//	HoldPositionInfo GetHoldInfo()const;
	double GetPosition()const;
	void SetPosition(double pos);

	int GetUniqueID() const;
	void SetUniqueID(int val);
public:
	static void ReadHoldPositionList(const int nObjectID,std::vector<HoldPosition>& vHoldPosition);
	void ReadData(CADORecordset& adoRecordset);
	//nOjectID is the object(taxiway or runway id) which the hold position belong to
	void SaveData(int nObjectID);

	//read hold information
	//throw CADOException if failed
	void ReadHold(int nHoldID);

	void DeleteData();
	void UpdateData();
	//the hold's ID
	int m_nUniqueID;

	//taxiway or runway id
	int m_nObjectID;

public:
	static void ExportHoldPositions(CAirsideExportFile& exportFile);
	static void ImportHoldPositions(CAirsideImportFile& importFile);


}; 
typedef std::vector<HoldPosition> HoldPositionList;

class UnExsitHoldPredicate
{

public:
	UnExsitHoldPredicate(const HoldPositionList& holdinfo):m_holdInfoList(holdinfo){}


	bool operator()(const HoldPosition& hold)const;

protected:
	int m_objID;
	const HoldPositionList& m_holdInfoList;
};

class CPath2008;
class CLine2008;

class INPUTAIRSIDE_API HoldPositionFunc
{
public:

	//void static CalculateIntersectionNodes(const HoldPositionList& holdpositions, IntersectNodeList& vNodes);

	bool static CaculateHoldPositionLine(const CPath2008& path,const DistanceUnit& width, const HoldPositionList& holdlist, int idx, CLine2008& line);

	//void static UpdateIntersectionList(int nObjID, IntersectNodeList& lstIntersecNodeReturn);

};

