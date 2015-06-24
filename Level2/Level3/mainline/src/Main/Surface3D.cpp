#include "StdAfx.h"
#include ".\surface3d.h"
#include ".\..\InputAirside\Surface.h"
#include ".\3DView.h"
#include "../Main/TessellationManager.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\glrender\glTextureResource.h"
#include "TermPlanDoc.h"

static GLdouble scoord[]={0.00005,0.0};
static GLdouble tcoord[]={0.0,0.00005};
void __stdcall SurfaceVertexdv(double* v){
	GLdouble s=v[0]*scoord[0]+v[1]*scoord[1]+v[2]*scoord[2];
	GLdouble t=v[0]*tcoord[0]+v[1]*tcoord[1]+v[2]*tcoord[2];
	glTexCoord2d(s,t);
	glVertex3dv(v);
}

CSurface3D::CSurface3D( int nObjID ) : ALTObject3D(nObjID)
{
	m_pObj = new Surface;
	//m_pDisplayProp = new SurfaceDisplayProp;
	m_bInEdit = false;
	m_nID = nObjID;
}
CSurface3D::~CSurface3D(void)
{
}

Surface * CSurface3D::GetSurface() const
{
	return (Surface*) m_pObj.get();
}

void CSurface3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync())
		DoSync();
	
	glPolygonOffset(2.0,2.0);	
	
	RenderSurface(pView);		

	
}
void CSurface3D::DrawSelect( C3DView * pView )
{
	RenderSurface(pView);
	if(IsInEdit())
	{
		for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
			(*itr)->DrawSelect(pView);		 
		}
	}	
}

ARCPoint3 CSurface3D::GetLocation( void ) const
{
	const CPath2008 & path = GetSurface()->GetPath();
	int nptCnt = path.getCount();
	ARCPoint3 pt;
	for(int i=0;i<nptCnt;i++){
		pt += path.getPoint(i);
	}
	pt /= nptCnt;
	return pt;
}

void CSurface3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetSurface()->m_path.DoOffset(dx,dy,dz);
}

void CSurface3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetSurface()->m_path.getCount();
	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetSurface()->m_path,i) );	
	}
}

void CSurface3D::FlushChange()
{
	try
	{
		GetSurface()->UpdatePath();

	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
}

void CSurface3D::RenderSurface( C3DView * pView )
{
	
	if(IsNeedSync())
		DoSync();

	SurfaceDisplayProp * pDisplayPrope = (SurfaceDisplayProp* ) GetDisplayProp();

	if(!pDisplayPrope->m_dpShape.bOn)
		return;

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glColor3ubv(pDisplayPrope->m_dpShape.cColor);
	CString texturefilename = pView->GetDocument()->GetSurfaceMaterialLib().GetTexturefilename(GetSurface()->GetTexture());
	CString texturename = GetSurface()->GetTexture();
	CTexture * pTexture = pView->getTextureResource()->NewTextureAndReside(texturefilename,texturename);
	if(pTexture)pTexture->Apply();
	glNormal3f(0,0,1);
	ARCPoint3 buf[512];
	GLUtesselatorObj *tess;
	tess=gluNewTess();
	gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
	gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr) &glEnd);
	gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) &SurfaceVertexdv);
	gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	gluTessBeginPolygon(tess,NULL);
	gluTessBeginContour(tess);	
	for( int i=0;i<GetSurface()->GetPath().getCount();i++){
		buf[i] = ARCVector3( GetSurface()->GetPath().getPoint(i) );
		gluTessVertex(tess,buf[i],buf[i]);
	}
	gluTessEndContour(tess);
	gluTessEndPolygon(tess);
	gluDeleteTess(tess);	
	glDisable(GL_TEXTURE_2D);

	if(IsInEdit())
	{		
		for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++)
		{
			(*itr)->DrawSelect(pView);		 
		}
	}	
}