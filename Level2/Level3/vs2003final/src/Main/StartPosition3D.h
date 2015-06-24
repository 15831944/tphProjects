#pragma once
#include "ALTObject3D.h"
#include "Taxiway3D.h"

class CStartPosition;
class CStartPosition3D :
	public ALTObject3D
{
public:
	CStartPosition3D(int _Id);
	virtual ~CStartPosition3D(void);

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
	CStartPosition* getStartPos()const;
	CTaxiway3D::RefPtr mpRelateTaxiway;

};
