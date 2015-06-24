// Floor2.cpp: implementation of the CFloor2 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "termplan.h"
#include "Floor2.h"

#include "pbuffer.h"
#include "TessellationManager.h"
#include "TVNode.h"
#include "3DView.h"
#include <common\ProgressBar.h>
#include <CXImage/ximage.h>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static const int TEX_SIZE=1024;
static const double TEX_WORLD_SIZE=10000.0;
static BOOL AUTO_SIZE_WORLD=TRUE;
static BOOL AUTO_SIZE_TEX=FALSE;

//////////////////////////////////////////////////////////////////////////

inline static void DrawThickRect(double cx, double cy, double x, double y, double thickness)
{
	glPushMatrix();
	glTranslated(cx, cy, 0.0);

	glBegin(GL_QUADS);		
	//top
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(-x, -y, 0.0);
	glVertex3d(x, -y, 0.0);
	glVertex3d(x, y, 0.0);
	glVertex3d(-x, y, 0.0);
	//bottom
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(-x, -y, -thickness);
	glVertex3d(x, -y, -thickness);
	glVertex3d(x, y, -thickness);
	glVertex3d(-x, y, -thickness);
	//sides
	//-y
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(-x, -y, 0.0);
	glVertex3d(-x, -y, -thickness);
	glVertex3d(x, -y, -thickness);
	glVertex3d(x, -y, 0.0);
	//+y
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(-x, y, 0.0);
	glVertex3d(-x, y, -thickness);
	glVertex3d(x, y, -thickness);
	glVertex3d(x, y, 0.0);
	//-x
	glNormal3d(-1.0, 0.0, 0.0);
	glVertex3d(-x, y, 0.0);
	glVertex3d(-x, y, -thickness);
	glVertex3d(-x, -y, -thickness);
	glVertex3d(-x, -y, 0.0);
	//+x
	glNormal3d(1.0, 0.0, 0.0);
	glVertex3d(x, y, 0.0);
	glVertex3d(x, y, -thickness);
	glVertex3d(x, -y, -thickness);
	glVertex3d(x, -y, 0.0);
	//
	glEnd();

	glPopMatrix();
}


inline static void DrawFloorGrid(const CGrid& _grid, BOOL bIsActive)
{
	if(_grid.bVisibility) {
		glNormal3d(0.0,0.0,1.0);
		glDepthMask(FALSE);
		//1) draw lines & subdivs
		double i;
		glBegin(GL_LINES);
		if(_grid.nSubdivs > 1) {
			glColor4ub(_grid.cSubdivsColor[RED],_grid.cSubdivsColor[GREEN],_grid.cSubdivsColor[BLUE], 64);
			double step = _grid.dLinesEvery/_grid.nSubdivs;
			for(i=step; i<=_grid.dSizeX; i+=step) {
				glVertex3d(i, _grid.dSizeY, 0.0);
				glVertex3d(i, -_grid.dSizeY, 0.0);
				glVertex3d(-i, _grid.dSizeY, 0.0);
				glVertex3d(-i, -_grid.dSizeY, 0.0);
			}
			for(i=step; i<=_grid.dSizeY; i+=step) {
				glVertex3d(_grid.dSizeX, i, 0.0);
				glVertex3d(-_grid.dSizeX, i, 0.0);
				glVertex3d(_grid.dSizeX, -i, 0.0);
				glVertex3d(-_grid.dSizeX, -i, 0.0);
			}
		}
		glColor4ub(_grid.cLinesColor[RED],_grid.cLinesColor[GREEN],_grid.cLinesColor[BLUE], 64);
		for(i=_grid.dLinesEvery; i<=_grid.dSizeX; i+=_grid.dLinesEvery) {
			glVertex3d(i, _grid.dSizeY, 0.0);
			glVertex3d(i, -_grid.dSizeY, 0.0);
			glVertex3d(-i, _grid.dSizeY, 0.0);
			glVertex3d(-i, -_grid.dSizeY, 0.0);
		}
		for(i=_grid.dLinesEvery; i<=_grid.dSizeY; i+=_grid.dLinesEvery) {
			glVertex3d(_grid.dSizeX, i, 0.0);
			glVertex3d(-_grid.dSizeX, i, 0.0);
			glVertex3d(_grid.dSizeX, -i, 0.0);
			glVertex3d(-_grid.dSizeX, -i, 0.0);
		}
		glEnd();

		if(bIsActive)
			glColor3ub(0,0,75);
		else
			glColor3ubv(_grid.cAxesColor);
		glBegin(GL_LINES);
		glVertex3d(0.0, _grid.dSizeY, 0.0);
		glVertex3d(0.0, -_grid.dSizeY, 0.0);
		glVertex3d(_grid.dSizeX, 0.0, 0.0);
		glVertex3d(-_grid.dSizeX, 0.0, 0.0);
		glEnd();
		glDepthMask(TRUE);
	}
}

static const char* FLRPREFIX[] = {
	"1st",
		"2nd",
		"3rd",
};


//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloor2::CFloor2(int nLevel)
	: CRenderFloor(nLevel)
{
	InitDefault_();
}

CFloor2::CFloor2(int nLevel, const CString& sName)
	: CRenderFloor(nLevel, sName)
{
	InitDefault_();
}

CFloor2::~CFloor2()
{
	DeleteAllVisibleRegions();
	glDeleteLists(m_nMapDLID,1);
	glDeleteLists(m_nVRsurfaceDLID,1);
	glDeleteLists(m_nVRborderDLID,1);
	glDeleteLists(m_nVRsidesDLID,1);
	if (m_bPicTextureValid)
	{
		glDeleteTextures(1,&m_iPicTexture);
	}
	/*if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}*/
}

void CFloor2::DrawGrid(double dAlt)
{
	glPushMatrix();
	glTranslated(0,0,dAlt);
	DrawFloorGrid(m_Grid, m_bIsActive);
	glPopMatrix();	
}

void CFloor2::DrawOGLasLines(double dAlt)
{
	if(!m_bIsVisible)return;
	glPushMatrix();
	glTranslated(0.0,0.0,dAlt);

	if(m_bIsMapValid && m_bIsShowMap) {
		glPushMatrix();
		glTranslated(m_vOffset[VX], m_vOffset[VY], 0.0);
		glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
		//translate
		glTranslated(m_CadFile.vOffset[VX], m_CadFile.vOffset[VY], 0.0);
		//rotate
		//glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
		//scale
		glScaled(m_CadFile.dScale,m_CadFile.dScale,m_CadFile.dScale);
		//draw layers
		for(size_t i=0; i < m_vLayers.size(); i++) {
			((CCADLayer*) m_vLayers[i])->DrawOGL();
		}
		glPopMatrix();
	}
	//draw grid
	//DrawGrid(m_Grid, m_bIsActive);
	glPopMatrix();
}

//---------------------------START---------------------------------
// 
//for get outline of visible region just for test
GLenum  _glbeginType;
std::vector<Point> _lVids;// store the vertex 
std::vector< std::vector<Point> > _VRoutlines;
std::vector< std::vector<Point> > _InVRoutlines;
std::vector< std::vector<Point> > _AllMergeoutlines;
enum whRegion{VREGION,INVREGION,ALLMERGEVR};
whRegion Regionwhich;
void  __stdcall   _tFloorBegin  (GLenum type)  { 
	_glbeginType = type;_lVids.clear(); 
}; 
void  __stdcall   _tFloorVertex (void * pdata)   
{ 
	GLdouble *vexdata=(GLdouble  *) pdata;
	_lVids.push_back( Point(vexdata[0],vexdata[1],vexdata[2]) );
}
void __stdcall _tFloorcombineCallback( GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut ) 
{ 
	_lVids.push_back(Point (coords[0],coords[1],coords[2]));
	GLdouble *vertex = new GLdouble[3];
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	*dataOut = vertex; 
} 
void  __stdcall   _tFloorEnd    (void){
	if(_glbeginType== GL_LINE_LOOP){
		switch (Regionwhich){			
		case VREGION :
			_VRoutlines.push_back(_lVids);
			break;
		case INVREGION :
			_InVRoutlines.push_back(_lVids);
			break;
		case ALLMERGEVR :
			_AllMergeoutlines.push_back(_lVids);
			break;
		}

	}
	_glbeginType=-1;

}
void __stdcall floorglVertexdv(double* v){

	glTexCoord2d(v[3],v[4]);
	glVertex3dv(v);
}
//-------------------------END-----------------------------------
//
void CFloor2::DrawOGLVisRegions(bool bThick, bool bDrawOutline,bool bDrawTop)
{
	if(m_bVRDirtyFlag) {
		// need to re-tesselate VRs
		glDeleteLists(m_nVRsurfaceDLID, 1);
		glDeleteLists(m_nVRsidesDLID, 1);
		glDeleteLists(m_nVRborderDLID, 1);
		_VRoutlines.clear();
		_InVRoutlines.clear();
		_AllMergeoutlines.clear();

		//get the bufsize;
		int bufsize=0;
		int vrptcount=0;
		for(size_t i=0;i<m_vVisibleRegions.size(); ++i){
			vrptcount+= m_vVisibleRegions[i]->polygon.getCount();
		}
		int invrptcount=0;
		for(size_t i=0;i<m_vInVisibleRegions.size(); ++i){
			invrptcount+= m_vInVisibleRegions[i]->polygon.getCount();
		}
		bufsize=max(vrptcount,invrptcount);

		double **vPts=new double* [bufsize];
		for(int i=0;i<bufsize;++i)vPts[i]=new double[3];


		int nPtIdx = 0;
		GLUtesselatorObj *tess;
		tess=gluNewTess();
		gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) &_tFloorBegin);    
		gluTessCallback(tess, GLU_TESS_END, (_GLUfuncptr) &_tFloorEnd);
		gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) & _tFloorVertex);
		gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
		gluTessCallback(tess, GLU_TESS_COMBINE, (_GLUfuncptr) &_tFloorcombineCallback);
		gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
		gluTessProperty(tess,GLU_TESS_BOUNDARY_ONLY,GL_TRUE);
		gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);

		nPtIdx=0;_VRoutlines.clear();
		Regionwhich=VREGION;
		size_t nVRCount = m_vVisibleRegions.size(); 

		gluTessBeginPolygon(tess,NULL);		
		for(size_t i=0; i<nVRCount; i++) {
			CVisibleRegion* pVR = m_vVisibleRegions[i];
			int nPtCount = pVR->polygon.getCount();
			Point* pPtList = pVR->polygon.getPointList();
			gluTessBeginContour(tess);	
			if(pVR->polygon.IsCountClock()){
				for(int k=0; k<nPtCount; k++) {
					vPts[nPtIdx+k][0] = pPtList[k].getX();
					vPts[nPtIdx+k][1] = pPtList[k].getY();
					vPts[nPtIdx+k][2] = 0.0;
					gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
				}
				nPtIdx += nPtCount;
			}
			else{
				for(int k=0; k<nPtCount; k++) {
					vPts[nPtIdx+k][0]  = pPtList[nPtCount-k-1].getX();
					vPts[nPtIdx+k][1]  = pPtList[nPtCount-k-1].getY();
					vPts[nPtIdx+k][2]  = 0.0;
					gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
				}
				nPtIdx += nPtCount;
			}			
			gluTessEndContour(tess);			
		}
		gluTessEndPolygon(tess);		
		//Invisible regions
		//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NEGATIVE);
		nPtIdx=0;_InVRoutlines.clear();	
		Regionwhich=INVREGION;
		gluTessBeginPolygon(tess,NULL);	

		size_t nInVRCount=m_vInVisibleRegions.size();
		for(size_t i=0; i<nInVRCount; i++) {
			int nIn=m_vInVisibleRegions.size();
			CVisibleRegion* pVR = m_vInVisibleRegions[i];
			int nPtCount = pVR->polygon.getCount();
			Point* pPtList = pVR->polygon.getPointList();
			gluTessBeginContour(tess);	
			if(! pVR->polygon.IsCountClock()){
				for(int k=0; k<nPtCount; k++) {
					vPts[nPtIdx+k][0]= pPtList[k].getX();
					vPts[nPtIdx+k][1] = pPtList[k].getY();
					vPts[nPtIdx+k][2] = 0.0;
					gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
				}
				nPtIdx += nPtCount;
			}
			else{
				for(int k=0; k<nPtCount; k++) {
					vPts[nPtIdx+k][0]  = pPtList[nPtCount-k-1].getX();
					vPts[nPtIdx+k][1] = pPtList[nPtCount-k-1].getY();
					vPts[nPtIdx+k][2] = 0.0;
					gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
				}
				nPtIdx += nPtCount;
			}			
			gluTessEndContour(tess);			
		}		
		gluTessEndPolygon(tess);

		for(int i=0;i<bufsize;++i)delete[] vPts[i];
		delete[] vPts;
		//get the merg outline of VR and InVR
		//get all merge regions
		bufsize=0;
		for(size_t i=0;i<_VRoutlines.size(); ++i){
			bufsize += _VRoutlines[i].size();
		}
		for(size_t i=0;i<_InVRoutlines.size(); ++i){
			bufsize += _InVRoutlines[i].size();
		}
		vPts=new double* [bufsize];
		for(int i=0;i<bufsize;++i)vPts[i]=new double[3];

		gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);	
		_AllMergeoutlines.clear();
		Regionwhich=ALLMERGEVR;
		nPtIdx=0;
		gluTessBeginPolygon(tess,NULL);
		for(size_t i=0;i<_VRoutlines.size();i++){
			int nPtCount = _VRoutlines[i].size();
			gluTessBeginContour(tess);
			for(int k=0; k<nPtCount; k++) {
				vPts[nPtIdx+k][0]= _VRoutlines[i][k].getX();
				vPts[nPtIdx+k][1] = _VRoutlines[i][k].getY();
				vPts[nPtIdx+k][2]= 0.0;
				gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
			}
			nPtIdx += nPtCount;
			gluTessEndContour(tess);
		}
		for(size_t i=0;i<_InVRoutlines.size() ;i++){
			int nPtCount = _InVRoutlines[i].size();
			gluTessBeginContour(tess);
			for(int k=0; k<nPtCount; k++) {
				vPts[nPtIdx+k][0] = _InVRoutlines[i][k].getX();
				vPts[nPtIdx+k][1] = _InVRoutlines[i][k].getY();
				vPts[nPtIdx+k][2] = 0.0;
				gluTessVertex(tess,vPts[nPtIdx+k],vPts[nPtIdx+k]);
			}
			nPtIdx += nPtCount;
			gluTessEndContour(tess);
		}
		gluTessEndPolygon(tess);


		for(int i=0;i<bufsize;++i)delete[] vPts[i];
		delete[] vPts;
		//get bufsize 
		bufsize=0;
		for(size_t i=0;i<_AllMergeoutlines.size(); ++i){
			bufsize += _AllMergeoutlines[i].size();
		}		
		vPts=new double* [bufsize];
		for(int i=0;i<bufsize;++i)vPts[i]=new double[3];

		gluDeleteTess(tess);
		m_outlines=_AllMergeoutlines;
		// [1] Surface
		m_nVRsurfaceDLID = glGenLists(1);

		CTessellationManager* pTM = CTessellationManager::GetInstance();
		pTM->Init(CTessellationManager::Render);		

		glNewList( m_nVRsurfaceDLID, GL_COMPILE	);
		nPtIdx=0;
		pTM->BeginPolygon();		
		glNormal3d(0,0,1);
		for(size_t i=0; i<_AllMergeoutlines.size(); i++) {

			size_t nPtCount=_AllMergeoutlines[i].size();
			pTM->BeginContour();
			for(size_t k=0; k<nPtCount; k++) {
				Point  _p=_AllMergeoutlines[i][k];
				vPts[nPtIdx+k][0] = _p.getX();
				vPts[nPtIdx+k][1] = _p.getY();
				vPts[nPtIdx+k][2] = 0.0;
				pTM->ContourVertex(vPts[nPtIdx+k]);
			}
			nPtIdx += nPtCount;
			pTM->EndContour();
		}

		pTM->EndPolygon();
		pTM->End();
		glEndList();

		for(int i=0;i<bufsize;++i)delete[] vPts[i];
		delete[] vPts;
		// [2] Sides & Border
		m_nVRsidesDLID = glGenLists(1);
		glNewList(m_nVRsidesDLID, GL_COMPILE);
		glBegin(GL_QUADS);

		for(size_t i=0; i<_AllMergeoutlines.size(); i++) {
			size_t nCount=_AllMergeoutlines[i].size();			
			for(size_t k=0; k<_AllMergeoutlines[i].size()-1; k++) {
				Point _p=_AllMergeoutlines[i][k];
				glVertex3d(_p.getX(),_p.getY(),_p.getZ());
				glVertex3d(_p.getX(),_p.getY(),_p.getZ()-m_dThickness);
				_p=_AllMergeoutlines[i][k+1];
				glVertex3d(_p.getX(),_p.getY(),_p.getZ()-m_dThickness);
				glVertex3d(_p.getX(),_p.getY(),_p.getZ());
			}	
			Point _p=_AllMergeoutlines[i][nCount-1];
			glVertex3d(_p.getX(),_p.getY(),_p.getZ());
			glVertex3d(_p.getX(),_p.getY(),_p.getZ()-m_dThickness);
			_p=_AllMergeoutlines[i][0];
			glVertex3d(_p.getX(),_p.getY(),_p.getZ()-m_dThickness);
			glVertex3d(_p.getX(),_p.getY(),_p.getZ());
		}		

		glEnd();
		glEndList();
		//outline of bound		

		m_nVRborderDLID = glGenLists(1);
		glNewList(m_nVRborderDLID, GL_COMPILE);
		for(size_t i=0;i<_AllMergeoutlines.size();i++){
			glBegin(GL_LINE_LOOP);
			for(size_t j=0;j<_AllMergeoutlines[i].size();j++){
				Point& _p=_AllMergeoutlines[i][j];
				glVertex3d(_p.getX(),_p.getY(),_p.getZ());
			}
			glEnd();			
		}			
		glEndList();
		m_bVRDirtyFlag = FALSE;
		m_bMapdirtyFlag = TRUE;
	}

	ASSERT(glIsList(m_nVRsurfaceDLID));
	ASSERT(glIsList(m_nVRsidesDLID));
	ASSERT(glIsList(m_nVRborderDLID));
	// VR display lists have already been created
	// so draw using DLs
	glColor4ubv(ARCColor4::VISIBLEREGION);
	if(bDrawTop)
		glCallList(m_nVRsurfaceDLID); // top surface

	if(bThick) {
		glPushMatrix();
		glTranslated(0.0, 0.0, -m_dThickness);
		glCallList(m_nVRsurfaceDLID); // bottom surface
		glPopMatrix();
		glCallList(m_nVRsidesDLID);   // sides
	}

	if(bDrawOutline) {
		glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
		glCallList(m_nVRborderDLID);	// top outline

		if(bThick) {
			glPushMatrix();
			glTranslated(0.0, 0.0, -m_dThickness);
			glCallList(m_nVRborderDLID);	// bottom outline
			glPopMatrix();
		}
	}



}

void CFloor2::DrawOGLNoTexture(bool bMinimal,double dAlt)
{
	if(!m_bIsVisible)
		return;	
	//translate to altitude
	glPushMatrix();
	glTranslated(0,0,dAlt);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f,1.0f);
	DrawPicture(0);

	if(m_bIsMapValid && m_bIsShowMap) {
		// Map: ON
		if(m_bUseVisibleRegions) {
			//  VR: ON		
			glPolygonOffset(2.0,2.0);
			DrawOGLVisRegions(m_bIsOpaque ? true : false, !bMinimal);			
		}
		else {
			// VR: OFF
			ARCVector2 vWorldSize((m_vMapExtentsMax - m_vMapExtentsMin)*m_CadFile.dScale);
			int mapSizeWorld[2];
			double u[2];
			double v[2];
			mapSizeWorld[0] = (int) ceil(vWorldSize[VX]/2);
			mapSizeWorld[1] = (int) ceil(vWorldSize[VY]/2);
			if(vWorldSize[VX] > vWorldSize[VY]) { //x>y
				u[0] = 0.0; u[1] = 1.0;
				v[0] = 1.0-(1/m_dMapAspectRatio); v[1] = 1/m_dMapAspectRatio;
			}
			else {
				u[0] = 1.0-m_dMapAspectRatio; u[1] = m_dMapAspectRatio;
				v[0] = 0.0; v[1] = 1.0;
			}			
			//translate to offset
			glPushMatrix();			
			glTranslated(m_vOffset[VX], m_vOffset[VY], 0.0);
			glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);			
			glPolygonOffset(0,0);
			if(m_bIsOpaque) {
				// Thickness: ON				
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
				DrawThickRect(0.0, 0.0, mapSizeWorld[0], mapSizeWorld[1], m_dThickness);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glPolygonOffset(2.0,2.0);		
				glColor4ubv(ARCColor4::VISIBLEREGION);
				DrawThickRect(0.0, 0.0, mapSizeWorld[0], mapSizeWorld[1], m_dThickness);

			}
			else {
				// Thickness: OFF
				//draw outline of map
				glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
				glBegin(GL_LINE_LOOP);
				glVertex2d(-mapSizeWorld[0],-mapSizeWorld[1]);
				glVertex2d(mapSizeWorld[0],-mapSizeWorld[1]);
				glVertex2d(mapSizeWorld[0],mapSizeWorld[1]);
				glVertex2d(-mapSizeWorld[0],mapSizeWorld[1]);
				glEnd();
				glColor4ubv(ARCColor4::VISIBLEREGION);
				glPolygonOffset(2.0,2.0);	
				glBegin(GL_QUADS);
				glNormal3d(0.0,0.0,1.0);
				glVertex2d(-mapSizeWorld[0],-mapSizeWorld[1]);
				glVertex2d(mapSizeWorld[0],-mapSizeWorld[1]);
				glVertex2d(mapSizeWorld[0],mapSizeWorld[1]);
				glVertex2d(-mapSizeWorld[0],mapSizeWorld[1]);
				glEnd();			
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
			glPopMatrix();
		}
	}
	else {
		// Map: OFF
		if(m_bIsOpaque) {
			// Thickness: ON
			if(m_bUseVisibleRegions) {
				// VR: ON
				glPolygonOffset(2.0,2.0);
				DrawOGLVisRegions(true, !bMinimal);
			}
			else {
				// VR: OFF			

				if(!bMinimal) {	
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
					DrawThickRect(0.0, 0.0, m_Grid.dSizeX, m_Grid.dSizeY, m_dThickness);
				}

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPolygonOffset(2.0,2.0);
				glColor4ubv(ARCColor4::VISIBLEREGION);
				DrawThickRect(0.0, 0.0, m_Grid.dSizeX, m_Grid.dSizeY, m_dThickness);
			}
		}
		else {
			/// Thickness: OFF
			if(m_bUseVisibleRegions) {
				//VR: ON
				DrawOGLVisRegions(false, !bMinimal);
			}
			else {
				//VR: OFF
			}
		}
	}
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopMatrix();
}



void CFloor2::DrawOGL(C3DView * pView, double dAlt)
{	
	m_pView=pView;
	if(!m_bIsVisible)
		return;

	if(!m_bIsTexValid && m_bIsMapValid && m_bIsCADProcessed) {
		//create the texture
		GenerateTextureMap();
	}

	//translate to altitude
	glPushMatrix();
	glTranslated(0.0, 0.0, dAlt);

	{
		// Visible: ON
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f,1.0f);
		DrawPicture(0);

		if(m_bIsMapValid && m_bIsTexValid && m_bIsShowMap) {
			//Map: ON
			glPolygonOffset(2.0f,2.0f);
			ARCVector2 vWorldSize((m_vMapExtentsMax - m_vMapExtentsMin)*m_CadFile.dScale);
			int mapSizeWorld[2];
			double u[2];
			double v[2];
			mapSizeWorld[0] = (int) ceil(vWorldSize[VX]/2);
			mapSizeWorld[1] = (int) ceil(vWorldSize[VY]/2);
			if(vWorldSize[VX] > vWorldSize[VY]) { //x>y
				u[0] = 0.0; u[1] = 1.0;
				v[0] = 0.5*(1.0-(1.0/m_dMapAspectRatio)); v[1] = 1.0-v[0];
			}
			else {
				u[0] = 0.5*(1.0-m_dMapAspectRatio); u[1] = 1.0-u[0];
				v[0] = 0.0; v[1] = 1.0;
			}		

			//translate to offset			

			glEnable(m_eTexTarget);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			//glColor4ubv(ARCColor4::WHITE);		
			glColor4ubv(ARCColor4::VISIBLEREGION);
			glBindTexture(m_eTexTarget, m_piTexId[0]);
			GLint nMagFilter, nMinFilter;
			glGetTexParameteriv(m_eTexTarget, GL_TEXTURE_MAG_FILTER, &nMagFilter);
			glGetTexParameteriv(m_eTexTarget, GL_TEXTURE_MIN_FILTER, &nMinFilter);
			glTexParameteri(m_eTexTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(m_eTexTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);

			if(!m_bUseVisibleRegions){
				glPushMatrix();
				glTranslated(m_vOffset[VX], m_vOffset[VY], 0.0);
				glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
				glBegin(GL_QUADS);
				glNormal3d(0.0,0.0,1.0);
				glTexCoord2d(u[0],v[0]);
				glVertex2d(-mapSizeWorld[0],-mapSizeWorld[1]);
				glTexCoord2d(u[1], v[0]);
				glVertex2d(mapSizeWorld[0],-mapSizeWorld[1]);
				glTexCoord2d(u[1], v[1]);
				glVertex2d(mapSizeWorld[0],mapSizeWorld[1]);
				glTexCoord2d(u[0],v[1]);
				glVertex2d(-mapSizeWorld[0],mapSizeWorld[1]);
				glEnd();
				glPopMatrix();
			}
			else
			{				
				//draw culled area map
				if(m_bMapdirtyFlag){
					//generate map 
					glDeleteLists(m_nMapDLID,1);
					m_nMapDLID=glGenLists(1);

					glNewList(m_nMapDLID,GL_COMPILE);
					{
						int bufsize=0;
						for(size_t i=0;i<m_outlines.size();i++){
							bufsize += m_outlines[i].size();
						}
						double **vPts=new double* [bufsize];
						for(int i=0;i<bufsize;++i)vPts[i]=new double[5];

						int nPtIdx = 0;				
						GLUtesselator *m_pTObj;
						m_pTObj = gluNewTess();
						gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
						gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &glEnd);
						gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) & floorglVertexdv);
						gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
						gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
						gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
						gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);							

						gluTessBeginPolygon(m_pTObj, NULL);				
						for(size_t i=0; i<m_outlines.size(); i++) {					
							gluTessBeginContour(m_pTObj);
							int nPtCount = m_outlines[i].size();					
							for(int k=0; k<nPtCount; k++) {
								Point _p=m_outlines[i][k];
								vPts[nPtIdx+k][0] = _p.getX();
								vPts[nPtIdx+k][1] = _p.getY();
								vPts[nPtIdx+k][2] = 0.0;

								_p.Translated(-m_vOffset[VX], -m_vOffset[VY], 0.0);
								_p.rotate(m_CadFile.dRotation);
								vPts[nPtIdx+k][3] = (u[1]-u[0])*(_p.getX()+mapSizeWorld[0])/(2*mapSizeWorld[0])+u[0];
								vPts[nPtIdx+k][4] = (v[1]-v[0])*(_p.getY()+mapSizeWorld[1])/(2*mapSizeWorld[1])+v[0];
								gluTessVertex(m_pTObj, vPts[nPtIdx+k],vPts[nPtIdx+k]);
							}
							nPtIdx += nPtCount;
							gluTessEndContour(m_pTObj);				
						}		
						gluTessEndPolygon(m_pTObj);					
						gluDeleteTess(m_pTObj);

						//delete buf
						for(int i=0;i<bufsize;++i)delete[] vPts[i];
						delete[] vPts;
					}glEndList();
					m_bMapdirtyFlag=FALSE;
				}
				glCallList(m_nMapDLID);


			}
			glDisable(m_eTexTarget);

			glPolygonOffset(3.0f,3.0f);
			//glDepthMask(GL_TRUE);			
			//glPolygonOffset(2.0,2.0);
			if(m_bIsOpaque) {
				//Thickness: ON				

				if(m_bUseVisibleRegions) {
					// VR: ON					
					DrawOGLVisRegions(true, true,false);					
				}
				else {
					glPushMatrix();
					glTranslated(m_vOffset[VX], m_vOffset[VY], 0.0);
					glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);	
					// VR: OFF
					glColor4ubv(ARCColor4::VISIBLEREGION);
					//glPolygonOffset(2.0f,2.0f);
					glBegin(GL_QUADS);
					//bottom
					glNormal3d(0.0, 0.0, -1.0);
					glVertex3d(-mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					glVertex3d(-mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					//sides
					//-y
					glNormal3d(0.0, -1.0, 0.0);
					glVertex3d(-mapSizeWorld[0], -mapSizeWorld[1], 0.0);
					glVertex3d(-mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], -mapSizeWorld[1], 0.0);
					//+y
					glNormal3d(0.0, 1.0, 0.0);
					glVertex3d(-mapSizeWorld[0], mapSizeWorld[1], 0.0);
					glVertex3d(-mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], mapSizeWorld[1], 0.0);
					//-x
					glNormal3d(-1.0, 0.0, 0.0);
					glVertex3d(-mapSizeWorld[0], mapSizeWorld[1], 0.0);
					glVertex3d(-mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					glVertex3d(-mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(-mapSizeWorld[0], -mapSizeWorld[1], 0.0);
					//+x
					glNormal3d(1.0, 0.0, 0.0);
					glVertex3d(mapSizeWorld[0], mapSizeWorld[1], 0.0);
					glVertex3d(mapSizeWorld[0], mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], -mapSizeWorld[1], -m_dThickness);
					glVertex3d(mapSizeWorld[0], -mapSizeWorld[1], 0.0);
					//
					glEnd();

					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
					DrawThickRect(0.0, 0.0, mapSizeWorld[0], mapSizeWorld[1], m_dThickness);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glPopMatrix();
				}
			}else{
				//Thichness :OFF
				if(m_bUseVisibleRegions)
					DrawOGLVisRegions(false, true,false);	
				else
				{
					glPushMatrix();
					glTranslated(m_vOffset[VX], m_vOffset[VY], 0.0);
					glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
					DrawThickRect(0.0, 0.0, mapSizeWorld[0], mapSizeWorld[1], 0);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glPopMatrix();
				}
			}			
		}		
		else {
			// Map: OFF
			glPolygonOffset(3.0f,3.0f);
			if(m_bIsOpaque) {
				// Thickness: ON
				if(m_bUseVisibleRegions) {
					// VR: ON
					DrawOGLVisRegions(true, true);
				}
				else {
					// VR: OFF	
					//////////////////////////////////////	

					/*glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0,-1.0);		*/
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
					glColor4ubv(ARCColor4::VISIBLEREGIONOUTLINE);
					DrawThickRect(0.0, 0.0, m_Grid.dSizeX, m_Grid.dSizeY, m_dThickness);
					//glDisable(GL_POLYGON_OFFSET_LINE);

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glColor4ubv(ARCColor4::VISIBLEREGION);
					DrawThickRect(0.0, 0.0, m_Grid.dSizeX, m_Grid.dSizeY, m_dThickness);

					//////////////////////////////////////					
					//WTF(Nic): glDisable(m_eTexTarget); 
				}
			}
			else {
				// Thickness: OFF
				if(m_bUseVisibleRegions) {
					// VR: ON					
					DrawOGLVisRegions(false, true);
				}
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);	
	}

	glPopMatrix();
	//draw grid
	//DrawGrid(m_dVisualAltitude);

}
void CFloor2::InitDefault()
{
	CRenderFloor::InitDefault();
	InitDefault_();
}


BOOL CFloor2::GenerateTextureMap()
{

	CWaitCursor wait;
	//get a texture id
	//glDeleteTextures(1,m_piTexId);
	m_eTexTarget = GL_TEXTURE_2D;

	glDeleteTextures(1,m_piTexId);
	glGenTextures(1, m_piTexId);
	glEnable(m_eTexTarget);
	glBindTexture(m_eTexTarget, m_piTexId[0]);

	HGLRC hglrc = wglGetCurrentContext(); //save the current context and dc
	HDC hdc = wglGetCurrentDC();
	glTexParameteri(m_eTexTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_eTexTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	float max_anisotropy;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy );
	// TRACE("Max anisotropy = %.2f\n",max_anisotropy);
	glTexParameterf(m_eTexTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);


	int texSize;
	ARCVector2 vWorldSize((m_vMapExtentsMax - m_vMapExtentsMin)*m_CadFile.dScale);
	m_dMapAspectRatio = vWorldSize[VX]/vWorldSize[VY];
	texSize = TEX_SIZE;
	if(vWorldSize[VX] > vWorldSize[VY]) { //x>y
		m_nTextureSizeWorld = (int) ceil(vWorldSize[VX]/2); //x
	}
	else { //y>x
		m_nTextureSizeWorld = (int) ceil(vWorldSize[VY]/2); //y
	}
	//progress dialog
	CProgressBar bar( "Generating Textures...", 100, 100, TRUE );
	ASSERT(m_pView);
	PBufferPointer & pbufpt = m_pView->getFloorPBufferPt();
	if(!pbufpt){
		pbufpt=new PBuffer(texSize, texSize, 0);
		pbufpt->Initialize(hdc,true);
	}
	else if(pbufpt->width !=  texSize || pbufpt->height != texSize )
	{
		delete pbufpt;
		pbufpt = new PBuffer(texSize , texSize , 0);		
		pbufpt->Initialize(hdc, true);
	}
	/*PBuffer pbuf(texSize,texSize,0)	;
	pbuf.Initialize();*/
	pbufpt->MakeCurrent();
	GLubyte* pix = new GLubyte[4*(pbufpt->width)*(pbufpt->height)];
	bar.SetPos(10);
	//now generate 1 texture
	glBindTexture(m_eTexTarget, m_piTexId[0]);

	glTexImage2D(m_eTexTarget, 0, GL_RGBA4, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glClearColor( ARCColor3::GREY_VISIBLEREGION_FLOAT, ARCColor3::GREY_VISIBLEREGION_FLOAT, ARCColor3::GREY_VISIBLEREGION_FLOAT, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//draw the dxf layers to pbuffer
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-m_nTextureSizeWorld , m_nTextureSizeWorld, -m_nTextureSizeWorld, m_nTextureSizeWorld, -1, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0,2.0);		
	glColor4ubv(ARCColor4::VISIBLEREGION);	


	/*if(m_bUseVisibleRegions) {
	glPushMatrix();
	glTranslated(-m_vOffset[VX], -m_vOffset[VY], 0.0);
	glRotated(-m_CadFile.dRotation, 0.0, 0.0, 1.0);
	DrawOGLVisRegions(false, false ,false);
	glPopMatrix();
	}
	else {
	glBegin(GL_QUADS);
	glNormal3d(0.0,0.0,1.0);
	glVertex2d(-m_nTextureSizeWorld,-m_nTextureSizeWorld);
	glVertex2d(m_nTextureSizeWorld,-m_nTextureSizeWorld);
	glVertex2d(m_nTextureSizeWorld,m_nTextureSizeWorld);
	glVertex2d(-m_nTextureSizeWorld,m_nTextureSizeWorld);
	glEnd();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);*/
	glPushMatrix();
	//translate
	glTranslated(m_CadFile.vOffset[VX], m_CadFile.vOffset[VY], 0.0);
	//rotate
	//glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
	//scale
	glScaled(m_CadFile.dScale,m_CadFile.dScale,m_CadFile.dScale);
	//draw layers
	for(int _i=0; _i<static_cast<int>(m_vLayers.size()); _i++) {
		((CCADLayer*) m_vLayers[_i])->DrawOGL();
	}
	glPopMatrix();

	glColor3f(ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT);
	glLineWidth(5.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-m_nTextureSizeWorld,-m_nTextureSizeWorld);
	glVertex2d(m_nTextureSizeWorld,-m_nTextureSizeWorld);
	glVertex2d(m_nTextureSizeWorld,m_nTextureSizeWorld);
	glVertex2d(-m_nTextureSizeWorld,m_nTextureSizeWorld);
	glEnd();
	glLineWidth(1.0f);

	glBindTexture(m_eTexTarget, m_piTexId[0]);
	glCopyTexSubImage2D(m_eTexTarget, 0, 0, 0, 0, 0, texSize, texSize);
	glGetTexImage(m_eTexTarget, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) pix);
	gluBuild2DMipmaps(m_eTexTarget, 4, texSize, texSize, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) pix);
	// TRACE("mipmaps built... errors: %s\n", gluErrorString(glGetError()));
	bar.SetPos(95);
	delete [] pix;
	bar.SetPos(100);

	m_bIsTexValid = TRUE;
	wglMakeCurrent( hdc, hglrc );
	return TRUE;
}


void CFloor2::DrawPicture(double dAlt)
{	

	if( !m_bPicLoaded )
	{
		m_bPicLoaded = TRUE;
		GenPicTexture();
	}

	if( m_bPicTextureValid && m_picInfo.bShow  )
	{
		if(m_picInfo.refLine.getCount() < 2)
			return;	


		ARCVector2 fixv1 = ARCVector2(m_picInfo.refLine.getPoint(0));
		fixv1[VY] = -fixv1[VY];
		ARCVector2 fixv2 = ARCVector2(m_picInfo.refLine.getPoint(1));
		fixv2[VY] = -fixv2[VY];

		ARCVector2 vDir = fixv2 - fixv1;

		if(vDir.Magnitude() < ARCMath::EPSILON )
		{
			return;
		}

		GLenum texTarget = GL_TEXTURE_2D;
		glEnable(texTarget);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glColor4ubv(ARCColor4::VISIBLEREGION);
		glBindTexture(texTarget, m_iPicTexture);		
		glTexParameteri(texTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(texTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);

		ARCVector2 vNorth (0,1);
		double dAngle = vDir.GetAngleOfTwoVector(vNorth);

		double dRotate = -m_picInfo.dRotation +dAngle;
		double dScale = m_picInfo.dScale / m_picInfo.refLine.GetTotalLength();

		glPushMatrix();

		glTranslated(m_picInfo.vOffset[VX],m_picInfo.vOffset[VY],dAlt);
		glRotated(dRotate,0,0,1);
		glScaled(dScale,dScale,1);

		ARCVector2 vPicOffset = (fixv1 + fixv2) * 0.5;
		glTranslated(-vPicOffset[VX],-vPicOffset[VY],0);
		glBegin(GL_POLYGON);
		glTexCoord2d(0,1);
		glVertex2d(0,0);
		glTexCoord2d(0,0);
		glVertex2d(0,-m_picInfo.vSize[VY]);
		glTexCoord2d(1,0);
		glVertex2d(m_picInfo.vSize[VX],-m_picInfo.vSize[VY]);
		glTexCoord2d(1,1);
		glVertex2d(m_picInfo.vSize[VX],0);
		glEnd();

		glPopMatrix();
		glDisable(texTarget);
	}

}

//ILenum ilutGLFormat(ILenum Format, ILubyte Bpp)
//{
//	if (Format == IL_RGB || Format == IL_BGR) {
//		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
//			return GL_RGB8;
//		}
//	}
//	else if (Format == IL_RGBA || Format == IL_BGRA) {
//		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
//			return GL_RGBA8;
//		}
//	}
//
//	return Bpp;
//}

GLenum getFormatEx(BYTE b)
{
	if (b == 2)
	{
		return GL_RGB8;
	}
	else if (b == 4) {
		return GL_RGBA8;
	}

	return b;
}

GLenum getFormat(BYTE b)
{
	if (b == 2)
	{
		return GL_RGB;
	}
	else if (b == 4) {
		return GL_RGBA;
	}

	return b;
}

CxImage* CFloor2::LoadImage(CString sFileName)
{
	CString ext(FindExtension(sFileName));
	ext.MakeLower();
	if (ext == _T("")) return NULL;

	if(FileManager::IsFile(sFileName.GetBuffer()) == 0)
		return NULL;


	int type = CxImage::GetTypeIdFromName(ext);

	CxImage* pImg = new CxImage(sFileName, type);
	if (!pImg->IsValid()){
		CString s = pImg->GetLastError();
		AfxMessageBox(s);
		delete pImg;
		pImg = NULL;
		return pImg;
	}
	return pImg;
}

void CFloor2::GenPicTexture()
{
	m_bPicLoaded = TRUE;
	m_bPicTextureValid = FALSE;
	if (glIsTexture(m_iPicTexture))
	{
		glDeleteTextures(1, &m_iPicTexture);
	}
	CString strFile = GetPictureFileName();
	CProgressBar bar( "Load Picture...", 100, 100, TRUE );

	CxImage* pImg = LoadImage( strFile.GetBuffer() );
	bar.SetPos(50);

	if(pImg)
	{

		DWORD Width = pImg->GetWidth();
		DWORD Height = pImg->GetHeight();

		GLuint texFormat = 0;
		texFormat = GL_TEXTURE_2D;

		m_picInfo.vSize[VX] = Width;
		m_picInfo.vSize[VY] = Height;

		GLint maxTexWidth =0;
		GLint maxTexHeight =0;
		GLint picWidth = ARCMath::FloorTo2n(Width);
		GLint picHeight =  ARCMath::FloorTo2n(Height);
		while(maxTexHeight==0 && maxTexWidth==0)
		{			
			glTexImage2D(GL_PROXY_TEXTURE_2D, 0,getFormatEx(pImg->GetColorType()) , \
				picWidth,picHeight,0,getFormat(pImg->GetColorType())\
				,GL_UNSIGNED_BYTE, 0 );
			glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &maxTexWidth);
			glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &maxTexHeight);
			picHeight/=2;
			picWidth/=2;
			if(picHeight < 2 || picWidth < 2)
				break;
		}

		ILuint scaleWidth = maxTexWidth;
		ILuint scaleHeigh = maxTexHeight; 
		scaleWidth = min( 2048, scaleWidth );
		scaleHeigh = min( 2048, scaleHeigh );
		if(scaleWidth!= Width || scaleHeigh!= Height )
			pImg->Resample(scaleWidth,scaleHeigh,2);

		glGenTextures(1, &m_iPicTexture);
		glBindTexture(GL_TEXTURE_2D,m_iPicTexture);

		glTexParameteri(texFormat, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
		glTexParameteri(texFormat, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering

		glTexImage2D(texFormat, 0, 3, pImg->GetWidth(),pImg->GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, pImg->GetBits());
		m_bPicTextureValid = glIsTexture(m_iPicTexture);
		
		delete pImg;
	}
	
	bar.SetPos(100);
}

void CFloor2::InitDefault_()
{
	m_piTexId[0] = -1;
	m_nVRsurfaceDLID = m_nVRsidesDLID = m_nVRborderDLID = 0;
	m_iPicTexture = -1;
	m_nMapDLID = 0;
	m_pView=NULL;
	//m_pImage = NULL;
}