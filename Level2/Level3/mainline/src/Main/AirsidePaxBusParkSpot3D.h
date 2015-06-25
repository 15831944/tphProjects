#pragma once
#include "ALTObject3D.h"
class C3DView;

class AirsideParkSpot;
class CAirsideParkingPos3D  : public ALTObject3D
{
public:
	CAirsideParkingPos3D(int id):ALTObject3D(id){}
	ARCPoint3 GetLocation(void) const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx);


	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);	
protected:
	AirsideParkSpot* getSpot(){ return (AirsideParkSpot*)GetObject(); }
	const AirsideParkSpot* getSpot()const{ return  (const AirsideParkSpot*)GetObject(); }
};


class AirsidePaxBusParkSpot;
class CAirsidePaxBusParkSpot3D : public CAirsideParkingPos3D
{
public:
	CAirsidePaxBusParkSpot3D(int id);	
protected:
	
};


class CAirsideBagCartParkPos3D : public CAirsideParkingPos3D
{
public:
	CAirsideBagCartParkPos3D(int id);
};