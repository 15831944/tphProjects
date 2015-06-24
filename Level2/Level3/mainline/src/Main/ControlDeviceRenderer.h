#ifndef __CONTROL_DEVICE_RENDERER
#define __CONTROL_DEVICE_RENDERER
#pragma once
#include "procrenderer.h"
#include "./ShapesManager.h"
#include "./3DModelResource.h"

class CProcessor2;
class LandControlDevice;

class ControlDeviceRenderer :
	public ProcessorRenderer
{
public:
	
	ControlDeviceRenderer(CProcessor2  * pProc2) ;
	virtual ~ControlDeviceRenderer(void);

	virtual void Render(C3DView* pView);

	virtual void RenderSelect(C3DView * pView,int selidx);

	virtual bool Selectable(){ return true; }

	//get the device relate shapes name; b3D == true return model in 3d view ,else in 2D 
	static C3DModelResource::ModelType GetDeviceModelType(LandControlDevice  * device,bool b3D);
	//static CString GetProcRelateShapeName(LandControlDevice * device) ;
private:
	//CShape * m_pShape;
	C3dModel*	m_pModel;
	C3dModel*   m_pModel2D;
};

#endif