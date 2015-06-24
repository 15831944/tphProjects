#pragma once
#include "selectionmap.h"



class CPath2008;
class ALTObject3D; 

class CPathControlPoint2008 :
	public Selectable
{
public:

	CPathControlPoint2008(CPath2008& path,int ptID);
	virtual ~CPathControlPoint2008(void);

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);
	virtual void DeleteEditPoint(ALTObject3D* pObj3D);
	virtual CPath2008& GetPath(){return m_path;}

	Selectable::SelectType GetSelectType()const { return ALT_PATH_CONTROLPOINT; };

	ARCPoint3 GetLocation()const;

	virtual void DrawSelect(C3DView * pView);


protected:
	CPath2008 & m_path;
	int m_id;
};

typedef std::vector< ref_ptr<CPathControlPoint2008> > PathControlPoint2008List;


class ALTObject3D;
class CALTObjectPathControlPoint2008 : public CPathControlPoint2008{

public:
	CALTObjectPathControlPoint2008(ALTObject3D* pObject, CPath2008& path, int ptID);
	virtual	~CALTObjectPathControlPoint2008();

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);

	Selectable::SelectType GetSelectType()const { return Selectable::ALT_OBJECT_PATH_CONTROLPOINT; };

	virtual void FlushChange();

	ALTObject3D * GetObject()const{return m_pObject; }

	virtual void DrawSelect(C3DView * pView);
protected:
	ALTObject3D * m_pObject;
};
typedef std::vector< ref_ptr<CALTObjectPathControlPoint2008> > ALTObjectControlPoint2008List;


class CALTObjectPointControlPoint2008
	:public Selectable {
public:
	CALTObjectPointControlPoint2008(ALTObject3D * pObject, CPoint2008& pt);

	Selectable::SelectType GetSelectType()const { return Selectable::ALT_OBJECT_POINT_CONTROLPOINT; }

	virtual void FlushChange();
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);


	ALTObject3D * GetObject()const{ return m_pObject; }

	virtual void DrawSelect(C3DView * pView);
	ARCPoint3 GetLocation()const;

protected:
	ALTObject3D * m_pObject;
	CPoint2008& m_pt;

	};