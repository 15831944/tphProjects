#pragma once
#include "terminal3dobject.h"
#include "Terminal3DList.h"

#include "Airside3DObjectWrapper.h"

#include <InputAirside/Taxiway.h>

#include "RenderHoldPosition3D.h"
#include "RenderFillet3D.h"

class CRenderTaxiwayHoldPosition3D : public CRenderHoldPosition3D
{
public:	
	SCENE_NODE(CRenderTaxiwayHoldPosition3D, CRenderHoldPosition3D)

	Taxiway * GetTaxiway(){ 
		return (Taxiway*)m_pObject.get();
	}
	const Taxiway* GetTaxiway()const{ return (Taxiway*)m_pObject.get(); }

	const CPath2008&GetPath()const{ return GetTaxiway()->GetPath(); }

	virtual void UpdateLine();
	virtual HoldPosition & GetHoldPosition();

protected:

};

class CRenderFilletTaxiway3DInAirport;
class ALTAirportLayout;
class CRenderTaxiway3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderTaxiway3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj );
	virtual void Update3D(ALTObjectDisplayProp* pDispObj );

	static double m_dCenterlineZOffset;

	struct FilletPtRef
	{
		FilletPtRef(const FilletTaxiway& _fillet, bool first){ fistPt = first;fillet = &_fillet; }
		const FilletTaxiway* fillet;
		bool fistPt;
		double GetDistInObj()const;
		double GetTaxiwayDist(int taxiwayId)const;
	};

protected:
	void RefreshHoldPosition3D(Taxiway* pTaxiway);
	void Build(Taxiway* pTaxiway, Ogre::ManualObject* pObj);
	void UpdateHoldPositions( Taxiway* pTaxiway, ALTAirportLayout& airport, const FilletTaxiwaysInAirport& vFillets, const IntersectionNodesInAirport& nodelist);
	void UpdateIntersectionNodes( Taxiway* pTaxiway, const IntersectionNodesInAirport& nodelist );
	void SetRelatFillets( const FilletTaxiwaysInAirport& vFillets, Taxiway* pTaxiway );

private:
	std::vector<FilletPtRef> m_vFilletPoints;
	//std::vector<IntersectionNode> m_vIntersectionNodes;
	//CRenderHoldPosition3D::List m_vAllHoldPositions3D;
	//CRenderFilletPoint3D::PtrList m_vFilletPoints;
};

//#include "Terminal3DList.h"
//class Taxiway;
//class CRenderTaxiway3DList : 
//	public CAirportObject3DList<CRenderTaxiway3DList, Taxiway , CRenderTaxiway3D>
//{
//
//};