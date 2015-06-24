#pragma once
#include "altobject3d.h"


//class CAirspace3D;
class CAirRoute;
class AirRouteDisplayProp;
class CAirside3D;
#include "../InputAirside/ALTObject.h"
#include "../Common/ARCPath.h"

class CAirRoute3D :
	public ALTObject3D
{
public:
	CAirRoute3D(int id);
	virtual ~CAirRoute3D(void);
	

	virtual ARCPoint3 GetLocation(void) const;
	virtual CString GetDisplayName()const;

	//void SetAirspace(CAirspace3D * pAirspace){ m_pAirspace  = pAirspace; }
	//CAirspace3D * GetAirspace()const{ return m_pAirspace; }

	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);
	
	CAirRoute * GetAirRoute()const;
	//AirRouteDisplayProp  *GetDisplayProp()const;

	void RenderShape(C3DView * pview);
	void Update(CAirside3D* pAirside);
	virtual void DoSync();


protected:

	ALTObject3DList m_vWayPointList;
	ref_ptr<ALTObject3D> m_Runway;
	int m_iRunwayside;


	CAirside3D * m_pAirside;

};
