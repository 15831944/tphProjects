#pragma once
#include "IntersectionNode.h"
#include "InputAirsideAPI.h"


class Taxiway;
class Runway;
class Stand;
class DeicePad;
class Stretch;
class CircleStretch;
//Intersections between Runways, taxiways, stands , deice pads
class INPUTAIRSIDE_API IntersectionNodesInAirport
{
public:
	IntersectionNodesInAirport();
	IntersectionNodesInAirport(const IntersectionNodesInAirport&);
	~IntersectionNodesInAirport();
	void AddNode(const IntersectionNode& newNode);
	void AddNodes(const std::vector<IntersectionNode> newNodes );
	void Clear(){ m_vNodeList.clear(); }

	int GetCount()const;
	IntersectionNode& NodeAt(int idx);
	const IntersectionNode& NodeAt(int idx)const;

	static void GetIntersectionNodes(Runway * pRunway, Taxiway * pTaxiway, std::vector<IntersectionNode>& reslts);
	static void GetIntersectionNodes(Taxiway * pTaxiway1, Taxiway * pTaxiway2, std::vector<IntersectionNode>& reslts);
	static void GetIntersectionNodes(Stand * pStand , Taxiway * pTaxiway,std::vector<IntersectionNode>& reslts );
	static void GetIntersectionNodes(DeicePad * pDeicepad, Taxiway * pTaxiway, std::vector<IntersectionNode>& reslts);
	static void GetIntersectionNodes(Runway *pRunway1, Runway * pRunway2,std::vector<IntersectionNode>& reslts );
	static void GetIntersectionNodes(ALTObject *pObj1, ALTObject * pObj2,std::vector<IntersectionNode>& reslts );
	static void GetIntersectionNodes(Stretch* pStretch1, Stretch* pStretch2, std::vector<IntersectionNode>& reslts);
	static void GetIntersectionNodes(CircleStretch* pCirStretch, Stretch* pStretch, std::vector<IntersectionNode>& reslts);

	void GetIntersectionNodeOf(int nObjID, std::vector<IntersectionNode>& reslts)const;
	//return ID of Node
	int FindNodeIndex(int nObjID1, DistanceUnit distofObj1, int nObjID2)const;

	IntersectionNode GetNodeByID(int id) const;
	bool HasNode(int nId)const;

	std::vector<int> ReflectChangeOf(ALTObjectList changObjs,ALTObjectList objList );
	std::vector<int> ReflectRemoveOf(ALTObjectList objList);

	void Init(int nAirportID, ALTObjectList objList);
	static void ExportIntersectionNodes(CAirsideExportFile& exportFile);
	static void ImportIntersectionNodes(CAirsideImportFile& importFile);
	void ReadData(int nAirportID);	
	typedef std::vector<IntersectionNode> IntersectionNodeList ;
  
protected:

	void CaculateIntersectionNodes(ALTObjectList objList);

	void UpdateDataToDB( int nAirportID, const std::vector<int>& vUpdateNodeIndexs );
protected:
	IntersectionNodeList m_vNodeList;
	int m_nAirportID;
};

