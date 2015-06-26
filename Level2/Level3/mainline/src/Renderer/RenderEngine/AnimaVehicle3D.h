#pragma once
// #include "3DNodeWithTag.h"
#include "3DNodeObject.h"
#include "animaMob3Dlist.h"

#define ALLOC_FLIGHT3D_USE_POOL

#ifdef ALLOC_FLIGHT3D_USE_POOL
#include <boost/pool/object_pool.hpp>
#endif // ALLOC_FLIGHT3D_USE_POOL

class RENDERENGINE_API CAnimaVehicle3D : public C3DNodeObject
{
public:
	CAnimaVehicle3D(){}
	CAnimaVehicle3D(int nVehicleID, Ogre::SceneNode* pNode);
	~CAnimaVehicle3D(void);

	int GetVehicleID() const { return m_nVehicleID; }

	void SetShape( int nVehicleShapeIndex/*Shape Index in GUI*/ );
	void SetShape( CString strShapeName/*Shape Name in GUI*/ );

	void SetColor( COLORREF color );
	void DestroyEntity();

	void SetDimension(const ARCVector3& vDim);

protected:
	void UpdateShape( const CString& strShape );


	int m_nVehicleID;
	Ogre::Entity* m_pVehicleEnt;
	CString m_strShape;
};

class RENDERENGINE_API CAnimaVehicle3DList : public CAnimaMob3DList<CAnimaVehicle3D> 
{
public:
	CAnimaVehicle3DList():CAnimaMob3DList(_T("AirsideVehicle")){}
};
//class RENDERENGINE_API CAnimaVehicle3DList
//{
//public:
//	CAnimaVehicle3DList();
//	~CAnimaVehicle3DList();
//	CAnimaVehicle3D* CreateOrRetrieveVehicle3D(int nVehicleID, bool& bCreated);
//
//	void SetRootNode(C3DNodeObject rootNode) { m_rootNode = rootNode; }
//	void UpdateAllVehicle( bool bShow );
//	void ShowVehicle3D( int nVehicleID, bool bShow );
//	void ShowVehicle3D( CAnimaVehicle3D* pVehicle3D, bool bShow );
//	bool IsVehicle3DShow( CAnimaVehicle3D* pVehicle3D );
//
//protected:
//
//	void Clear();
//
//	C3DNodeObject m_rootNode; // hold the root scene node of the scene
//
//
//#ifdef ALLOC_FLIGHT3D_USE_POOL
//	typedef boost::object_pool<CAnimaVehicle3D> Pax3DPool;
//	Pax3DPool m_vehiclePool;
//#endif // ALLOC_FLIGHT3D_USE_POOL
//
//	typedef std::vector<CAnimaVehicle3D*> VehicleList;
//	VehicleList m_vVehicleList;
//
//	class Vehicle3DIDCmp
//	{
//	public:
//		bool operator() (CAnimaVehicle3D* pVehicle3D, int nVehicleID)
//		{
//			return pVehicle3D->GetVehicleID() < nVehicleID;
//		}
//	};
//
//	class Vehicle3DCmp
//	{
//	public:
//		bool operator() (CAnimaVehicle3D* lhs, CAnimaVehicle3D* rhs)
//		{
//			return lhs->GetVehicleID() < rhs->GetVehicleID();
//		}
//	};
//};