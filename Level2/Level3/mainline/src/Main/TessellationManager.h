// TessellationManager.h: interface for the CTessellationManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__TESSELLATIONMANAGER_H___INCLUDED_)
#define __TESSELLATIONMANAGER_H___INCLUDED_

#pragma once

#include "GL\gl.h"
#include "GL\glu.h"

#include <vector>

#include "common\ARCVector.h"

#define TESSALLATIONMGR			CTessellationManager::GetInstance()
#define DESTROY_TESSALLATIONMGR	CTessellationManager::DeleteInstance();
void __stdcall combineCallback( GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut ) ;
void __stdcall errorCallback(GLenum errorCode);
class CTessellationManager  
{
protected:
	CTessellationManager();
	virtual ~CTessellationManager();

public:
	static CTessellationManager* GetInstance();
	static void DeleteInstance();

	enum Mode {
		Render,
		GenBoundary,
	};

	void Init(Mode m);
	void RenderContour(GLdouble objdata[][3], int nVertexCount);
	void ContourVertex(GLdouble* objdata);
	void BeginPolygon(GLvoid);
	void EndPolygon(GLvoid);
	void BeginContour(GLvoid);
	void EndContour(GLvoid);
	void End();
	void GetBoundary(std::vector< ARCVector3 >& v); //returns a list of point pairs for the boundary.


private:
	static CTessellationManager* m_pInstance;
	GLUtesselator* m_pTObj; // the tessellation object
	bool m_bIsInit;

};

#endif // !defined(__TESSELLATIONMANAGER_H___INCLUDED_)
