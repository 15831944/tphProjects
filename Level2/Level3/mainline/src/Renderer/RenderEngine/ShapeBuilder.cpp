#include "StdAfx.h"
#include ".\shapebuilder.h"
#include <common/Path2008.h>
#include <common/ARCPipe.h>
#include <common/Pollygon2008.h>
#include "./Tesselator.h"
#include "materialdef.h"
#include "OgreConvert.h"
#include "OgreUtil.h"
#include "renderAirport3D.h"
#include "RenderRunway3D.h"
using namespace Ogre;
#define MAT_STEP_TEXTURE _T("Escalator/Steps")
#define MAT_NEWEL _T("Escalator/Newel")
#define MAT_SKIRT _T("Escalator/Skirt")
#define MAT_HANDDRAL _T("Escalator/HandDrall")

#define MAT_SIDE _T("Stair/Side")
#define MAT_STEP _T("Stair/Steps")

#define MAT_LINE _T("basic/line")
#define PROC_LINE_MAT   _T("ShapesCommon/ProcessorLineMaterial")
#define PROC_ARROW_MAT  _T("ShapesCommon/ProcessorArrowMaterial")
#define Z_OFFSET   0.1

void CShapeBuilder::DrawSteps( Ogre::ManualObject* pObj,const ARCPath3& path,double dWidth,const ARCColor3& color,bool bTex)
{

	bTex = false; //no use texture
	ColourValue c = OGRECOLOR(color);
	const static double steplen = 30;
	ARCPath3 path2d = path;
	for(size_t i=0;i<path2d.size();i++)
	{
		path2d[i].z  = 0;
	}
	//ARCPipe pipe(path,200);
	double dlen = path2d.GetLength();
	int nCount = int(dlen/steplen);
	
	Vector3 prePoint = OGREVECTOR(path.at(0));
	Vector3 preNorm = -OGREVECTOR(path2d.GetIndexDir(0)).normalisedCopy();

	int nVertexAdd =0;
	if(bTex)
		pObj->begin(MAT_STEP_TEXTURE,RenderOperation::OT_TRIANGLE_LIST);
	else
		pObj->begin(MAT_STEP,RenderOperation::OT_TRIANGLE_LIST);
	for(int i=1;i<=nCount;i++)
	{
		double ptdist = i*dlen/nCount;
		float ptindex = path2d.GetDistIndex(ptdist);
		Vector3 thisPoint = OGREVECTOR(path.GetIndexPos(ptindex));
		Vector3 thisNorm = -OGREVECTOR(path2d.GetIndexDir(ptindex)).normalisedCopy();

		Vector3 offset = thisPoint - prePoint;
		Vector3 dUpHight = Vector3(0,0,offset.z);	//get up height
		
		{
			Vector3 preExt = Vector3(-preNorm.y,preNorm.x,0)*dWidth*0.5;	
			pObj->position(prePoint-preExt);pObj->normal(preNorm); pObj->colour(c); if(bTex)pObj->textureCoord(0,0); 
			pObj->position(prePoint+preExt);pObj->normal(preNorm); pObj->colour(c); if(bTex)pObj->textureCoord(0,1);
			pObj->position(prePoint+preExt+dUpHight);pObj->normal(preNorm);  pObj->colour(c); if(bTex)pObj->textureCoord(1,1);
			pObj->position(prePoint-preExt+dUpHight);pObj->normal(preNorm);  pObj->colour(c); if(bTex)pObj->textureCoord(1,0);
			pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
			nVertexAdd+=4;
		
			Vector3 ext = Vector3(-thisNorm.y,thisNorm.x,0)*dWidth*0.5;			
			pObj->position(prePoint-preExt+dUpHight);pObj->normal(Vector3::UNIT_Z);if(bTex)pObj->textureCoord(0,0);
			pObj->position(prePoint+preExt+dUpHight);pObj->normal(Vector3::UNIT_Z);if(bTex)pObj->textureCoord(0,1);
			pObj->position(thisPoint+ext);pObj->normal(Vector3::UNIT_Z);if(bTex)pObj->textureCoord(1,1);
			pObj->position(thisPoint-ext);pObj->normal(Vector3::UNIT_Z);if(bTex)pObj->textureCoord(1,0);
			pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
			nVertexAdd+=4;
		}
		
		prePoint = thisPoint;
		preNorm = thisNorm;
	}

	pObj->end();
}

void CShapeBuilder::DrawStairSide( Ogre::ManualObject* pObj,const ARCPipe& pipe )
{
	const static double dwidth = 8;
	const static  double dHeight = 100;
	DrawWallShape(pObj,MAT_SKIRT,ARCPipe(pipe.m_sidePath1.getPath2008(),dwidth),dHeight);
	DrawWallShape(pObj,MAT_SKIRT,ARCPipe(pipe.m_sidePath2.getPath2008(),dwidth),dHeight);
}

void CShapeBuilder::DrawWallShape( Ogre::ManualObject* pObj,const CString& matName,const ARCPipe& sidePipe, double dheight )
{
	/*ARCPipe sidePipe(path.getPath2008(),dwidth);*/
	int nPtCount = (int)sidePipe.m_centerPath.size();
	if(nPtCount<2)
		return;
	
	pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_LIST);
	int nVertexAdd = 0;
    Vector3 prePts[4];
	prePts[0] = OGREVECTOR(sidePipe.m_sidePath1.at(0));
	prePts[1] = OGREVECTOR(sidePipe.m_sidePath2.at(0));
	prePts[2] = prePts[1] + Vector3::UNIT_Z*dheight;
	prePts[3] = prePts[0] + Vector3::UNIT_Z*dheight;
	{//begin face
		Vector3 norm = Vector3::UNIT_Z.crossProduct(prePts[0]-prePts[1]); 
		norm.normalise();
		pObj->position(prePts[0]);pObj->normal(norm);
		pObj->position(prePts[1]);pObj->normal(norm);
		pObj->position(prePts[2]);pObj->normal(norm);
		pObj->position(prePts[3]);pObj->normal(norm);
		pObj->quad(0,1,2,3);
		nVertexAdd+=4;
	}
	
	for(int i=1;i<nPtCount;++i)
	{
		Vector3 pts[4];
		pts[0] = OGREVECTOR(sidePipe.m_sidePath1.at(i));
		pts[1] = OGREVECTOR(sidePipe.m_sidePath2.at(i));
		pts[2] = pts[1] + Vector3::UNIT_Z*dheight;
		pts[3] = pts[0] + Vector3::UNIT_Z*dheight;
		//

		for(int j=0;j<4;j++)
		{
			int nextj = (j+1)%4;
			Vector3 norm = -(pts[nextj]-pts[j]).crossProduct(pts[j]-prePts[j]);
			norm.normalise();//face1
			pObj->position(prePts[j]);pObj->normal(norm);
			pObj->position(pts[j]);pObj->normal(norm);
			pObj->position(pts[nextj]);pObj->normal(norm);
			pObj->position(prePts[nextj]);pObj->normal(norm);
			pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
			nVertexAdd+=4;
		}
		for(int j=0;j<4;j++)prePts[j] = pts[j];		
	}
	{//end face
		Vector3 norm = -Vector3::UNIT_Z.crossProduct(prePts[0]-prePts[1]);
		norm.normalise();
		pObj->position(prePts[0]);pObj->normal(norm);
		pObj->position(prePts[3]);pObj->normal(norm);
		pObj->position(prePts[2]);pObj->normal(norm);
		pObj->position(prePts[1]);pObj->normal(norm);
		pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
	}
	pObj->end();
}

void CShapeBuilder::DrawPipe( Ogre::ManualObject* pObj,const ARCPipe& pipepath,bool bBackFace)
{
	int nPtCnt = (int)pipepath.m_centerPath.size();
	pObj->begin(MAT_STEP,RenderOperation::OT_TRIANGLE_LIST);

	int nVertexAdd = 0;
	Vector3 p1 = OGREVECTOR(pipepath.m_sidePath1[0]);
	Vector3 p2 = OGREVECTOR(pipepath.m_sidePath2[0]);
	Vector3 vNorm= Vector3::UNIT_Z;

	pObj->position(p1);
	pObj->normal(vNorm);
	pObj->position(p2);

	for(int i=1;i<nPtCnt;i++)
	{
		p1 = OGREVECTOR(pipepath.m_sidePath1[i]);
		p2 = OGREVECTOR(pipepath.m_sidePath2[i]);		

		pObj->position(p1);
		pObj->position(p2);
		pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+3,nVertexAdd+2);

		nVertexAdd+=2;
	}
	pObj->end();

}

void CShapeBuilder::DrawPipe( Ogre::ManualObject* pObj, const CPath2008& pathLeft, const CPath2008& pathRight, const OgrePainter& option )
{
	int nPtCnt = pathLeft.getCount();

	pObj->begin(option.msmat,RenderOperation::OT_TRIANGLE_LIST);

	int nVertexAdd = 0;
	Vector3 p1 = OGREVECTOR(pathLeft[0]);
	Vector3 p2 = OGREVECTOR(pathRight[0]);
	Vector3 vNorm= Vector3::UNIT_Z;

	pObj->position(p1); if(option.mbUseColor) pObj->colour(option.mColor);
	pObj->normal(vNorm);
	pObj->position(p2);

	for(int i=1;i<nPtCnt;i++)
	{
		p1 = OGREVECTOR(pathLeft[i]);
		p2 = OGREVECTOR(pathRight[i]);		

		pObj->position(p1);
		pObj->position(p2);
		pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+3,nVertexAdd+2);

		nVertexAdd+=2;

	}
	pObj->end();
}

static ARCPath3 ExtPath(const ARCPath3& path, double exLen)
{

	ARCVector3 exDirB = -path.GetIndexDir(0); exDirB.z = 0; exDirB.SetLength(exLen);
	ARCVector3 exDirE = path.GetIndexDir((int)path.size()-1).Normalize(); exDirE.z = 0; exDirE.SetLength(exLen);
	ARCVector3 exPt = *path.begin() + exDirB;
	ARCPath3 ret = path;
	ret.insert(ret.begin(),exPt);
	exPt = *path.rbegin()+exDirE;
	ret.push_back(exPt);
	return ret;
}
//
void CShapeBuilder::DrawEscalator( Ogre::ManualObject* pObj,const ARCPipe& pipepath,const ARCColor3& color )
{
	//build smooth path
	
	
	//draw skirt
	const static double skirttheight = 24;
	const static double skritwidth = 16;
	const static double extLen = 80;
	ARCVector3 vOffset(0,0,skirttheight);
	ARCPath3 extPath1 = ExtPath(pipepath.m_sidePath1,extLen);
	ARCPath3 extPath2 = ExtPath(pipepath.m_sidePath2,extLen);
	DrawWallShape(pObj,MAT_SKIRT,ARCPipe(extPath1.getPath2008(),skritwidth),skirttheight);
	DrawWallShape(pObj,MAT_SKIRT,ARCPipe(extPath2.getPath2008(),skritwidth),skirttheight);	
	//draw newel
	DrawEscalatorNewewl(pObj,extPath1.DoOffset(vOffset),color);
	DrawEscalatorNewewl(pObj,extPath2.DoOffset(vOffset),color);
	
	//draw steps
	DrawSteps(pObj,pipepath.m_centerPath,pipepath.m_dWidth-skritwidth,ARCColor3(0.1,0.1,0.1));
	
}

void CShapeBuilder::DrawStair( Ogre::ManualObject* pObj,const ARCColor3& color,const ARCPipe& pipe )
{
	DrawSteps(pObj,pipe.m_centerPath,pipe.m_dWidth,color);
	DrawStairSide(pObj,pipe);
}
typedef std::vector<Vector3> Vector3List;

static void DrawThickBelt(Ogre::ManualObject* pObj,const Vector3List& path, const Vector3List& ext, double dthick,double dwidth, const CString& matName,const ColourValue& color);
void CShapeBuilder::DrawEscalatorNewewl( Ogre::ManualObject* pObj,const ARCPath3& path,const ARCColor3& color)
{
	if(path.size()<2)
		return;

	ColourValue newewlColor = OGRECOLOR(color,0.4f);
	const float dRad = 100/2.0f;
	const Vector3 vHight = Vector3::UNIT_Z*dRad;
	const int nRadCount = 8;
	Vector3List vBeltePts;
	Vector3List Updir;

	Vector3 vDir = OGREVECTOR(path.GetIndexDir(0));
	Vector3 norm = vDir.crossProduct(-Vector3::UNIT_Z).normalisedCopy();

	int nVertexAdd=0;
	pObj->begin(MAT_NEWEL,RenderOperation::OT_TRIANGLE_LIST);
	//draw begin
	Vector3 vPosB = OGREVECTOR(*path.begin());    vBeltePts.push_back(vPosB);Updir.push_back(-Vector3::UNIT_Z);
	Vector3 vCenterB =  vPosB+vHight;
	Vector3 vPosBH = vCenterB+ vHight;
	pObj->position(vCenterB);pObj->normal(norm);  pObj->colour(newewlColor);nVertexAdd++;
	pObj->position(vPosB);nVertexAdd++;
	for(int i=1;i<nRadCount;i++)
	{
		Vector3 v = Quaternion(Degree(i*180.0f/nRadCount),norm)*(-Vector3::UNIT_Z);
		Vector3 vPt = v*dRad+vCenterB;
		pObj->position(vPt);nVertexAdd++;   vBeltePts.push_back(vPt); Updir.push_back(v);
		pObj->triangle(0,nVertexAdd-2,nVertexAdd-1);
	}
	pObj->position(vPosBH);nVertexAdd++;  vBeltePts.push_back(vPosBH);Updir.push_back(Vector3::UNIT_Z);
	pObj->triangle(0,nVertexAdd-2,nVertexAdd-1);	
	//draw mid
	Vector3 prePos1 = vPosB;
	Vector3 prePos2 = vPosBH;
	
	for(int i=1;i<(int)path.size();i++)
	{
		Vector3 thisPos1 = OGREVECTOR(path[i]);
		Vector3 thisPos2 = thisPos1+vHight*2.0f;
		vDir = (thisPos1-prePos1);
		norm = vDir.crossProduct(-Vector3::UNIT_Z).normalisedCopy();
		pObj->position(prePos1);pObj->normal(norm);  
		pObj->position(prePos2);pObj->normal(norm);   
		pObj->position(thisPos1);pObj->normal(norm);
		pObj->position(thisPos2);pObj->normal(norm); 	vBeltePts.push_back(thisPos2);Updir.push_back(Vector3::UNIT_Z);
		pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+3,nVertexAdd+2);
		nVertexAdd+=4;
		prePos1 = thisPos1;
		prePos2 = thisPos2;
	}
	//draw end
	Vector3 vPosE = prePos1;
	Vector3 vPosEH = prePos2;
	Vector3 vCenterE = (vPosE+vPosEH)*0.5f;
	int nCenterIdx = nVertexAdd;
	pObj->position(vCenterE);pObj->normal(norm);nVertexAdd++;
	pObj->position(vPosEH);nVertexAdd++;
	for( i=1;i<nRadCount;i++)
	{
		Vector3 v = Quaternion(Degree(i*180.0f/nRadCount),norm)*(Vector3::UNIT_Z);
		Vector3 vPt = v*dRad+vCenterE;
		pObj->position(vPt);nVertexAdd++;		vBeltePts.push_back(vPt); Updir.push_back(v);
		pObj->triangle(nCenterIdx,nVertexAdd-2,nVertexAdd-1); 
	}
	pObj->position(vPosE);nVertexAdd++;					vBeltePts.push_back(vPosE);Updir.push_back(-Vector3::UNIT_Z);
	pObj->triangle(nCenterIdx,nVertexAdd-2,nVertexAdd-1);	

	pObj->end();

	//draw 
	DrawThickBelt(pObj,vBeltePts,Updir,3,9,MAT_HANDDRAL,ColourValue::Black);
	

}

void CShapeBuilder::DrawArea( Ogre::ManualObject* pObj,CString matName, const CPath2008& path,const ARCColor3& color )
{
	CPollygon2008  poly;
	poly.init(path.getCount(),path.getPointList());
	CTesselator tess;
	ARCVectorListList vlist; 
	vlist.push_back(CTesselator::GetAntiClockVectorList(poly));
	tess.MakeMeshPositive(vlist,Vector3::UNIT_Z);
	tess.BuildMeshToObject(pObj,matName.GetString(),Vector3::UNIT_Z,OGRECOLOR(color));
}

void CShapeBuilder::DrawPath( Ogre::ManualObject* pObj,CString matName, const CPath2008& path,const ARCColor3& color )
{
	ColourValue c = OGRECOLOR(color);
	/*pObj->begin(matName.GetString(),RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<path.getCount();i++)
	{
		CPoint2008 pt = path.getPoint(i);
		pObj->position(pt.getX(),pt.getY(),pt.getZ());
		pObj->colour(c);
	}
	pObj->end();*/
	///////////////////////////////////////////////
	ARCPipe pipe(path,20);
	if(pipe.m_centerPath.size()<2)
		return;

	pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_LIST);	
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	unsigned point_index = 0;
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		//Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->colour(c);//pObj->normal(norm);
		pObj->position(prepos2);

		pObj->position(pos1);
		pObj->position(pos2);

		pObj->triangle(point_index,point_index+1,point_index+2);
		pObj->triangle(point_index+3,point_index+2,point_index+1);
		point_index+=4;
			
		prepos1=  pos1;
		prepos2 = pos2;
	}
	pObj->end();
}

OgrePainter::OgrePainter()
:mbBackFace(false)
,mbTexture(false)
,mbUseColor(false)
,mpTexMap(NULL)
{

}


void CShapeBuilder::DrawPipeOutLine( Ogre::ManualObject* pObj,const ARCPipe& pipepath,const OgrePainter& option )
{
	pObj->begin(option.msmat,RenderOperation::OT_LINE_STRIP);

	const ColourValue& c = option.mColor;

	for(size_t i=0;i<pipepath.m_sidePath1.size();i++)
	{
		const ARCVector3& v1 = pipepath.m_sidePath1[i];
		pObj->position(v1.x,v1.y,v1.z);	
		if(option.mbUseColor){ 	pObj->colour(c); }
	}

	for(int i=(int)pipepath.m_sidePath2.size()-1;i>=0;i--)
	{
		const ARCVector3& v1 = pipepath.m_sidePath2[i];
		pObj->position(v1.x,v1.y,v1.z);
		if(option.mbUseColor){ 	pObj->colour(c); }
	}
	if(!pipepath.m_sidePath1.empty()){
		const ARCVector3& v1 = pipepath.m_sidePath1[0];
		pObj->position(v1.x,v1.y,v1.z);
		if(option.mbUseColor){ 	pObj->colour(c); }
	}
	pObj->end();
}

void CShapeBuilder::DrawArrowPath( Ogre::ManualObject* pObj, const Ogre::Vector3& ptFrom, const Ogre::Vector3& ptTo, const OgrePainter& option,double dLength /*= 40.0*/, double dWidth /*= 40.0*/ )
{
	const double dx = dWidth/2;
	const double dy = dLength;

	Vector3 l=  ptTo - ptFrom;
	l.normalise();
	Vector3 ptCenter = ptTo - l*dy;

	Vector3 l_norm = Vector3(-l.y, l.x, 0.0);
	//l_norm.length(dx);
	Vector3 pt2 = ptCenter + l_norm * dx;
	Vector3 pt3 = ptCenter - l_norm * dx;
	Vector3 zoffset = Vector3::UNIT_Z * Z_OFFSET;

	pObj->begin(option.msmat, RenderOperation::OT_LINE_LIST);
	pObj->position(ptTo + zoffset);	if(option.mbUseColor)pObj->colour(option.mColor);
	pObj->position(pt2+ zoffset);
	pObj->position(ptTo + zoffset);
	pObj->position(pt3 + zoffset);
	pObj->position(ptFrom  +zoffset);
	pObj->position(ptTo + zoffset);
	pObj->end();


}



void DrawThickBelt( Ogre::ManualObject* pObj,const Vector3List& path, const Vector3List& Updir, double dthick,double dwidth,const CString& matName, const ColourValue& c )
{
	int nPtCount = (int)path.size();
	const float dHWidth = dwidth*0.5f;	

	pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_LIST);
	int nVertexAdd = 0;
	Vector3 vPreCenter = path.at(0);
	Vector3 vDir = (path.at(1)-vPreCenter).normalisedCopy();
	Vector3 vUpExt = Updir.at(0);
	Vector3 vext = vUpExt.crossProduct(vDir).normalisedCopy();

	Vector3 prePts[4];
	prePts[0] = vPreCenter+vext*dHWidth;
	prePts[1] = vPreCenter-vext*dHWidth;
	prePts[2] = prePts[1] + vUpExt*dthick;
	prePts[3] = prePts[0] + vUpExt*dthick;
	{//begin face		
		Vector3 norm = -vDir;
		pObj->position(prePts[0]);pObj->normal(norm);pObj->colour(c);
		pObj->position(prePts[1]);pObj->normal(norm);pObj->colour(c);
		pObj->position(prePts[2]);pObj->normal(norm);pObj->colour(c);
		pObj->position(prePts[3]);pObj->normal(norm);pObj->colour(c);
		pObj->quad(0,1,2,3);
		nVertexAdd+=4;
	}

	for(int i=1;i<nPtCount;++i)
	{
		Vector3 vCenter  = path.at(i);
		vDir = (vCenter-vPreCenter).normalisedCopy();
		vUpExt = Updir.at(i);
		vext = vUpExt.crossProduct(vDir).normalisedCopy();

		Vector3 pts[4];
		pts[0] = vCenter+vext*dHWidth;
		pts[1] = vCenter-vext*dHWidth;
		pts[2] = pts[1] + vUpExt*dthick;
		pts[3] = pts[0] + vUpExt*dthick;
		//

		for(int j=0;j<4;j++)
		{
			int nextj = (j+1)%4;
			Vector3 norm = -(pts[nextj]-pts[j]).crossProduct(pts[j]-prePts[j]);
			norm.normalise();//face1
			pObj->position(prePts[j]);pObj->normal(norm);
			pObj->position(pts[j]);pObj->normal(norm);
			pObj->position(pts[nextj]);pObj->normal(norm);
			pObj->position(prePts[nextj]);pObj->normal(norm);
			pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
			nVertexAdd+=4;
		}
		for(int j=0;j<4;j++)prePts[j] = pts[j];		
		vPreCenter = vCenter;
	}
	{//end face		
		Vector3 norm = vDir;		
		pObj->position(prePts[0]);pObj->normal(norm);
		pObj->position(prePts[3]);pObj->normal(norm);
		pObj->position(prePts[2]);pObj->normal(norm);
		pObj->position(prePts[1]);pObj->normal(norm);
		pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+2,nVertexAdd+3);
	}
	pObj->end();
}
//manual object
//void ManualObjectEx::DrawSquareLine( double dwidth, double dLength, const ARCColor3& color )
//{
//	if(!m_pObject)
//		return;
//
//	Ogre::Real hdwidth = dwidth*0.5;
//	Ogre::Real hdLength = dLength*0.5;
//	Ogre::ColourValue c = OGRECOLOR(color);
//	Vector3  pts[4];
//	pts[0] = m_Transform.GetTranfMatix()*Vector3(-hdwidth,-hdLength,0);
//	pts[1] = m_Transform.GetTranfMatix()*Vector3(hdwidth,-hdLength,0);
//	pts[2] = m_Transform.GetTranfMatix()*Vector3(hdwidth,hdLength,0);
//	pts[3] = m_Transform.GetTranfMatix()*Vector3(-hdwidth,hdLength,0);
//
//	m_pObject->begin(MAT_LINE,RenderOperation::OT_LINE_STRIP);
//		m_pObject->position(pts[0]);m_pObject->colour(c);
//		m_pObject->position(pts[1]);
//		m_pObject->position(pts[2]);
//		m_pObject->position(pts[3]);
//		m_pObject->position(pts[0]);
//	m_pObject->end();
//}



//////////////////////////////////////////////////////////////////////////

void ProcessorBuilder::DrawPath(ManualObject* pObj, const Path* path, const ColourValue& clr,double dAlt  , bool bLoop)
{
	if (NULL == path)
		return;

	int c = path->getCount();
	if (c<=0)
	{
		// 		ASSERT(FALSE);
	}
	else if (c == 1)
	{
		static const double delta = 10.0;
		Point pt = path->getPoint(0);
		pObj->begin(PROC_LINE_MAT, RenderOperation::OT_LINE_STRIP);
		pObj->position(pt.getX()-delta, pt.getY()-delta, dAlt);
		pObj->colour(clr);
		pObj->position(pt.getX()-delta, pt.getY()+delta, dAlt);
		pObj->position(pt.getX()+delta, pt.getY()+delta, dAlt);
		pObj->position(pt.getX()+delta, pt.getY()-delta, dAlt);
		pObj->position(pt.getX()-delta, pt.getY()-delta, dAlt);
		pObj->end();
	}
	else
	{
		pObj->begin(PROC_LINE_MAT, RenderOperation::OT_LINE_STRIP);
		Point* pts = path->getPointList();
		for (int i=0;i<c;i++)
		{
			pObj->position(pts[i].getX(), pts[i].getY(), dAlt);
			pObj->colour(clr);
		}
		if (bLoop)
			pObj->position(pts[0].getX(), pts[0].getY(), dAlt);
		pObj->end();
	}

}

void ProcessorBuilder::DrawArrow( Ogre::ManualObject* pObj, const Point& ptFrom, const Point& ptTo, const Ogre::ColourValue& clr,
								 double dLength /*= 40.0*/, double dWidth /*= 40.0*/ )
{
	const double dx = dWidth/2;
	const double dy = dLength;
	Point l(ptFrom, ptTo);
	l.length(dy);
	Point ptCenter = ptTo - l;

	Point l_norm(-l.getY(), l.getX(), 0.0);
	l_norm.length(dx);
	Point pt2 = ptCenter + l_norm;
	Point pt3 = ptCenter - l_norm;

	pObj->begin(PROC_ARROW_MAT, RenderOperation::OT_TRIANGLE_FAN);
	pObj->position(ptTo.getX(), ptTo.getY(), Z_OFFSET);
	pObj->colour(clr);
	pObj->position(pt2.getX(), pt2.getY(), Z_OFFSET);
	pObj->position(pt3.getX(), pt3.getY(), Z_OFFSET);
	pObj->end();
}
void ProcessorBuilder::DrawHeadArrow(ManualObject* pObj, const Path* path, const ColourValue& clr)
{
	int c = path->getCount();
	if(path->getCount() >= 2)
	{
		DrawArrow(pObj, path->getPoint(c-2), path->getPoint(c-1), clr);
	}
}

void ProcessorBuilder::DrawTailArrow(ManualObject* pObj, const Path* path, const ColourValue& clr)
{
	if(path->getCount() >= 2)
	{
		DrawArrow(pObj, path->getPoint(1), path->getPoint(0), clr);
	}
}

void ProcessorBuilder::DrawArea( Ogre::ManualObject* pObj, const Path* path,const Ogre::String& mat,const Ogre::ColourValue& clr )
{
	CTesselator tess;
	if( tess.MakeMeshPositive(path) )
	{
		tess.BuildMeshToObject(pObj,mat,Vector3::UNIT_Z,clr );
	}
}
void ProcessorBuilder::DrawColorPath( Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, const ARCPath3&path, double dWidth, bool bLoop, double dZOffset )
{
	DrawColorPath(pObj, clr, Ogre::OgreUtil::DefaultColor, path, dWidth, bLoop, dZOffset);
}

void ProcessorBuilder::DrawColorPath( Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, Ogre::OgreUtil::ColorTemplateType ctt, const ARCPath3&path, double dWidth, bool bLoop, double dZOffset )
{
	if (path.size()<=1)
		return;

	ARCPath3 path2 = path;
	path2.DoOffset(ARCVector3(0.0, 0.0, dZOffset));

	ARCPipe pipe(path2,dWidth, bLoop);
	MaterialPtr clrMat = OgreUtil::createOrRetrieveColorMaterialFromTemplate(OgreConvert::GetColor(clr), ctt);

	DrawColorPath(pObj, pipe, clrMat->getName().c_str());
}

void ProcessorBuilder::DrawColorPath( Ogre::ManualObject* pObj, const ARCPipe& pipe, CString matName )
{
	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_STRIP);
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_centerPath.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->textureCoord(0,dLen1);/*pObj->colour(clr);*/pObj->normal(norm);
		pObj->position(prepos2);pObj->textureCoord(1,dLen2);

		dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();
		pObj->position(pos1);pObj->textureCoord(0,dLen1);
		pObj->position(pos2);pObj->textureCoord(1,dLen2);

		//pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);

		//nVexCnt+=4;
		prepos1 = pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	pObj->end();
}


void ProcessorBuilder::DrawColorPath( Ogre::ManualObject* pObj, const ARCPipe& pipe, CString matName, Ogre::ColourValue color )
{
	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_STRIP);
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_centerPath.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->textureCoord(0,dLen1);pObj->normal(norm);pObj->colour(color);
		pObj->position(prepos2);pObj->textureCoord(1,dLen2);
		
		dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();
		pObj->position(pos1);pObj->textureCoord(0,dLen1);
		pObj->position(pos2);pObj->textureCoord(1,dLen2);

		//pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);

		//nVexCnt+=4;
		prepos1 = pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	pObj->end();
}


void ProcessorBuilder::DrawColorCircle( Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, const ARCVector3& center, double dMaxRadius, double dMinRadius, double dHeight )
{
	DrawColorCircle(pObj, clr, Ogre::OgreUtil::DefaultColor, center, dMaxRadius, dMinRadius, dHeight);
}

void ProcessorBuilder::DrawColorCircle( Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, Ogre::OgreUtil::ColorTemplateType ctt, const ARCVector3& center, double dMaxRadius, double dMinRadius, double dHeight )
{
	if (dMaxRadius < dMinRadius)
		std::swap(dMaxRadius, dMinRadius);

	double dWidth = dMaxRadius - dMinRadius;
	double dRadius = (dMaxRadius + dMinRadius)/2;

	ARCPath3 drawPath;
	const int nDividCnt = 50;
	for(int i=0;i<nDividCnt;i++)
	{
		double drad = ARCMath::DegreesToRadians(i*360/nDividCnt);
		drawPath.push_back(ARCVector3(center.x + dRadius*cos(drad), center.y + dRadius*sin(drad), 0.1));
	}
	DrawColorPath(pObj, clr, ctt, drawPath, dWidth, true, dHeight);
}


void ProcessorBuilder::DrawTexturePath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth, double dSideHeight, bool bLoop/* = false*/ )
{
	if(path.size()<1)return;

	ARCPipe pipe(path, dWidth, bLoop);
	DrawTexturePath(pObj, texmat, clr, pipe, dSideHeight);
}

void ProcessorBuilder::DrawTexturePath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path, double dStartWidth, double dEndWidth, double dSideHeight )
{
	if(path.size()<1)return;

	ARCPipe pipe(path, dStartWidth, dEndWidth);
	DrawTexturePath(pObj, texmat, clr, pipe, dSideHeight);
}

void ProcessorBuilder::DrawTexturePath( Ogre::ManualObject* pObj, const Ogre::String& texmat, const Ogre::ColourValue& clr, const ARCPipe& pipe, double dSideHeight )
{
	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	pObj->begin(texmat,RenderOperation::OT_TRIANGLE_STRIP);
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->textureCoord(0,dLen1);pObj->colour(clr);pObj->normal(norm);
		pObj->position(prepos2);pObj->textureCoord(1,dLen2);

		dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();
		pObj->position(pos1);pObj->textureCoord(0,dLen1);
		pObj->position(pos2);pObj->textureCoord(1,dLen2);

		//pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);

		//nVexCnt+=4;
		prepos1=  pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	pObj->end();


	Vector3 heightV(0,0,dSideHeight);
	int nVexCnt = 0;
	pObj->begin(VERTEXCOLOR_LIGHTON_NOCULL,RenderOperation::OT_TRIANGLE_LIST);//side1
	prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);

		Vector3 norm = -(pos1-prepos1).crossProduct(heightV).normalisedCopy();
		pObj->position(prepos1);pObj->colour(clr);pObj->normal(norm);
		pObj->position(prepos1+heightV);
		pObj->position(pos1);
		pObj->position(pos1+heightV);	
		pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);
		nVexCnt+=4;

		norm = (pos2-prepos2).crossProduct(heightV).normalisedCopy();
		pObj->position(prepos2);pObj->colour(clr);pObj->normal(norm);
		pObj->position(prepos2+heightV);
		pObj->position(pos2);
		pObj->position(pos2+heightV);	
		pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);
		nVexCnt+=4;

		prepos1=  pos1;
		prepos2 = pos2;		
	}
	pObj->end();
}

void ProcessorBuilder::DrawTextureRepeatedPath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3& path ,double dWidth,
											   double dTexLeft, double dTexRight, bool bLoop /*= false */ )
{
	if(path.size()<1)return;

	ARCPipe pipe(path,dWidth, bLoop);

	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	pObj->begin(texmat,RenderOperation::OT_TRIANGLE_STRIP);
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->textureCoord(dTexLeft,dLen1);pObj->colour(clr);pObj->normal(norm);
		pObj->position(prepos2);pObj->textureCoord(dTexRight,dLen2);

		dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();
		pObj->position(pos1);pObj->textureCoord(dTexLeft,dLen1);
		pObj->position(pos2);pObj->textureCoord(dTexRight,dLen2);

		//pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);

		//nVexCnt+=4;
		prepos1=  pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	pObj->end();
}

void ProcessorBuilder::DrawTextureCircle( Ogre::ManualObject* pObj, const Ogre::String& texmat,const Ogre::ColourValue& clr,
										 const ARCVector3& center, double dMaxRadius, double dMinRadius, double dSideHeight )
{
	if (dMaxRadius < dMinRadius)
		std::swap(dMaxRadius, dMinRadius);

	double dWidth = dMaxRadius - dMinRadius;
	double dRadius = (dMaxRadius + dMinRadius)/2;

	ARCPath3 drawPath;
	const int nDividCnt = 50;
	for(int i=1;i<nDividCnt;i++)
	{
		double drad = ARCMath::DegreesToRadians(i*360/nDividCnt);
		drawPath.push_back(ARCVector3(cos(drad),sin(drad), 0.0));
	}

	DrawTexturePath(pObj, texmat, clr, drawPath, dWidth, dSideHeight);
}

void ProcessorBuilder::DrawTextureClips( Ogre::ManualObject* pObj,const TextureClips& texclips,const Ogre::ColourValue& clr, double dHeight )
{
	for(size_t i=0;i<texclips.size();i++)
	{
		const TextureClip& clip  = texclips[i];
		pObj->begin(clip.texture_name,RenderOperation::OT_TRIANGLE_FAN);
		ASSERT(clip.vertex_coords.size() == 4);
		for(size_t j=0;j<clip.vertex_coords.size();j++)
		{
			pObj->position(clip.vertex_coords[j][VX],clip.vertex_coords[j][VY], dHeight);
			pObj->textureCoord(clip.texture_coords[j][VX],clip.texture_coords[j][VY]);
			pObj->normal(Vector3::UNIT_Z);
		}
		pObj->end();
	}
}

//inline
//static void RenderTexturedPath( Ogre::ManualObject* pObj, const String& mat,
//							   const Path* pPath,		//centerline
//							   double dWidth,			//width of the path
//							   double dHeight,		//height of "handrails"
//							   const ARCColor3& cColor,//color							   
//							   double dTextureOffset//texture offset (for animated textures)
//							   	//floor altitudes
//							   ) //the active floor (only needed if bFadeFromActiveFloor is true)	
//{
//	int c = pPath->getCount();
//	if(c >= 2) {
//
//		double dHalfWidth = dWidth/2.0;
//
//		Point* pts = pPath->getPointList();
//
//		std::vector<Point> vBisect1;
//		std::vector<Point> vBisect2;
//		std::vector<double> vBisectOffsets;
//		vBisect1.reserve(c);
//		vBisect2.reserve(c);
//		vBisectOffsets.reserve(c-1);
//
//		//calculate centerline point altitudes
//		std::vector<double> vPtAlts;
//		vPtAlts.reserve(c);
//		for(int j=0; j<c; j++) {
//			double dFloorIdx, dFraction;
//			dFraction = modf( pts[j].getZ()/SCALE_FACTOR, &dFloorIdx );
//			int nFloorIdx = static_cast<int>(dFloorIdx);
//			vPtAlts.push_back(
//				dFloorAlts[nFloorIdx] + dFraction*( dFloorAlts[nFloorIdx+1] - dFloorAlts[nFloorIdx] ) );
//		}
//
//		//if fading is enabled, calculate alpha vals
//		std::vector<COLORUNIT> vPtAlpha;
//		if(bFadeFromActiveFloor) {
//			vPtAlpha.reserve(c);
//			for(int j=0; j<c; j++) {
//				double dFraction = 
//					1.0 - min( 1.0, fabs( (pts[j].getZ()/SCALE_FACTOR) - static_cast<double>(nActiveFloor) ) );
//				vPtAlpha.push_back( static_cast<COLORUNIT>( dFraction * 255.0 ) );
//			}
//		}
//
//		//calculate the bisect line for the first segment
//		{
//			Point c0(pts[0], pts[1]);
//			Point p0 = c0.perpendicular();
//			p0.length( dHalfWidth );
//			vBisect1.push_back(p0 + pts[0]);
//			p0 = c0.perpendicularY();
//			p0.length( dHalfWidth );
//			vBisect2.push_back(p0 + pts[0]);
//			vBisectOffsets.push_back(0.0);
//		}
//
//		//calculate bisect lines for middle segements
//		for(j=1; j<c-1; j++) {
//			Point c0(pts[j-1], pts[j]);
//			Point c1(pts[j], pts[j+1]);
//
//			Point p0 = c0.perpendicular();
//			p0.length( 1.0 );
//			Point p1 = c1.perpendicular();
//			p1.length( 1.0 );
//			p1+=p0;
//			p1*=0.5; //avg
//
//			//find angle between bisect line and c1
//			double dCosVal = p1.GetCosOfTwoVector( c1 );
//			double dAngle = acos( dCosVal );
//			double dAdjustedWidth = dHalfWidth / sin( dAngle );
//			vBisectOffsets.push_back(0.01 * dHalfWidth / tan( dAngle ));
//			p1.length( dAdjustedWidth );
//			vBisect1.push_back(pts[j] + p1);
//
//			p0 = c0.perpendicularY();
//			p0.length( 1.0 );
//			p1 = c1.perpendicularY();
//			p1.length( 1.0 );
//			p1+=p0;
//			p1*=0.5;
//			p1.length( dAdjustedWidth );
//			vBisect2.push_back(pts[j] + p1);
//		}
//
//		//calculate the bisect line for the last segment	 
//		{
//			Point c0(pts[c-2], pts[c-1]);
//			Point p0 = c0.perpendicular();
//			p0.length( dHalfWidth );
//			vBisect1.push_back(p0 + pts[c-1]);
//			p0 = c0.perpendicularY();
//			p0.length( dHalfWidth );
//			vBisect2.push_back(p0 + pts[c-1]);
//		}
//
//		//now draw
//		if(bFadeFromActiveFloor) {
//			glLineWidth(2.0);
//			glBegin(GL_LINE_LOOP);
//			glNormal3f(0.0,0.0,1.0);
//			for(j=0; j<c; j++) {
//				glColor4ub(msoutlinecolor[RED], msoutlinecolor[GREEN], msoutlinecolor[BLUE], vPtAlpha[j]);
//				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
//			}
//			for(j=c-1; j>=0; j--) {
//				glColor4ub(msoutlinecolor[RED], msoutlinecolor[GREEN], msoutlinecolor[BLUE], vPtAlpha[j]);
//				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
//			}
//			glEnd();
//			glLineWidth(1.0);
//			glEnable(GL_TEXTURE_2D);
//			glBindTexture( GL_TEXTURE_2D, nTextureID);
//			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//
//			glBegin(GL_QUADS);
//			glNormal3f(0.0,0.0,1.0);
//
//			double dPrevLen1=0.0, dPrevLen2=0.0, dPrevLenC=dTextureOffset;
//			for(j=0; j<c-1; j++) {
//				dPrevLen1 = dPrevLenC+vBisectOffsets[j];
//				dPrevLen2 = dPrevLenC-vBisectOffsets[j];
//				double dLen1 = Point(vBisect1[j], vBisect1[j+1]).length()/100.0;
//				double dLen2 = Point(vBisect2[j], vBisect2[j+1]).length()/100.0;
//				double dLenC = Point(pts[j], pts[j+1]).length()/100.0;
//
//				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j]);
//				glTexCoord2d(0.0,dPrevLen1);
//				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
//
//				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j+1]);
//				glTexCoord2d(0.0,dPrevLen1+dLen1);
//				glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
//				glTexCoord2d(1.0,dPrevLen2+dLen2);
//				glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);
//
//				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j]);
//				glTexCoord2d(1.0,dPrevLen2);
//				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
//
//				dPrevLenC += dLenC;
//			}
//
//			glEnd();
//			glDisable(GL_TEXTURE_2D);
//		}
//		else {
//			glColor3ubv(msoutlinecolor);//grey
//			glLineWidth(2.0);
//			glBegin(GL_LINE_LOOP);
//			glNormal3f(0.0,0.0,1.0);
//			for(j=0; j<c; j++) {
//				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
//			}
//			for(j=c-1; j>=0; j--) {
//				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
//			}
//			glEnd();
//			glLineWidth(1.0);
//			glEnable(GL_TEXTURE_2D);
//			glBindTexture( GL_TEXTURE_2D, nTextureID);
//			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//
//			glColor3ubv(cColor);
//
//			glBegin(GL_QUADS);
//			glNormal3f(0.0,0.0,1.0);
//
//			double dPrevLen1=0.0, dPrevLen2=0.0, dPrevLenC=dTextureOffset;
//			for(j=0; j<c-1; j++) {
//				dPrevLen1 = dPrevLenC+vBisectOffsets[j];
//				dPrevLen2 = dPrevLenC-vBisectOffsets[j];
//				double dLen1 = Point(vBisect1[j], vBisect1[j+1]).length()/100.0;
//				double dLen2 = Point(vBisect2[j], vBisect2[j+1]).length()/100.0;
//				double dLenC = Point(pts[j], pts[j+1]).length()/100.0;
//
//				glTexCoord2d(0.0,dPrevLen1);
//				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
//				glTexCoord2d(0.0,dPrevLen1+dLen1);
//				glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
//				glTexCoord2d(1.0,dPrevLen2+dLen2);
//				glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);
//				glTexCoord2d(1.0,dPrevLen2);
//				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
//
//				dPrevLenC += dLenC;
//			}
//
//			glEnd();
//			glDisable(GL_TEXTURE_2D);
//
//			if(dHeight > 0.1) {
//				glBegin(GL_QUADS);
//				glColor3ubv(cColor);
//				for(j=0; j<c-1; j++) {
//					glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
//					glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
//					glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]+dHeight);
//					glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]+dHeight);
//
//					glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
//					glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);
//					glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]+dHeight);
//					glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]+dHeight);
//				}
//				glEnd();
//			}
//		}
//	}
//}
//
//
