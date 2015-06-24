#pragma once
#include "..\InputAirside\ALTAirport.h"
#include "..\Common\Referenced.h"
#include "..\Common\ref_ptr.h"
#include ".\ALTObject3D.h"


class C3DView;
class CAirside3D;
class SelectableSettings;

class CAirspace3D 
{
public:
	CAirspace3D(int id, CAirside3D * pAirside);
	virtual ~CAirspace3D(void);

	void Init();

	ALTObject3D * GetObject3D(int id);
	bool RemoveObject(int id);
	ALTObject3D* AddObject(ALTObject* pObj);

	void UpdateAddorRemoveObjects();

	void GetObject3DList(ALTObject3DList& objList)const;

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelectOGL(C3DView  *pView, SelectableSettings& selsetting);

	void UpdateAirRoute(int nAirRouteID);
	
private :
	int m_nID;

	ALTObject3DList m_vWayPoint;
	ALTObject3DList m_vHold;
	ALTObject3DList m_vSector;
	ALTObject3DList m_vAirRoute;

	CAirside3D * m_pAirside;
};
