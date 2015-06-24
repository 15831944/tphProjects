// TessellationManager.cpp: implementation of the CTessellationManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TessellationManager.h"

static std::vector< ARCVector3 > g_vPoints;
static ARCVector3 g_pTempPoints[3];
static bool g_pTempEdges[3];
static bool g_bEdgeFlag;
static int nPointIdx;

void __stdcall beginCallback(GLenum which)
{
	//since we have edges on, we know this will be tris
	ASSERT(GL_TRIANGLES == which);
	nPointIdx = 0;
}

void __stdcall endCallback()
{
}

void __stdcall vertexCallback(GLvoid* pData)
{
	const double* data = static_cast<const double*>(pData);
	g_pTempPoints[nPointIdx][VX] = data[0];
	g_pTempPoints[nPointIdx][VY] = data[1];
	g_pTempPoints[nPointIdx][VZ] = data[2];
	g_pTempEdges[nPointIdx] = g_bEdgeFlag;
	nPointIdx++;

	if(3 == nPointIdx) {
		if(g_pTempEdges[0]) {
			g_vPoints.push_back(g_pTempPoints[0]);
			g_vPoints.push_back(g_pTempPoints[1]);
		}
		if(g_pTempEdges[1]) {
			g_vPoints.push_back(g_pTempPoints[1]);
			g_vPoints.push_back(g_pTempPoints[2]);
		}
		if(g_pTempEdges[2]) {
			g_vPoints.push_back(g_pTempPoints[2]);
			g_vPoints.push_back(g_pTempPoints[0]);
		}
		nPointIdx = 0;
	}
}

void __stdcall edgeflagCallback(GLboolean flag)
{
	g_bEdgeFlag = flag ? true : false;
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


void __stdcall errorCallback(GLenum errorCode)
{
	const GLubyte* estring;
	estring = gluErrorString(errorCode);
	ASSERT(FALSE);
}



CTessellationManager* CTessellationManager::m_pInstance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTessellationManager::CTessellationManager()
{
	m_bIsInit = false;
	m_pTObj = NULL;
}

CTessellationManager::~CTessellationManager()
{
	m_bIsInit = false;
	gluDeleteTess(m_pTObj);
	m_pTObj = NULL;
}

CTessellationManager* CTessellationManager::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CTessellationManager();
	return m_pInstance;
}

void CTessellationManager::DeleteInstance()
{
	if(m_pInstance != NULL) {
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CTessellationManager::Init(Mode m)
{
	// Create a new tessellation object 
	if(!m_pTObj)
		m_pTObj = gluNewTess(); 

	if(GenBoundary == m) {
		// Set callback functions
		gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &beginCallback);    
		gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &endCallback);
		gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) &vertexCallback);
		gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &edgeflagCallback);
		gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
		gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
		gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);

		g_vPoints.clear();
	}
	else if(Render == m) {
		// Set callback functions
		gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
		gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &glEnd);
		gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) &glVertex3dv);
		gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
		gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
		gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
		gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	
	}
	
	m_bIsInit = true;
}

void CTessellationManager::End()
{
	m_bIsInit = false;
}

void CTessellationManager::BeginPolygon(GLvoid)
{
	gluTessBeginPolygon(m_pTObj, NULL);
}

void CTessellationManager::EndPolygon(GLvoid)
{
	gluTessEndPolygon(m_pTObj);
}

void CTessellationManager::BeginContour(GLvoid)
{
	gluTessBeginContour(m_pTObj);
}

void CTessellationManager::EndContour(GLvoid)
{
	gluTessEndContour(m_pTObj);
}

void CTessellationManager::RenderContour(GLdouble objdata[][3], int nVertexCount)
{
	for (int i = 0; i < nVertexCount; i++) { //loop through the vertices
		gluTessVertex(m_pTObj, objdata[i], objdata[i]); //store the vertex
	}

}

void CTessellationManager::ContourVertex(GLdouble* objdata)
{
	gluTessVertex(m_pTObj, objdata, objdata); //store the vertex
}

void CTessellationManager::GetBoundary(std::vector< ARCVector3 >& v)
{
	v = g_vPoints;
}