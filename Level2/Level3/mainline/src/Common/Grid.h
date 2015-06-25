#pragma once
#include <common/ARCVector.h>
#include <Common/ARCColor.h>

class TiXmlElement;

class CGrid
{
public:	//_gridstruct();
	CGrid();

	ARCColor3 cAxesColor;
	ARCColor3 cLinesColor;
	ARCColor3 cSubdivsColor;
	double dSizeX;
	double dSizeY;
	double dLinesEvery;
	int nSubdivs;
	double dSizeZ;
	BOOL bVisibility;

	void InitDefault(double sizex = 1000,double sizey=1000,double lineevery=500.0);

public: //save/load to/from xml file
	virtual TiXmlElement * serializeTo(TiXmlElement *, int nVersion)const;
	virtual void serializeFrom(const TiXmlElement* pElm,int nVersion);

};
