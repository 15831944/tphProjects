#pragma once


#include "3DSceneNode.h"
#include <Common/Line2008.h>

#include <InputAirside/ALTObject.h>

class HoldPosition;
class CPath2008;

//#include "Terminal3DObject.h"

class CRenderHoldPosition3D : public C3DSceneNode
{

public:
	SCENE_NODE(CRenderHoldPosition3D,C3DSceneNode)
	//virtual EnumObjectType GetObjectType() const { return ObjectType_Airside; }

// 	virtual SelectType GetSelectType()const { return HOLD_LINE; }
// 	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0) ;

	void Update();

	ARCPoint3 GetLocation()const;

// 	virtual void DrawSelect(C3DView * pView);
// 	virtual void Draw(C3DView* pView);

	virtual void UpdateLine() = 0;
	virtual HoldPosition& GetHoldPosition() = 0;

// 	virtual void AfterMove();

	virtual void FlushChange();

protected:
	virtual const CPath2008&GetPath()const = 0;

	CLine2008 m_line;	
	ALTObject::RefPtr m_pObject;
	int m_nid;
};
