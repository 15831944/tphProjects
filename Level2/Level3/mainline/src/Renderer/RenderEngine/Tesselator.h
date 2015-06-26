#pragma once
#include <gl/gl.h>
#include <common/ARCVector.h>

//////////////////////////////////////////////////////////////////////////
typedef std::vector<ARCVector3> ARCVectorList;
typedef std::vector<std::vector<ARCVector3> > ARCVectorListList;
class Pollygon;
class Path;
class CPollygon2008;

namespace Ogre
{
	typedef std::vector<Vector3> VectorList;
	typedef std::vector<std::vector<Vector3> > VectorListList;

	class CTessSection
	{
	public:
		CTessSection(GLenum tessType){ glOperation = tessType ; }

		GLenum glOperation;

		void AddPoint(double x, double y, double z);
		void AddPoint(const ARCVector3& v){ m_vPoints.push_back(v); }
		int  getCount()const{ return (int) m_vPoints.size(); }
	public:
		ARCVectorList m_vPoints;
	};

	class CTessMesh
	{

	public:
		~CTessMesh(){ Clear(); }

		void Clear();

		CTessSection* m_pCurSection;
		void Begin(GLenum tessType);
		void End();
		void AddPoint(double x, const double y, const double z);
		double * Combine( double x, const double y, const double z);
		int getCount()const{ return (int)m_vSections.size(); }
		CTessSection* getSection(int idx){ return m_vSections[idx]; }
	protected:	
		std::vector<CTessSection*> m_vSections;
		ARCVectorList tempPointList;
	};


	class CTesselator
	{
	public:
		bool MakeMeshPositive(const ARCVectorListList& contourList,const Vector3& normal);
		bool MakeMeshNegative(const ARCVectorListList& contourList,const Vector3& normal);
		bool MakeMeshOdd(const ARCVectorListList& contourList,const Vector3& normal);
		bool MakeOutLine(const ARCVectorListList& contourList,const Vector3& normal);
		bool MakeMeshPositive(const Path* path); //utility  draw path as a area

		//make current mesh to manual object in ogre
		bool BuildMeshToObject(ManualObject* pobj, const String& matName, const Vector3& normal,const ColourValue& color,double dZoffset=0, const Matrix4& texCoordTrans=Matrix4::IDENTITY); //top face
		bool BuildSideToObject(ManualObject* pobj, const String& matName,const ColourValue& color,double dThick ); //side face
		bool BuildOutlineToObject(ManualObject* pobj,const String& matName, const ColourValue& color,double dThick ); //outline

		CTessMesh mCurMesh;

		static ARCVectorList GetAntiClockVectorList( const Path& path );
		static ARCVectorList GetAntiClockVectorList( const Pollygon& poly);
		static ARCVectorList GetAntiClockVectorList( const CPollygon2008& poly);

	protected:
		bool MakeMesh(const ARCVectorListList& contourList, const Vector3& normal,GLenum windrule, GLboolean outline);
	};


}

