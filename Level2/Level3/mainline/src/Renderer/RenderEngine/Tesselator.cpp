#include "StdAfx.h"
#include ".\tesselator.h"

#include <common/pollygon.h>
#include <common/Pollygon2008.h>
#include "../include/GL/glu.h"
using namespace Ogre;


//typedef GLvoid (*GLUTesselatorFunction)(GLvoid);
typedef void (CALLBACK * GLUTesselatorFunction)(void);

void CALLBACK ftglError( GLenum errCode, CTessMesh* mesh)
{
	//mesh->Error( errCode);
	const GLubyte* estring;
	estring = gluErrorString(errCode);
}


void CALLBACK ftglVertex( void* data, CTessMesh* mesh)
{
	double* vertex = static_cast<double*>(data);
	mesh->AddPoint( vertex[0], vertex[1], vertex[2]);
}


void CALLBACK ftglCombine( double coords[3], void* vertex_data[4], GLfloat weight[4], void** outData, CTessMesh* mesh)
{
	mesh->Combine( coords[0], coords[1], coords[2]);
	GLdouble *vertex = new GLdouble[3];
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	*outData = vertex; 	
}


void CALLBACK ftglBegin( GLenum type, CTessMesh* mesh)
{
	mesh->Begin(type);
}


void CALLBACK ftglEnd( CTessMesh* mesh)
{
	mesh->End();
}

bool CTesselator::MakeOutLine( const ARCVectorListList& contourList,const Vector3& normal )
{
	return MakeMesh(contourList,normal,GLU_TESS_WINDING_POSITIVE,GL_TRUE);	
}

bool CTesselator::MakeMeshPositive( const ARCVectorListList& contourList, const Vector3& normal )
{
	return MakeMesh(contourList,normal,GLU_TESS_WINDING_POSITIVE,GL_FALSE);		
}

bool CTesselator::MakeMeshOdd( const ARCVectorListList& contourList, const Vector3& normal )
{	
	return MakeMesh(contourList,normal,GLU_TESS_WINDING_ODD,GL_FALSE);		
}




void CTessMesh::Clear()
{
	for(int i=0;i<getCount();i++)
		delete getSection(i);
	m_vSections.clear();
}

double * CTessMesh::Combine( double x, const double y, const double z )
{
	tempPointList.push_back( ARCVector3( x, y,z) );
	return (double*)tempPointList.back().n;
}

void CTessMesh::Begin( GLenum tessType )
{
	m_pCurSection = new CTessSection(tessType);
}

void CTessMesh::End()
{
	if(m_pCurSection && m_pCurSection->getCount())
	{		
		m_vSections.push_back(m_pCurSection);
	}
}

void CTessMesh::AddPoint( double x, const double y, const double z )
{
	if(m_pCurSection)
		m_pCurSection->AddPoint(x,y,z);
}

void Ogre::CTessSection::AddPoint( double x, double y, double z )
{
	m_vPoints.push_back(ARCVector3(x,y,z));
}

bool Ogre::CTesselator::BuildMeshToObject( ManualObject* pobj, const String& matName, const Vector3& normal,const ColourValue& color, double dZoffset,const Matrix4& texCoordTrans/*=Matrix4::IDENTITY*/ )
{
	for(int i=0;i<mCurMesh.getCount();i++)
	{
		CTessSection* pSec = mCurMesh.getSection(i);		

		RenderOperation::OperationType rop;
		switch(pSec->glOperation)
		{
		case GL_LINE_LOOP:
		case GL_LINE_STRIP:
			rop = RenderOperation::OT_LINE_STRIP;
			break;
		case GL_TRIANGLE_FAN:
			rop = RenderOperation::OT_TRIANGLE_FAN;
			break;
		case GL_TRIANGLE_STRIP:
			rop = RenderOperation::OT_TRIANGLE_STRIP;
			break;
		case GL_TRIANGLES:
			rop = RenderOperation::OT_TRIANGLE_LIST;
			break;
		default:
			continue;
		}
		{
			pobj->begin(matName,rop);
			for(int j=0;j<pSec->getCount();j++)
			{
				ARCVector3& v = pSec->m_vPoints[j];
				Vector3 vpos(v[VX],v[VY],v[VZ]+dZoffset);
				pobj->position(vpos);	
				pobj->colour(color);
				pobj->normal(normal);
				Vector3 texcod =  texCoordTrans.transformAffine(vpos);
				pobj->textureCoord(texcod.x,texcod.y);

				if(j==pSec->getCount()-1 && pSec->glOperation == GL_LINE_LOOP )
				{	
					ARCVector3& v = pSec->m_vPoints[0];
					pobj->position(v[VX],v[VY],v[VZ]+dZoffset);					
				}
			}
			pobj->end();
		}

	}
	return true;
}

bool Ogre::CTesselator::BuildSideToObject( ManualObject* pobj, const String& matName,const ColourValue& color,double dThick )
{
	for(int i=0;i<mCurMesh.getCount();i++)
	{
		CTessSection* pSec = mCurMesh.getSection(i);	
		
		if(pSec->glOperation == GL_LINE_LOOP)
		{
			pobj->begin(matName,RenderOperation::OT_TRIANGLE_STRIP);
			for(int j=0;j<pSec->getCount();j++)
			{
				ARCVector3& v1 = pSec->m_vPoints[j];
				ARCVector3& v2 = pSec->m_vPoints[(j+1)%pSec->getCount()];
				Vector3 vnormal = OGREVECTOR(v2-v1).crossProduct(Vector3::UNIT_Z);
				vnormal.normalise();

				pobj->position( v1[VX],v1[VY],v1[VZ] );	
				pobj->colour(color);
				pobj->normal(vnormal);	
				pobj->position( v1[VX],v1[VY],v1[VZ]+dThick );
				pobj->position( v2[VX],v2[VY],v2[VZ] );
				pobj->position( v2[VX],v2[VY],v2[VZ]+dThick );
				
			}
			pobj->end();
		}		

	}
	return true;
}

bool Ogre::CTesselator::BuildOutlineToObject( ManualObject* pobj,const String& matName, const ColourValue& color,double dThick )
{
	for(int i=0;i<mCurMesh.getCount();i++)
	{
		CTessSection* pSec = mCurMesh.getSection(i);	

		if(pSec->glOperation == GL_LINE_LOOP)
		{
			int nCount = pSec->getCount();
			pobj->begin(matName,RenderOperation::OT_LINE_STRIP);			
			for(int j=0;j<nCount;j++)
			{
				const ARCVector3& v1 = pSec->m_vPoints[j];						
				pobj->position( v1[VX],v1[VY],v1[VZ] );	
				pobj->colour(color);
				if(j==pSec->getCount()-1){
					const ARCVector3& v = pSec->m_vPoints[0];
					pobj->position( v[VX],v[VY],v[VZ] );	
				}
			}
			pobj->end();

			if( abs(dThick) >0) //thick enabled
			{
				pobj->begin(matName,RenderOperation::OT_LINE_STRIP);			
				for(int j=0;j<nCount;j++)
				{
					const ARCVector3& v1 = pSec->m_vPoints[j];						
					pobj->position( v1[VX],v1[VY],v1[VZ] +dThick);	
					pobj->colour(color);
					if(j==pSec->getCount()-1){
						const ARCVector3& v = pSec->m_vPoints[0];
						pobj->position( v[VX],v[VY],v[VZ]+dThick );	
					}
				}
				pobj->end();

				pobj->begin(matName,RenderOperation::OT_LINE_LIST);			
				for(int j=0;j<nCount;j++)
				{
					const ARCVector3& v1 = pSec->m_vPoints[j];						
					pobj->position( v1[VX],v1[VY],v1[VZ] );	pobj->colour(color);			
					pobj->position( v1[VX],v1[VY],v1[VZ]+dThick );		
				}
				pobj->end();
			}

			
		}		

	}
	return true;
}

bool Ogre::CTesselator::MakeMesh( const ARCVectorListList& contourList, const Vector3& normal,GLenum windrule, GLboolean outline )
{
	mCurMesh.Clear();

	GLUtesselator* tobj = gluNewTess();
	gluTessCallback( tobj, GLU_TESS_BEGIN_DATA,     (_GLUfuncptr)ftglBegin);
	gluTessCallback( tobj, GLU_TESS_VERTEX_DATA,    (_GLUfuncptr)ftglVertex);
	gluTessCallback( tobj, GLU_TESS_COMBINE_DATA,   (_GLUfuncptr)ftglCombine);
	gluTessCallback( tobj, GLU_TESS_END_DATA,       (_GLUfuncptr)ftglEnd);
	gluTessCallback( tobj, GLU_TESS_ERROR_DATA,     (_GLUfuncptr)ftglError);
	gluTessCallback( tobj, GLU_TESS_EDGE_FLAG,      (_GLUfuncptr)glEdgeFlag);

	gluTessProperty(tobj,GLU_TESS_BOUNDARY_ONLY,outline);
	gluTessProperty( tobj, GLU_TESS_WINDING_RULE, windrule);

	gluTessProperty( tobj, GLU_TESS_TOLERANCE, 0);

	gluTessNormal( tobj, normal.x, normal.y, normal.z);
	gluTessBeginPolygon( tobj, &mCurMesh );

	for( size_t c = 0; c < contourList.size(); ++c)
	{
		const ARCVectorList& contour = contourList[c];

		gluTessBeginContour( tobj);

		for( int p = 0; p < (int)contour.size(); ++p)
		{
			double* d = (double*)(contour[p].n);
			gluTessVertex( tobj, d, d);
		}
		gluTessEndContour( tobj);
	}

	gluTessEndPolygon( tobj);

	gluDeleteTess( tobj);

	return true;
}

bool Ogre::CTesselator::MakeMeshNegative( const ARCVectorListList& contourList,const Vector3& normal )
{
	return MakeMesh(contourList,normal,GLU_TESS_WINDING_NEGATIVE,GL_FALSE);
}

ARCVectorList Ogre::CTesselator::GetAntiClockVectorList( const Pollygon& poly )
{
	ARCVectorList v;v.reserve(poly.getCount());

	for(int i=0;i<poly.getCount();i++){
		v.push_back( ARCVector3(poly[i].getX(),poly[i].getY(),poly[i].getZ()) );
	}
	if(!poly.IsCountClock())
	{
		std::reverse(v.begin(),v.end());
	}
	return v;
}

ARCVectorList Ogre::CTesselator::GetAntiClockVectorList(  const CPollygon2008& poly )
{
	ARCVectorList v;v.reserve(poly.getCount());

	for(int i=0;i<poly.getCount();i++){
		v.push_back( ARCVector3(poly[i].getX(),poly[i].getY(),poly[i].getZ()) );
	}
	if(!poly.IsCountClock())
	{
		std::reverse(v.begin(),v.end());
	}
	return v;
}

ARCVectorList Ogre::CTesselator::GetAntiClockVectorList( const Path& path )
{
	Pollygon p;
	p.init(path.getCount(),path.getPointList());
	return GetAntiClockVectorList(p);
}

bool Ogre::CTesselator::MakeMeshPositive( const Path* path )
{
	if(!path)
		return false;
	ARCVectorListList vlistlist;
	vlistlist.push_back(GetAntiClockVectorList(*path));

	return MakeMeshPositive( vlistlist,Vector3::UNIT_Z );
}
