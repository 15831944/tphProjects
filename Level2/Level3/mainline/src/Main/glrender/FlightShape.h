#pragma once
#include "glshape.h"

class FlightShape :
	public CglShape
{
public:
	FlightShape(void);
	~FlightShape(void);

	ref_ptr<CTexture> m_airlineTexture;

	void setAirlineTexture(CTexture * ptextures);

	//virtual void display();
	virtual void displayMesh(Lib3dsMesh *mesh);
	//virtual void displayNode(Lib3dsNode *node);


};
