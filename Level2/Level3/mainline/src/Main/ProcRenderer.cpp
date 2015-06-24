#include "StdAfx.h"
#include ".\procrenderer.h"
#include "../Engine/process.h"
#include "./Processor2.h"
#include "./StretchRenderer.h"
#include "./IntersectionRenderer.h"
#include "./TurnOffRenderer.h"
#include "./RoundAboutRenderer.h"
#include "./ControlDeviceRenderer.h"
#include "./LineParkingRenderer.h"
#include "./NoseInParkingRenderer.h"
#include "./LaneAdapterRenderer.h"

ProcessorRenderer::ProcessorRenderer()
{


}

ProcessorRenderer::ProcessorRenderer(CProcessor2 * pProc):m_pProc2(pProc)
{
	m_selMode = SelectMain;
}

ProcessorRenderer::~ProcessorRenderer(void)
{

}

void ProcessorRenderer::DoSync()
{
	m_bNeedSync = false;
	
}


void ProcessorRenderer::RenderSelect(C3DView * pView,int selidx) 
{
	
}

void ProcessorRenderer::Render(C3DView* pView)
{

}

Processor* ProcessorRenderer::getProcessor()
{
	return m_pProc2->GetProcessor();
}

ProcessorRenderer* ProcessorRenderer::NewRenderer(CProcessor2  * pProc)
{
	ProcessorClassList proctype = (ProcessorClassList)pProc->GetProcessor()->getProcessorType(); 
	if(proctype ==StretchProcessor )
		return new StretchRenderer(pProc);
	else if ( proctype == IntersectionProcessor )
		return new IntersectionRenderer(pProc);
	else if( proctype == RoundAboutProcessor)
		return new RoundAboutRenderer(pProc);
	else if( proctype == LaneAdapterProcessor )
		return new LaneAdapterRenderer(pProc);
	else if( proctype == TrafficLightProceesor 
			|| proctype == TollGateProcessor
			|| proctype == YieldDeviceProcessor 
			|| proctype == StopSignProcessor)
		return new ControlDeviceRenderer(pProc);
	else if( proctype == TurnoffProcessor)
		return new TurnOffRenderer(pProc);
	else if(proctype == LineParkingProcessor)
		return new LineParkingRenderer(pProc);
	else if(proctype == NoseInParkingProcessor)
		return new NoseInParkingRenderer(pProc);

	return NULL;
}
void ProcessorRenderer::SetSelectMode(SelectMode selmode)
{
	m_selMode = selmode;
}