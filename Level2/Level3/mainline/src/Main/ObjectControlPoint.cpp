#include "StdAfx.h"
#include ".\objectcontrolpoint.h"

#include "./../Common/path.h"
#include "./ShapeRenderer.h"

#include "./Taxiway3D.h"
#include "./../InputAirside/Taxiway.h"

PathControlPoint::PathControlPoint( Path& path,int ptID ) : m_path (path)
{
	m_id = ptID;
}
void PathControlPoint::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if(m_id >= m_path.getCount() ) return;

	m_path[m_id] += Point(dx,dy,dz);
}

void PathControlPoint::DeleteEditPoint()
{
//	ASSERT(m_id < m_path.getCount());
	if(m_path.getCount()<3)
		return;
	m_path.RemovePointAt(m_id);
	if(m_id>0)
	{
		m_id = m_path.getCount()-1;
	}

}

PathControlPoint::~PathControlPoint( void )
{
	
}

ARCPoint3 PathControlPoint::GetLocation() const
{
	ASSERT(m_id < m_path.getCount());
	return m_path.getPoint(m_id);
}

void PathControlPoint::DrawSelect( C3DView * pView )
{
	glNormal3i(0,0,1);

	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],100);
}


ALTObjectPathControlPoint::~ALTObjectPathControlPoint()
{
	
}





void ALTObjectPathControlPoint::FlushChange()
{
	m_pObject->FlushChange();
}

ALTObjectPathControlPoint::ALTObjectPathControlPoint( ALTObject3D* pObject, Path& path, int ptID ) : PathControlPoint(path,ptID)
{
	m_pObject = pObject;
}

void ALTObjectPathControlPoint::DrawSelect( C3DView * pView )
{
	if( m_id >= m_path.getCount() ) return;
	
	glNormal3i(0,0,1);
	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],500);
}
ARCPoint3 ALTObjectPointControlPoint::GetLocation() const
{
	return m_pt;
}

ALTObjectPointControlPoint::ALTObjectPointControlPoint( ALTObject3D * pObject, Point& pt ): m_pt(pt),m_pObject(pObject)
{
	
}

void ALTObjectPointControlPoint::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	m_pt += Point(dx,dy,dz);
}

void ALTObjectPointControlPoint::DrawSelect( C3DView * pView )
{
	glNormal3i(0,0,1);

	ARCPoint3 pt = GetLocation();
	DrawFlatSquare(pt[VX],pt[VY],pt[VZ],500);
}

void ALTObjectPointControlPoint::FlushChange()
{
	m_pObject->FlushChange();
}