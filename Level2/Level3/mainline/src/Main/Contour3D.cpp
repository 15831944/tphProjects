#include "StdAfx.h"
#include ".\contour3d.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"
#include ".\3DView.h"
#include "TessellationManager.h"
#include "Airside3D.h"
#include "Airport3D.h"
#include <deque>
#include "glrender/glTextureResource.h"
GLenum  _lType;
std::list<CPoint2008> _lVids;
int   _lFcnt;
struct _tTriangle{
	CPoint2008 pts[3];
	_tTriangle(const CPoint2008&p1,const CPoint2008&p2,const CPoint2008&p3){
		pts[0]=p1;pts[1]=p2;pts[2]=p3;
	}
	_tTriangle(){};
};
std::vector<_tTriangle> _lTriangles;

void  __stdcall   _tBegin  (GLenum type)  { _lType = type; _lFcnt = 0; }; 
void  __stdcall   _tVertex (void * pdata)   
{ 
	double *ppoint=(double *)pdata;
	_lVids.push_back( CPoint2008(ppoint[0],ppoint[1],ppoint[2]) );
}

void  __stdcall   _tEnd    (void){
	unsigned int   fst, sec;
	size_t    i, n;

	_tTriangle tri;
	switch (_lType) {
		case GL_TRIANGLES:                  // single triangle
			while(!_lVids.empty()){		
				CPoint2008 p1= *_lVids.rbegin();_lVids.pop_back();
				CPoint2008 p2=*_lVids.rbegin();_lVids.pop_back();
				CPoint2008 p3=*_lVids.rbegin();_lVids.pop_back();
				_lTriangles.push_back(_tTriangle(p1,p2,p3));

				_lFcnt++;
			}
			break;

		case GL_TRIANGLE_STRIP:             // strip of triangles
			tri.pts [0] =  *_lVids.rbegin();_lVids.pop_back();
			tri.pts[1] =  *_lVids.rbegin();_lVids.pop_back();

			n = _lVids.size ();

			for (i = 0, fst = 1, sec = 2; i < n; i++) {
				tri.pts [sec] =  *_lVids.rbegin();_lVids.pop_back();

				_lTriangles.push_back(tri);

				tri.pts [0] = tri.pts [fst];

				if ( i & 1 ) {
					fst = 2; sec = 1;
				}
				else {
					fst = 1; sec = 2;
				}

				_lFcnt++;
			}
			break;

		case GL_TRIANGLE_FAN:        // triangles fanning from single vertex
			tri.pts [0] =  *_lVids.rbegin();_lVids.pop_back();
			tri.pts [1] =  *_lVids.rbegin();_lVids.pop_back();
			n = _lVids.size();
			for (i = 0; i < n; i++) {
				tri.pts [2] = *_lVids.rbegin();_lVids.pop_back();
				_lTriangles.push_back(tri);
				tri.pts [1] = tri.pts [2];
				_lFcnt++;
			}
			break;
	}                                  // switch
	_lType = -1;
	_lVids.clear();

}; 

unsigned int IDcounter;
unsigned int layPointCount;

CContour3D::CContour3D(int nID) : ALTObject3D(nID)
{

	m_pObj = new Contour;
//	m_pDisplayProp = new ContourDisplayProp;
	m_bInEdit = false;
	min=max=0;
}

CContour3D::~CContour3D(void)
{
}

Contour * CContour3D::GetContour() const
{
	return (Contour*)m_pObj.get();
}

ARCPoint3 CContour3D::GetLocation( void ) const
{
	ARCPoint3 centerPos;
	for(int i=0;i< GetContour()->GetPath().getCount();i++)
	{
		centerPos += GetContour()->GetPath().getPoint(i);
	}
	return centerPos / GetContour()->GetPath().getCount();
	
}

void CContour3D::DrawOGL( C3DView * pView )
{	
	glPushMatrix();
//	glTranslated(0,0,GetContour()->GetAltitude());
//	glBegin(GL_LINE_STRIP);
//	for(int i =0;i< GetContour()->GetPath().getCount();i++)
//	{
//		CPoint2008 p = GetContour()->GetPath().getPoint(i);
//	}

	if(IsInEdit())
	{
		RenderOutLine(pView);
		RenderEditPoint(pView);
	}
	else
	{
		RenderSurface(pView);
	}
	
	glPopMatrix();
}

void CContour3D::DoSync()
{
	m_vControlPoints.clear();

	Contour* pContour = NULL;
	std::deque<Contour*> vList;
	vList.push_front(GetContour());
	while(vList.size() != 0) 
	{
		pContour = vList[0];
		vList.pop_front();

		if (pContour)
		{
			int nPtCnt = pContour->GetPath().getCount();

			for(int i=0;i<nPtCnt;++i)
				m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,pContour->GetPath(),i) );	

		}

		for (int i = 0; i < pContour->GetChildCount(); i++)
		{
			vList.push_front(pContour->GetChildItem(i));
		}
	}
	ALTObject3D::DoSync();
}

void CContour3D::FlushChange()
{
	try
	{
		Contour* pContour = NULL;
		std::deque<Contour*> vList;
		vList.push_front(GetContour());
		while(vList.size() != 0) 
		{
			pContour = vList[0];
			vList.pop_front();

			if (pContour)
			{
				pContour->UpdatePath();
			}

			for (int i = 0; i < pContour->GetChildCount(); i++)
			{
				vList.push_front(pContour->GetChildItem(i));
			}
		}
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
}

void CContour3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetContour()->GetPath().DoOffset(dx,dy,dz);
}

void CContour3D::DrawSelect( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}
	
	if(IsInEdit())
	{
		RenderOutLine(pView);
		RenderEditPoint(pView);
	}
	else
	{
		RenderSurface(pView);
	}
}

void CContour3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	glColor3ub(255,255,255);
	if(!IsInEdit()) return;
	if(IsNeedSync()){
		DoSync();
	}	

	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++)
	{
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}
void CContour3D::GenAllTriBlock(Contour* m_pFather,const DistanceUnit& dAptAlt)
{
	
	if(m_pFather==NULL)
		return;
	
	for (int i=0; i<m_pFather->GetChildCount(); i++)
	{
		Contour *m_pSon = m_pFather->GetChildItem(i);
		GenAllTriBlock(m_pSon, dAptAlt);
	}
    double buf[512][3];

	GLUtesselator *tess;
	tess = gluNewTess();
	gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) &_tBegin);    
	gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr) &_tEnd);
	gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) &_tVertex);
	gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

	gluTessNormal(tess,0.0,0.0,1.0);

	gluTessBeginPolygon(tess, NULL);
	gluTessBeginContour(tess);

	int noffset = 0;
	for(int i=0; i<m_pFather->GetPath().getCount(); i++)
	{
		CPoint2008 _p=m_pFather->GetPath().getPoint(i);
		_p.setZ( m_pFather->GetAltitude() - dAptAlt );
		buf[i][0] = _p.getX();
		buf[i][1] = _p.getY();
		buf[i][2] = _p.getZ();
		if(buf[i][2]<min)
			min=buf[i][2];
		if(buf[i][2]>max)
			max=buf[i][2];

		//buf[i] = ARCVector3(m_pFather->GetPath().getPoint(i));
		//buf[i].n[2]=m_pFather->GetAltitude();
	    gluTessVertex(tess,buf[i],buf[i]);
	}
	gluTessEndContour(tess);
    noffset += m_pFather->GetPath().getCount();

	for(int i=0; i<m_pFather->GetChildCount(); i++)
	{
		Contour *m_pSon = m_pFather->GetChildItem(i);
		gluTessBeginContour(tess);
		for(int j=0; j<m_pSon->GetPath().getCount(); j++)
		{
			CPoint2008 _p = m_pSon->GetPath().getPoint(j);
			_p.setZ( m_pSon->GetAltitude()-dAptAlt );
			buf[j+noffset][0]=_p.getX();
			buf[j+noffset][1]=_p.getY();
			buf[j+noffset][2]=_p.getZ();
			//buf[j+noffset] = ARCVector3(m_pFather->GetPath().getPoint(j));
			//buf[j+noffset].n[2] = m_pSon->GetAltitude();
			gluTessVertex(tess, buf[j+noffset],buf[j+noffset]);
		}
		noffset += m_pSon->GetPath().getCount();
	    gluTessEndContour(tess);
	}
	gluTessEndPolygon(tess);
    gluDeleteTess(tess);
}
void CContour3D::RenderOutLine(C3DView * pView)
{
	if(IsNeedSync())
		DoSync();
	
	Contour* pContour = NULL;
	std::deque<Contour*> vList;
	vList.push_front(GetContour());
	while(vList.size() != 0) 
	{
		pContour = vList[0];
		vList.pop_front();

		if (pContour)
		{
			RenderContourOutLine(pContour);
		}

		for (int i = 0; i < pContour->GetChildCount(); i++)
		{
			vList.push_front(pContour->GetChildItem(i));
		}
	}
}

void CContour3D::RenderContourOutLine(Contour* pContour)
{
	glColor3ub(170,198,57);
	int nCount = pContour->GetPath().getCount();
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < nCount; i++)
	{
		CPoint2008 pt = pContour->GetPath().getPoint(i);
		glVertex3d(pt.getX(),pt.getY(),pt.getZ());
	}
	glEnd();
}
void CContour3D::RenderSurface(C3DView * pView)
{
	if(IsNeedSync())
		DoSync();
	double scoord[]={0.0001,0.0,0.0};
	double tcoord[]={0.0,0.0001,0.0};

	IDcounter=0;
	_lVids.clear();
	_lTriangles.clear();
	_lType=-1;
   
	double dAirportAlt = pView->GetAirside3D()->GetActiveAirport()->m_altAirport.getElevation();

	GenAllTriBlock(GetContour(), dAirportAlt);

	size_t nTriCount=_lTriangles.size();

    glEnable(GL_LIGHTING);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	CTexture * pTexture = pView->getTextureResource()->getTexture("Hill");
	if(pTexture)pTexture->Apply();
	glColor3ub(255,255,255);

	for(size_t i=0;i<nTriCount;i++){
		//get the normal of triangle
		
		CPoint2008 p1,p2,p3;
		//ARCPoint p1,p2,p3;
		p1=_lTriangles[i].pts[0];p2=_lTriangles[i].pts[1];p3=_lTriangles[i].pts[2];
		double t_normalx=p1.getY()*(p2.getZ()-p3.getZ()) + p2.getY()*(p3.getZ()-p1.getZ()) + p3.getY()*(p1.getZ()-p2.getZ());
		//double t_normalx 
		double t_normaly=p1.getZ()*(p2.getX()-p3.getX()) + p2.getZ()*(p3.getX()-p1.getX()) + p3.getZ()*(p1.getX()-p2.getX());
		double t_normalz=p1.getX()*(p2.getY()-p3.getY()) + p2.getX()*(p3.getY()-p1.getY()) + p3.getX()*(p1.getY()-p2.getY());
		CPoint2008 t_normal;
		t_normal.setPoint(-t_normalx,-t_normaly,-t_normalz);
		t_normal.Normalize();
        glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);

			double s,t;
			glNormal3d(t_normal.getX(),t_normal.getY(),t_normal.getZ());	

			s=p1.getX()*scoord[0]+p1.getY()*scoord[1]+p1.getZ()*scoord[2];
			t=p1.getX()*tcoord[0]+p1.getY()*tcoord[1]+p1.getZ()*tcoord[2];
			glTexCoord2d(s,t);
	
			if(p1.getZ()==max)
				glColor3ub(209,169,7);
			else if(p1.getZ()==min)
				glColor3ub(50,205,54);
			else
				glColor3ub(170,198,57);
			glVertex3d(p1.getX(),p1.getY(),p1.getZ());


			s=p2.getX()*scoord[0]+p2.getY()*scoord[1]+p2.getZ()*scoord[2];
			t=p2.getX()*tcoord[0]+p2.getY()*tcoord[1]+p2.getZ()*tcoord[2];
			glTexCoord2d(s,t);
	//		glColor3ub(0,255,255);
			if(p2.getZ()==max)
				glColor3ub(209,169,7);
			else if(p2.getZ()==min)
				glColor3ub(50,205,54);
			else
				glColor3ub(170,198,57);
			glVertex3d(p2.getX(),p2.getY(),p2.getZ());


			s=p3.getX()*scoord[0]+p3.getY()*scoord[1]+p3.getZ()*scoord[2];
			t=p3.getX()*tcoord[0]+p3.getY()*tcoord[1]+p3.getZ()*tcoord[2];
			glTexCoord2d(s,t);
	//		glColor3ub(255,0,255);
			if(p3.getZ()==max)
				glColor3ub(209,169,7);
			else if(p3.getZ()==min)
				glColor3ub(50,205,54);
			else
				glColor3ub(170,198,57);
			glVertex3d(p3.getX(),p3.getY(),p3.getZ());


		glEnd();
	}
	glDisable(GL_TEXTURE_2D);


//	glDisable(GL_CULL_FACE);

	//SurfaceDisplayProp * pDisplayPrope = (SurfaceDisplayProp* ) GetDisplayProp();

	//if(!pDisplayPrope->m_dpShape.bOn)
	//	return;

	//glEnable(GL_BLEND);
	//glEnable(GL_TEXTURE_2D);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glDisable(GL_LIGHTING);
	//glColor3ubv(pDisplayPrope->m_dpShape.cColor);
	//CTexture * pTexture = pView->getTextureResource()->getTexture(GetSurface()->GetTexture());
	//if(pTexture)pTexture->Apply();
	//glNormal3f(0,0,1);
	//ARCPoint3 buf[512];
	//GLUtesselatorObj *tess;
	//tess=gluNewTess();
	//gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
	//gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr) &glEnd);
	//gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) &SurfaceVertexdv);
	//gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	//gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	//gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	//gluTessBeginPolygon(tess,NULL);
	//gluTessBeginContour(tess);	
	//for( int i=0;i<GetSurface()->GetPath().getCount();i++){
	//	buf[i] = ARCVector3( GetSurface()->GetPath().getPoint(i) );
	//	gluTessVertex(tess,buf[i],buf[i]);
	//}
	//gluTessEndContour(tess);
	//gluTessEndPolygon(tess);
	//gluDeleteTess(tess);	

	//if(IsInEdit())
	//{
	//	glDisable(GL_TEXTURE_2D);
	//	for(ALTObjectControlPointList::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++)
	//	{
	//		(*itr)->DrawSelect(pView);		 
	//	}
	//}
}