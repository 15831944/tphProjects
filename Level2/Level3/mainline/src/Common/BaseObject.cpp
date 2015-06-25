#include "StdAfx.h"
#include ".\baseobject.h"

CGUIDObject::CGUIDObject(void)
: m_guid(CGuid::Create())
{
}



void CBaseObject::RotateObject( double dr, const ARCVector3& point )
{
	ASSERT(0);
}
void CBaseObject::MoveObject( double dx, double dy )
{
	ASSERT(0);
}

ARCVector3 CBaseObject::GetLocation() const
{
	ASSERT(0);
	return ARCVector3();
}

int CBaseObject::GetFloorIndex() const
{
	ASSERT(0);
	return -1;
}