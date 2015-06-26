#include "StdAfx.h"

#include <Common/replace.h>

#include "AnimaVehicle3D.h"
#include "OgreUtil.h"
#include "OgreConvert.h"
#include "CustomizedRenderQueue.h"
#include <boost/bind.hpp>

using namespace Ogre;

#include "Shape3DMeshManager.h"

#include <Plugins/ConvertToOgreMesh/ConvertToOgreMeshCommon.h>

CAnimaVehicle3D::CAnimaVehicle3D( int nVehicleID, Ogre::SceneNode* pNode )
	: C3DNodeObject(pNode)
	, m_nVehicleID(nVehicleID)
	, m_pVehicleEnt(NULL)
{

}

CAnimaVehicle3D::~CAnimaVehicle3D( void )
{

}

void CAnimaVehicle3D::SetShape( int nVehicleShapeIndex ) // Do not use this method, further version may make it work OK, 2010/04/27
{
	CString strShape;
	VERIFY(CShape3DMeshManager::GetInstance().GetVehicleShapeName(strShape, nVehicleShapeIndex));

	UpdateShape(strShape);
}

void CAnimaVehicle3D::SetShape( CString strShapeName )
{
	CString strShape; // shape mesh
	VERIFY(CShape3DMeshManager::GetInstance().GetVehicleShapeName(strShape, strShapeName));

	UpdateShape(strShape);
}

void CAnimaVehicle3D::SetColor( COLORREF color )
{
	if(m_pVehicleEnt)
	{
		MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(color);
		if(!matPtr.isNull())
		{
			m_pVehicleEnt->setMaterial(matPtr);
		}
	}
}

void CAnimaVehicle3D::DestroyEntity()
{
	if(GetScene() && m_pVehicleEnt)
	{
		GetSceneNode()->detachAllObjects();
		OgreUtil::DetachMovableObject(m_pVehicleEnt);
		m_pVehicleEnt = NULL;
	}

}

void CAnimaVehicle3D::UpdateShape( const CString& strShape )
{
	if(m_strShape!=strShape)
	{
		m_strShape = strShape;

		DestroyEntity();
		CString strEntName;
		strEntName.Format("VehicleEnt%x",(int)this);
		CString strMeshName;
		strMeshName.Format("%s.mesh", strShape.GetString());	

		m_pVehicleEnt = OgreUtil::createOrRetrieveEntity(strEntName.GetString(), strMeshName.GetString(), GetScene() );
		if (m_pVehicleEnt)
		{
			m_pVehicleEnt->setRenderQueueGroup(RenderObject_AnimationObject);
			AddObject(m_pVehicleEnt/*, true*/);
		}
	}
}

void CAnimaVehicle3D::SetDimension( const ARCVector3& vDim )
{
	if(m_pVehicleEnt)
	{		 
		Vector3 vSize = m_pVehicleEnt->getBoundingBox().getSize();
		double dscalex = vDim[VX]/vSize.x;
		double dscaley = vDim[VY]/vSize.y;
		double dscalez = vDim[VZ]/vSize.z;
		SetScale(ARCVector3(dscalex,dscaley,dscalez));
	}
}
//void CAnimaVehicle3DList::Clear()
//{
//	for(size_t i=0;i<m_vVehicleList.size();i++)
//#ifdef ALLOC_FLIGHT3D_USE_POOL
//		m_vehiclePool.destroy(m_vVehicleList[i]);
//#else
//		delete m_vVehicleList[i];
//#endif // ALLOC_FLIGHT3D_USE_POOL
//	m_vVehicleList.clear();
//}
//
//
//CAnimaVehicle3DList::CAnimaVehicle3DList()
//{
//
//}
//
//CAnimaVehicle3DList::~CAnimaVehicle3DList()
//{
//	Clear();
//}
//
//void CAnimaVehicle3DList::ShowVehicle3D( int nVehicleID, bool bShow )
//{
//	VehicleList::iterator ite = std::lower_bound(m_vVehicleList.begin(), m_vVehicleList.end(), nVehicleID, Vehicle3DIDCmp());
//	if (ite != m_vVehicleList.end() && (*ite)->GetVehicleID() == nVehicleID)
//	{
//		ShowVehicle3D(*ite, bShow);
//	}
//}
//
//void CAnimaVehicle3DList::ShowVehicle3D( CAnimaVehicle3D* pVehicle3D, bool bShow )
//{
//	bShow ? pVehicle3D->AttachTo(m_rootNode) : pVehicle3D->Detach();
//}
//
//bool CAnimaVehicle3DList::IsVehicle3DShow( CAnimaVehicle3D* pVehicle3D )
//{
//	return pVehicle3D->GetParent()/* == m_rootNode*/;
//}
//
//CAnimaVehicle3D* CAnimaVehicle3DList::CreateOrRetrieveVehicle3D( int nVehicleID, bool& bCreated )
//{
//	bCreated = false;
//	VehicleList::iterator ite = std::lower_bound(m_vVehicleList.begin(), m_vVehicleList.end(), nVehicleID, Vehicle3DIDCmp());
//	if (ite != m_vVehicleList.end() && (*ite)->GetVehicleID() == nVehicleID)
//		return *ite;
//
//#ifdef ALLOC_FLIGHT3D_USE_POOL
//	CAnimaVehicle3D* pVehicle3D = m_vehiclePool.malloc();
//	new (pVehicle3D) CAnimaVehicle3D(nVehicleID, m_rootNode.CreateNewChild().GetSceneNode());
//#else
//	CAnimaVehicle3D* pVehicle3D = new CAnimaVehicle3D(nVehicleID, m_rootNode.CreateNewChild().GetSceneNode());
//#endif // ALLOC_FLIGHT3D_USE_POOL
//
//	m_vVehicleList.push_back(pVehicle3D);
//	std::sort(m_vVehicleList.begin(), m_vVehicleList.end(), Vehicle3DCmp());
//	bCreated = true;
//	return pVehicle3D;
//}
//
//void CAnimaVehicle3DList::UpdateAllVehicle( bool bShow )
//{
//	if(bShow)
//	{
//// 		std::for_each(m_vVehicleList.begin(), m_vVehicleList.end(), boost::BOOST_BIND(&CAnimaVehicle3D::AttachTo, _1, m_rootNode));
//	}
//	else
//	{
//		Clear();
//	}
//}
