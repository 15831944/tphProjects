#include "stdafx.h"
#include "RenderIntegratedStation3D.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "MaterialDef.h"
#include "ShapeTransformer.h"
#include "ShapeBuilder.h"
using namespace Ogre;

static const ColourValue railwaycolor(0, 0, 51/255.0f);
static const ColourValue railwaycolorselected(233/255.0f,233/255.0f,255/255.0f);
static const ColourValue msoutlinecolor(137/255.0f,137/255.0f,137/255.0f);
static const ColourValue traincolor(80/255.0f,80/255.0f,120/255.0f);
static const ColourValue linecolor(100/255.0f,100/255.0f,100/255.0f);
static const ColourValue markercolor(200/255.0f,200/255.0f,200/255.0f);
static const ColourValue processorselectboxcolor1(210/255.0f,0,34/255.0f);
static const ColourValue processorselectboxcolor2(210/255.0f,72/255.0f,95/255.0f);
static const ColourValue processortagcolor(32/255.0f,32/255.0f,32/255.0f);
static const ColourValue processortagbkgcolor(191/255.0f,223/255.0f,255/255.0f);
static const ColourValue actagcolor1(172/255.0f,171/255.0f,196/255.0f);
static const ColourValue actagcolor2(228/255.0f,227/255.0f,227/255.0f);
static const ColourValue accolor(75/255.0f,75/255.0f,75/255.0f);


void CRenderIntegratedStation3D::UpdateSelected( bool b )
{

}

//void CRenderIntegratedStation3D::Update3D()
//{
//	CProcessor2Base* pProc = GetProcessor();
//	ASSERT(pProc);
//	ProcessorProp* pProcProp = pProc->GetProcessorProp();
//	ASSERT(pProcProp && pProcProp->getProcessorType() == IntegratedStationProc);
//	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
//	ASSERT(pExtraProp && pExtraProp->GetType() == ExtraProcProp::IntegratedStation_Prop_Type);
//	IntegratedStationProp* pISExtraProp = (IntegratedStationProp*)pExtraProp;
//	CStationLayoutBase* pLayout = pISExtraProp->GetStationLayout();
//	ASSERT(pLayout);
//
//	//
//
//	UpdateSelected(GetSelected());
//}

void CRenderIntegratedStation3D::Update3D( CProcessor2Base* pProc )
{
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == IntegratedStationProc);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::IntegratedStation_Prop_Type);
	IntegratedStationProp* pStation = (IntegratedStationProp*)pExtraProp;
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;
	int nCars = pStation->GetCarCount();
	ARCVector3 center = GetWorldPosition();
	if (!pdp.bDisplay[PDP_DISP_SHAPE])
	{
		RemoveFromParent();
	}
	Point ptPort1;
	if(Path* path = pStation->GetPort1Path())
	{		
		ptPort1 = path->getPoint(0);
		ptPort1.DoOffset(-center.x,-center.y,0);
	}	
	Point ptPort2;
	if(Path*path = pStation->GetPort2Path()) {
		ptPort2 = path->getPoint(0);
		ptPort2.DoOffset(-center.x,-center.y,0);
	}

	ManualObject* pShapeObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject(GetIDName()+_T("/Shape"),GetScene());
	if(pShapeObj)
	{
		ARCColor3 stateOffColor(128,128,128);

		pShapeObj->clear();
		pShapeObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_STRIP);
		pShapeObj->position(ptPort1.getX(),ptPort1.getY(),0);pShapeObj->colour(linecolor);
		pShapeObj->position(ptPort2.getX(),ptPort2.getY(),0);
		pShapeObj->end();

		if(pdp.bDisplay[PDP_DISP_INC]) {
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_INC] : stateOffColor;
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_INC]*/cDisplay);
			Path* path = pStation->GetPreBoardAreaInConstrain();
			if(path){
				Path offsetpath = *path;
				offsetpath.DoOffset(-center.x,-center.y,-pProc->GetFloor()*SCALE_FACTOR);
				ProcessorBuilder::DrawPath(pShapeObj,&offsetpath, c,0, false);
			}
		}
		if(pdp.bDisplay[PDP_DISP_PREBOARD]) {
			//draw pre-boarding area
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_PREBOARD] : stateOffColor;
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_PREBOARD]*/cDisplay);
			Path* path = pStation->GetPreBoardAreaServPath();
			if(path){	
				Path offsetpath = *path;
				offsetpath.DoOffset(-center.x,-center.y,-pProc->GetFloor()*SCALE_FACTOR);
				ProcessorBuilder::DrawArea(pShapeObj,&offsetpath,VERTEXCOLOR_LIGHTOFF_NOCULL,c);
			}
		}
		for(int i=0; i<nCars; i++) { 
		//for each car, draw car area 	
		Path* path = pStation->GetCarAreaServPath(i);
		if(path && pdp.bDisplay[PDP_DISP_CARAREA]) {	
			ARCColor3 cPathDisplay;
			cPathDisplay = pProc->GetProcessorStateOffTag() ? ARCColor3(0,0,0) : stateOffColor;
			ColourValue cPath= OGRECOLOR(cPathDisplay);
			Path offsetpath = *path;
			offsetpath.DoOffset(-center.x,-center.y,-pProc->GetFloor()*SCALE_FACTOR);
			ProcessorBuilder::DrawPath(pShapeObj,&offsetpath,/*ColourValue::Black*/cPath,0,true);
			
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_CARAREA] : stateOffColor;
			ColourValue c= OGRECOLOR(/*pdp.color[PDP_DISP_CARAREA]*/cDisplay);

			ProcessorBuilder::DrawArea(pShapeObj,&offsetpath,VERTEXCOLOR_LIGHTOFF_NOCULL,c);
		}
		//glPolygonOffset(-3.0,-3.0);
		if(pdp.bDisplay[PDP_DISP_ENTRYDOORS]) {
			int nDoors = pStation->GetCarEntryDoorCount(i);
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_ENTRYDOORS] : stateOffColor;
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_ENTRYDOORS]*/cDisplay);
			for(int j=0;j<nDoors;j++) {
				//for each door, draw entry and exit paths
				path = pStation->GetCarEntryDoorServPath(i,j);
				if(path) {
					const static Real delta = 70.0f;
					const static Real height = 200.0f;
					const static Vector3 norm(0,1,0);
					Point pt = path->getPoint(0);
					pt.DoOffset(-center.x,-center.y,0);

					ShapeTransformer tranf;
					tranf.Rotate(ARCVector3(0,0,1),-pStation->GetOrientation());
					tranf.Translate( ARCVector3(pt.getX(), pt.getY(),0) );
					pShapeObj->begin(VERTEXCOLOR_LIGHTOFF_NOCULL,RenderOperation::OT_TRIANGLE_FAN);
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(-delta,0,0));
						//pShapeObj->normal(tranf.GetTranfMatix()*norm);
						pShapeObj->colour(c);
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(delta,0,0));
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(delta,0,height));
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(-delta,0,height));
					pShapeObj->end();				
				}
			}
		}
		if(pdp.bDisplay[PDP_DISP_EXITDOORS]) {

			int nDoors = pStation->GetCarExitDoorCount(i);
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_EXITDOORS] : stateOffColor;
			ColourValue c = OGRECOLOR(/*pdp.color[PDP_DISP_EXITDOORS]*/cDisplay);
			for(int j=0;j<nDoors;j++) {
				path = pStation->GetCarExitDoorServPath(i,j);
				if(path) {
					const static Real delta = 70.0f;
					const static Real height = 200.0f;
					const static Vector3 norm(0,1,0);
					Point pt = path->getPoint(0);
					pt.DoOffset(-center.x,-center.y,0);

					ShapeTransformer tranf;
					tranf.Rotate(ARCVector3(0,0,1),-pStation->GetOrientation());
					tranf.Translate( ARCVector3(pt.getX(), pt.getY(),0) );
					pShapeObj->begin(VERTEXCOLOR_LIGHTOFF_NOCULL,RenderOperation::OT_TRIANGLE_FAN);
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(-delta,0,0));
					//pShapeObj->normal(tranf.GetTranfMatix()*norm);
					pShapeObj->colour(c);
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(delta,0,0));
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(delta,0,height));
					pShapeObj->position(tranf.GetTranfMatix()*Vector3(-delta,0,height));
					pShapeObj->end();		
				}
			}
		}

		AddObject(pShapeObj,true);
	}


	
	}
}

void CRenderIntegratedStation3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);

	Update3D(pProc);
	////CProcessor2Base* pProc = 
	//ASSERT(pProc);
	//ProcessorProp* pProcProp = pProc->GetProcessorProp();
	//ASSERT(pProcProp && pProcProp->getProcessorType() == IntegratedStationProc);
	//ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	//ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::IntegratedStation_Prop_Type);
	//IntegratedStationProp* pISExtraProp = (IntegratedStationProp*)pExtraProp;
	//CStationLayoutBase* pLayout = pISExtraProp->GetStationLayout();
	//ASSERT(pLayout);
	//

	//UpdateSelected(GetSelected());
	UpdateNoShape(pProc,DRAWALL);
}
