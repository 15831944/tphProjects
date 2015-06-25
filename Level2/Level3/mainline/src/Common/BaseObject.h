#pragma once
#include <Common/ARCVector.h>
#include <Common/Guid.h>


class CGUIDObject
{
public:
	CGUIDObject(void);
	virtual ~CGUIDObject(){}
	inline const CGuid& getGuid() const { return m_guid; }
protected:
	CGuid m_guid;
};

class CBaseObject : public virtual CGUIDObject
{
public:
	virtual void RotateObject(double dr, const ARCVector3& point);
	virtual void MoveObject(double dx, double dy);

	virtual ARCVector3 GetLocation() const;
	virtual int GetFloorIndex()const;
};
