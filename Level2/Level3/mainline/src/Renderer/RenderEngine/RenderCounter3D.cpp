#include "StdAfx.h"
#include ".\RenderCounter3D.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside/Surface.h"
#include "Boundbox3D.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include ".\MaterialDef.h"
#include "ShapeBuilder.h"
#include "InputAirside/contour.h"
#include <deque>
#include "Common\IARCportLayoutEditContext.h"
#include "InputAirside/ALTAirportLayout.h"
#include "../include/GL/glu.h"

typedef void (CALLBACK * GLUTesselatorFunction)(void);
using namespace Ogre;

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

}

void __stdcall errorCallback(GLenum errorCode)
{
	const GLubyte* estring;
	estring = gluErrorString(errorCode);
	ASSERT(FALSE);
}


void __stdcall combineCallback( GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut ) 
{ 
	GLdouble *vertex = new GLdouble[3];

	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	//for( int i=3; i<6; i++ ) {
	//	vertex[i]
	//}

	*dataOut = vertex; 
} 

unsigned int IDcounter;
unsigned int layPointCount;

//CRenderCounter3D::CRenderCounter3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//	min=max=0;
//}
//
//CRenderCounter3D::~CRenderCounter3D(void)
//{
//}

void CRenderCounter3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_CONTOUR)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName());// OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);

	Build(pBaseObj,pObj);

	AddObject(pObj,true);
	//UpdateSelected(GetSelected());
}

void CRenderCounter3D::UpdateSelected( bool b )
{

}

void CRenderCounter3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj )
{
	if(pBaseObject == NULL || pBaseObject->GetType() != ALT_CONTOUR)
		return;

	Contour *pContour = (Contour *)pBaseObject;
	if(pContour == NULL)
		return;

	CString strText = _T("surfaceterminal/hilltexture");
	strText.MakeLower();
	DrawContour(pObj,strText,pContour);
}

void CRenderCounter3D::DrawContour(Ogre::ManualObject* pObj,CString matName, Contour *pContour)
{
// 	double scoord[]={0.0001,0.0,0.0};
// 	double tcoord[]={0.0,0.0001,0.0};

	IDcounter=0;
	_lVids.clear();
	_lTriangles.clear();
	_lType=-1;

	InputAirside *pAirside =  GetRoot().GetModel()->OnQueryInputAirside();
	ALTAirportLayout *pAirport = pAirside->GetActiveAirport();
	DistanceUnit dAirportAlt = pAirport->getElevation();

	GenAllTriBlock(pContour, dAirportAlt);

	size_t nTriCount=_lTriangles.size();

	const Matrix4& texCoordTrans = Matrix4::IDENTITY;

	ARCColor3 color1(209,169,7);
	Ogre::ColourValue ogreColor1 = OgreConvert::GetColor(color1);

	ARCColor3 color2(50,205,54);
	Ogre::ColourValue ogreColor2 = OgreConvert::GetColor(color2);

	ARCColor3 color3(170,198,57);
	Ogre::ColourValue ogreColor3 = OgreConvert::GetColor(color3);

	for(size_t i=0;i<nTriCount;i++){
		//get the normal of triangle

		CPoint2008 p1,p2,p3;
		//ARCPoint p1,p2,p3;
		p1=_lTriangles[i].pts[0];p2=_lTriangles[i].pts[1];p3=_lTriangles[i].pts[2];
		double t_normalx=p1.getY()*(p2.getZ()-p3.getZ()) + p2.getY()*(p3.getZ()-p1.getZ()) + p3.getY()*(p1.getZ()-p2.getZ());
		//double t_normalx 
		double t_normaly=p1.getZ()*(p2.getX()-p3.getX()) + p2.getZ()*(p3.getX()-p1.getX()) + p3.getZ()*(p1.getX()-p2.getX());
		double t_normalz=p1.getX()*(p2.getY()-p3.getY()) + p2.getX()*(p3.getY()-p1.getY()) + p3.getX()*(p1.getY()-p2.getY());
		ARCVector3 t_normal(-t_normalx,-t_normaly,-t_normalz);
		t_normal.Normalize();
// 		glShadeModel(GL_SMOOTH);
// 		glBegin(GL_TRIANGLES);
		pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_LIST);
		pObj->position(p1.getX(),p1.getY(),p1.getZ());
		pObj->normal(t_normal.n[VX],t_normal.n[VY],t_normal.n[VZ]);
		

		if(p1.getZ()==max)
			pObj->colour(ogreColor1);
		else if(p1.getZ()==min)
			pObj->colour(ogreColor2);
		else
			pObj->colour(ogreColor3);
		Vector3 vPos1(p1.getX(),p1.getY(),p1.getZ());
		Vector3 texcod1 =  texCoordTrans.transformAffine(vPos1);
		pObj->textureCoord(texcod1.x,texcod1.y);

		pObj->position(p2.getX(),p2.getY(),p2.getZ());
		if(p2.getZ()==max)
			pObj->colour(ogreColor1);
		else if(p2.getZ()==min)
			pObj->colour(ogreColor2);
		else
			pObj->colour(ogreColor3);
		Vector3 vPos2(p2.getX(),p2.getY(),p2.getZ());
		Vector3 texcod2 =  texCoordTrans.transformAffine(vPos2);
		pObj->textureCoord(texcod2.x,texcod2.y);
		
		pObj->position(p3.getX(),p3.getY(),p3.getZ());
		if(p3.getZ()==max)
			pObj->colour(ogreColor1);
		else if(p3.getZ()==min)
			pObj->colour(ogreColor2);
		else
			pObj->colour(ogreColor3);
		Vector3 vPos3(p3.getX(),p3.getY(),p3.getZ());
		Vector3 texcod3 =  texCoordTrans.transformAffine(vPos3);
		pObj->textureCoord(texcod3.x,texcod3.y);
		
		pObj->end();
	}
}

void CRenderCounter3D::GenAllTriBlock(Contour* m_pFather,const DistanceUnit& dAptAlt)
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
	gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr)_tBegin);    
	gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr)_tEnd);
	gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr)_tVertex);
	gluTessCallback(tess, GLU_TESS_EDGE_FLAG,(_GLUfuncptr)glEdgeFlag);
	gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr)combineCallback);
	gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr)errorCallback);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

	gluTessNormal(tess,0.0,0.0,1.0);

	gluTessBeginPolygon(tess, NULL);
	gluTessBeginContour(tess);

	int noffset = 0;
	for(i=0; i<m_pFather->GetPath().getCount(); i++)
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

	for(i=0; i<m_pFather->GetChildCount(); i++)
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

ALTObject* CRenderCounter3D::GetBaseObject() const
{
	return NULL;
}

void CRenderCounter3D::Setup3D( ALTObject* pBaseObj)
{
	Update3D(pBaseObj);
}

