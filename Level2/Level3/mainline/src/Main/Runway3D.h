#pragma once
#include "altobject3d.h"
#include "..\Common\ARCVector.h"
#include "..\Common\ARCPath.h"
#include "HoldPosition3D.h"
#include "..\InputAirside\Runway.h"
#include "./ObjectControlPoint2008.h"

class RunwayDisplayProp;

struct RunwayBlock{
	std::string texture_name;
	std::vector<ARCVector2>  vertex_coords;
	std::vector<ARCVector2>  texture_coords;
};



class RunwayHoldPosition3D : public HoldPosition3D
{
public:
	RunwayHoldPosition3D(Runway* pRunway, int id): HoldPosition3D(pRunway,id){}

	Runway  *GetRunway(){ return (Runway*)m_pObject.get(); }
	const Runway * GetRunway()const{ return (Runway*)m_pObject.get(); }
	
	virtual const CPath2008& GetPath()const{ return GetRunway()->GetPath(); }

	//virtual void UpdateLine();
	//virtual HoldPosition& GetHoldPosition();

	virtual void DrawSelect(C3DView * pView){}

	virtual void Draw(C3DView* pView){}


protected:

};


class HoldPositionRunwayToRunway3D : public RunwayHoldPosition3D
{
	
public:
	HoldPositionRunwayToRunway3D(Runway* pRunway, int id):RunwayHoldPosition3D(pRunway,id){}

};

class HoldPositionRunwayToTaxiway3D : public RunwayHoldPosition3D
{
public:
	HoldPositionRunwayToTaxiway3D(Runway * pRunway,int id):RunwayHoldPosition3D(pRunway,id){}

};



class CRunway3D :
	public ALTObject3D
{
public:
	CRunway3D(int id);

	virtual ~CRunway3D(void);
	
	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	Runway * GetRunway()const ;
	RunwayDisplayProp * GetRunwayDisplay()const;
	const ARCPath& GetOutLine()const{ return m_vOutline; }

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	virtual ARCPoint3 GetLocation(void) const;

	virtual void DoSync();

	virtual void FlushChange();

	//void UpdateHoldPositions(const ALTObject3DList& otherObjList);
	
	void RenderBase(C3DView * pView);
	void RenderBaseWithSideLine(C3DView *pView);
	void RenderHoldPosition(C3DView * pView);
	void RenderEditPoint(C3DView * pView, bool bSelectMode = false);
	void RenderMarkings(C3DView * pView);

	virtual void OnRotate(DistanceUnit dx);

	
protected:
	std::vector<RunwayBlock> m_vBlocks;
	void GenBlocks();
	
	ARCPath m_vOutline;
	
	HoldPosition3DList m_vAllHoldPositions3D;

	ALTObjectControlPoint2008List m_vControlPoints;

};


