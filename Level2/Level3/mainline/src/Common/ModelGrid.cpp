#include "StdAfx.h"
#include ".\modelgrid.h"
#include <common/tinyxml/tinyxml.h>
#include <common/ARCStringConvert.h>


CModelGrid::CModelGrid()
{
	bVisibility = TRUE;
	cAxesColor = RGB(0,0,0);
	cLinesColor = RGB(88,88,88);
	cSubdivsColor = RGB(140,140,140);
	dSizeX = 10000.0;
	dSizeY = 10000.0;
	dSizeZ = 10000.0;
	dLinesEvery = 500.0;
	nSubdivs = 0;
}

//serialize to xml
//#define STR_XMAX _T("maxX")
//#define STR_XMIN _T("minX")
//#define STR_YMIN _T("minY")
//#define STR_YMAX _T("maxY")
//#define STR_ZMIN _T("minZ")
//#define STR_ZMAX _T("maxZ")
#define STR_SIZEX _T("sizeX")
#define STR_SIZEY _T("sizeY")
#define STR_SIZEZ _T("sizeZ")

#define STR_LINEEVERY _T("LinesEvery")
#define STR_SUBDIV _T("SubDivide")
#define STR_VISIBLE _T("Visible")
#define STR_AXSCOLOR _T("AxisColor")
#define STR_LINESCOLOR _T("LinesColor")
#define STR_SUBDIVCOLOR _T("SubDivColor")
#define STR_UNIT _T("Unit")

TiXmlElement * CModelGrid::serializeTo( TiXmlElement * tiElm, int nVersion) const
{
	//tiElm->SetDoubleAttribute(STR_XMIN,dMinX);
	//tiElm->SetDoubleAttribute(STR_XMAX, dMaxX);
	//tiElm->SetDoubleAttribute(STR_YMIN,dMinY);
	//tiElm->SetDoubleAttribute(STR_YMAX, dMaxY);
	//tiElm->SetDoubleAttribute(STR_ZMIN,dMinZ);
	//tiElm->SetDoubleAttribute(STR_ZMAX, dMaxZ);


	tiElm->SetDoubleAttribute(STR_SIZEX, dSizeX);
	tiElm->SetDoubleAttribute(STR_SIZEY, dSizeY);
	tiElm->SetDoubleAttribute(STR_SIZEZ, dSizeZ);

	tiElm->SetDoubleAttribute(STR_LINEEVERY,dLinesEvery);
	tiElm->SetAttribute(STR_SUBDIV,nSubdivs);
	tiElm->SetAttribute(STR_VISIBLE, bVisibility);
	tiElm->SetAttribute(STR_AXSCOLOR,(COLORREF)cAxesColor);
	tiElm->SetAttribute(STR_LINESCOLOR,(COLORREF)cLinesColor);
	tiElm->SetAttribute(STR_SUBDIVCOLOR,(COLORREF)cSubdivsColor);
	//tiElm->SetAttribute(STR_UNIT,mUnit);
	return tiElm;
}

void CModelGrid::serializeFrom( const TiXmlElement* pElm , int nVersion)
{
	int tmpInt;
	//double tmpFloat;

	//pElm->Attribute(STR_XMIN,&dMinX);
	//pElm->Attribute(STR_XMAX,&dMaxX);
	//pElm->Attribute(STR_YMIN,&dMinY);
	//pElm->Attribute(STR_YMAX,&dMaxY);
	//pElm->Attribute(STR_ZMIN,&dMinZ);
	//pElm->Attribute(STR_ZMAX,&dMaxZ);
	pElm->Attribute(STR_SIZEX,&dSizeX);
	pElm->Attribute(STR_SIZEY,&dSizeY);
	pElm->Attribute(STR_SIZEZ,&dSizeZ);
	pElm->Attribute(STR_LINEEVERY,&dLinesEvery);
	pElm->Attribute(STR_SUBDIV,&nSubdivs);
	pElm->Attribute(STR_VISIBLE,&bVisibility);
	pElm->Attribute(STR_AXSCOLOR,&tmpInt); cAxesColor = tmpInt;
	pElm->Attribute(STR_LINESCOLOR,&tmpInt);cLinesColor= tmpInt;
	pElm->Attribute(STR_SUBDIVCOLOR,&tmpInt);cSubdivsColor= tmpInt;
	//pElm->Attribute(STR_UNIT,&tmpInt); mUnit = (ARCUnit::LengthUnit)tmpInt;

}