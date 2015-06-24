#pragma once
#include "selectionmap.h"



class Path;

class PathControlPoint :
	public Selectable
{
public:

	PathControlPoint(Path& path,int ptID);
	virtual ~PathControlPoint(void);

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);
	virtual void DeleteEditPoint();
	virtual Path& GetPath(){return m_path;}

	Selectable::SelectType GetSelectType()const { return ALT_PATH_CONTROLPOINT; };

	ARCPoint3 GetLocation()const;

	virtual void DrawSelect(C3DView * pView);

	
protected:
	Path & m_path;
	int m_id;
};

typedef std::vector< ref_ptr<PathControlPoint> > PathControlPointList;


class ALTObject3D;
class ALTObjectPathControlPoint : public PathControlPoint{

public:
	ALTObjectPathControlPoint(ALTObject3D* pObject, Path& path, int ptID);
	virtual	~ALTObjectPathControlPoint();


	Selectable::SelectType GetSelectType()const { return Selectable::ALT_OBJECT_PATH_CONTROLPOINT; };

	virtual void FlushChange();

	ALTObject3D * GetObject()const{return m_pObject; }

	virtual void DrawSelect(C3DView * pView);
protected:
	ALTObject3D * m_pObject;
};
typedef std::vector< ref_ptr<ALTObjectPathControlPoint> > ALTObjectControlPointList;


class ALTObjectPointControlPoint
	:public Selectable {
public:
	ALTObjectPointControlPoint(ALTObject3D * pObject, Point& pt);
	
	Selectable::SelectType GetSelectType()const { return Selectable::ALT_OBJECT_POINT_CONTROLPOINT; }

	virtual void FlushChange();
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);


	ALTObject3D * GetObject()const{ return m_pObject; }

	virtual void DrawSelect(C3DView * pView);
	ARCPoint3 GetLocation()const;

protected:
	ALTObject3D * m_pObject;
	Point& m_pt;

};