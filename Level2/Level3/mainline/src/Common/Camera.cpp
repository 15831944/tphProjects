#include "StdAfx.h"
#include ".\camera.h"
#include <Common/util.h>
#include <common/tinyxml/tinyxml.h>
#include <common/ARCStringConvert.h>

#define STR_TB_NAME _T("name")
#define STR_POS _T("position")
#define STR_LOOKAT _T("lookat") //not used anymore left for compatible problem
#define STR_UPDIR _T("updir")
#define STR_WWIDTH _T("width")
#define STR_FOVY _T("fovy")
#define STR_PROJECT _T("project")
#define STR_PROJ2D _T("2D")
#define STR_PROJ3D _T("3D")
#define STR_LOOK _T("look")

TiXmlElement * CCameraData::serializeTo( TiXmlElement *tiElm, int nVersion) const
{	
	tiElm->SetAttribute(STR_TB_NAME,m_sName.GetString());
	tiElm->SetAttribute(STR_POS, TOSTRING(m_vLocation) );
	tiElm->SetAttribute(STR_LOOK,TOSTRING(m_vLook));
	tiElm->SetAttribute(STR_UPDIR,TOSTRING(m_vUp) );
	tiElm->SetDoubleAttribute(STR_WWIDTH,dWorldWidth);
	tiElm->SetDoubleAttribute(STR_FOVY,dFovy);
	tiElm->SetAttribute(STR_PROJECT,(m_eProjType==parallel)?STR_PROJ2D:STR_PROJ3D);

	return tiElm;
}

void CCameraData::serializeFrom( const TiXmlElement* pElm, int nVersion )
{
	//int tmpInt;
	//double tmpFloat;
	m_sName = pElm->Attribute(STR_TB_NAME);
	m_vLocation = TOVECTOR3(pElm->Attribute(STR_POS));
	m_vLook= TOVECTOR3(pElm->Attribute(STR_LOOK));
	m_vUp = TOVECTOR3(pElm->Attribute(STR_UPDIR));
	pElm->Attribute(STR_WWIDTH,&dWorldWidth);
	pElm->Attribute(STR_FOVY,&dFovy);
	m_eProjType = (CString(pElm->Attribute(STR_PROJECT)).CompareNoCase(STR_PROJ2D)==0)?parallel:perspective;

	//compatible 
	const char* tmpChar = pElm->Attribute(STR_LOOKAT);
	if( tmpChar){ m_vLook = TOVECTOR3(tmpChar)-m_vLocation; }
}

CCameraData::CCameraData()
{
	InitDefault(perspective);	
}

bool CCameraData::operator ==(const CCameraData& cam)const
{
	if (m_vLocation == cam.m_vLocation && m_vLook == cam.m_vLook
		&& m_vUp == cam.m_vUp && dWorldWidth == cam.dWorldWidth 
		&& dFovy == cam.dFovy && m_eProjType == cam.m_eProjType)
	{
		return true;
	}
	return false;
}
void CCameraData::InitDefault( PROJECTIONTYPE projType )
{
	m_vLocation =ARCVector3(1600,0,1600);
	m_vLook = ARCVector3(-1,0,-1);
	m_vUp = ARCVector3(-1,0,1);
	dWorldWidth = 100;
	dFovy = 30;
	m_eProjType = projType;
}