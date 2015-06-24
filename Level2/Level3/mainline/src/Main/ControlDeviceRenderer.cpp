#include "StdAfx.h"
#include ".\Controldevicerenderer.h"
#include "../Common/ARCVector.h"
#include "./3DView.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
#include "../Engine/LandControlDevice.h"
#include "./3DModelResource.h"
#include "../Engine/TrafficlightProc.h"
#include "./Processor2.h"

ControlDeviceRenderer::ControlDeviceRenderer(CProcessor2 * pProc2)  : ProcessorRenderer(pProc2)
{
	m_pModel = NULL;
	m_pModel2D = NULL;
}

ControlDeviceRenderer::~ControlDeviceRenderer(void)
{
	
}
void ControlDeviceRenderer::Render(C3DView* pView)
{
	
	LandControlDevice * pctrldev = (LandControlDevice * )getProcessor();
	ARCVector3 position = pctrldev->GetPath()[0];
	
	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	position[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( position[VZ] );
	
	
	glPushMatrix();
	glTranslated(position[VX],position[VY],position[VZ]);	
	glRotated(m_pProc2->GetRotation(), 0.0, 0.0, 1.0);
	glScaled(m_pProc2->GetScale()[VX], m_pProc2->GetScale()[VY], m_pProc2->GetScale()[VZ]);
	
	if(!m_pModel)
		 m_pModel =  pView->getModelRes().getModel(GetDeviceModelType(pctrldev,true));
	if(!m_pModel2D)
		m_pModel2D = pView->getModelRes().getModel(GetDeviceModelType(pctrldev,false));

	if(pView->GetCamera()->GetProjectionType() == C3DCamera::perspective )
	{	
		if(m_pModel)
			m_pModel->Render(pView);
	}
	else 
	{
		if(m_pModel2D)
			m_pModel2D->Render(pView);
	}
	glPopMatrix();
	
}

void ControlDeviceRenderer::RenderSelect(C3DView * pView,int selidx)
{
	glLoadName(NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0));
	Render(pView);
}
C3DModelResource::ModelType ControlDeviceRenderer::GetDeviceModelType(LandControlDevice  * device, bool b3D)
{
	if( device->getProcessorType()== TrafficLightProceesor )
		return b3D?C3DModelResource::TrafficlightModel:C3DModelResource::Trafficlight2DModel;
	if( device->getProcessorType()== TollGateProcessor )
		return b3D?C3DModelResource::TollGateModel:C3DModelResource::TollGate2DModel;
	if( device->getProcessorType() == YieldDeviceProcessor)
		return b3D?C3DModelResource::YieldSignModel:C3DModelResource::YieldSign2DModel;
	if( device->getProcessorType() == StopSignProcessor )
		return b3D?C3DModelResource::StopSignModel:C3DModelResource::StopSign2DModel;
	
	ASSERT(FALSE);
	return C3DModelResource::StopSignModel;
}
//CString ControlDeviceRenderer::GetProcRelateShapeName(LandControlDevice * device) 
//{
//	if(device->getProcessorType() == TrafficLightProceesor )
//		return "TrafficLight";
//	if(device->getProcessorType() == TollGateProcessor )
//		return "TollGate";
//	if(device->getProcessorType() == YieldDeviceProcessor)
//		return "YieldSign";
//	if(device->getProcessorType() == StopSignProcessor)
//		return "StopSign";
//	return "";
//}
