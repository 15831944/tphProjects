#pragma once
#include "ALTObject3D.h"
#include "Taxiway3D.h"

class CMeetingPoint;
class CMeetingPoint3D :
	public ALTObject3D
{
public:
	CMeetingPoint3D(int _Id);
	virtual ~CMeetingPoint3D(void);

	virtual void DrawOGL(C3DView * m_pView);
	virtual void AddEditPoint(double x, double y, double z);


	virtual SelectType GetSelectType() const;

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx);
	virtual void AfterMove();

	virtual void DrawSelect(C3DView * pView);
	virtual ARCPoint3 GetLocation() const;

	virtual ARCPoint3 GetSubLocation(int index) const;

	// write data to database
	virtual void FlushChange();

	// update data from database
	virtual void Update();

	// sync geo date for drawing
	virtual void DoSync();

	virtual CString GetDisplayName() const;

	//
	CMeetingPoint* getMeetingPoint()const;
	CTaxiway3D::RefPtr m_pRelateTaxiway;

};
