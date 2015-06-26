#pragma once
#include "3dnodeobject.h"
class InputOnboard;
class AircraftAliasManager;

class RENDERENGINE_API CAircraftModel3DNode :
	public C3DNodeObject
{
public:
	CAircraftModel3DNode(void);
	CAircraftModel3DNode(Ogre::SceneNode* pNode);

	Ogre::Entity* GetFlightEnt(){ return m_pFlightEnt; }
	void Load( CString actype ); //load default models, actype include the aliase
	void Load( CString actype, InputOnboard* pOnboard); //load onboard user define models, will 

	void SetFlightSize(double dwingspan, double dlength);
	void SetFadeMaterial( CString strAirline, COLORREF color);
protected:
	Ogre::Entity* m_pFlightEnt;
};
