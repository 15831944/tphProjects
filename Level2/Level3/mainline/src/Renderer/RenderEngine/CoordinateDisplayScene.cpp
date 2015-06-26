#include "StdAfx.h"
#include ".\coordinatedisplayscene.h"
#include "OgreConvert.h"
#include "OgreUtil.h"
#include "ShapeBuilder.h"
#include "MaterialDef.h"
#include "ManualObjectUtil.h"

using namespace Ogre;
CCoordinateDisplayScene::CCoordinateDisplayScene(void)
{
}

CCoordinateDisplayScene::~CCoordinateDisplayScene(void)
{
}

void DrawCylinder(ManualObject* pObj, const Vector3& p1, const Vector3& p2, double dradius1, double dradius2, const OgrePainter& paint )
{
	int iSlice =10;
	Vector3 vDir = (p2-p1).normalisedCopy();
	Vector3 vBegine = vDir.perpendicular();

	pObj->begin(paint.msmat,RenderOperation::OT_TRIANGLE_LIST);
	Vector3 vDisk = vBegine;
	int nVetBegin = 0;
	pObj->position(vDisk * dradius1 + p1); pObj->normal(vDisk); if( paint.mbUseColor ) pObj->colour(paint.mColor);
	pObj->position(vDisk * dradius2 + p2); 
	for(int i=1;i<iSlice;i++){
		nVetBegin+=2;
		Ogre::Quaternion quat;
		quat.FromAngleAxis( Degree(i*360/iSlice),vDir );
		Vector3 vDisk =  quat * vBegine; 
		pObj->position(vDisk * dradius1 + p1);pObj->normal(vDisk);
		pObj->position(vDisk * dradius2 + p2);
		pObj->quad(nVetBegin-1,nVetBegin-2,nVetBegin,nVetBegin+1);	
	}
	pObj->quad(nVetBegin+1,nVetBegin,0,1);
	
	pObj->end();
}

//void DrawSphere(ManualObject* pObj, GLdouble radius, const Vector3& pos, const OgrePainter& painter)
//{
//
//}


CString camName = _T("MainCamera");
void CCoordinateDisplayScene::Setup()
{
	if(!MakeSureInited()){ ASSERT(FALSE); return; }
	
	CCameraNode localCam = GetCamera(camName);
	//LookAt(-ARCVECTOR(vDir*distToCenter),ARCVector3(0,0,0),ARCVECTOR(vUpDir) );

	CString coordEnt = _T("Coord");
	SceneNode* pNodes = AddSceneNode(coordEnt);
	pNodes->scale(25,25,25);
	//
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(coordEnt,mpScene);
	if(!pObj)return ;

	OgrePainter painter;
	painter.SetMat(VERTEXCOLOR_LIGHTON);

	//altitude
	painter.UseColor(ColourValue::Red);
	DrawCylinder(pObj,Vector3::NEGATIVE_UNIT_Z, Vector3::UNIT_Z,0.05,0.05,painter);
	DrawCylinder(pObj,Vector3::UNIT_Z,Vector3::UNIT_Z*1.5, 0.15,0,painter);
	//North-South
	painter.UseColor(ColourValue::Red+ColourValue::Green);
	DrawCylinder(pObj,Vector3::NEGATIVE_UNIT_Y,Vector3::UNIT_Y,       0.05, 0.05, painter);
	DrawCylinder(pObj,Vector3::UNIT_Y,         Vector3::UNIT_Y*1.5,  0.15, 0,painter);

	//	// East-West
	painter.UseColor(ColourValue::Blue);
	DrawCylinder(pObj,Vector3::NEGATIVE_UNIT_X,Vector3::UNIT_X,       0.05, 0.05, painter);
	DrawCylinder(pObj,Vector3::UNIT_X,         Vector3::UNIT_X*1.5,  0.15, 0,painter);

	OgreUtil::AttachMovableObject(pObj,pNodes);

	Entity *mSphere = OgreUtil::createOrRetrieveEntity("Sphere","sphere.mesh",mpScene);
	mSphere->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor3(0,0,0)));
	SceneNode *mNode = OgreUtil::createOrRetrieveSceneNode("SphereNode",mpScene);
	mNode->setScale(0.0025,0.0025,0.0025);
	pNodes->addChild(mNode);
	mNode->attachObject(mSphere);

	// altitude
	//glPushMatrix();
	//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	//glTranslatef(0.0, 0.0, -1.0);
	//gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	//gluSphere(pQuadObj, 0.05, 10, 10);
	//glTranslatef(0.0, 0.0, 2.0);
	//gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	//gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	//glPopMatrix();

	//// North-South
	//glPushMatrix();
	//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	//glRotatef(-90, 1.0, 0.0, 0.0);
	//glTranslatef(0.0, 0.0, -1.0);
	//gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	//gluSphere(pQuadObj, 0.05, 10, 10);
	//glTranslatef(0.0, 0.0, 2.0);
	//gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	//gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	//glPopMatrix();

	// East-West
	/*glPushMatrix();
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, -1.0);
	gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	gluSphere(pQuadObj, 0.05, 10, 10);
	glTranslatef(0.0, 0.0, 2.0);
	gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	glPopMatrix();

	glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
	gluSphere(pQuadObj, 0.25, 5, 5);*/
	//Entity * pEnt = OgreUtil::createOrRetrieveEntity(coordEnt,"coord.mesh",mpScene);
}

void CCoordinateDisplayScene::UpdateCamera( Ogre::Camera* pWatchCam )
{		
	Camera * pLocalCam = GetLocalCamera();
	CCameraNode camera(pLocalCam);
	Vector3 vDir = pWatchCam->getRealDirection();
	Vector3 vUpDir = pWatchCam->getRealUp();
	
	double sceneRad = 60;
	double distToCenter  = 300;//distToCenter>=sceneRad/tan(fovy/2),default fovy=30;

	camera.LookAt(-ARCVECTOR(vDir*distToCenter),ARCVector3(0,0,0),ARCVECTOR(vUpDir) );
	camera.Set2D(sceneRad*2);
	camera.SetNearFar(distToCenter-sceneRad,distToCenter+sceneRad);
}
Ogre::Camera* CCoordinateDisplayScene::GetLocalCamera()
{
	return GetCamera(camName).mpCamera;
}