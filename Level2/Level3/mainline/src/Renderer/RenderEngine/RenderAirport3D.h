#pragma once
//#include "RenderMeetingPoint3D.h"
//#include "RenderRunway3D.h"
//#include "RenderHeliport3D.h"
//#include "RenderTaxiway3D.h"
//#include "RenderStand3D.h"
//#include "RenderDeicePad3D.h"
//#include "RenderStretch3D.h"
//#include "RenderPoolParking3D.h"
//#include "RenderTrafficLight3D.h"
//#include "RenderTollGate3D.h"
//#include "RenderStopSign3D.h"
//#include "RenderRoadIntersection3D.h"
//#include "RenderYieldSign3D.h"
//#include "RenderCircleStretch3D.h"
//#include "RenderStartPosition3D.h"
//#include "RenderSurface3D.h"
//#include "RenderStructure3D.h"
//
//
//#include "Terminal3DObject.h"
//#include "RenderFloor3D.h"

//
//class CRenderAirport3D;
//class ALTAirport;
//
//#include <InputAirside/IntersectionNodesInAirport.h>
//
//#include "RenderFillet3D.h"
//#include "RenderInterStretch3D.h"
//class ALTAirport;

//class CRenderAirside3D : public CModeBase3DObject
//{
//public:
//	CRenderAirside3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid);
//	~CRenderAirside3D(void);
//
//	typedef boost::shared_ptr<CRenderAirside3D> SharedPtr;
//	typedef std::vector<SharedPtr> List;
//	virtual CModeBase3DObject::EnumObjectType GetObjectType() const {return ObjectType_Airside;}
//public:
//	//CRenderRunway3DList m_lstRunway;
//	/*CRenderHeliport3DList m_lstHeliport;
//	CRenderTaxiway3DList m_lstTaxiway;
//	CRenderStand3DList m_lstStand;
//	CRenderDeicePad3DList m_lstDeicePad;
//	CRenderStretch3DList m_lstStretch;
//	CRenderRoadIntersection3DList m_lstRoadIntersection;
//	CRenderPoolParking3DList m_lstPoolParking;
//	CRenderTrafficLight3DList m_lstTrafficLight;
//	CRenderTollGate3DList m_lstToolGate;
//	CRenderStopSign3DList m_lstStopSign;
//	CRenderYieldSign3DList m_lstYieldSign;
//	CRenderCircleStretch3DList m_lstCircleStretch;
//	CRenderStartPosition3DList m_lstStartPosition;
//	CRenderMeetingPoint3DList m_lstMeetingPoint;*/
//
//	IntersectionNodesInAirport m_vAirportNodes;
//CRenderFilletTaxiway3DInAirport m_vFilletTaxiways;
//	CRenderInterStretch3DInAirport m_vIntersectedStretch;
//
//	CRenderFloor3DList m_groundfloors;
//	// Data Objects Query
//	// the specified class DataType must has a static member of unique GUID value with name class_guid
//	template <class DataType>
//		DataType* QueryDataObject(const CGuid& guid)
//	{
//		return (DataType*)OnQueryDataObject(guid, DataType::getTypeGUID());
//	}
//	template <class DataType>
//		bool QueryDataObjectList(std::vector<DataType*>& vData)
//	{
//		std::vector<void*> vPtrs;
//		if (OnQueryDataObjectList(vPtrs, DataType::getTypeGUID()))
//		{
//			vData.reserve(vData.size() + vPtrs.size());
//			for (std::vector<void*>::iterator ite = vPtrs.begin();ite!=vPtrs.end();ite++)
//			{
//				vData.push_back((DataType*)*ite);
//			}
//			return true;
//		}
//		return false;
//	}
//
//	virtual void* OnQueryDataObject(const CGuid& guid, const GUID& class_guid);
//	virtual bool OnQueryDataObjectList(std::vector<void*>& vData, const GUID& cls_guid);
//
//
//	ALTAirport* GetALTAirport() const;
//
//	void Update();	
//	void Update3D(ALTAirport *pAirport, int nUpdateCode = Terminal3DUpdateAll);
//	bool UpdateObject(const CGuid& guid, const GUID& cls_guid);
//
//	const CGuid& GetGuid() const { return m_guid; }
//
//protected:
//
//protected:
//};
//
//
//
//class CRenderAirport3DList : 
//	public CAirside3DList<CRenderAirport3DList,ALTAirport , CRenderAirside3D>
//{
//public:
//	CRenderAirport3DList();
//	~CRenderAirport3DList();
//
//	void UpdateObject(const CGuid& guid, const GUID& cls_guid);
//
//};



//class /*RENDERENGINE_API*/ CRenderAirport3DList : public CTerminal3DListBase
//{
//public:
//
//	typedef typename CRenderAirport3D::SharedPtr Shared3DPtr;
//	typedef typename CRenderAirport3D::List      Shared3DList;
//
//	CRenderAirport3D *m_pAirport3D;
//	// for Update() and Update3DObject() only, external code should not use this method
//	// class derived from CTerminal3DList may sometimes need to rewrite this method
//	CRenderAirport3D* CreateNew3D(ALTAirport* pData, size_t /*nIndex*/) const
//	{
//		return new CRenderAirport3D(m_p3DScene->AddQueryableSceneNode(pData), m_p3DScene, pData->getGuid());
//	}
//
//	// Update entire object list
//	bool Update()
//	{
//		std::for_each(m_3DObjects.begin(), m_3DObjects.end(), boost::BOOST_BIND(&C3DNodeObject::Detach, _1)); // must detach all first
//		m_3DObjects.clear();
//		std::vector<DataType*> vecData ;
//
//		if (m_p3DScene->QueryDataObjectList<DataType>(vecData))
//		{
//			m_3DObjects.reserve(vecData.size());
//			std::vector<ALTAirport*>::iterator ite = vecData.begin();
//			std::vector<ALTAirport*>::iterator iteBg = ite;
//			std::vector<ALTAirport*>::iterator iteEn = vecData.end();
//			for (;ite!=iteEn;ite++)
//			{
//				Shared3DPtr p3D = Shared3DPtr(static_cast<CRenderAirport3DList*>(this)->CreateNew3D(*ite, ite - iteBg));
//
//				p3D->AttachTo(m_p3DScene->GetRoot());
//				p3D->Update3D(*ite);
//				m_3DObjects.push_back(p3D);
//			}
//			return true;
//		}
//		return false;
//	}
//
//	// update specified 3d object with guid and update code
//	// update code can be declared and implemented in C3DObjType
//	bool Update3DObject(const CGuid& guid, int nUpdateCode = Terminal3DUpdateAll)
//	{
//		Shared3DList::iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
//			boost::BOOST_BIND(&CRenderAirport3D::GetGuid, _1) == guid);
//		DataType* pData = m_p3DScene->QueryDataObject<DataType>(guid);
//
//		if (m_3DObjects.end() != ite)
//		{
//			if (pData)
//			{
//				// update
//				(*ite)->Update3D(pData, nUpdateCode);
//			}
//			else
//			{
//				// delete
//				(*ite)->Detach();
//				m_3DObjects.erase(ite);
//			}
//		}
//		else if (pData)
//		{
//			// add
//
//			Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(pData, m_3DObjects.size()));
//			p3D->AttachTo(m_p3DScene->GetRoot());
//			p3D->Update3D(pData);
//			m_3DObjects.push_back(p3D);
//
//		}
//		else
//		{
//			ASSERT(FALSE);
//			return false;
//		}
//		return true;
//	}
//	Shared3DPtr Find3DObj( const CGuid& guid ) const
//	{
//		Shared3DList::const_iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
//			boost::BOOST_BIND(&CAirside3DObject::GetGuid, _1) == guid);
//		if (m_3DObjects.end() != ite)
//			return *ite;
//		return Shared3DPtr();
//	}
//
//private:
//	Shared3DList m_3DObjects;
//};
//
