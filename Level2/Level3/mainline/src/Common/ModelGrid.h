#pragma once
#include <common/ARCVector.h>
#include <Common/ARCColor.h>

class TiXmlElement;

class CModelGrid
{
public:	//_gridstruct();
	CModelGrid();

	ARCColor3 cAxesColor;
	ARCColor3 cLinesColor;
	ARCColor3 cSubdivsColor;
	double dSizeX;
	double dSizeY;
	double dSizeZ;
	double dLinesEvery;
	int nSubdivs;
	BOOL bVisibility;

public: //save/load to/from xml file
	virtual TiXmlElement * serializeTo(TiXmlElement *, int nVersion)const;
	virtual void serializeFrom(const TiXmlElement* pElm,int nVersion);

};
