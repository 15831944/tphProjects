#include "stdafx.h"
#include "Structure.h"
#include "../Engine/terminal.h"
/*#include "CommonData/Textures.h"*/
#include "../Main/TessellationManager.h"
#include "CommonData/SurfaceMaterialLib.h"
#include "Main/glrender/glTextureResource.h"
//test data

CStructure::CStructure()
{
	m_strText= _T("");	
//	m_NameStr = "";
	m_FloorNum=0;
	m_dsiplaylist=0;
	m_bDirtflag=true;
}
CStructure::CStructure(Point  * pointlist,int pointNum/*,GLuint texid*/){
	for(int i=0;i<pointNum;i++){
		addPoint(pointlist[i]);
	}
//	m_NameStr = "";
	setTexture(m_strText);
	
}

CStructure::~CStructure()
{
	glDeleteLists(m_dsiplaylist,1);
}

static void DrawFlatPoint(double x, double y, double z, double size) {
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(size,size,size);
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	glEnd();
	glPopMatrix();
}

void CStructure::DrawSelectArea(double dAlt){
	genDisList();
	glPushMatrix();
	glTranslated(0.0,0.0,dAlt);
		glCallList(m_dsiplaylist);
	glPopMatrix();
}

static GLdouble scoord[]={0.0004,0.0};
static GLdouble tcoord[]={0.0,0.0004};
void __stdcall StructglVertexdv(double* v){
	GLdouble s=v[0]*scoord[0]+v[1]*scoord[1]+v[2]*scoord[2];
	GLdouble t=v[0]*tcoord[0]+v[1]*tcoord[1]+v[2]*tcoord[2];
	glTexCoord2d(s,t);
	glVertex3dv(v);
}

void CStructure::genDisList(){

	glDeleteLists(m_dsiplaylist,1);
	m_dsiplaylist=glGenLists(1);
	glNewList(m_dsiplaylist,GL_COMPILE);	

	GLdouble buf[512][3];
	GLUtesselatorObj *tess;
	tess=gluNewTess();
	gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
	gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr) &glEnd);
	gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) &StructglVertexdv);
	gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	gluTessBeginPolygon(tess,NULL);
	gluTessBeginContour(tess);	
	for(unsigned int i=0;i<m_pointlist.size();i++){
		buf[i][0]=m_pointlist[i]._p.getX();
		buf[i][1]=m_pointlist[i]._p.getY();
		buf[i][2]=0.0;

		gluTessVertex(tess,buf[i],buf[i]);
	}
	gluTessEndContour(tess);
	gluTessEndPolygon(tess);
	gluDeleteTess(tess);	
	
	glEndList();
}

void CStructure::DrawOGL(CTextureResource* TextureRes,CSurfaceMaterialLib& SurfaceMaterial,double dAlt, BOOL InEditMode){
		
	GLuint glTexid = 0;
	CString texturefilename = SurfaceMaterial.GetTexturefilename(m_strText);
	CTexture* Texture = TextureRes->NewTextureAndReside(texturefilename,m_strText);

	if(m_bDirtflag==true){
		genDisList();
		m_bDirtflag=false;
	}

	if (Texture)
	{
		Texture->Apply();
	}
	glPushMatrix();
	glTranslated(0.0,0.0,dAlt);
	glCallList(m_dsiplaylist);	
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	if(InEditMode){		
		//draw outline and the points		
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_LINE_LOOP);
		for(unsigned int i=0;i<m_pointlist.size();i++){
			glVertex3d(m_pointlist[i]._p.getX(),m_pointlist[i]._p.getY(),dAlt);
		}
		glEnd();	
		
		//glPointSize(5.0f);
		//glBegin(GL_POINTS);	

		for(unsigned int i=0;i<m_pointlist.size();i++){
			Point & p=m_pointlist[i]._p;
			//glVertex3d(p.getX(),p.getY(),dAlt);
			DrawFlatPoint(p.getX(),p.getY(),dAlt,50.0);
		}
		//glEnd();
		
		//glPointSize(1.0f);
		
		
	}
	
	
}

void CStructure::SetTerminal( Terminal* _pTerm )
{ 
	m_pTerm = _pTerm;  
	m_strucID.SetStrDict( _pTerm->inStrDict ); 
}

CStructure* CStructure::GetNewCopy()
{
	CStructure *result = new CStructure;
	*result = *this;
	result->m_bDirtflag = true;	
	result->m_dsiplaylist  = -1;
	return result;
}


CStructure* StructureArray::FindStructureByGuid( const CGuid& guid ) const
{
	int nCount = getCount();
	for(int i=0;i<nCount;i++)
	{
		CStructure* pItem = getItem(i);
		if (pItem->getGuid() == guid)
			return pItem;
	}
	return NULL;
}