#include "stdafx.h"
#include "3DAirsideNode.h"
#include "Common/IARCportLayoutEditContext.h"
#include <common/IEnvModeEditContext.h>
#include "3DFloorNode.h"
#include "RenderRunway3D.h"
#include "RenderAirWayPoint3D.h"
#include "RenderAirHold3D.h"
#include "RenderAirport3D.h"
#include "RenderAirSector3D.h"
#include "RenderAirsideStructure3D.h"
#include "RenderAirRoute3D.h"
#include "RenderAirsideStructure3D.h"
#include "RenderSurface3D.h"
#include "RenderCounter3D.h"
#include "InputAirside/InputAirside.h"
#include "RenderMeetingPoint3D.h"
#include "RenderRunway3D.h"
#include "RenderHeliport3D.h"
#include "RenderTaxiway3D.h"
#include "RenderStand3D.h"
#include "RenderDeicePad3D.h"
#include "RenderStretch3D.h"
#include "RenderPoolParking3D.h"
#include "RenderTrafficLight3D.h"
#include "RenderTollGate3D.h"
#include "RenderStopSign3D.h"
#include "RenderRoadIntersection3D.h"
#include "RenderYieldSign3D.h"
#include "RenderCircleStretch3D.h"
#include "RenderStartPosition3D.h"
#include "RenderSurface3D.h"
#include "RenderStructure3D.h"
#include "InputAirside/ALTAirportLayout.h"
#include "RenderFillet3D.h"

using namespace Ogre;

template <class DataType,class _3DType>
void C3DAirsideNode::SetupObjectList(IARCportLayoutEditContext* pEditContext )
{
	std::vector<void*> vecData ;
	if (pEditContext->OnQueryDataObjectList(vecData,DataType::getTypeGUID()))
	{
		for(int i=0;i<(int)vecData.size();i++)
		{
			DataType* pipe = (DataType*)vecData.at(i);		
				
			_3DType pipe3D = GetCreateChildNode(pipe->getGuid());
			pipe3D.Setup3D(pipe);				
		} 
	}
}

template <class DataType,class _3DType>
void C3DAirportNode::UpdateObjectList( ALTAirportLayout* airportID, IARCportLayoutEditContext* pEditContext )
{
	std::vector<void*> vecData ;	
	if (airportID->GetObjectList(DataType::getTypeGUID(),vecData))
	{
		for(int i=0;i<(int)vecData.size();i++)
		{
			DataType* pipe = (DataType*)vecData.at(i);		

			_3DType pipe3D = GetCreateChildNode(pipe->getGuid());
			pipe3D.Update3D(pipe);				
		}
	}
}


void C3DAirsideNode::UpdateAll( IARCportLayoutEditContext* pmodel )
{	
	//update airspace
	SetupObjectList<AirWayPoint, CRenderAirWayPoint3D>(pmodel);
	SetupObjectList<AirHold, CRenderAirHold3D>(pmodel);
	SetupObjectList<AirSector, CRenderAirSector3D>(pmodel);
	SetupObjectList<CAirRoute, CRenderAirRoute3D>(pmodel);
	////update topgraphy
	SetupObjectList<Contour, CRenderCounter3D>(pmodel);
	SetupObjectList<Surface, CRenderSurface3D>(pmodel);
	SetupObjectList<Structure, CRenderAirsideStructure3D>(pmodel);

	////udpate airports
	InputAirside* airside = pmodel->OnQueryInputAirside();
	if(!airside)return;
	ALTAirportLayout* pairport = airside->GetActiveAirport();
	C3DAirportNode airport3D  = GetCreateChildNode( pairport->getGuid() );
	airport3D.Setup3D(pairport);		

	/*int nAirportCount = airside->GetAirportCount();
	for(int i=0;i<nAirportCount;i++)
	{
		ALTAirportLayout* pairport = airside->GetAirportbyIndex(i);
	}*/
}
//////////////////////////////////////////////////////////////////////////
//CRenderHeliport3DList m_lstHeliport;
//CRenderTaxiway3DList m_lstTaxiway;
//CRenderStand3DList m_lstStand;
//CRenderDeicePad3DList m_lstDeicePad;
//CRenderStretch3DList m_lstStretch;
//CRenderRoadIntersection3DList m_lstRoadIntersection;
//CRenderPoolParking3DList m_lstPoolParking;
//CRenderTrafficLight3DList m_lstTrafficLight;
//CRenderTollGate3DList m_lstToolGate;
//CRenderStopSign3DList m_lstStopSign;
//CRenderYieldSign3DList m_lstYieldSign;
//CRenderCircleStretch3DList m_lstCircleStretch;
//CRenderStartPosition3DList m_lstStartPosition;
//CRenderMeetingPoint3DList m_lstMeetingPoint;
//
//IntersectionNodesInAirport m_vAirportNodes;
//CRenderFilletTaxiway3DInAirport m_vFilletTaxiways;
//CRenderInterStretch3DInAirport m_vIntersectedStretch;

void C3DAirportNode::Setup3D( ALTAirportLayout* pairport )
{
	int nAirportID = pairport->getID();
	SetAirportID(nAirportID);

	IARCportLayoutEditContext* pmodel = GetRoot().GetModel();
	CRenderFloorList mlevelist = pmodel->OnQueryAirsideFloor(nAirportID);
	bool isAirside = (NULL!=pmodel->GetCurContext()->toAirside());
	
	for(size_t i=0;i<mlevelist.size();++i){
		CRenderFloor* floor = mlevelist.at(i);
		C3DFloorNode floorNode  = GetCreateChildNode(floor->GetUID());
		floorNode.UpdateAll(floor,isAirside );
	}
	//update objects
	UpdateObjectList<Runway,CRenderRunway3D>(pairport,pmodel);
	UpdateObjectList<Heliport,CRenderHeliport3D>(pairport,pmodel);
	UpdateObjectList<Taxiway,CRenderTaxiway3D>(pairport,pmodel);

	UpdateObjectList<Stand,CRenderStand3D>(pairport,pmodel);
	UpdateObjectList<DeicePad,CRenderDeicePad3D>(pairport,pmodel);
	UpdateObjectList<Stretch,CRenderStretch3D>(pairport,pmodel);
	UpdateObjectList<Intersections,CRenderRoadIntersection3D>(pairport,pmodel);
	UpdateObjectList<VehiclePoolParking,CRenderPoolParking3D>(pairport,pmodel);
	UpdateObjectList<TrafficLight,CRenderTrafficLight3D>(pairport,pmodel);
	UpdateObjectList<TollGate,CRenderTollGate3D>(pairport,pmodel);
	UpdateObjectList<StopSign,CRenderStopSign3D>(pairport,pmodel);
	UpdateObjectList<YieldSign,CRenderYieldSign3D>(pairport,pmodel);

	UpdateObjectList<CircleStretch,CRenderCircleStretch3D>(pairport,pmodel);
	UpdateObjectList<CStartPosition,CRenderStartPosition3D>(pairport,pmodel);

	UpdateObjectList<CMeetingPoint,CRenderMeetingPoint3D>(pairport,pmodel);
}

#define STRING_DEFINE(x) const static CString x = _T("#x")
STRING_DEFINE(AirportID);

void C3DAirportNode::SetAirportID( int id )
{
	SetUserAny(Any(id),AirportID);
}

int C3DAirportNode::GetAirportID() const
{
	try
	{
		return any_cast<int>( GetUserAny(AirportID) );
	}
	catch (Ogre::Exception&)
	{
	}
	return -1;
}
