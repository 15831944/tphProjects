// TempProcInfo.cpp: implementation of the CTempProcInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TempProcInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include <algorithm>
using namespace std;


static const COLORREF DEFAULT_DISPLAY_COLORS[] = {
	RGB(255,255,255),
	RGB(128,0,128),
	RGB(64,255,64),
	RGB(255,0,0),
	RGB(0,0,255),
	RGB(0,0,0)
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempProcInfo::CTempProcInfo():m_iProcIndex(-1),m_bIsCommonProcessor(TRUE),m_pTempTrafficeGraph(NULL),m_pIntegratedStation( NULL )
,m_pTempTaxiwayTrafficGraph(NULL)
{
	//m_pIntegratedStation = new IntegratedStation();

}

CTempProcInfo::~CTempProcInfo()
{
	//delete m_pIntegratedStation;
ClearAll();
}

void CTempProcInfo::SetTempServicLocation(const std::vector<ARCVector3>& _newServiceLocation)
{
	m_vServiceLocation.clear();
	for(std::vector<ARCVector3>::const_iterator iter=_newServiceLocation.begin();iter!=_newServiceLocation.end();++iter)
	{
		m_vServiceLocation.push_back(*iter);
		
	}
}

void CTempProcInfo::SetTempInConstraint(const std::vector<ARCVector3>& _newInConstrain)
{
	m_vInConstraint.clear();
	for(std::vector<ARCVector3>::const_iterator iter=_newInConstrain.begin();iter!=_newInConstrain.end();++iter)
	{
		m_vInConstraint.push_back( *iter );
		
	}
}

void CTempProcInfo::SetTempOutConstrain(const std::vector<ARCVector3>& _newOutConstrain)
{
	m_vOutConstraint.clear();
	for(std::vector<ARCVector3>::const_iterator iter=_newOutConstrain.begin();iter!=_newOutConstrain.end();++iter)
	{
		m_vOutConstraint.push_back(*iter);		
	}
}

void CTempProcInfo::SetTempQueue(const std::vector<ARCVector3>& _newQueue)
{
	m_vQueieList.clear();
	for(std::vector<ARCVector3>::const_iterator iter=_newQueue.begin();iter!=_newQueue.end();++iter)
	{
		m_vQueieList.push_back( *iter );		
	}
}
void CTempProcInfo::ClearAll()
{
	m_iProcIndex=-1;
	m_pTempTrafficeGraph=NULL;
	
	//if(m_vServiceLocation.size()>0)
		m_vServiceLocation.clear();
	//if(m_vInConstraint.size()>0)
		m_vInConstraint.clear();
	//if(m_vOutConstraint.size()>0)
		m_vOutConstraint.clear();
	//if(m_vQueieList.size()>0)
		m_vQueieList.clear();

		m_vBaggageBarrier.clear();
		m_vBaggageMovingPath.clear();

		m_vStoreAreaPath.clear();

		//m_integratedStation.m_cars.clear();
	m_pTempTaxiwayTrafficGraph = NULL;
}

/*
void CTempProcInfo:: SetIntegratedStationStruct(CAR & _carVector)
{
	m_integratedStation.m_cars.clear();
	copy(_carVector.begin(),_carVector.end(),back_inserter(m_integratedStation.m_cars));
}
*/

/*
static void Draw3DVectorList(C3DMath::C3DVectorList, double dAlt)
{
}

void CTempProcInfo::DrawOGL(double dAlt, BOOL* pbDO)
{
	COLORREF col;
	glPushMatrix();
	Path* path = NULL;
	//draw service loc
	if(!pbDO[PDP_DISP_SERVLOC]) {
		col = DEFAULT_DISPLAY_COLORS[PDP_DISP_SERVLOC];
		glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		Draw3DVectorList(m_vServiceLocation, dAlt);
	}

	//draw in constraint
	if(!pbDO[PDP_DISP_INC] && m_dispProperties.bDisplay[PDP_DISP_INC]) {
		col = m_dispProperties.color[PDP_DISP_INC];
		glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		path = m_pProcessor->inConstraint();
		if(path)
			DrawPath(path, dAlt);
	}

	//draw out constraint
	if(!pbDO[PDP_DISP_OUTC] && m_dispProperties.bDisplay[PDP_DISP_OUTC]) {
		col = m_dispProperties.color[PDP_DISP_OUTC];
		glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		path = m_pProcessor->outConstraint();
		if(path)
			DrawPath(path, dAlt);
	}

	//draw queue
	if(!pbDO[PDP_DISP_QUEUE] && m_dispProperties.bDisplay[PDP_DISP_QUEUE]) {
		col = m_dispProperties.color[PDP_DISP_QUEUE];
		glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		path = m_pProcessor->queuePath();
		if(path)
			DrawPath(path, dAlt);
	}
}
*/