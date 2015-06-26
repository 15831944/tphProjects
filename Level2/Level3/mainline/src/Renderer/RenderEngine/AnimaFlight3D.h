#pragma once
// #include "3DNodeWithTag.h"
#include "3DNodeObject.h"
#include "AircraftModel3DNode.h"
#include "Aircraftlayout3DNode.h"
#include "AnimaMob3DList.h"

class CModel;
class AircraftAliasManager;
class InputOnboard;
class CAircaftLayOut;

#define ALLOC_FLIGHT3D_USE_POOL

#ifdef ALLOC_FLIGHT3D_USE_POOL
#include <boost/pool/object_pool.hpp>
#endif // ALLOC_FLIGHT3D_USE_POOL


class RENDERENGINE_API CAnimaFlight3D : public C3DNodeObject
{
public:
	CAnimaFlight3D(){}
	CAnimaFlight3D(int nFlightID, Ogre::SceneNode* pNode);
	~CAnimaFlight3D(void);

	int GetFlightID() const { return m_nFlightID; }

	void SetShape(  const CString& strAcType, double dlen, double dwingspan,InputOnboard* pOnboard = NULL  ); //set 
	void LoadLayout( InputOnboard* pOnboard, int nFlightId);
	void SetMatByAirlineColor( CString strAirline, COLORREF color,bool bOnBoard  );
	void DrawDeckCells(int nDeckIndex, const CString& strDeckName, std::vector<std::pair<ARCColor3, Path > >& vCells );
protected:
	int m_nFlightID;
	CString m_strShape;
	int m_nOnboardFlightID;
	CAircraftLayout3DNode m_layoutNode;
	CAircraftModel3DNode m_acModel;
};


class RENDERENGINE_API CAnimaFlight3DList : public CAnimaMob3DList<CAnimaFlight3D>
{

};

//class RENDERENGINE_API CAnimaFlight3DList
//{
//public:
//	CAnimaFlight3DList();
//	~CAnimaFlight3DList();
//	CAnimaFlight3D* CreateOrRetrieveFlight3D(int nFlightID, bool& bCreated);
//
//	void SetRootNode(C3DNodeObject rootNode) { m_rootNode = rootNode; }
//	void UpdateAllFlight( bool bShow );
//	void ShowFlight3D( int nFlightID, bool bShow );
//	void ShowFlight3D( CAnimaFlight3D* pFlight3D, bool bShow );
//	bool IsFlight3DShow( CAnimaFlight3D* pFlight3D );
//
//protected:
//
//	void Clear();
//
//	C3DNodeObject m_rootNode; // hold the root scene node of the scene
//
//
//#ifdef ALLOC_FLIGHT3D_USE_POOL
//	typedef boost::object_pool<CAnimaFlight3D> Pax3DPool;
//	Pax3DPool m_flightPool;
//#endif // ALLOC_FLIGHT3D_USE_POOL
//
//	typedef std::vector<CAnimaFlight3D> FlightList;
//	FlightList m_vFlightList;
//
//	class Flight3DIDCmp
//	{
//	public:
//		bool operator() (CAnimaFlight3D* pFlight3D, int nFlightID)
//		{
//			return pFlight3D->GetFlightID() < nFlightID;
//		}
//	};
//
//	class Flight3DCmp
//	{
//	public:
//		bool operator() (CAnimaFlight3D* lhs, CAnimaFlight3D* rhs)
//		{
//			return lhs->GetFlightID() < rhs->GetFlightID();
//		}
//	};
//};