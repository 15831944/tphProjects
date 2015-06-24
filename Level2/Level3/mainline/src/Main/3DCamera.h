// 3DCamera.h: interface for the C3DCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DCAMERA_H__4E41AC30_7502_11D3_9152_0080C8F982B1__INCLUDED_)
#define AFX_3DCAMERA_H__4E41AC30_7502_11D3_9152_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "common\ARCVector.h"
#include <common\Camera.h>

#define NORMAL_ZOOM_FOV		30.0

#define CAM_MAX_ZOOM		10000
#define CAM_MIN_ZOOM		10

class ArctermFile;
class ARCVector2;

class C3DCamera : public CCameraData
{
public:

	static void Interpolate(const C3DCamera& c1, const C3DCamera& c2, double t, C3DCamera& cres);

	C3DCamera();
	C3DCamera(CCameraData::PROJECTIONTYPE eType);
	C3DCamera(LPCTSTR lpszName, CCameraData::PROJECTIONTYPE eType);
	C3DCamera(const C3DCamera& cam); // copy constructor
	C3DCamera(const CCameraData& camd);
	virtual ~C3DCamera();

	C3DCamera& operator	=(const C3DCamera& _rhs);

	//operations
	//tumble (3d), rotate (2d)
	void Tumble(const ARCVector2& _mm);
	//pan (2d and 3d)
	void Pan(const ARCVector2& _mm);
	//zoom (dolly) (2d and 3d)
	void Zoom(DistanceUnit _mm);

	//gl stuff
	void ApplyGLView() const;
	void ApplyCompassGLView() const;	// applies the modelview transform, ignoring the camera position,
								// this is used to render axes showing the user where she is.
	void ApplyGLProjection(double dAspectRatio) const;
	void ApplyCompassGLProjection() const;

	//data access
	
	double GetFocusDistance() const { return m_dFocusDistance; }
	//data modifiers
	void SetFocusDistance(double dFocusDist) { m_dFocusDistance = dFocusDist; }
	//needs a current opengl rendering context
	void AutoAdjustCameraFocusDistance(double dFocusPlaneAlt);


	BOOL WriteCamera(ArctermFile* pFile) const;
	BOOL ReadCamera(ArctermFile* pFile);

	void SetCamera(const ARCVector3& _eye, const ARCVector3& _look, const ARCVector3& _up);

	void SetWBSData(const ARCVector3& _c, double _r) { m_vWorldC = _c; m_dWorldR = _r; }

	void Make2D();	//changes type of camera to 2D, adjusting parameters as necessary
	void Make3D();	//changes type of camera to 3D, adjusting parameters as necessary
	
	bool PointInCamera(DistanceUnit x,DistanceUnit y,DistanceUnit z );
	bool SphereInCamera(const Point & centerpt, double radius);
	bool CubeInCamera(const Point & p1,const Point & p2);
	void ExtractFrustum(void) ;

	void Update();
protected:
	void InitDefault(CCameraData::PROJECTIONTYPE eType);
	
	//clip the scene for speed 
	
	double frustum[6][4];
	bool m_bdirty;
	

protected:
	//focus distance
	double m_dFocusDistance;
	//camera zoom
	int m_iZoom;				// zoom (percentage 10% -> 1000%)
	// clipping planes
	mutable double m_dFarPlane;	
	mutable double m_dNearPlane;
	//world bounding sphere data
	ARCVector3 m_vWorldC;	//the center of the "world bounding sphere"
	double m_dWorldR;				//the radius of the "world bounding sphere"
};

#endif // !defined(AFX_3DCAMERA_H__4E41AC30_7502_11D3_9152_0080C8F982B1__INCLUDED_)
