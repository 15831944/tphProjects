#pragma once

#include "altobject3d.h"
#include "../Common/line2008.h"
#include "../InputAirside/ALTObject.h"

class HoldPosition;

class HoldPosition3D : public Selectable{
public:
	typedef ref_ptr<HoldPosition3D> RefPtr;

	HoldPosition3D(ALTObject* pObject, int id);

	virtual SelectType GetSelectType()const { return HOLD_LINE; }

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0) ;

	virtual ARCPoint3 GetLocation()const;

	virtual void DrawSelect(C3DView * pView);

	virtual void Draw(C3DView* pView);

	virtual void UpdateLine() = 0;
	virtual HoldPosition& GetHoldPosition() = 0;

	virtual void AfterMove();

	virtual void FlushChange();

protected:
	virtual const CPath2008&GetPath()const = 0;

	CLine2008 m_line;	
	ALTObject::RefPtr m_pObject;
	int m_nid;
};
typedef std::vector< HoldPosition3D::RefPtr > HoldPosition3DList;
