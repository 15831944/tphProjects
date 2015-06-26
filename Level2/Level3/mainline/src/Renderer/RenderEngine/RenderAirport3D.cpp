#include "StdAfx.h"
#include ".\renderairport3d.h"
//#include "Render3DScene.h"
//#include "InputAirside\Runway.h"
//#include "InputAirside\Taxiway.h"
//#include "InputAirside\stand.h"
//#include "InputAirside\DeicePad.h"
//#include "InputAirside/Heliport.h"
//#include "InputAirside/CircleStretch.h"
//#include "InputAirside/StartPosition.h"
//#include "InputAirside/MeetingPoint.h"
//#include "InputAirside/DeicePad.h"
//#include "InputAirside/TrafficLight.h"
//#include "InputAirside/Stretch.h"
//#include "InputAirside/TollGate.h"
//#include "InputAirside/VehiclePoolParking.h"
//#include "InputAirside/StopSign.h"
//#include "InputAirside/YieldSign.h"
//#include "InputAirside\Intersections.h"
//#include "InputAirside\InputAirside.h"
//#include "InputAirside\ALTAirport.h"
//
//
//CRenderAirside3D::CRenderAirside3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)
//	: CModeBase3DObject(pNode,p3DScene,guid)
//	, m_vFilletTaxiways(p3DScene)
//	, m_vIntersectedStretch(p3DScene)
//{
//
//	/*m_groundfloors = p3DScene->AddSceneNode(guid);
//	m_lstRunway.SetRenderAirport3D(this);
//	m_lstHeliport.SetRenderAirport3D(this);
//	m_lstTaxiway.SetRenderAirport3D(this);
//	m_lstStand.SetRenderAirport3D(this);
//	m_lstDeicePad.SetRenderAirport3D(this);
//	m_lstStretch.SetRenderAirport3D(this);
//	m_lstRoadIntersection.SetRenderAirport3D(this);
//	m_lstPoolParking.SetRenderAirport3D(this);
//	m_lstTrafficLight.SetRenderAirport3D(this);
//	m_lstToolGate.SetRenderAirport3D(this);
//	m_lstStopSign.SetRenderAirport3D(this);
//	m_lstYieldSign.SetRenderAirport3D(this);
//	m_lstCircleStretch.SetRenderAirport3D(this);
//	m_lstStartPosition.SetRenderAirport3D(this);
//	m_lstMeetingPoint.SetRenderAirport3D(this);
//;
//	m_lstRunway.Set3DScene(p3DScene);
//	m_lstHeliport.Set3DScene(p3DScene);
//	m_lstTaxiway.Set3DScene(p3DScene);
//	m_lstStand.Set3DScene(p3DScene);
//	m_lstDeicePad.Set3DScene(p3DScene);
//	m_lstStretch.Set3DScene(p3DScene);
//	m_lstRoadIntersection.Set3DScene(p3DScene);
//	m_lstPoolParking.Set3DScene(p3DScene);
//	m_lstTrafficLight.Set3DScene(p3DScene);
//	m_lstToolGate.Set3DScene(p3DScene);
//	m_lstStopSign.Set3DScene(p3DScene);
//	m_lstYieldSign.Set3DScene(p3DScene);
//	m_lstCircleStretch.Set3DScene(p3DScene);
//	m_lstStartPosition.Set3DScene(p3DScene);
//	m_lstMeetingPoint.Set3DScene(p3DScene);*/
//
//
//	ALTObjectList vObjectList;
//	ALTAirport* pAirport = GetALTAirport();
//	if (pAirport)
//	{
//		pAirport->GetEntireALTObjectList(vObjectList);
//
//		int nAirportID = pAirport->getID();
//		m_vAirportNodes.Init(nAirportID, vObjectList);
//		m_vFilletTaxiways.Init(nAirportID, m_vAirportNodes);
//		m_vIntersectedStretch.Init(nAirportID, m_vAirportNodes);
//	}
//}
//
//CRenderAirside3D::~CRenderAirside3D(void)
//{
//
//}
//
//void CRenderAirside3D::Update()
//{
//	//draw airport itself
//
//
//	//draw airport objects
//	//m_lstRunway.Update();
//	//m_lstHeliport.Update();
//	//m_lstTaxiway.Update();
//	//m_lstStand.Update();
//	//m_lstDeicePad.Update();
//	//m_lstStretch.Update();
//	//m_lstRoadIntersection.Update();
//	//m_lstPoolParking.Update();
//	//m_lstTrafficLight.Update();
//	//m_lstToolGate.Update();
//	//m_lstStopSign.Update();
//	//m_lstYieldSign.Update();
//	//m_lstCircleStretch.Update();
//	//m_lstStartPosition.Update();
//	//m_lstMeetingPoint.Update();
//
//
//	m_vFilletTaxiways.Update();
//	m_vIntersectedStretch.Update();
//}
//
//void CRenderAirside3D::Update3D( ALTAirport *pAirport, int nUpdateCode /*= Terminal3DUpdateAll*/ )
//{
//	//update itself
//	
//
//	Update();
//
//}
//
//
////update the objects of airport
//bool CRenderAirside3D::UpdateObject( const CGuid& guid, const GUID& cls_guid )
//{
//	
//
//	return false;
//}
//void* CRenderAirside3D::OnQueryDataObject( const CGuid& guid, const GUID& class_guid )
//{
//	ALTAirport *pAirport = GetALTAirport();
//
//	if(pAirport == NULL)
//		return NULL;
//	
//	return pAirport->GetObject(guid,class_guid);
//
//}
//
//bool CRenderAirside3D::OnQueryDataObjectList( std::vector<void*>& vData, const GUID& cls_guid )
//{
//	ALTAirport *pAirport = GetALTAirport();
//
//	if(pAirport == NULL)
//		return NULL;
//	if(pAirport->GetObjectList(cls_guid,vData))
//		return true;
//
//	return false;
//}
//
//ALTAirport* CRenderAirside3D::GetALTAirport() const
//{
//	return m_p3DScene ? m_p3DScene->OnQueryInputAirside()->GetAirport(GetGuid()) : NULL;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////
////Set3DScene
//CRenderAirport3DList::CRenderAirport3DList()
//{
//	//m_p3DScene = NULL;
//}
//
//CRenderAirport3DList::~CRenderAirport3DList()
//{
//
//}
//
//void CRenderAirport3DList::UpdateObject( const CGuid& guid, const GUID& cls_guid )
//{
//	Shared3DList::iterator iter =m_3DObjects.begin();
//	for (; iter != m_3DObjects.end(); ++iter)
//	{
//		if((*iter)->UpdateObject(guid,cls_guid))
//			return;
//	}
//}