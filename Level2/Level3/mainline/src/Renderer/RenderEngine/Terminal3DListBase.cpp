#include "StdAfx.h"
#include "Terminal3DListBase.h"


CRender3DListBase::CRender3DListBase()
	: m_p3DScene(NULL)
{

}

void CRender3DListBase::Set3DScene( CRender3DScene* p3DScene )
{
	m_p3DScene = p3DScene;
}

CAirportObject3DListBase::CAirportObject3DListBase()
	: m_pAirport3D(NULL)
{

}

void CAirportObject3DListBase::SetRenderAirport3D( CRenderAirside3D *pAirport3D )
{
	m_pAirport3D = pAirport3D;
}