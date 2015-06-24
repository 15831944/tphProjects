#pragma once
#include "..\InputAirside\ALTAirport.h"
#include "..\Common\Referenced.h"
#include "..\Common\ref_ptr.h"
#include ".\ALTObject3D.h"
#include ".\AirsideLevelList.h"
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/IntersectionNodesInAirport.h"

#include "FilletTaxiway3D.h"
#include "IntersectedStretch3D.h"


class C3DView;
class CAirsideGround;
class SelectableSettings;

class CAirport3D
{
public:
	CAirport3D(int _Id);
	virtual ~CAirport3D(void);
	
	ALTAirport m_altAirport;

	int GetID()const{ return m_nID; }
	bool RemoveObject(int id);
	ALTObject3D* AddObject(ALTObject* pObj);


	ALTObject3D* GetObject3D(int id);	
	virtual void DrawSelectOGL(C3DView * pView,  SelectableSettings& selsetting);

	void GetObject3DList(ALTObject3DList& objList)const;

	void UpdateAddorRemoveObjects();

	bool ReflectChangeOf(ALTObject3DList vObj3Ds);
	void ReflectRemoveOf(ALTObject3DList vObj3Ds);

	void UpdateObjectsRelations();	

	CAirsideLevelList& GetLevelList(){ return m_vLevelList; }

	void DrawOGL(C3DView * pView,bool vWithfloors = true);
	void RenderNodeNames(C3DView * pView);
	void RenderHoldLineNames(C3DView* pView);
	//read data form the database
	void ReadGroundInfo();
	void Update();

    void RenderStenciles();
protected: 
	void AutoSnap(ALTObject3DList vObj3Ds);
	
	void RenderNodes(C3DView *pView);
	void RenderToDepth(C3DView  *pView);

	virtual void Init();
	
	void UpdateObject(int objID);


public :
	int m_nID;
	

	ALTObject3DList m_vRunways;
	ALTObject3DList m_vHeliport;
	ALTObject3DList m_vTaxways;
	ALTObject3DList m_vStands;
	ALTObject3DList m_vDeicePad;
	ALTObject3DList m_vStretchs;
	ALTObject3DList m_vRoadIntersections;
	ALTObject3DList m_vVehiclePoolParking;
	ALTObject3DList m_vrTrafficLights;
	ALTObject3DList m_vrTollGate;
	ALTObject3DList m_vrStopSign;
	ALTObject3DList m_vrYieldSign;
	ALTObject3DList m_vCircleStretchs;
	ALTObject3DList m_vStartPositions;
	ALTObject3DList m_vMeetingPoints;
	//ALTObject3DList m_vrLaneAdapter;

	IntersectionNodesInAirport m_vAirportNodes;
	CFilletTaxiway3DInAirport m_vFilletTaxiways;
	CIntersectedStretch3DInAirport m_vIntersectedStretch;
	
	CAirsideLevelList m_vLevelList;



};


