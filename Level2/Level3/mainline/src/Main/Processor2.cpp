// Processor2.cpp: implementation of the CProcessor2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Processor2.h"
#include <CommonData/Shape.h>
#include "TermPlanDoc.h"
#include "Engine\IntegratedStation.h"
#include "Engine\MovingSideWalk.h"
#include "Engine\ElevatorProc.h"
#include "Engine\Stair.h"
#include "Engine\Conveyor.h"
#include "Engine\baggage.h"
#include "Engine\hold.h"
#include "Engine\RunwayProc.h"
#include "Engine\TaxiwayProc.h"
#include "Engine\FixProc.h"
#include "Engine\StandProc.h"
#include "Engine\DeiceBayProc.h"
#include "Engine\ApronProc.h"
#include "Engine\BillboardProc.h"
#include <Common/ShapeGenerator.h>
#include "./BillBoardRenderer.h"
#include "ShapesManager.h"
#include "./HoldRenderer.h"
#include "Engine/HoldProc.h"
#include "Engine/SectorProc.h"
#include "SectorRenderer.h"
#include "TaxiwayRenderer.h"
#include "./3DView.h"
#include "./ChildFrm.h"
#include "ShapeRenderer.h"
#include "Placement.h"
#include "../Engine/BridgeConnector.h"
#include "RenderUtility.h"
#include "Resource.h"
#include <Common/Guid.h>
#include <boost/bind.hpp>
#include "../Engine/fixedq.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const ARCColor3 DEFAULT_DISPLAY_COLORS[] = {
	(255,255,255),
	(128,0,128),
	(64,255,64),
	(255,0,0),
	(0,0,255),
	(0,0,0)
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessor2::CProcessor2() : high_light_EntryPoint(FALSE)
{

	m_nSelectName = -1;
	m_pProcessor = NULL;
	m_pShape = NULL;
	m_nFloor = -1;
	m_pGroup=NULL;
	//// TRACE("sizeof(BOOL) = %d, sizeof(DWORD) = %d, sizeof(COLORREF) = %d\n", sizeof(BOOL), sizeof(DWORD), sizeof(COLORREF));
	m_pRenderer = NULL;	

	m_dspType = ObjectDisplayType_Processor2;
}



CProcessor2::~CProcessor2()
{
	delete m_pRenderer;
}



static void DrawList(std::vector<ARCVector3>* pListToDraw, BOOL bLoop = FALSE)
{
	if(pListToDraw->size() == 1) {
		
		ARCVector3 pt = pListToDraw->at(0);
		const double delta = 100.0;
		glBegin(GL_LINE_LOOP);
		glVertex3d(pt[VX]-delta, pt[VY]-delta, pt[VZ]);
		glVertex3d(pt[VX]-delta, pt[VY]+delta, pt[VZ]);
		glVertex3d(pt[VX]+delta, pt[VY]+delta, pt[VZ]);
		glVertex3d(pt[VX]+delta, pt[VY]-delta, pt[VZ]);
		glEnd();
	}
	else {
		if(bLoop)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_LINE_STRIP);
		for(int i=0; i<static_cast<int>(pListToDraw->size()); i++) {
			glVertex3d(pListToDraw->at(i)[VX], pListToDraw->at(i)[VY], pListToDraw->at(i)[VZ]);
		}
		glEnd();
	}
}

static void DrawPath(Path* pPathToDraw, double dAlt, BOOL bLoop = FALSE)
{
	static const double delta = 10.0;
	int c = pPathToDraw->getCount();
	if(c == 1) {
		Point pt = pPathToDraw->getPoint(0);
		glBegin(GL_LINE_LOOP);
		glVertex3d(pt.getX()-delta, pt.getY()-delta, dAlt);
		glVertex3d(pt.getX()-delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()-delta, dAlt);
		glEnd();
	}
	else {
		glBegin(bLoop ? GL_LINE_LOOP : GL_LINE_STRIP);
		Point* pts = pPathToDraw->getPointList();
		for(int i=0; i<c; i++) {
			glVertex3d(pts[i].getX(), pts[i].getY(),dAlt);
		}
		glEnd();
	}
}

static void DrawHeadArrow(Path* pPathToDraw, double dAlt)
{
	static const double dx = 20.0;
	static const double dy = 40.0;
	int c = pPathToDraw->getCount();
	if(c > 1) {

		Point pt =  pPathToDraw->getPoint(c-1);
		Point l(pPathToDraw->getPoint(c-2), pPathToDraw->getPoint(c-1));
		glPushMatrix();
		glTranslated(pt.getX(), pt.getY(), dAlt);
		glRotated(-l.getHeading(), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}

static void DrawTailArrow(Path* pPathToDraw, double dAlt)
{
	static const double dx = 20.0;
	static const double dy = 40.0;
	int c = pPathToDraw->getCount();
	if(c > 1) {
		Point pt =  pPathToDraw->getPoint(0);
		Point l(pPathToDraw->getPoint(1), pPathToDraw->getPoint(0));
		glPushMatrix();
		glTranslated(pt.getX(), pt.getY(), dAlt);
		glRotated(-l.getHeading(), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}

static void RotatePath(Path* pPath, double dr, double cx, double cy)
{
	int c = pPath->getCount();
	double dc = cos(dr);
	double ds = sin(dr);
	Point* pts = pPath->getPointList();
	for(int i=0; i<c; i++) {
		double dx = pts[i].getX() - cx;
		double dy = pts[i].getY() - cy;
		//pts[i].setX(pts[i].getX()+dx);
		//pts[i].setY(pts[i].getY()+dy);
	}
}

static void OffsetPath(Path* pPath, double dx, double dy)
{
	int c = pPath->getCount();
	Point* pts = pPath->getPointList();
	for(int i=0; i<c; i++) {
		pts[i].setX(pts[i].getX()+dx);
		pts[i].setY(pts[i].getY()+dy);
	}
}

//mirrors point p across line ab
//assume a, b Z component == 0
static void MirrorPoint(ARCVector3& p, ARCVector3& a, ARCVector3& b)
{
	double z = p[VZ];
	p[VZ] = 0;
	ARCVector3 ab(b-a);
	ARCVector3 ap(p-a);
	double magSquared = ab[VX]*ab[VX]+ab[VY]*ab[VY];
	ARCVector3 an(((ap.dot(ab))/magSquared)*ab);
	ARCVector3 pn((an+a)-p);
	p += 2*pn;
	p[VZ] = z;
}

void CProcessor2::MirrorProperties(const std::vector<ARCVector3>& vLine)
{
	ARCVector3 a(vLine[0][VX],vLine[0][VY],0.0);
	ARCVector3 b(vLine[1][VX],vLine[1][VY],0.0);
	ARCVector3 ab(b-a);

	//////////////Step 1 - Mirror CProcessor2-related data///////////////////
	//determine new rotation to compensate for mirror
	//get cos and sin of rotation angle
	double dCos = cos(ARCMath::DegreesToRadians(GetRotation()));
	double dSin = sin(ARCMath::DegreesToRadians(GetRotation()));
	// compensate for rounding errors
	if(fabs(dCos) < ARCMath::EPSILON)
		dCos = 0.0;
	if(fabs(dSin) < ARCMath::EPSILON)
		dSin = 0.0;
	ARCVector3 dir(100.0*dCos,100.0*dSin,0.0);
	dir+=m_vLocation;
	MirrorPoint(dir,a,b);
	//mirror shape location
	MirrorPoint(m_vLocation,a,b);

	dir -= m_vLocation;

	double dRot1 = ARCMath::RadiansToDegrees(acos(dir[VX]/100.0));
	double dRot2 = ARCMath::RadiansToDegrees(asin(dir[VY]/100.0));
	dRot1 = _copysign(dRot1, dRot2);

	double dRotDiff = dRot1 - GetRotation();
	SetRotation(dRot1);


	//////////////Step 2 - Mirror Processor-related data///////////////////
	Path p;
	p.init(2);
	Point* ptList = p.getPointList();
	for(int i=0; i<2; i++) {
		ptList[i].setPoint(vLine[i][VX], vLine[i][VY], vLine[i][VZ]);
	}

	if(m_pProcessor)
		m_pProcessor->Mirror(&p);
}

void CProcessor2::RotateProperties(double dr, const ARCVector3& point)
{
	if(m_pProcessor == NULL) 
	{
		// TRACE("Processor == NULL\n");
		return;
	}
	if(m_pProcessor->getProcessorType() == IntegratedStationProc) 
	{
		int nr = (int) dr;
		int nRotation = (int) -m_dRotation;
		while(nRotation < 0)
			nRotation = 360+nRotation;
		((IntegratedStation*)m_pProcessor)->SetOrientation(nRotation);		
		m_pProcessor->Rotate( -nr );
	}
	else if ( m_pProcessor->getProcessorType() == Elevator ) 
	{
		int nRotation = (int) -m_dRotation;
		while(nRotation < 0)
			nRotation = 360+nRotation;		
		((ElevatorProc*)m_pProcessor)->SetOrientation(nRotation);	
		m_pProcessor->Rotate( -dr );
	}
	else {
		m_pProcessor->DoOffset(-point[VX], -point[VY]);
		m_pProcessor->Rotate(-dr);
		m_pProcessor->DoOffset(point[VX], point[VY]);
	}
}

void CProcessor2::OffsetProperties(double dx, double dy)
{
	if(m_pProcessor == NULL) {
		// TRACE("Processor == NULL\n");
		return;
	}
	Path* path = NULL;
	if( m_pProcessor->getProcessorType() == IntegratedStationProc
		|| m_pProcessor->getProcessorType() == BarrierProc
		|| m_pProcessor->getProcessorType() == BaggageProc 
		|| m_pProcessor->getProcessorType() == Elevator 
		|| m_pProcessor->getProcessorType() == LineProc  
		|| m_pProcessor->getProcessorType() == RunwayProcessor
		|| m_pProcessor->getProcessorType() == TaxiwayProcessor
		|| m_pProcessor->getProcessorType() == BridgeConnectorProc
		)
	{
		m_pProcessor->DoOffset(dx,dy);
	}
	else {
		path = m_pProcessor->serviceLocationPath();
		if(path)
			OffsetPath(path, dx, dy);
		path = m_pProcessor->inConstraint();
		if(path)
			OffsetPath(path, dx, dy);
		path = m_pProcessor->outConstraint();
		if(path)
			OffsetPath(path, dx, dy);
		path = m_pProcessor->queuePath();
		if(path)
			OffsetPath(path, dx, dy);
	}
	m_pProcessor->UpdateMinMax();
}

static void DrawCircle(const Point& pt,double dAlt)
{
	int nCircle = 30;
	double dRadius = 50.0;
	double Pi = 3.1415926536f;
	glPushMatrix();
	glTranslated(pt.getX(),pt.getY(),dAlt);
	glBegin(GL_POLYGON);
	for(int i=0; i<nCircle; ++i)
	{
		double dx = dRadius * cos(2 * Pi / nCircle * i);
		double dy = dRadius * sin(2 * Pi / nCircle * i);
		double dz = 0.0;
		glVertex3d(dx,dy,dz);
	}
	glEnd();
	glPopMatrix();
}
void CProcessor2::DrawOGL(C3DView* pView,double dAlt, BOOL* pbDO, UINT nDrawMask)
{
	static const ARCColor4 shadowcolor(128,128,128,196);
	static const ARCColor3 linecolor(100,100,100);
	int i,j;
	if(m_pProcessor == NULL) {
		//if this is an under construction processor, draw only shape
		if(!pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE] && m_pShape != NULL) {
			glEnable(GL_LIGHTING);
			glPushMatrix();
			glTranslatef(0.0,0.0,3.0);
			glColor3ubv(m_dispProperties.color[PDP_DISP_SHAPE]);
			glTranslated(m_vLocation[VX], m_vLocation[VY], m_vLocation[VZ]+dAlt);
			glRotated(m_dRotation, 0.0, 0.0, 1.0);
			glScaled(m_vScale[VX]*m_pShape->GetScale(), m_vScale[VY]*m_pShape->GetScale(), m_vScale[VZ]*m_pShape->GetScale());
			m_pShape->DrawOGL();
			glPopMatrix();

			glDisable(GL_LIGHTING);
			//Added by Tim for shape's shadow In 2/21/2003///////////////
			if(m_vLocation[VZ]!=0)
			{
				glPushMatrix();
				glColor4ubv(shadowcolor);
				glTranslated(m_vLocation[VX],m_vLocation[VY],dAlt);
				glRotated(m_dRotation,0.0,0.0,1.0);
				glScaled(m_vScale[VX],m_vScale[VY],0);
				m_pShape->DrawOGL();
				glPopMatrix();
			}
			////////////////////////////////////////////////////////////

		}
		return;
	}

	/*m_pcpmControlPointManager.Draw(pView);*/

	if(m_bIsSelected)
		glLineWidth(3.0f);

	ARCColor3 stateOffColor(128,128,128);
	Path* path = NULL;
	glPushMatrix();
	glTranslatef(0.0,0.0,3.0);
	int nProcType = m_pProcessor->getProcessorType();
	if(nProcType == IntegratedStationProc) {
		//glEnable(GL_LIGHTING);
		IntegratedStation* pStation = (IntegratedStation*) m_pProcessor;
		//STATION processor
		int nCars = pStation->GetCarCount();

		//draw port1
		//col = StationProcColors[PORT1];
		//glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		path = pStation->GetPort1Path();
		Point ptPort1;
		if(path)
		{
			//DrawPath(path, dAlt, FALSE);
			ptPort1 = path->getPoint(0);
		}

		//draw port2
		//col = StationProcColors[PORT2];
		//glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
		path = pStation->GetPort2Path();
		Point ptPort2;
		if(path) {
			ptPort2 = path->getPoint(0);
		}

		//draw line between ports
		glColor3ubv(linecolor);
		glBegin(GL_LINES);
		glVertex3d(ptPort1.getX(), ptPort1.getY(), dAlt);
		glVertex3d(ptPort2.getX(), ptPort2.getY(), dAlt);
		glEnd();


		//draw inconstraint of preboarding area.
		if(m_dispProperties.bDisplay[PDP_DISP_INC]) {
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_INC] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_INC]*/cDisplay);
			path = pStation->GetPreBoardingArea()->inConstraint();
			if(path)
				DrawPath(path, dAlt, FALSE);
			glPushMatrix();
			glTranslated(0,0,dAlt);
			m_pcpmControlPointManager.DrawInConstraintControlPoints(pView);
			//m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
			glPopMatrix();

		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.0,-1.0);
		if(m_dispProperties.bDisplay[PDP_DISP_PREBOARD]) {
			//draw pre-boarding area
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_PREBOARD] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_PREBOARD]*/cDisplay);
			path = pStation->GetPreBoardingArea()->serviceLocationPath();
			if(path) {
				int c = path->getCount();
				Point* pts = path->getPointList();
				glBegin(GL_QUADS);
				glNormal3f(0.0,0.0,1.0);
				for(j=0; j<c; j++) {
					glVertex3d(pts[j].getX(), pts[j].getY(),dAlt);
				}
				
				glEnd();
				
				glPushMatrix();
				glTranslated(0,0,dAlt);
				m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
				glPopMatrix();
			}
		}

		for(i=0; i<nCars; i++) { 
			//for each car, draw car area 
			//col = StationProcColors[CARAREA];
			//glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
			//glPolygonOffset(2.0,1.0);
			path = pStation->GetCar(i)->GetCarArea()->serviceLocationPath();
			if(path && m_dispProperties.bDisplay[PDP_DISP_CARAREA]) {
				int c = path->getCount();
				Point* pts = path->getPointList();
				glColor3ubv(ARCColor3::BLACK);
				glLineWidth(2.0);
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-2.0,-2.0);
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBegin(GL_LINE_LOOP);
				glNormal3f(0.0,0.0,1.0);
				for(j=0; j<c; j++) {
					glVertex3d(pts[j].getX(), pts[j].getY(),dAlt);
				}
				glEnd();

				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDisable(GL_POLYGON_OFFSET_LINE);

				glLineWidth(1.0);
				glPolygonOffset(-1.0,-1.0);
				ARCColor3 cDisplay;
				cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_CARAREA] : stateOffColor;
				glColor3ubv(/*m_dispProperties.color[PDP_DISP_CARAREA]*/cDisplay);
				glBegin(GL_QUADS);
				glNormal3f(0.0,0.0,1.0);
				for(j=0; j<c; j++) {
					glVertex3d(pts[j].getX(), pts[j].getY(),dAlt);
				}
				glEnd();

			}
			glPolygonOffset(-3.0,-3.0);
			if(m_dispProperties.bDisplay[PDP_DISP_ENTRYDOORS]) {
				int nDoors = pStation->GetCar(i)->GetEntryDoorCount();
				ARCColor3 cDisplay;
				cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_ENTRYDOORS] : stateOffColor;
				glColor3ubv(/*m_dispProperties.color[PDP_DISP_ENTRYDOORS]*/cDisplay);
				for(int j=0;j<nDoors;j++) {
					//for each door, draw entry and exit paths
					path = pStation->GetCar(i)->GetEntryDoor(j)->serviceLocationPath();
					if(path) {
						const double delta = 70.0;
						Point pt = path->getPoint(0);
						glPushMatrix();
						glTranslated(pt.getX(), pt.getY(),dAlt);
						glRotated(-pStation->GetOrientation(),0.0,0.0,1.0);
						glBegin(GL_QUADS);
						glNormal3f(0.0,1.0,0.0);
						/*
						glVertex3d(-delta, 0.0, dAlt);
						glVertex3d(delta, 0.0, dAlt);
						glVertex3d(delta, 0.0, dAlt+200);
						glVertex3d(-delta, 0.0, dAlt+200);
						*/

						// modified by tutu in 2-12-2003					
						glVertex3d(-delta, 0.0, 0);
						glVertex3d(delta, 0.0, 0);
						glVertex3d(delta, 0.0, 200);
						glVertex3d(-delta, 0.0,200);					
						glEnd();
						glPopMatrix();
					}
				}
			}
			if(m_dispProperties.bDisplay[PDP_DISP_EXITDOORS]) {

				int nDoors = pStation->GetCar(i)->GetExitDoorCount();
				ARCColor3 cDisplay;
				cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_EXITDOORS] : stateOffColor;
				glColor3ubv(/*m_dispProperties.color[PDP_DISP_EXITDOORS]*/cDisplay);
				for(j=0;j<nDoors;j++) {
					path = pStation->GetCar(i)->GetExitDoor(j)->serviceLocationPath();
					if(path) {
						const double delta = 70.0;
						Point pt = path->getPoint(0);
						glPushMatrix();
						glTranslated(pt.getX(), pt.getY(),dAlt);
						glRotated(-pStation->GetOrientation(),0.0,0.0,1.0);
						glBegin(GL_QUADS);
						glNormal3f(0.0,1.0,0.0);
						/*
						glVertex3d(-delta, 0.0, dAlt);
						glVertex3d(delta, 0.0, dAlt);
						glVertex3d(delta, 0.0, dAlt+200);
						glVertex3d(-delta, 0.0, dAlt+200);
						*/

						// modified by tutu in 2-12-2003					
						glVertex3d(-delta, 0.0, 0);
						glVertex3d(delta, 0.0, 0);
						glVertex3d(delta, 0.0, 200);
						glVertex3d(-delta, 0.0,200);					
						glEnd();
						glPopMatrix();
					}
				}
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	/*
	else if(m_pProcessor->getProcessorType() == Elevator) {	
	ElevatorProc* pElevator = ( ElevatorProc*) m_pProcessor;
	int nMinFloor = pElevator->GetMinFloor();
	int nMaxFloor = pElevator->GetMaxFloor();
	for(int nFloor=nMinFloor; nFloor<=nMaxFloor; nFloor++) {
	std::vector<Pollygon> vAreas;
	pElevator->GetDataAtFloor(nFloor).GetLayoutData(vAreas);
	for(int i=0; i<vAreas.size(); i++) {
	DrawPath(&(vAreas[i]), dAlt, TRUE);
	}
	}
	}
	else if(m_pProcessor->getProcessorType() == FloorChangeProc) {
	FloorChangeProcessor* pFlrChangeProc = (FloorChangeProcessor*)m_pProcessor;
	int nConnections = pFlrChangeProc->getConnectionCount();
	for(int nCon=0; nCon<nConnections; nCon++) {
	//pFlrChangeProc->getConnection(nCon).start 
	}
	}
	*/
	else if( m_pProcessor->getProcessorType() == BillboardProcessor)
	{
		if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE])
		{
			glEnable(GL_LIGHTING);
			glPushMatrix();
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;

			glColor3ubv(/*m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay);
			glTranslated(0, 0, m_vLocation[VZ]+dAlt);
			//glRotated(m_dRotation, 0.0, 0.0, 1.0);
			//glScaled(m_vScale[VX], m_vScale[VY], m_vScale[VZ]);
			//cal the shape		
			if(!m_pRenderer)
			{
				m_pRenderer = new BillBoardRenderer((BillboardProc*)m_pProcessor,pView);
			}
			m_pRenderer->Render(pView);

			glTranslated(0,0,-m_vLocation[VZ]);
			if(/*(nDrawMask & DRAWOUTC) && */!pbDO[PDP_DISP_OUTC] && m_dispProperties.bDisplay[PDP_DISP_OUTC]) {
				ARCColor3 cDisplay;
				cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_OUTC] : stateOffColor;
				glColor3ubv(/*m_dispProperties.color[PDP_DISP_OUTC]*/cDisplay);
				glBegin(GL_LINE_LOOP);
				Path *_exposrArea = ( (BillboardProc*) m_pProcessor)->GetExpsurePath();
				for(int i=0;i<_exposrArea->getCount();++i)
				{
					glVertex3dv( ARCVector3(_exposrArea->getPoint(i)) );
				}
				glEnd();
				/*glColor3ubv(m_dispProperties.color[PDP_DISP_OUTC]);
				path = m_pProcessor->outConstraint();
				if(path) {
				DrawPath(path, dAlt);
				DrawHeadArrow(path, dAlt);
				}*/
			}
			m_pcpmControlPointManager.Draw(pView);

			glPopMatrix();
			glDisable(GL_LIGHTING);
		}
	}
	//else if(m_pProcessor->getProcessorType()==TaxiwayProcessor)
	//{		
	//	glPushMatrix();
	//	glTranslated(0,0,dAlt);
	//	if(!m_pRenderer) m_pRenderer = new TaxiwayRenderer(this);
	//	if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE]){
	//		m_pRenderer->Render(pView);	
	//	}
	//	glEnable(GL_POLYGON_OFFSET_FILL);
	//	glPolygonOffset(-2.0f,-2.0f);
	//	if((nDrawMask & DRAWSERVLOC) && !pbDO[PDP_DISP_SERVLOC] && m_dispProperties.bDisplay[PDP_DISP_SERVLOC]){

	//		((TaxiwayRenderer*)m_pRenderer)->RenderMarkLine(pView);
	//		if(pView->GetDocument()->m_bdisplayTaxiDir){
	//			glColor3ub(255,255,0);
	//			((TaxiwayRenderer*)m_pRenderer)->RenderSegmentDir(pView);
	//		}
	//	}

	//	m_pcpmControlPointManager.Draw(pView);

	//	glDisable(GL_POLYGON_OFFSET_FILL);
	//	glPopMatrix();
	//}
	//else if(m_pProcessor->getProcessorType() == FixProcessor ){
	//	if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE]){
	//		DistanceUnit lowalt = ((FixProc*)(GetProcessor()))->GetLowerLimit();
	//		DistanceUnit highalt = ((FixProc*)(GetProcessor()))->GetUpperLimit();
	//		const double FixHight=100000.0;
	//		const double FixTopRad=8000*0.3048;

	//		double diskrlow=lowalt*FixTopRad/FixHight; 
	//		double  diskrhigh=highalt*FixTopRad/FixHight;
	//		glEnable(GL_LIGHTING);
	//		if(m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
	//			GLUquadric* pQuadObj = gluNewQuadric();
	//			glPushMatrix();
	//			//glTranslated(GetProcessor()->getServicePoint(0).getX(), GetProcessor()->getServicePoint(0).getY(), dAlt[nProcFloor] + pProc2->GetProcessor()->getServicePoint(0).getZ());

	//			glColor3ubv(ARCColor3::WHITE);
	//			gluCylinder(pQuadObj, 0.0, diskrlow,lowalt, 36, 12);

	//			glPushMatrix();
	//			glTranslated(0,0,lowalt);
	//			glColor3ubv(m_dispProperties.color[PDP_DISP_SHAPE]);
	//			gluCylinder(pQuadObj,diskrlow,diskrhigh,highalt-lowalt,36,12);
	//			glPopMatrix();

	//			/*glPushMatrix();
	//			glTranslated(0,0,highalt);
	//			glColor3ubv(ARCColor3::WHITE);
	//			gluCylinder(pQuadObj,diskrhigh,FixTopRad,100000.0-highalt,36,12);
	//			glPopMatrix();*/

	//			glPushMatrix();		
	//			glTranslated(0.0, 0.0, highalt);
	//			gluDisk(pQuadObj, 0.0, diskrhigh, 36, 8);								
	//			glPopMatrix();

	//			glPopMatrix();
	//			gluDeleteQuadric(pQuadObj);
	//		}
	//		m_pcpmControlPointManager.Draw(pView);
	//	}

	//}
	//else if(m_pProcessor->getProcessorType() == HoldProcessor){
	//	if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE])
	//	{
	//		//glEnable(GL_LIGHTING);
	//		glPushMatrix();
	//		//glColor3ubv(m_dispProperties.color[PDP_DISP_SHAPE]);
	//		glColor4f(float(m_dispProperties.color[PDP_DISP_SHAPE][RED])/255.0f,float(m_dispProperties.color[PDP_DISP_SHAPE][GREEN])/255.0f,float(m_dispProperties.color[PDP_DISP_SHAPE][BLUE])/255.0f,0.3f);
	//		glTranslated(0, 0, dAlt);
	//		//glRotated(m_dRotation, 0.0, 0.0, 1.0);
	//		//glScaled(m_vScale[VX], m_vScale[VY], m_vScale[VZ]);
	//		//cal the shape		
	//		if(!m_pRenderer)
	//		{
	//			m_pRenderer = new HoldRenderer(this);
	//		}
	//		m_pRenderer->Render(pView);
	//		((HoldRenderer*)m_pRenderer)->RenderShadow(pView);
	//		glPopMatrix();
	//		//glDisable(GL_LIGHTING);
	//		m_pcpmControlPointManager.Draw(pView);
	//	}
	//}
	//else if(m_pProcessor->getProcessorType() == SectorProcessor){
	//	if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_dispProperties.bDisplay[PDP_DISP_SHAPE]){
	//		glPushMatrix();			
	//		glColor4f(float(m_dispProperties.color[PDP_DISP_SHAPE][RED])/255.0f,float(m_dispProperties.color[PDP_DISP_SHAPE][GREEN])/255.0f,float(m_dispProperties.color[PDP_DISP_SHAPE][BLUE])/255.0f,0.25f);
	//		glTranslated(0, 0, dAlt);			
	//		if(!m_pRenderer)
	//		{
	//			m_pRenderer = new SectorRenderer(this);
	//		}
	//		m_pRenderer->Render(pView);
	//		//((SectorRenderer*)m_pRenderer)->RenderShadow(pView);
	//		glPopMatrix();
	//		m_pcpmControlPointManager.Draw(pView);
	//	}
	//}
	//else if(m_pProcessor->getProcessorType() == StandProcessor){
	//	if((nDrawMask & DRAWINC) && !pbDO[PDP_DISP_INC] && m_dispProperties.bDisplay[PDP_DISP_INC]) {
	//		glColor3ubv(m_dispProperties.color[PDP_DISP_INC]);
	//		path = m_pProcessor->inConstraint();
	//		DrawPath(path, dAlt);
	//		m_pcpmControlPointManager.DrawInConstraintControlPoints(pView);
	//		DrawHeadArrow(path, dAlt);
	//	}
	//	if((nDrawMask & DRAWOUTC) && !pbDO[PDP_DISP_OUTC] && m_dispProperties.bDisplay[PDP_DISP_OUTC]) {
	//		glColor3ubv(m_dispProperties.color[PDP_DISP_OUTC]);
	//		path = m_pProcessor->outConstraint();
	//		if(path && path->getCount()>0 ) 
	//		{		
	//			DrawPath(path, dAlt);
	//			m_pcpmControlPointManager.DrawOutConstraintControlPoints(pView);
	//			DrawHeadArrow(path, dAlt);
	//		}
	//	}

	//}
	//else if(m_pProcessor->getProcessorType() == DeiceBayProcessor){
	//	DeiceBayProc * pdeice=(DeiceBayProc *)GetProcessor();

	//	//render

	//	glEnable(GL_POLYGON_OFFSET_FILL);
	//	glPolygonOffset(-2.0f,-2.0f);
	//	glColor3ubv(m_dispProperties.color[PDP_DISP_SERVLOC]);
	//	pdeice->DrawToScene(dAlt);
	//	ARCVector2 vdir = pdeice->GetDir(); vdir.Rotate(90);

	//	ARCVector3 vpos1 = pdeice->getServicePoint(0) + ARCVector3( 0.4 * pdeice->GetWinspan() * vdir,0) + ARCVector3(0.4 * pdeice->GetLength()* pdeice->GetDir(),0 ) ;
	//	ARCVector3 vpos2 = pdeice->getServicePoint(0) - ARCVector3( 0.4 * pdeice->GetWinspan() * vdir,0)+ARCVector3(0.4 * pdeice->GetLength()* pdeice->GetDir(),0 );		

	//	double dAngleFromX =- ARCMath::RadiansToDegrees(acos(vdir[VX]));


	//	glColor3ubv(m_dispProperties.color[PDP_DISP_QUEUE]);
	//	glDisable(GL_POLYGON_OFFSET_FILL);
	//	glPushMatrix();
	//	glTranslated(vpos1[VX],vpos1[VY],vpos1[VZ]);
	//	glRotated(dAngleFromX,0,0,1);
	//	//pView->m_pRenderer->render_shape("deicetruck");
	//	glPopMatrix();

	//	glPushMatrix();
	//	glTranslated(vpos2[VX],vpos2[VY],vpos2[VZ]);
	//	glRotated(dAngleFromX+90,0,0,1);
	//	//pView->m_pRenderer->render_shape("deicetruck");
	//	glPopMatrix();

	//	glDisable(GL_LIGHTING);
	//	if((nDrawMask & DRAWINC) && !pbDO[PDP_DISP_INC] && m_dispProperties.bDisplay[PDP_DISP_INC]) {
	//		glColor3ubv(m_dispProperties.color[PDP_DISP_INC]);
	//		path = m_pProcessor->inConstraint();
	//		DrawPath(path, dAlt);
	//		m_pcpmControlPointManager.DrawInConstraintControlPoints(pView);
	//		DrawHeadArrow(path, dAlt);
	//	}
	//	if((nDrawMask & DRAWOUTC) && !pbDO[PDP_DISP_OUTC] && m_dispProperties.bDisplay[PDP_DISP_OUTC]) {
	//		glColor3ubv(m_dispProperties.color[PDP_DISP_OUTC]);
	//		path = m_pProcessor->outConstraint();
	//		if(path && path->getCount()>0 ) 
	//		{		
	//			DrawPath(path, dAlt);
	//			m_pcpmControlPointManager.DrawOutConstraintControlPoints(pView);
	//			DrawHeadArrow(path, dAlt);
	//		}
	//	}
	//	glEnable(GL_LIGHTING);


	//}
	else { //Normal Processor
		//draw shape
		if((nDrawMask & DRAWSHAPE) && !pbDO[PDP_DISP_SHAPE] && m_pShape != NULL && m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
			glEnable(GL_LIGHTING);
			glPushMatrix();
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay);
			glTranslated(m_vLocation[VX], m_vLocation[VY], m_vLocation[VZ]+dAlt);
			glRotated(m_dRotation, 0.0, 0.0, 1.0);
			glScaled(m_vScale[VX], m_vScale[VY], m_vScale[VZ]);
			m_pShape->DrawOGL();
			glPopMatrix();

			glDisable(GL_LIGHTING);
			
		
			//Added by Tim for shape's shadow In 2/21/2003///////////////
			if(m_vLocation[VZ]!=0)
			{
				glPushMatrix();
				glColor4ubv(shadowcolor);
				glTranslated(m_vLocation[VX],m_vLocation[VY],dAlt);
				glRotated(m_dRotation,0.0,0.0,1.0);
				glScaled(m_vScale[VX],m_vScale[VY],0);
				m_pShape->DrawOGL();
				glPopMatrix();
			}
			////////////////////////////////////////////////////////////
		}

		//if(nDrawMask & DRAWALLNONSHAPE) {
		//draw service loc
		if((nDrawMask & DRAWSERVLOC) && !pbDO[PDP_DISP_SERVLOC] && m_dispProperties.bDisplay[PDP_DISP_SERVLOC]) {
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_SERVLOC] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_SERVLOC]*/cDisplay);
			if(  m_pProcessor->getProcessorType() != BaggageProc   )
			{
				path = m_pProcessor->serviceLocationPath();
				if(path)
				{
					DrawPath(path, dAlt, m_pProcessor->getProcessorType() == HoldAreaProc  );

					glPushMatrix();
					glTranslated(0,0,dAlt);
					m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
					glPopMatrix();
					//m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
				}

			}		
			else
			{
				path = ((BaggageProcessor*)m_pProcessor)->serviceLocationPath();
				if( path )
				{
					ARCColor3 cDisplay;
					cDisplay = m_bStateOnProcessor ? ARCColor3(128,128,255): stateOffColor;
					glColor3ubv(/*128,128,255*/cDisplay);
					DrawPath(path, dAlt );
					glPushMatrix();
					glTranslated(0,0,dAlt);
					m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
					glPopMatrix();
					
					//m_pcpmControlPointManager.DrawServiceLocationControlPoints(pView);
				}

				path = ((BaggageProcessor*)m_pProcessor)->GetBarrier()->serviceLocationPath();
				if( path )
				{
					ARCColor3 cDisplay;
					cDisplay = m_bStateOnProcessor ? ARCColor3(0,0,0): stateOffColor;
					glColor3ubv(/*0,0,0*/cDisplay);
					DrawPath( path, dAlt, true );
				}

				path = ((BaggageProcessor*)m_pProcessor)->GetPathBagMovingOn();
				if( path )
				{
					ARCColor3 cDisplay;
					cDisplay = m_bStateOnProcessor ? ARCColor3(128,64,64): stateOffColor;
					glColor3ubv(/*128,64,64*/cDisplay);
					DrawPath( path, dAlt, true );
				}
			}
		}

		//draw in constraint
		if((nDrawMask & DRAWINC) && !pbDO[PDP_DISP_INC] && m_dispProperties.bDisplay[PDP_DISP_INC]) {

			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_INC] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_INC]*/cDisplay);
			path = m_pProcessor->inConstraint();
			if(path && path->getCount()>0 ) 
			{
				double _dAlt = dAlt;

				//_dAlt = pView->GetDocument()->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude( path->getPoint(0).getZ());

				DrawPath(path, _dAlt);

				glPushMatrix();
				glTranslated(0,0,_dAlt);
				m_pcpmControlPointManager.DrawInConstraintControlPoints(pView);
				glPopMatrix();

				DrawHeadArrow(path, _dAlt);
			}
		}

		//draw out constraint
		if((nDrawMask & DRAWOUTC) && !pbDO[PDP_DISP_OUTC] && m_dispProperties.bDisplay[PDP_DISP_OUTC]) {
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_OUTC] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_OUTC]*/cDisplay);
			path = m_pProcessor->outConstraint();
			if(path && path->getCount()>0 ) 
			{
				double _dAlt = dAlt;
				//_dAlt = pView->GetDocument()->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude( path->getPoint(0).getZ());
				DrawPath(path, _dAlt);

				glPushMatrix();
				glTranslated(0,0,_dAlt);
				m_pcpmControlPointManager.DrawOutConstraintControlPoints(pView);
				glPopMatrix();

				DrawHeadArrow(path, _dAlt);
			}
		}

		//draw queue
		if((nDrawMask & DRAWQUEUE) && !pbDO[PDP_DISP_QUEUE] && m_dispProperties.bDisplay[PDP_DISP_QUEUE]) {
			if(!m_pProcessor->queueIsFixed()) {
				glLineStipple(1, 0x00FF);
				glEnable(GL_LINE_STIPPLE);
			}
			ARCColor3 cDisplay;
			cDisplay = m_bStateOnProcessor ? m_dispProperties.color[PDP_DISP_QUEUE] : stateOffColor;
			glColor3ubv(/*m_dispProperties.color[PDP_DISP_QUEUE]*/cDisplay);
			path = m_pProcessor->queuePath();
			if(path) 
			{
				double _dAlt = dAlt;
				if (m_pProcessor->queueIsFixed())
				{
					FixedQueue* pFixQueue = (FixedQueue*)m_pProcessor->GetQueue();
					ASSERT(pFixQueue);
					std::vector<int>* vEntryPoint = pFixQueue->GetEntryPoints(); 
					ASSERT(vEntryPoint);
					for (unsigned i = 0; i < vEntryPoint->size(); i++)
					{
						int nPoint = vEntryPoint->at(i);
						Point pt = path->getPoint(nPoint);
						DrawCircle(pt,_dAlt);
					}
				}
				//_dAlt = pView->GetDocument()->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude( path->getPoint(0).getZ());

				DrawPath(path, _dAlt);

				glPushMatrix();
				glTranslated(0,0,_dAlt);
				m_pcpmControlPointManager.DrawQueueControlPoints(pView);
				glPopMatrix();

				DrawTailArrow(path, _dAlt);
			}
			glDisable(GL_LINE_STIPPLE);
		}
		//}
	}
	glPopMatrix();

	glLineWidth(1.0f);
}

void CProcessor2::DrawBB(double dAlt, BOOL bFirst)
{
	static const ARCColor3 limegreen(0,255,0);
	static const ARCColor3 darkergreen(64,128,64);
	if(m_pShape != NULL) {
		glPushMatrix();
		glTranslated(m_vLocation[VX], m_vLocation[VY], m_vLocation[VZ]+dAlt);
		glScaled(m_vScale[VX], m_vScale[VY], m_vScale[VZ]);
		glRotated(m_dRotation, 0.0, 0.0, 1.0);
		//draw bounding box
		
		ARCVector3 vExtMax = m_pShape->ExtentsMax();
		ARCVector3 vExtMin = m_pShape->ExtentsMin();
		ARCVector3 buff = 0.05*(vExtMax-vExtMin);
		vExtMax += buff;
		vExtMin -= buff;
		if(bFirst)
			glColor3ubv(limegreen);	//lime-green
		else
			glColor3ubv(darkergreen);	//darker-green
		glBegin(GL_LINE_LOOP);
			glVertex3dv(vExtMax);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMax[VZ]);
			glVertex3d(vExtMin[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMax[VZ]);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3dv(vExtMin);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMax[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMin[VZ]);
		glEnd();
		glBegin(GL_LINES);
			glVertex3dv(vExtMax);
			glVertex3d(vExtMax[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMin[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3dv(vExtMin);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMin[VZ]);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMax[VZ]);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMin[VZ]);
		glEnd();

		glPopMatrix();
	}
}

void CProcessor2::DrawSelectOGL(CTermPlanDoc* pDoc, double* dAlt)
{
	int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	if(m_pProcessor != NULL) {
		//m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		if(m_pProcessor->getProcessorType() == IntegratedStationProc) {
			ASSERT(m_nSelectName != -1);
			IntegratedStation* pStation = (IntegratedStation*) m_pProcessor;
			int nCars = pStation->GetCarCount();
			glLoadName(m_nSelectName);
			glPushMatrix();
			for(int i=0; i<nCars; i++) { 
				//for each car, draw car area 
				//col = StationProcColors[CARAREA];
				//glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
				Path* path = pStation->GetCar(i)->GetCarArea()->serviceLocationPath();
				if(path) {
					int j;
					int c = path->getCount();
					Point* pts = path->getPointList();
					glColor3ub(255,255,102); //yellow
					glBegin(GL_QUADS);
					glNormal3f(0.0,0.0,1.0);
					for(j=0; j<c; j++) {
						glVertex3d(pts[j].getX(), pts[j].getY(),dAlt[m_nFloor]);//1
					}					
					glEnd();					
				}
			}	
			glPopMatrix();
			glPushMatrix();
			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();
			
			return;
		}
		else if(m_pProcessor->getProcessorType() ==MovingSideWalkProc) {
			ASSERT(m_nSelectName != -1);
			MovingSideWalk* pSidewalk = (MovingSideWalk*) m_pProcessor;
			glLoadName(m_nSelectName);
			glPushMatrix();
			Path* path = pSidewalk->serviceLocationPath();
			if(path) {
				int c = path->getCount();
				Point* pts = path->getPointList();
				glBegin(GL_QUADS);
				for(int j=1; j<c; j++) {
					//get perpedicular vector
					double dOffset=pts[j].getY()-pts[j-1].getY();
					if(dOffset==0)
						dOffset=0.000000001;

					ARCVector2 v(1.0, -(pts[j].getX()-pts[j-1].getX())/(dOffset));
					v.Normalize();
					v*=100.0;
					glVertex3d(pts[j-1].getX()-v[VX], pts[j-1].getY()-v[VY],dAlt[m_nFloor]);
					glVertex3d(pts[j].getX()-v[VX], pts[j].getY()-v[VY],dAlt[m_nFloor]);
					glVertex3d(pts[j].getX()+v[VX], pts[j].getY()+v[VY],dAlt[m_nFloor]);
					glVertex3d(pts[j-1].getX()+v[VX], pts[j-1].getY()+v[VY],dAlt[m_nFloor]);
				}				
				glEnd();
			}		
			glPopMatrix();
			glPushMatrix();
			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();
			return;
		}
		else if(m_pProcessor->getProcessorType() ==StairProc) {
			ASSERT(m_nSelectName != -1);
			int nFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
			Stair* pStair = (Stair*) m_pProcessor;
			glLoadName(m_nSelectName);
			glPushMatrix();
			Path* stairPath = pStair->serviceLocationPath();
			int nPtCount = stairPath->getCount();
			ASSERT(nPtCount>1);
			Point* pts = stairPath->getPointList();

			for(int j=0; j<nPtCount-1; j++) 
			{
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				double dAltDiff1;
				if(nIndexInFloor1<nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				double dPtRealAlt1=dAlt[nIndexInFloor1]+mult*dAltDiff1;

				int nIndexInFloor2=static_cast<int>(pts[j+1].getZ()/SCALE_FACTOR);
				mult = (pts[j+1].getZ()/SCALE_FACTOR)-nIndexInFloor2;
				double dAltDiff2;
				if(nIndexInFloor2<nFloorCount-1) {
					dAltDiff2=dAlt[nIndexInFloor2+1]-dAlt[nIndexInFloor2];
				}
				else {
					dAltDiff2=1000.0;
				}
				double dPtRealAlt2=dAlt[nIndexInFloor2]+mult*dAltDiff2;
				
				ARCVector3 v1(pts[j+1].getX()-pts[j].getX(), pts[j+1].getY()-pts[j].getY(), dPtRealAlt2 - dPtRealAlt1);
				ARCVector3 v2(v1);
				v2[VZ]=0.0;

				if(v1==v2) {
					v2[VX]=0.0;v2[VY]=0.0;v2[VZ]=1.0;
				}

				double dLength = v1.Magnitude()/100.0; //in meters
				
				ARCVector3 vPerp(v1^v2);
				vPerp.Normalize();
				vPerp*=pStair->GetWidth()/2.0; //half-width of conveyor
				
				ARCVector3 vOtherPerp(v1^vPerp);
				vOtherPerp.Normalize();
				vOtherPerp*=-30.0;
				
				glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0);
				glVertex3d(pts[j].getX()-vPerp[VX], pts[j].getY()-vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,0.0);
				glVertex3d(pts[j].getX()+vPerp[VX], pts[j].getY()+vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,dLength);
				glVertex3d(pts[j+1].getX()+vPerp[VX], pts[j+1].getY()+vPerp[VY], dPtRealAlt2);
				glTexCoord2d(0.0,dLength);
				glVertex3d(pts[j+1].getX()-vPerp[VX], pts[j+1].getY()-vPerp[VY], dPtRealAlt2);
				
				glEnd();
			}
			glPopMatrix();
			glPushMatrix();
			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();

			return;
		}
		// add by Mark
		else if(m_pProcessor->getProcessorType() == EscalatorProc) {
			ASSERT(m_nSelectName != -1);
			int nFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
			Stair* pStair = (Stair*) m_pProcessor;
			glLoadName(m_nSelectName);
			glPushMatrix();
			Path* stairPath = pStair->serviceLocationPath();
			int nPtCount = stairPath->getCount();
			ASSERT(nPtCount>1);
			Point* pts = stairPath->getPointList();
			
			for(int j=0; j<nPtCount-1; j++) 
			{
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				double dAltDiff1;
				if(nIndexInFloor1<nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				double dPtRealAlt1=dAlt[nIndexInFloor1]+mult*dAltDiff1;
				
				int nIndexInFloor2=static_cast<int>(pts[j+1].getZ()/SCALE_FACTOR);
				mult = (pts[j+1].getZ()/SCALE_FACTOR)-nIndexInFloor2;
				double dAltDiff2;
				if(nIndexInFloor2<nFloorCount-1) {
					dAltDiff2=dAlt[nIndexInFloor2+1]-dAlt[nIndexInFloor2];
				}
				else {
					dAltDiff2=1000.0;
				}
				double dPtRealAlt2=dAlt[nIndexInFloor2]+mult*dAltDiff2;
				
				ARCVector3 v1(pts[j+1].getX()-pts[j].getX(), pts[j+1].getY()-pts[j].getY(), dPtRealAlt2 - dPtRealAlt1);
				ARCVector3 v2(v1);
				v2[VZ]=0.0;
				
				if(v1==v2) {
					v2[VX]=0.0;v2[VY]=0.0;v2[VZ]=1.0;
				}
				
				double dLength = v1.Magnitude()/100.0; //in meters
				
				ARCVector3 vPerp(v1^v2);
				vPerp.Normalize();
				vPerp*=pStair->GetWidth()/2.0; //half-width of conveyor
				
				ARCVector3 vOtherPerp(v1^vPerp);
				vOtherPerp.Normalize();
				vOtherPerp*=-30.0;
				
				glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0);
				glVertex3d(pts[j].getX()-vPerp[VX], pts[j].getY()-vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,0.0);
				glVertex3d(pts[j].getX()+vPerp[VX], pts[j].getY()+vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,dLength);
				glVertex3d(pts[j+1].getX()+vPerp[VX], pts[j+1].getY()+vPerp[VY], dPtRealAlt2);
				glTexCoord2d(0.0,dLength);
				glVertex3d(pts[j+1].getX()-vPerp[VX], pts[j+1].getY()-vPerp[VY], dPtRealAlt2);
				glEnd();
			}

			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();
			return;
		}
		else if(m_pProcessor->getProcessorType() ==Elevator) {
			ASSERT(m_nSelectName != -1);
			ElevatorProc* pElevator = (ElevatorProc*) m_pProcessor;
			int nMinFloor = pElevator->GetMinFloor();
			int nMaxFloor = pElevator->GetMaxFloor();
			glLoadName(m_nSelectName);
			glPushMatrix();
			for(int nFloor=nMinFloor; nFloor<=nMaxFloor; nFloor++) {
				std::vector<Pollygon> vAreas;
				pElevator->GetDataAtFloor(nFloor).GetLayoutData(vAreas);
				for(int i=0; i<static_cast<int>(vAreas.size()); i++) {
					Path* path =&(vAreas[i]);
					if(path) {
						int j;
						int c = path->getCount();
						Point* pts = path->getPointList();
						glColor3ub(255,255,102); //yellow
						glBegin(GL_QUADS);
						glNormal3f(0.0,0.0,1.0);
						for(j=0; j<c; j++) {
							glVertex3d(pts[j].getX(), pts[j].getY(),dAlt[nFloor]);
						}
						glEnd();
					}
				}
			}

			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();
			return;
		}
		else if(m_pProcessor->getProcessorType() == ConveyorProc) {
			ASSERT(m_nSelectName != -1);
			Conveyor* pConveyor = (Conveyor*)( m_pProcessor );
			int nFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
			glLoadName(m_nSelectName);
			glPushMatrix();

			Path* conveyorpath = pConveyor->serviceLocationPath();
			int nConveyorPtCount = conveyorpath->getCount();
			ASSERT(nConveyorPtCount>1);
			Point* pts = conveyorpath->getPointList();

			for(int j=0; j<nConveyorPtCount-1; j++) 
			{
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				double dAltDiff1;
				if(nIndexInFloor1<nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				double dPtRealAlt1=dAlt[nIndexInFloor1]+mult*dAltDiff1;

				int nIndexInFloor2=static_cast<int>(pts[j+1].getZ()/SCALE_FACTOR);
				mult = (pts[j+1].getZ()/SCALE_FACTOR)-nIndexInFloor2;
				double dAltDiff2;
				if(nIndexInFloor2<nFloorCount-1) {
					dAltDiff2=dAlt[nIndexInFloor2+1]-dAlt[nIndexInFloor2];
				}
				else {
					dAltDiff2=1000.0;
				}
				double dPtRealAlt2=dAlt[nIndexInFloor2]+mult*dAltDiff2;
				
				ARCVector3 v1(pts[j+1].getX()-pts[j].getX(), pts[j+1].getY()-pts[j].getY(), dPtRealAlt2 - dPtRealAlt1);
				ARCVector3 v2(v1);
				v2[VZ]=0.0;

				if(v1==v2) {
					v2[VX]=0.0;v2[VY]=0.0;v2[VZ]=1.0;
				}

				ARCVector3 vPerp(v1^v2);
				vPerp.Normalize();
				vPerp*=50;
				double dLength = v1.Magnitude()/100.0; //in meters
				glBegin(GL_QUADS);
				glTexCoord2d(0.0,0.0);
				glVertex3d(pts[j].getX()-vPerp[VX], pts[j].getY()-vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,0.0);
				glVertex3d(pts[j].getX()+vPerp[VX], pts[j].getY()+vPerp[VY], dPtRealAlt1);
				glTexCoord2d(1.0,dLength);
				glVertex3d(pts[j+1].getX()+vPerp[VX], pts[j+1].getY()+vPerp[VY], dPtRealAlt2);
				glTexCoord2d(0.0,dLength);
				glVertex3d(pts[j+1].getX()-vPerp[VX], pts[j+1].getY()-vPerp[VY], dPtRealAlt2);
				glEnd();
			}

			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

			glPopMatrix();
			return;
		}
		/****add by ben for fixprocessor 2005-8-23**/
		/*else if (m_pProcessor->getProcessorType()== FixProcessor){		

			ASSERT(m_nSelectName != -1);
			FixProc* pfixproc = (FixProc*) m_pProcessor;	
			const double FixHight=100000.0;
			const double FixTopRad=8000*0.3048;

			double diskrlow=pfixproc->GetLowerLimit()*FixTopRad/FixHight; 
			double  diskrhigh=pfixproc->GetUpperLimit()*FixTopRad/FixHight;
			glLoadName(m_nSelectName);			
			GLUquadric* pQuadObj = gluNewQuadric();			
			glPushMatrix();
			glTranslated(m_pProcessor->getServicePoint(0).getX(),m_pProcessor->getServicePoint(0).getY(),dAlt[m_nFloor]);	
				gluCylinder(pQuadObj, 0.0, diskrhigh,pfixproc->GetUpperLimit(), 36, 12);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
			glPopMatrix();
			gluDeleteQuadric(pQuadObj);
			return;
		}*/
		
		else if (m_pProcessor->getProcessorType() == BillboardProcessor )
		{
			glLoadName(m_nSelectName);
			glPushMatrix();
			glTranslated(0, 0, m_vLocation[VZ]+dAlt[m_nFloor]);
			if(!m_pRenderer)
			{
				C3DView *p3DView = pDoc->Get3DView();
				if (p3DView)
					m_pRenderer = new BillBoardRenderer((BillboardProc*)m_pProcessor,p3DView);
			}
			if(m_pRenderer)
				m_pRenderer->Render(pDoc->Get3DView());
			glPopMatrix();
			glPushMatrix();
			glTranslated(0,0,dAlt[m_nFloor]);
			m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
			glPopMatrix();
			return;
		}
		else if(m_pProcessor->getProcessorType() == BridgeConnectorProc)
		{
			glLoadName(m_nSelectName);
			BridgeConnector* pConnector = (BridgeConnector*)GetProcessor();
			//for(int i=0;i<pConnector->GetConnectPointCount();i++)
			{
				const BridgeConnector::ConnectPoint& pConectPoint =  pConnector->m_connectPoint;//(i);			
				CRenderUtility::SelectBridgeConPtr(pConectPoint,dAlt[m_nFloor] );				
			}
		}
		//else if (m_pProcessor->getProcessorType() == HoldProcessor )
		//{
		//	//glLoadName(m_nSelectName);
		//	if(m_pRenderer)
		//		m_pRenderer->RenderSelect(pDoc->Get3DView(),m_nSelectName);
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	return;
		//}
		//else if (m_pProcessor->getProcessorType()==StandProcessor){
		//	ASSERT(m_nSelectName != -1);
		//	glLoadName(m_nSelectName);
		//	GLUquadric* pQuadObj = gluNewQuadric();
		//	glPushMatrix();
		//	glTranslated(m_pProcessor->getServicePoint(0).getX(),m_pProcessor->getServicePoint(0).getY(),m_pProcessor->getServicePoint(0).getZ()+dAlt[m_nFloor]);						
		//	gluCylinder(pQuadObj, 25.0, 25.0, 500.0, 16, 5);			
		//	gluDisk(pQuadObj, 0.0, 250.0, 16, 1);
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	glPopMatrix();
		//	gluDeleteQuadric(pQuadObj);
		//}
		//else if(m_pProcessor->getProcessorType() == SectorProcessor){
		//	if(m_pRenderer)
		//		m_pRenderer->RenderSelect(pDoc->Get3DView(),m_nSelectName);
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	return;
		//}
		//
		//else if(m_pProcessor->getProcessorType()==DeiceBayProcessor){
		//	ASSERT(m_nSelectName != -1);
		//	DeiceBayProc * pdeicebar=(DeiceBayProc *)m_pProcessor;
		//	
		//	glLoadName(m_nSelectName);
		//	glPushMatrix();
		//	pdeicebar->DrawSelectArea(dAlt[m_nFloor]);
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	glPopMatrix();
		//	

		//}
		//else if(m_pProcessor->getProcessorType()==ApronProcessor)
		//{
		//	ASSERT(m_nSelectName != -1);
		//	ApronProc * pApron=(ApronProc *)m_pProcessor;
		//	Path * pApronpath=pApron->serviceLocationPath();
		//	glLoadName(m_nSelectName);
		//	glPushMatrix();
		//	pApron->DrawSelectArea(dAlt[m_nFloor]);
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	glPopMatrix();

		//}
		//////** end add by ben 20005-8-24**//
		//else if (m_pProcessor->getProcessorType() == RunwayProcessor)
		//{
		//	ASSERT(m_nSelectName != -1);
		//	RunwayProc* pRunway = (RunwayProc*) m_pProcessor;
		//	glLoadName(m_nSelectName);
		//	glPushMatrix();			
		//	Path* path = pRunway->serviceLocationPath();
		//	if(path) {
		//		int c = path->getCount();
		//		Point* pts = path->getPointList();
		//		glBegin(GL_QUADS);
		//		for(int j=1; j<c; j++) {
		//			//get perpedicular vector
		//			double dOffset=pts[j].getY()-pts[j-1].getY();
		//			if(dOffset==0)
		//				dOffset=0.000000001;

		//			ARCVector2 v(1.0, -(pts[j].getX()-pts[j-1].getX())/(dOffset));
		//			v.Normalize();
		//			v*=pRunway->GetWidth() / 2.0f;
		//			glVertex3d(pts[j-1].getX()-v[VX], pts[j-1].getY()-v[VY],dAlt[m_nFloor]);//m_nFloor
		//			glVertex3d(pts[j].getX()-v[VX], pts[j].getY()-v[VY],dAlt[m_nFloor]);//
		//			glVertex3d(pts[j].getX()+v[VX], pts[j].getY()+v[VY],dAlt[m_nFloor]);//
		//			glVertex3d(pts[j-1].getX()+v[VX], pts[j-1].getY()+v[VY],dAlt[m_nFloor]);//
		//		}
		//		m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//		glEnd();
		//	}
		//	glPopMatrix();
		//	return;
		//}
		//
		//else if (m_pProcessor->getProcessorType() == TaxiwayProcessor)
		//{
		//	ASSERT(m_nSelectName != -1);
		//	TaxiwayProc* pTaxiway = (TaxiwayProc*) m_pProcessor;
		//	glLoadName(m_nSelectName);
		//	//glPushMatrix();
		//	//Path* path = pTaxiway->serviceLocationPath();
		//	//if(path) {
		//	//	int c = path->getCount();
		//	//	Point* pts = path->getPointList();
		//	//	glBegin(GL_QUADS);
		//	//	for(int j=1; j<c; j++) {
		//	//		//get perpedicular vector
		//	//		double dOffset=pts[j].getY()-pts[j-1].getY();
		//	//		if(dOffset==0)
		//	//			dOffset=0.000000001;

		//	//		ARCVector2 v(1.0, -(pts[j].getX()-pts[j-1].getX())/(dOffset));
		//	//		v.Normalize();
		//	//		v*=40 / 2.0f;
		//	//		glVertex3d(pts[j-1].getX()-v[VX], pts[j-1].getY()-v[VY],dAlt[m_nFloor]);
		//	//		glVertex3d(pts[j].getX()-v[VX], pts[j].getY()-v[VY],dAlt[m_nFloor]);
		//	//		glVertex3d(pts[j].getX()+v[VX], pts[j].getY()+v[VY],dAlt[m_nFloor]);
		//	//		glVertex3d(pts[j-1].getX()+v[VX], pts[j-1].getY()+v[VY],dAlt[m_nFloor]);
		//	//	}
		//	//	glEnd();
		//	//}
		//	//glPopMatrix();
		//	if(!m_pRenderer)m_pRenderer = new TaxiwayRenderer(this);
		//	m_pRenderer->Render(pDoc->Get3DView());
		//	m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());
		//	return;
		//}
	}
	if(m_pShape != NULL) {
		ASSERT(m_nSelectName != -1);
		glLoadName(m_nSelectName);
		glPushMatrix();
		glTranslated(m_vLocation[VX], m_vLocation[VY], m_vLocation[VZ]+dAlt[m_nFloor]);
		glRotated(m_dRotation, 0.0, 0.0, 1.0);
		glScaled(m_vScale[VX], m_vScale[VY], m_vScale[VZ]);
	
		//draw bounding cube
		ARCVector3 vExtMax = m_pShape->ExtentsMax();
		ARCVector3 vExtMin = m_pShape->ExtentsMin();
		glBegin(GL_QUADS);
			glVertex3dv(vExtMax);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMax[VZ]);
			glVertex3d(vExtMin[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3dv(vExtMin);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMax[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMin[VZ]);
			glVertex3dv(vExtMax);
			glVertex3d(vExtMax[VX],vExtMax[VY],vExtMin[VZ]);
			glVertex3d(vExtMin[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3dv(vExtMin);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMax[VZ]);
			glVertex3d(vExtMax[VX],vExtMin[VY],vExtMin[VZ]);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMax[VZ]);
			glVertex3d(vExtMin[VX],vExtMax[VY],vExtMin[VZ]);
		glEnd();

		glPopMatrix();
		glPushMatrix();
		glTranslated(0,0,dAlt[m_nFloor]);
		m_pcpmControlPointManager.DrawSelect(pDoc->Get3DView());

		glPopMatrix();
	}

}




void CProcessor2::SetProcessor(Processor *_pProc)
{
	m_pProcessor = _pProc;

	if (m_pProcessor)
		m_pcpmControlPointManager.Initialize(m_pProcessor->serviceLocationPath(),m_pProcessor->queuePath(),m_pProcessor->inConstraint(),m_pProcessor->outConstraint(),this);
}

ProcessorRenderer* CProcessor2::GetRenderer()
{
	if(! m_pRenderer )
		m_pRenderer = ProcessorRenderer::NewRenderer( this );

	if( !m_pRenderer )
		m_pRenderer = new ProcessorRenderer(this);

	return m_pRenderer;
}
CProcessor2 *CProcessor2::GetCopy()
{
	CProcessor2*  pNewProc2= new CProcessor2();
	//set floor to active floor
	pNewProc2->SetFloor(GetFloor());
	//set location, scale and rotatio
	pNewProc2->SetLocation(GetLocation());
	pNewProc2->SetScale(GetScale());
	pNewProc2->SetRotation(GetRotation());
	//set shape
	pNewProc2->SetShape(GetShape());
	pNewProc2->ShapeName() = ShapeName();
	//set dispproperties
	pNewProc2->m_dispProperties = m_dispProperties;
	return pNewProc2;
}

inline
static bool isAllNumeric(const char* buf)
{
	int len = strlen(buf);
	for(int i=0; i<len; i++) {
		if(!isNumeric(buf[i]))
			return false;
	}
	return true;
}

static void NextAlphaNumeric(char* buf)
{
	int len = strlen(buf);
	if(len==1) {
		if(buf[0]=='Z')
			strcpy(buf, "AA");
		else
			buf[0]++;
	}
	else if(len==2) {
		if(buf[1]=='Z') {
			if(buf[0]!='Z') {
				buf[0]++;
				buf[1]='A';
			}
			else
				strcat(buf, "_COPY");
		}
		else {
			buf[1]++; //increment last
		}
	}
	else {
		strcat(buf, "_COPY");
	}
}
static ProcessorID* GetNextProcessorID(const ProcessorID* id, ProcessorList* procList)
{
	char buf[128];
	ProcessorID* pNewID = new ProcessorID(*id);
	int lastLevel = pNewID->idLength()-1;
	pNewID->getNameAtLevel(buf, lastLevel);

	//check if last level of ID is numeric or alphanumeric
	char c = toupper(buf[0]);
	int len = strlen(buf);
	if(isAllNumeric(buf)) {
		//numeric
		int i = atoi(buf);
		do {
			i++;
			_itoa(i,buf,10);
			pNewID->setNameAtLevel(buf,lastLevel);
		} while(procList->findProcessor(*pNewID) != INT_MAX);
	}
	else if(len <= 2 && c >= 'A' && c <= 'Z') {
		do {
			NextAlphaNumeric(buf);
			pNewID->setNameAtLevel(buf,lastLevel);
		} while(procList->findProcessor(*pNewID) != INT_MAX);
	}
	else {
		//other
		do {
			strcat(buf, "_COPY");
			pNewID->setNameAtLevel(buf,lastLevel);
		} while(procList->findProcessor(*pNewID) != INT_MAX);
	}

	return pNewID;
}

bool CProcessor2::CopyProcToDest(CTermPlanDoc* _pSrcDoc, CTermPlanDoc* _pDestDoc, CProcessor2* _pProc, const int _nFloor)
{
	if(_pSrcDoc == NULL || _pDestDoc == NULL || _pProc == NULL)
		return false;

	SetFloor(_nFloor);

	SetLocation(_pProc->GetLocation());
	SetScale(_pProc->GetScale());
	SetRotation(_pProc->GetRotation());

	m_dispProperties = _pProc->m_dispProperties;
	if (_pProc->HasShape())
	{
		SetShape(SHAPESMANAGER->GetShapeByName(_pProc->GetShape()->Name()));
	}
	SetSelectName(_pSrcDoc->GetUniqueNumber());

	Processor* pProcessor = _pProc->GetProcessor();

	if (pProcessor == NULL)//only shape
	{
		SetProcessor(NULL);
		CString strProc2Name = _pProc->ShapeName().GetIDString();
		CString strTemp = _T("");
		strTemp = _pDestDoc->GetCurrentPlacement()->m_vUndefined.GetName(strProc2Name);
	
		ShapeName(strTemp);
		_pDestDoc->GetCurrentPlacement()->m_vUndefined.setNode((ALTObjectID)strTemp);
		_pDestDoc->GetCurrentPlacement()->m_vUndefined.InsertUnDefinePro2(std::make_pair(strTemp,this));
		_pDestDoc->GetCurrentPlacement()->m_vUndefined.InsertShapeName(strTemp);
		_pDestDoc->GetCurrentPlacement()->m_vUndefined.insert(_pDestDoc->GetCurrentPlacement()->m_vUndefined.begin(), this);
		_pDestDoc->GetCurrentPlacement()->saveDataSet(_pDestDoc->m_ProjInfo.path,false);
		_pDestDoc->AddToUnderConstructionNode(this);
		return true;
	}

	
	if(_pProc->GetProcessor()->getProcessorType() == IntegratedStationProc)
	{
		CString strMsg = "Processor " + _pProc->GetProcessor()->getID()->GetIDString() +"'s type is Integrated station.It can not be pasted";
		AfxMessageBox(strMsg);
		return false;
	}

	Terminal& term = _pDestDoc->GetTerminal();

	ProcessorID PID,PID2;
	PID.SetStrDict(term.inStrDict);
	PID.setID(_pProc->GetProcessor()->getID()->GetIDString());
	//check Processor Name and Type--------------------------
	char nameLevel[256];
	PID.getNameAtLevel(nameLevel,0);
	PID2.SetStrDict(term.inStrDict);
	PID2.setID(nameLevel);
	GroupIndex GI = term.GetProcessorList()->getGroupIndex(PID2);
	if(GI.start!=-1)
	{
		Processor* pProcFinded=term.GetProcessorList()->getProcessor(GI.start);
		if(pProcFinded->getProcessorType()!=_pProc->GetProcessor()->getProcessorType())
		{
			CString strMsg="Processor "+_pProc->GetProcessor()->getID()->GetIDString() +"'s type can not match.It can not be pasted";
			AfxMessageBox(strMsg);
			return false;
		}
	}

	// set processor id
	Processor tempProc;
	tempProc.init(PID);
	ProcessorID* pNewID = GetNextProcessorID(tempProc.getID(),term.GetProcessorList());
	//check processor Name-----------------------------------
	char pNewIDName[256];
	pNewID->printID(pNewIDName);
	BOOL bCheck = term.GetProcessorList()->IsValidProcessorName(pNewIDName);
	if(!bCheck)
	{
		delete pNewID;
		return false;
	}

	int nFloorCount = _pDestDoc->GetFloorCount();
	int nDestFloorTotal = _pDestDoc->GetCurModeFloor().m_vFloors.size();
	Processor *pNewProc = _pProc->GetProcessor()->CopyProcessor(*pNewID, &term, _nFloor, nFloorCount, nDestFloorTotal);
	pNewProc->init(*pNewID);
	if(pNewProc->getProcessorType() == PointProc || 
	   pNewProc->getProcessorType() == LineProc  ||
	   pNewProc->getProcessorType() == GateProc)
	{
		pNewProc->setEmergentFlag(_pProc->GetProcessor()->getEmergentFlag());
	}
	if(pNewProc == NULL)
		return false;

	SetProcessor(pNewProc);

	term.GetProcessorList()->addProcessor(pNewProc);
	term.GetProcessorList()->setIndexes();			
	term.GetProcessorList()->saveDataSet(_pDestDoc->m_ProjInfo.path, false);

	_pDestDoc->GetCurrentPlacement()->m_vDefined.insert( _pDestDoc->GetCurrentPlacement()->m_vDefined.begin(), this );
	_pDestDoc->GetCurrentPlacement()->saveDataSet(_pDestDoc->m_ProjInfo.path, false);
	_pDestDoc->m_msManager.m_msImplTerminal.SetTermPlanDoc(_pDestDoc);
	//_pDestDoc->m_msManager.m_msImplTerminal.RebuildProcessorTree(NULL);
	std::stack<CString> vList;
	ProcessorID id = *(pNewProc->getID());
	int nLength = id.idLength();
	for (int i = 0; i < nLength; i++)
	{
		vList.push(id.GetLeafName());
		id.clearLevel(id.idLength() - 1);
	}

	int nIndex = 0;
	CTVNode* pNode = NULL;
	CTVNode* pTempNode = NULL;
	int idLength = id.idLength();
	CString strNodeName = _T("");

	CTVNode* pPlacementsNode = _pDestDoc->ProcessorsNode();
	pNode = pPlacementsNode;
	while(vList.size())
	{
		pTempNode = pNode;
		strNodeName = vList.top();
		vList.pop();
		pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
		if (pNode == NULL)
		{
			if (vList.size() == 0)
			{
				CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_PROCESSOR);
				pTempNode->AddChild(pChildNode);
				pChildNode->m_eState = CTVNode::expanded;
				pChildNode->m_dwData = (DWORD)this;
			}
			else
			{
				pNode = new CTVNode(strNodeName);
				pTempNode->AddChild(pNode);
				pNode->m_eState = CTVNode::expanded;
			}
		}	
	}
	

	_pDestDoc->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE, pPlacementsNode);

	return true;
}
//void CProcessor2::GetLocation(ARCVector3& location) const {
//	
//	location = m_vLocation;
//
//}	

bool CProcessor2::GetProCtrlPointVisible(void)
{
	return (m_pcpmControlPointManager.m_bVisible);
}
void CProcessor2::SetProCtrlPointVisible(bool bVisible/* = false*/)
{
	m_pcpmControlPointManager.m_bVisible = bVisible;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////CNudefinePro2Node
CNudefinePro2Node::CNudefinePro2Node()
:_nodeName(_T(""))
{

}

CNudefinePro2Node::~CNudefinePro2Node()
{
	for (size_t i = 0; i < vChild.size(); i++)
	{
		delete vChild[i];
	}
	vChild.clear();
}

void CNudefinePro2Node::addNode(CNudefinePro2Node* pNode)
{
	ASSERT(pNode);
	vChild.push_back(pNode);
}

CNudefinePro2Node* CNudefinePro2Node::getNode(const CString nodeName)
{
	if (mapNode.find(nodeName) == mapNode.end())
	{
		return NULL;
	}
	return mapNode[nodeName];
}

void CNudefinePro2Node::setNode(const ALTObjectID& idName,int ndx)
{
	CNudefinePro2Node* pNode = NULL;
	if (ndx == 4 || idName.at(ndx).empty())
	{
		return;
	}

	CString nodeName = idName.at(ndx).c_str();
	nodeName.MakeUpper();
	pNode = getNode(nodeName);
	if (pNode == NULL)
	{
		pNode= new CNudefinePro2Node();
		pNode->_nodeName = nodeName;
		addNode(pNode);
		InsertMapNode(std::make_pair(pNode->getNodeName(),pNode));
		pNode->setNode(idName,ndx+1);
	}
	else
	{
		pNode->setNode(idName,ndx+1);
	}
}

int CNudefinePro2Node::getNodeCount()
{
	return (int)vChild.size();
}

CNudefinePro2Node* CNudefinePro2Node::getItem(int ndx)
{
	ASSERT(ndx >= 0 && ndx < getNodeCount());
	return vChild[ndx];
}

void CNudefinePro2Node::InsertMapNode(std::pair<CString,CNudefinePro2Node*>&_Item)
{
	mapNode.insert(_Item);
}

void CNudefinePro2Node::removeNode(const ALTObjectID& idName,int ndx)
{
	CNudefinePro2Node* pNode = NULL;
	if (ndx == 4 || idName.at(ndx).empty())
	{
		return;
	}

	CString nodeName = idName.at(ndx).c_str();
	nodeName.MakeUpper();
	pNode = getNode(nodeName);
	if (pNode)
	{
		pNode->removeNode(idName,ndx+1);
		std::vector<CNudefinePro2Node*>::iterator iter;
		iter = std::find(vChild.begin(),vChild.end(),pNode);

		std::map<CString,CNudefinePro2Node*>::iterator iter1;
		iter1 = mapNode.find(idName.at(ndx).c_str());
		if (iter != vChild.end() && pNode->getNodeCount() == 0)
		{
			delete* iter;
			vChild.erase(iter);
			mapNode.erase(iter1);
		}
		return;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////CPROCESSOR2LIST
CPROCESSOR2LIST::CPROCESSOR2LIST()
{
	
}

CPROCESSOR2LIST::~CPROCESSOR2LIST()
{
	mapUnDefineObjIndex.clear();
	mapUnDefinePro2.clear();
	for (size_t i = 0; i < vChild.size(); i++)
	{
		delete vChild[i];
	}
	vChild.clear();
	mapNode.clear();
	mapShapeName.clear();
}

void CPROCESSOR2LIST::DeleteItem( int nidx )
{
	erase(begin() + nidx);
}

CProcessor2* CPROCESSOR2LIST::at( size_type i )
{
	return std::vector<CProcessor2*>::at(i);
}

const CProcessor2* CPROCESSOR2LIST::at( size_type i ) const
{
	return std::vector<CProcessor2*>::at(i);

}

void CPROCESSOR2LIST::clear()
{
	std::vector<CProcessor2*>::clear();
}

const size_t CPROCESSOR2LIST::size() const
{
	return std::vector<CProcessor2*>::size();
}

CString CPROCESSOR2LIST::GetUnquieName(const CString& strValue)
{
	int nIndex = 1;
	CString strTemp = _T("");

	if(mapUnDefineObjIndex.find(strValue) != mapUnDefineObjIndex.end())
	{
		nIndex = mapUnDefineObjIndex[strValue];
		mapUnDefineObjIndex[strValue] = nIndex +1;
	}
	else
	{
		mapUnDefineObjIndex[strValue] = 2;
	}

	strTemp.Format(_T("%s-%d"),strValue,nIndex);
	return strTemp;
}

void CPROCESSOR2LIST::InsertUnDefinePro2(const std::pair<CString,CProcessor2*>&_Item)
{
	mapUnDefinePro2.insert(_Item);
}

CProcessor2* CPROCESSOR2LIST::GetProc2(const CString& shapeName)
{
	std::map<CString,CProcessor2*>::const_iterator iter;
	iter = mapUnDefinePro2.find(shapeName);
	if (iter != mapUnDefinePro2.end())
	{
		return iter->second;
	}
	return NULL;
}

void CPROCESSOR2LIST::setNode(const ALTObjectID& idName)
{
	CNudefinePro2Node* pNode = NULL;
	pNode = getNode(idName.at(0).c_str());
	if (pNode)
	{
		pNode->setNode(idName,1);
		return;
	}

	pNode = new CNudefinePro2Node();
	CString nodeName = idName.at(0).c_str();
	nodeName.MakeUpper();
	pNode->setNodeName(nodeName);
	addNode(pNode);
	InsertMapNode(std::make_pair(pNode->getNodeName(),pNode));
	setNode(idName);
}

void CPROCESSOR2LIST::addNode(CNudefinePro2Node* pNode)
{
	ASSERT(pNode);
	vChild.push_back(pNode);
}

CNudefinePro2Node* CPROCESSOR2LIST::getNode(const CString nodeName)
{
	if (mapNode.find(nodeName) == mapNode.end())
	{
		return NULL;
	}
	return mapNode[nodeName];
}

int CPROCESSOR2LIST::getNodeCount()
{
	return (int)vChild.size();
}

CNudefinePro2Node* CPROCESSOR2LIST::getItem(int ndx)
{
	ASSERT(ndx >= 0 && ndx < getNodeCount());
	return vChild[ndx];
}

void CPROCESSOR2LIST::InsertMapNode(std::pair<CString,CNudefinePro2Node*>&_Item)
{
	mapNode.insert(_Item);
}

CString CPROCESSOR2LIST::GetName(CString strValue)
{
	CString strShapeName = _T("");
	if (strValue.Find('-') == -1)
	{
		strShapeName.Format(_T("%s-1"),strValue);
	}
	else
	{
		strShapeName = strValue;
	}
	ALTObjectID shapeName(strShapeName);
	while(mapShapeName.find(strShapeName) != mapShapeName.end() || IsInvalid(strShapeName))
	{
		strShapeName = shapeName.GetNext().GetIDString();
		shapeName = ALTObjectID(strShapeName);
	}
	mapShapeName.insert(strShapeName);	
	return strShapeName;
}

bool CPROCESSOR2LIST::IsInvalid(CString shapeName)
{
	std::set<CString>::const_iterator iter = mapShapeName.begin();
	for (; iter != mapShapeName.end(); ++iter)
	{
		if (ALTObjectID(*iter).idFits(ALTObjectID(shapeName)))
		{
			return true;
		}
	}
	return false;
}

void CPROCESSOR2LIST::InsertShapeName(const CString& strValue)
{
	mapShapeName.insert(strValue);
}

void CPROCESSOR2LIST::removeNode(const ALTObjectID& idName)
{
	CNudefinePro2Node* pNode = NULL;
	pNode = getNode(idName.at(0).c_str());
	if (pNode)
	{
		pNode->removeNode(idName,1);
		std::vector<CNudefinePro2Node*>::iterator iter = vChild.begin();
		iter = std::find(vChild.begin(),vChild.end(),pNode);

		std::map<CString,CNudefinePro2Node*>::iterator iter1 = mapNode.begin();
		iter1 = mapNode.find(idName.at(0).c_str());
		if (iter != vChild.end() && pNode->getNodeCount() == 0)
		{
			delete* iter;
			vChild.erase(iter);
			mapNode.erase(iter1);
		}
		return;
	}
}

void CPROCESSOR2LIST::removePro2(CProcessor2* pPro2)
{
	ASSERT(pPro2);
	CString strShapeName = pPro2->ShapeName().GetIDString();
	std::map<CString,CProcessor2*>::iterator iter;
	iter = mapUnDefinePro2.find(strShapeName);
	if (iter != mapUnDefinePro2.end())
	{
		mapUnDefinePro2.erase(iter);
	}

	std::set<CString>::iterator iter1;
	iter1 = mapShapeName.find(strShapeName);
	if (iter1 != mapShapeName.end())
	{
		mapShapeName.erase(iter1);
	}

	removeNode(pPro2->ShapeName());
}

CProcessor2* CPROCESSOR2LIST::GetProc2ByGuid( const CGuid& guid ) const
{
	const_iterator ite = std::find_if(begin(), end(), boost::BOOST_BIND(&CProcessor2::getGuid, _1) == guid);
	if (ite!=end())
		return * ite;
	return NULL;
}