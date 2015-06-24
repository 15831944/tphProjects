#include "StdAfx.h"
#include ".\objectcontrolpoint2008.h"

#include "./../Common/path2008.h"
#include "./ShapeRenderer.h"

#include "./Taxiway3D.h"
#include "./../InputAirside/Taxiway.h"
#include "../Main/ALTObject3D.h"

CPathControlPoint2008::CPathControlPoint2008( CPath2008& path,int ptID ) : m_path (path)
{
	m_id = ptID;
}
void CPathControlPoint2008::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if(m_id >= m_path.getCount() ) return;

	m_path[m_id] += CPoint2008(dx,dy,dz);
}

void CPathControlPoint2008::DeleteEditPoint(ALTObject3D* pObj3D)
{
	//	ASSERT(m_id < m_path.getCount());
	if(pObj3D->GetObjectType() == ALT_CIRCLESTRETCH)
	{
		if (m_path.getCount()<4)
			return;
	}
	else
	{
		if(m_path.getCount()<3)
			return;
	}

	m_path.RemovePointAt(m_id);
	if(m_id>0)
	{
		m_id = m_path.getCount()-1;
	}

}

CPathControlPoint2008::~CPathControlPoint2008( void )
{

}

ARCPoint3 CPathControlPoint2008::GetLocation() const
{
	ASSERT(m_id < m_path.getCount());
	return m_path.getPoint(m_id);
}

void CPathControlPoint2008::DrawSelect( C3DView * pView )
{
	glNormal3i(0,0,1);

	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],100);
}


CALTObjectPathControlPoint2008::~CALTObjectPathControlPoint2008()
{

}





void CALTObjectPathControlPoint2008::FlushChange()
{
	m_pObject->FlushChange();
}

CALTObjectPathControlPoint2008::CALTObjectPathControlPoint2008( ALTObject3D* pObject, CPath2008& path, int ptID ) : CPathControlPoint2008(path,ptID)
{
	m_pObject = pObject;
}

void CALTObjectPathControlPoint2008::DrawSelect( C3DView * pView )
{
	if( m_id >= m_path.getCount() ) return;

	glNormal3i(0,0,1);
	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],500);
}

void CALTObjectPathControlPoint2008::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if (m_pObject->GetObject()->GetLocked())
	{
		// do not move when the object is locked
		return;
	}
	CPathControlPoint2008::OnMove(dx, dy, dz);
}
ARCPoint3 CALTObjectPointControlPoint2008::GetLocation() const
{
	return m_pt;
}

CALTObjectPointControlPoint2008::CALTObjectPointControlPoint2008( ALTObject3D * pObject, CPoint2008& pt ): m_pt(pt),m_pObject(pObject)
{

}

void CALTObjectPointControlPoint2008::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if (m_pObject->GetObject()->GetLocked())
	{
		// do not move when the object is locked
		return;
	}
	m_pt += CPoint2008(dx,dy,dz);
}

void CALTObjectPointControlPoint2008::DrawSelect( C3DView * pView )
{
	glNormal3i(0,0,1);

	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],500);
}

void CALTObjectPointControlPoint2008::FlushChange()
{
	m_pObject->FlushChange();
}