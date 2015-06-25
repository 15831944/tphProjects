#include "StdAfx.h"
#include "../InputAirside/VehiclePoolParking.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\VehiclePoolParking3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "glRender/glTextureResource.h"
#include "glRender/glShapeResource.h"
#include "TessellationManager.h"
#include <gl/GLUT.H>
#include "glChart/Graphics/Vector3.h"
#include "Painter.h"

//#define PARK_SPACE_MAT "ParkingLot/ParkSpace"
#define LINE_WIDTH 20

void BuildParkingSpace3D(ParkingSpace& space, int iIndex );
void BuildDrivePipe3D(ParkingDrivePipe& pipe,bool bLeftDrive, BOOL IsInEditMode);
void RenderPallaleSpace(int nLine,ParkingSpace& space ,const CPoint2008& ptCenter);
void RenderAngleSpace(int nLine,ParkingSpace& space ,const CPoint2008& ptCenter);
void DrawPath(const ARCPipe& pipe, Painter mCurPen, double texcordL=0, double texcordR = 1, double texLenScale = 1);
void DrawSquare( const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen );

CVehiclePoolParking3D::CVehiclePoolParking3D(int nID):ALTObject3D(nID)
{
	m_pObj = new VehiclePoolParking();
	//m_pDisplayProp = new VehiclePoolParkingDisplayProp();
}

CVehiclePoolParking3D::~CVehiclePoolParking3D(void)
{
}

void CVehiclePoolParking3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}

	const CPath2008 & paths = GetVehiclePoolParking()->GetPath();
	if(paths.getCount()<1)return;
	std::vector<ARCVector3> vptsbuf;
	for (int j = 0;j < paths.getCount();j++) {
		vptsbuf.push_back(paths.getPoint(j));
	}
	if(  GetDisplayProp()->m_dpShape.bOn ) {
		glNormal3i(0,0,1);
		glColor4ubv(GetDisplayProp()->m_dpShape.cColor);
		glColor3ub(255,255,0);
		CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
		if(pTexture)
			pTexture->Apply();

		GLenum genmode = GL_OBJECT_LINEAR;
		GLenum curplane = GL_OBJECT_PLANE;
		static float curcoeffs[] = {0.0001f, 0.0f, 0.0f, 0.0f};
		static float curcoefft[] = {0.00f, 0.0001f, 0.0f, 0.0f};	
		//surface display list
		glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,genmode);
		glTexGenfv(GL_S,curplane,curcoeffs);
		glEnable(GL_TEXTURE_GEN_S);	
		glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,genmode);
		glTexGenfv(GL_T,curplane,curcoefft);
		glEnable(GL_TEXTURE_GEN_T);	

		GLUtesselator *m_pTObj;	
		m_pTObj = gluNewTess();
		gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
		gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &glEnd);
		gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) &glVertex3dv);
		gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
		gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
		gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
		gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);	
		gluTessNormal(m_pTObj,0.0,0.0,1.0);
		gluTessBeginPolygon(m_pTObj, NULL);
		gluTessBeginContour(m_pTObj);	
		for(size_t i = 0 ;i<vptsbuf.size();i++)
		{
			gluTessVertex(m_pTObj,vptsbuf[i],vptsbuf[i]);	
		}

		gluTessEndContour(m_pTObj);
		gluTessEndPolygon(m_pTObj);


		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_S);
		gluDeleteTess(m_pTObj);
	}

	ParkingLotParkingSpaceList spaces = GetVehiclePoolParking()->m_parkspaces;
	for(size_t i = 0; i < spaces.m_spaces.size() ;i++)
		//BuildParkingSpace3D(spaces.m_spaces[i], i, CPoint2008(GetLocation().x,GetLocation().y,GetLocation().z));
		BuildParkingSpace3D(spaces.m_spaces[i], i);

	ParkingDrivePipeList drivepipes = GetVehiclePoolParking()->m_dirvepipes;
	for(size_t i = 0; i < drivepipes.m_pipes.size() ;i++)
		BuildDrivePipe3D(drivepipes.m_pipes[i],true,true);
}



ARCPoint3 CVehiclePoolParking3D::GetLocation( void ) const
{
	VehiclePoolParking * pStretch = (VehiclePoolParking*)m_pObj.get();
	const CPath2008& path = pStretch->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}
//void CVehiclePoolParking3D::RenderBaseWithSideLine( C3DView * pView )
//{
//	if(IsNeedSync()){
//		DoSync();
//	}	
//
//	VehiclePoolParking * pStretch = GetVehiclePoolParking();
//	if(pStretch->GetPath().getCount()<1)return;
//
//	ARCPipe pipepath (pStretch->GetPath(),pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBaseLine");
//
//	if(pTexture)
//		pTexture->Apply();
//
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//
//}

VehiclePoolParking * CVehiclePoolParking3D::GetVehiclePoolParking()
{
	return (VehiclePoolParking*) GetObject();
}


void CVehiclePoolParking3D::DrawSelect( C3DView * pView )
{
	DrawOGL(pView);
}
//
//void CVehiclePoolParking3D::RenderBase( C3DView * pView )
//{
//	VehiclePoolParking * pVehiclePoolParking = GetVehiclePoolParking();
//
//	if(!pVehiclePoolParking->GetPath().getCount())
//		return ;
//
//	ARCPipe pipepath (pVehiclePoolParking->GetPath(),pVehiclePoolParking->GetLaneNumber() * pVehiclePoolParking->GetLaneWidth());
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
//	if(pTexture)
//		pTexture->Apply();
//
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//}
//
//void CVehiclePoolParking3D::RenderMarkings( C3DView * pView )
//{
//	VehiclePoolParking * pStretch = GetStretch();
//
//	if(!pStretch->GetPath().getCount())
//		return;
//
//	ARCPipe pipepath (pStretch->GetPath(),pStretch->GetLaneNumber() * pStretch->GetLaneWidth());
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RwCenter");
//	if(pTexture)
//		pTexture->Apply();
//
//	DistanceUnit offset = 0.1;
//	int lanNum = pStretch->GetLaneNumber();
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0.5 + offset,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(0.5 - offset + lanNum,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(0.5 - offset + lanNum ,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0.5 + offset,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//
//	RenderEditPoint(pView);
//}

void CVehiclePoolParking3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetVehiclePoolParking()->m_path.DoOffset(dx,dy,dz);

	ParkingLotParkingSpaceList& spaces = GetVehiclePoolParking()->m_parkspaces;
	for(size_t i = 0; i < spaces.m_spaces.size() ;i++)
		spaces.m_spaces[i].m_ctrlPath.DoOffset(dx,dy,dz);

	ParkingDrivePipeList& drivepipes = GetVehiclePoolParking()->m_dirvepipes;
	for(size_t i = 0; i < drivepipes.m_pipes.size() ;i++)
		drivepipes.m_pipes[i].m_ctrlPath.DoOffset(dx,dy,dz);

}

void CVehiclePoolParking3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	if(!IsInEdit()) return;
	if(IsNeedSync()){
		DoSync();
	}	
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CVehiclePoolParking3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetVehiclePoolParking()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this, GetVehiclePoolParking()->m_path, i) );
	}

	ALTObject3D::DoSync();	
}


void CVehiclePoolParking3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetVehiclePoolParking()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);

	ParkingLotParkingSpaceList& spaces = GetVehiclePoolParking()->m_parkspaces;
	for(size_t i = 0; i < spaces.m_spaces.size() ;i++)
	{
		CPath2008& spacepath = spaces.m_spaces[i].m_ctrlPath;
		spacepath.DoOffset(-center[VX],-center[VY],-center[VZ]);
		spacepath.Rotate(dx);
		spacepath.DoOffset(center[VX],center[VY],center[VZ]);
	}

	ParkingDrivePipeList& drivepipes = GetVehiclePoolParking()->m_dirvepipes;
	for(size_t i = 0; i < drivepipes.m_pipes.size() ;i++)
	{
		CPath2008& pipepath = drivepipes.m_pipes[i].m_ctrlPath;
		pipepath.DoOffset(-center[VX],-center[VY],-center[VZ]);
		pipepath.Rotate(dx);
		pipepath.DoOffset(center[VX],center[VY],center[VZ]);
	}
}

void BuildParkingSpace3D(ParkingSpace& space, int iIndex )
{
	/*CPath2008 path = space.m_ctrlPath;
	double dLen  = path.GetTotalLength();
	ARCVector3 ptCenter =  path.GetDistDir(dLen/2.0);
	CPoint2008 center(ptCenter.x,ptCenter.y,ptCenter.z);*/
	CPoint2008 center(0,0,0);
	if(space.m_type == ParkingSpace::_parallel)
	{
		RenderPallaleSpace(iIndex,space,center);
	}
	else
	{
		RenderAngleSpace(iIndex,space,center);
	}
}

void BuildDrivePipe3D(ParkingDrivePipe& pipe, bool bLeftDrive, BOOL IsInEditMode)
{

	Painter painter;
	painter.UseVertexColor(ARCColor4(150,150,150,255));

	if(pipe.m_bSelected){
		painter.msMat = _T("ParkingLot/DrivePipeSelected");	
	}
	else if(!IsInEditMode){
		painter.msMat = _T("ParkingLot/DrivePipe");	
		painter.mbAutoTextCoord = true;
	}
	else{
		painter.msMat = _T("ParkingLot/DrivePipeEditMode");	
	}

	/*pObj->setRenderQueueGroup(RenderObject_Surface);
	ManualObjectUtil drawObj(pObj);
	drawObj.clear();*/	
	CPath2008& path = pipe.m_ctrlPath;
	if(path.getCount()<2)
		return;

	/*drawObj.beginTriangleList(painter);
	drawObj.drawPath(ARCPipe(drawPath,pipe.m_width),0,pipe.m_nLaneNum);
	drawObj.end();*/
	DrawPath(ARCPipe(path,pipe.m_width),painter,0,pipe.m_nLaneNum);

	//if(pipe.m_nLaneNum<=0)
	//	return;
	//if(path.getCount()<2)
	//	return;

	//double dLen  = path.GetTotalLength();
	//ARCVector3 vDirCenter =  path.GetDistDir(dLen/2.0);
	//ARCVector3 vDirL = vDirCenter.PerpendicularLCopy().SetLength(pipe.m_width/2);
	//ARCVector3 vPosCenter = path.GetDistPoint(dLen/2.0);
	//ARCVector3 vPos1 = vPosCenter - vDirL;
	//ARCVector3 vPos2 = vPosCenter + vDirL;

	//for(int i=0;i<pipe.m_nLaneNum;i++)
	//{
	//	double drat = (i+.5)/pipe.m_nLaneNum;

	//	bool bresver = false;
	//	if(pipe.m_nType==ParkingDrivePipe::_bidirectional )
	//	{
	//		if( !bLeftDrive )
	//		{
	//			bresver  = (i*2>=pipe.m_nLaneNum);
	//		}
	//		else
	//			bresver = (i*2<pipe.m_nLaneNum);
	//	}

	//	ARCVector3 vPosStart = vPos1*(1-drat) +vPos2*drat;

	//	/*CommonShapes shapesEnd(pObj);shapesEnd.msMat =PARK_DRIVEPIPE_MARK_MAT;
	//	shapesEnd.DrawSquare(vPosStart,bresver?-vDirCenter:vDirCenter,pipe.m_width/pipe.m_nLaneNum,pipe.m_width);
	//	*/
	//	DrawSquare(vPosStart,bresver?-vDirCenter:vDirCenter,pipe.m_width/pipe.m_nLaneNum,pipe.m_width);
	//}
}

void RenderPallaleSpace(int nLine,ParkingSpace& space ,const CPoint2008& ptCenter)
{
	//ColourValue c = space.m_bSelected?ColourValue::Red:ColourValue::White;
	//c.a = 0.8;
	CPath2008 ctrlPath = space.m_ctrlPath;
	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);
	double dLength = ctrlPath.GetTotalLength();
	int nSpotCount = dLength/space.m_dLength;

	ARCPath3 pathleft;	ARCPath3 pathRight;
	pathleft.reserve(nSpotCount+1);
	pathRight.reserve(nSpotCount+1);

	{//draw visible object
		/*ManualObjectUtil drawobj(pObj);	
		drawobj.clear();*/


		Painter painter;
		//painter.UseVertexColor(space.m_bSelected?ARCColor4(255,0,0,200):ARCColor4(255,255,255,200));
		painter.UseVertexColor(ARCColor4(255,255,255,255));
		//painter.msMat = PARK_SPACE_MAT;

		ARCPath3 pathCenter;
		pathCenter.resize(2);
		for(int i=0;i<=nSpotCount;i++)
		{

			double distInpath = space.m_dLength*i;
			ARCVector3 ptC = ctrlPath.GetDistPoint(distInpath);
			ARCVector3 ptDir = ctrlPath.GetDistDir(distInpath);
			ptDir = ptDir.PerpendicularLCopy();
			ptDir.SetLength(space.m_dWidth*0.5);
			pathCenter[0] = ptC + ptDir;
			pathCenter[1] = ptC- ptDir;

			DrawPath(ARCPipe(pathCenter,LINE_WIDTH),painter);

			pathleft.push_back(pathCenter[0]);
			pathRight.push_back(pathCenter[1]);

		}

		DrawPath(ARCPipe(pathleft,LINE_WIDTH),painter);
		DrawPath(ARCPipe(pathRight,LINE_WIDTH),painter);

		//draw mark
		/*glColor3d(1,1,0);
		ARCVector3 markCenter =  (pathRight[0] + pathRight[1]+pathleft[0]+pathleft[1])*.25;
		ARCVector3 dir = pathRight[1] - pathRight[0];

		ARCVector3 axis = ARCVector3::UNIT_X.cross(-dir.Normalize());
		double angle = ARCVector3::UNIT_X.dot(-dir.Normalize())/(ARCVector3::UNIT_X.Length()*-dir.Normalize().Length());
		glRotatef(angle, axis.x, axis.y, axis.z);
		glTranslatef(markCenter.x,markCenter.y,markCenter.z);
		CString tex = ARCStringConvert::toString(nLine+1);
		char *p = (LPSTR)(LPCTSTR)tex;
		glutBitmapWidth
		double wide = tex.GetLength();
		glRasterPos2d(0,0);
		while(*p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);

		glColor3d(1,1,1);
		for(int i=0;i<nSpotCount;i++)
		{
			ARCVector3 markCenter =  (pathRight[i] + pathRight[i+1]+pathleft[i]+pathleft[i+1])*0.25;
			ARCVector3 dir = pathleft[i] - pathRight[i];
			ARCVector3 axis = ARCVector3::UNIT_X.cross(-dir.Normalize());
			double angle = ARCVector3::UNIT_X.dot(-dir.Normalize())/(ARCVector3::UNIT_X.Length()*-dir.Normalize().Length());
			glRotatef(angle, axis.x, axis.y, axis.z);
			glTranslatef(markCenter.x,markCenter.y,markCenter.z);
			drawobj.drawCenterAligmentText( ARCStringConvert::toString(i+1) );
		}*/
	}


	{//draw select obj

	/*	ManualObjectUtil selObj(pSelectObj); 
		selObj.clear();
		selObj.beginTriangleList(Painter());
		selObj.drawPipe(pathleft,pathRight);
		selObj.end();*/
	}
};

void RenderAngleSpace(int nLine,ParkingSpace& space ,const CPoint2008& ptCenter)
{
	CPath2008 ctrlPath = space.m_ctrlPath;
	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);
	double dLength = ctrlPath.GetTotalLength();
	int nSpotCount = dLength/space.m_dWidth;
	double dAngle = 0;
	if(space.m_type == ParkingSpace::_angle)
		dAngle = space.m_dAngle;

	//calculate all spots
	ARCPath3 pathleft;	ARCPath3 pathRight;
	pathleft.reserve(nSpotCount+1);
	pathRight.reserve(nSpotCount+1);
	for(int i=0;i<=nSpotCount;i++)
	{
		double distInpath = space.m_dWidth*i;
		ARCVector3 ptC = ctrlPath.GetDistPoint(distInpath);
		ARCVector3 ptDir = ctrlPath.GetDistDir(distInpath);

		ARCVector2 dir(ptDir);
		dir = dir.PerpendicularLCopy();
		dir.Rotate(dAngle);
		dir.SetLength(space.m_dLength);		

		pathleft.push_back(ptC);
		pathRight.push_back(ptC + ARCVector3(dir,0));
	}

	//draw basement
	//Painter painter;
	//ManualObjectUtil drawobj(pObj);
	//drawobj.clear();
	{//draw basement
		//painter.msMat = PARK_SPACEBASE_MAT;
		//drawobj.beginTriangleList(painter);
		//drawobj.drawPipe(pathRight,pathleft);
		//drawobj.end();
	}
	if(nSpotCount>0)
	{ //draw mark object
		Painter painter;
		painter.UseVertexColor(space.m_bSelected?ARCColor4(255,0,0,200):ARCColor4(255,255,255,200));
		//painter.msMat = PARK_SPACE_MAT;

		ARCPath3 path;
		path.resize(2);
		//begin
		/*drawobj.beginTriangleList(painter);*/
		for(int i=0;i<=nSpotCount;i++)
		{
			path[0]=pathRight[i];
			path[1]=pathleft[i];
			DrawPath(ARCPipe(path,LINE_WIDTH),painter);
		}
		if (space.m_opType ==  ParkingSpace::OperationType::_backup)
		{	
			DrawPath(ARCPipe(ctrlPath,LINE_WIDTH),painter);
		}
		/*drawobj.end();*/
		//draw text
		///*drawobj.beginText(100);
		//drawobj.setColor(ARCColor4(255,255,0,255));
		//ARCVector3 markCenter =  (pathRight[0] + pathRight[1]+pathleft[0]+pathleft[1])*.25;
		//ARCVector3 dir = pathRight[1] - pathRight[0];
		//drawobj.setIdentTransform();
		//drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
		//drawobj.translate(markCenter);
		//drawobj.drawCenterAligmentText( ARCStringConvert::toString(nLine+1) );

		//drawobj.setColor(ARCColor4(255,255,255,255));
		//for(int i=0;i<nSpotCount;i++)
		//{
		//	ARCVector3 markCenter =  (pathRight[i] + pathRight[i+1])*.5;
		//	ARCVector3 dir = pathRight[i+1] - pathRight[i];
		//	drawobj.setIdentTransform();
		//	drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
		//	drawobj.translate(markCenter);
		//	drawobj.drawCenterAligmentText( ARCStringConvert::toString(i+1) );
		//}
		//drawobj.end();*/

	}
	//{//draw select object
	//	ManualObjectUtil selObj(pSelectObj); 
	//	selObj.clear();
	//	selObj.beginTriangleList(Painter());
	//	//selObj.drawPipe(pathleft,pathRight);
	//	selObj.drawPipe(pathRight,pathleft);
	//	selObj.end();
	//}
}

void DrawPath(const ARCPipe& pipe, Painter mCurPen, double texcordL, double texcordR, double texLenScale)
{
	//pre check
	const ARCPath3& leftside = pipe.m_sidePath1; 
	const ARCPath3& rightside = pipe.m_sidePath2;
	if(leftside.size()!=rightside.size())
		return;
	int nPtCount = (int)leftside.size();
	if(nPtCount<2)
		return;

	double dLenL=0;double dLenR = 0;

	Vector3 preposL(leftside[0]);
	Vector3 preposR(rightside[0]);

	if(mCurPen.mbAutoTextCoord)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	for(int i=1;i<nPtCount;++i)
	{
		Vector3 posL(leftside[i]);
		Vector3 posR(rightside[i]);
		double dLenLNext(dLenL + preposL.distance(posL));
		double dLenRNext(dLenR + preposR.distance(posR));

		// first triangle 	
		if(mCurPen.mbUseVertexColor)
		{
			ARCColor4 color =  mCurPen.color;
			glColor4f(color[0]/255.0,color[1]/255.0,color[2]/255.0,color[3]/255.0);
			//glColor4f(0.5,0.5,0.5,1);
		}
		if(mCurPen.mbAutoNorm)
		{  
			Vector3 normal((preposR-preposL).crossProduct(posL-preposL).normalisedCopy());
			glNormal3f(normal.x,normal.y,normal.z);
		}
		glVertex3f(preposL.x,preposL.y,preposL.z);
		if(mCurPen.mbAutoTextCoord){ glVertex2f(texcordL,dLenL*texLenScale); }	
		glVertex3f(preposR.x,preposR.y,preposR.z);	
		if(mCurPen.mbAutoTextCoord){ glTexCoord2f(texcordR,dLenR*texLenScale); }
		glVertex3f(posL.x,posL.y,posL.z);		
		if(mCurPen.mbAutoTextCoord){ glTexCoord2f(texcordL,dLenLNext*texLenScale); }

		//second triangle
		glVertex3f(posL.x,posL.y,posL.z);	if(mCurPen.mbAutoTextCoord){  glTexCoord2f(texcordL,dLenLNext*texLenScale); }
		glVertex3f(preposR.x,preposR.y,preposR.z);	if(mCurPen.mbAutoTextCoord){  glTexCoord2f(texcordR,dLenR*texLenScale); }
		glVertex3f(posR.x,posR.y,posR.z);	if(mCurPen.mbAutoTextCoord){  glTexCoord2f(texcordR,dLenRNext*texLenScale); }
		//
		preposL = posL;
		preposR = posR;
		dLenL = dLenRNext;
		dLenR = dLenLNext;
		//std::swap(dLen1,dLen2);
	}
	glEnd();
	if(mCurPen.mbAutoTextCoord)
		glDisable(GL_TEXTURE_2D);
}

void DrawSquare( const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen )
{
	Vector3 vDir(dir);
	Vector3 vPos(pos);
	Vector3 vSpan(vDir.crossProduct( Vector3(0, 0, 1 )));
	vSpan = vSpan.normalisedCopy()*dwidth*0.5;
	vDir = vDir.normalisedCopy()*dlen*0.5;
	Vector3 vNorm(vDir.crossProduct(vSpan));

	//mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_FAN);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f((vPos - vSpan - vDir).x,(vPos - vSpan - vDir).y,(vPos - vSpan - vDir).z);  glTexCoord2f(0,1);  
	glNormal3f(vNorm.x,vNorm.y,vNorm.z);
	glVertex3f((vPos + vSpan - vDir).x,(vPos + vSpan - vDir).y,(vPos + vSpan - vDir).z);  glTexCoord2f(1,1);
	glVertex3f((vPos + vSpan + vDir).x,(vPos + vSpan + vDir).y,(vPos + vSpan + vDir).z);  glTexCoord2f(1,0);
	glVertex3f((vPos - vSpan + vDir).x,(vPos - vSpan + vDir).y,(vPos - vSpan + vDir).z);  glTexCoord2f(0,0);
	glEnd();
}