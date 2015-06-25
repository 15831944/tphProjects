#pragma once
#include <common/ARCVector.h>

class TiXmlElement;


class CCameraData
{
public:
	enum PROJECTIONTYPE {
		parallel=0,
		perspective=1
	};

	virtual void InitDefault(PROJECTIONTYPE projType);
	bool operator == (const CCameraData& cam)const;
public:
	PROJECTIONTYPE GetProjectionType() const { return m_eProjType; }
	void GetLocation(ARCVector3& v) const { v = m_vLocation; }
	void GetUpVector(ARCVector3& v) const { v = m_vUp; }
	void GetLookAtVector(ARCVector3& v) const { v = m_vLook; }
	const CString& GetName() const { return m_sName; }
	void SetName(LPCTSTR lpszName) { m_sName = lpszName; }

	//camera position
	ARCVector3 m_vLocation;		// x, y, z location
	//camera direction
	ARCVector3 m_vLook;			// viewing direction vector
	//camera up
	ARCVector3 m_vUp;			// up vector (local z)
	//camera type (2D or 3D)
	PROJECTIONTYPE m_eProjType;
	//camera name
	CString m_sName;

	double dWorldWidth; //for 2d
	double dFovy; //for 3d;

	CCameraData();

public: //save/load to/from xml file
	virtual TiXmlElement * serializeTo(TiXmlElement *, int nVersion)const;
	virtual void serializeFrom(const TiXmlElement* pElm, int nVersion);

};
