#include "StdAfx.h"
#include ".\commonshapes.h"
#include "common/Path2008.h"
#include "./3DNodeObject.h"
#include "OgreUtil.h"
#include <common/ARCPipe.h>
using namespace Ogre;
void CommonShapes::BuildCylinder3D( const CPath2008& path, double drad)
{	
	if(path.getCount()<2)
		return;

	ManualObject* pObj3D = mpObj;
	pObj3D->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_LIST);

	const static int nSlicCount = 12;
	
	for(int i=0;i<path.getCount()-1;i++)
	{
		int nIndexStart = nSlicCount*2*i;
		CPoint2008 pt1 = path.getPoint(i);
		CPoint2008 pt2 = path.getPoint(i+1);

		Vector3 v1(pt1.x,pt1.y,pt1.z);
		Vector3 v2(pt2.x,pt2.y,pt2.z);

		Vector3 vdir = (v2 - v1);	
		vdir.normalise();
		
		Vector3 vStart = vdir.perpendicular();
		
		pObj3D->position(v1+vStart*drad);
		pObj3D->colour(mColor);
		pObj3D->normal(vStart);
		pObj3D->position(v2+vStart*drad);pObj3D->normal(vStart);
		for(int k=1;k<nSlicCount;k++)
		{
			Quaternion q;
			q.FromAngleAxis(Degree(360*k/nSlicCount),-vdir);			
			Vector3 vOffset = q*vStart;
			pObj3D->position(v1+vOffset*drad);	pObj3D->normal(vOffset);
			pObj3D->position(v2+vOffset*drad);	pObj3D->normal(vOffset);

			int idxStart  =  nIndexStart+2*k-2;
			if( k<nSlicCount-1)
				pObj3D->quad(idxStart,idxStart+1,idxStart+3,idxStart+2);
			else
				pObj3D->quad(idxStart,idxStart+1,nIndexStart+1,nIndexStart);
		}
		
	}
	
	pObj3D->end();
}


void CommonShapes::BuildTexturePipe2D( const ARCPipe& pipe ,double duTexF, double duTextT)
{
	if(pipe.m_centerPath.size()<2)
		return;

	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_STRIP);

	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		mpObj->position(prepos1);mpObj->textureCoord(duTexF,dLen1);mpObj->normal(norm);mpObj->colour(mColor);
		mpObj->position(prepos2);mpObj->textureCoord(duTextT,dLen2);

		dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();
		mpObj->position(pos1);mpObj->textureCoord(duTexF,dLen1);
		mpObj->position(pos2);mpObj->textureCoord(duTextT,dLen2);
		
		prepos1=  pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	mpObj->end();
}



void CommonShapes::DrawWallShape( const ARCPipe& pipe, double dHeight, bool bBottom /*= false*/ )
{
	if(pipe.m_centerPath.size() <2)
		return;

	int nPtCount = (int)pipe.m_centerPath.size();

	mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_LIST);
	int nVertexAdd = 0;

	Vector3 offsetHeight = Vector3(0,0,dHeight);

	Vector3 prePts[4];
	prePts[0] = OGREVECTOR(pipe.m_sidePath1.at(0));
	prePts[3] = OGREVECTOR(pipe.m_sidePath2.at(0));
	prePts[1] = prePts[0] + offsetHeight;
	prePts[2] = prePts[3] + offsetHeight;
	
	Real dLen[4];
	dLen[0] = 0;
	for(int i=1;i<4;i++)
		dLen[i] = dLen[i-1] + prePts[i].distance(prePts[i-1]);

	{//begin face
		Vector3 norm = Vector3::UNIT_Z.crossProduct(prePts[0]-prePts[3]); 
		norm.normalise();
		mpObj->position(prePts[0]);mpObj->normal(norm);
		mpObj->position(prePts[1]);mpObj->normal(norm);
		mpObj->position(prePts[2]);mpObj->normal(norm);
		mpObj->position(prePts[3]);mpObj->normal(norm);		
		mpObj->quad(0,1,2,3);
		nVertexAdd+=4;
	}

	//side face
	for(int i=1;i<nPtCount;++i)
	{
		Vector3 pts[4];
		pts[0] = OGREVECTOR(pipe.m_sidePath1.at(i));
		pts[1] = OGREVECTOR(pipe.m_sidePath2.at(i));
		pts[2] = pts[1] + offsetHeight;
		pts[3] = pts[0] + offsetHeight;
		//

		for(int j=0;j<4;j++)
		{
			int nextj = (j+1)%4;
			Vector3 norm = -(pts[nextj]-pts[j]).crossProduct(pts[j]-prePts[j]);
			norm.normalise();//face1
		
			mpObj->position(prePts[j]);mpObj->normal(norm);
			mpObj->position(pts[j]);mpObj->normal(norm);
			mpObj->position(pts[nextj]);mpObj->normal(norm);
			mpObj->position(prePts[nextj]);mpObj->normal(norm);
			
			
		}
		for(int j=0;j<4;j++)prePts[j] = pts[j];		
	}
}

void CommonShapes::DrawSquare( const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen )
{
	Vector3 vDir = OGREVECTOR(dir);
	Vector3 vPos = OGREVECTOR(pos);
	Vector3 vSpan = vDir.crossProduct(Vector3::UNIT_Z);
	vSpan = vSpan.normalisedCopy()*dwidth*0.5;
	vDir = vDir.normalisedCopy()*dlen*0.5;
	Vector3 vNorm = vDir.crossProduct(vSpan);

	mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_FAN);
		mpObj->position(vPos - vSpan - vDir);  mpObj->textureCoord(0,1); mpObj->normal(vNorm);
		mpObj->position(vPos + vSpan -vDir); mpObj->textureCoord(1,1);
		mpObj->position(vPos + vSpan +vDir);	mpObj->textureCoord(1,0);
		mpObj->position(vPos - vSpan +vDir);	mpObj->textureCoord(0,0);
	mpObj->end();
}

void CommonShapes::DrawSquareFlip( const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen )
{
	Vector3 vDir = OGREVECTOR(dir);
	Vector3 vPos = OGREVECTOR(pos);
	Vector3 vSpan = vDir.crossProduct(Vector3::UNIT_Z);
	vSpan = vSpan.normalisedCopy()*dwidth*0.5;
	vDir = vDir.normalisedCopy()*dlen*0.5;
	Vector3 vNorm = vDir.crossProduct(vSpan);

	mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_FAN);
	mpObj->position(vPos - vSpan - vDir);  mpObj->textureCoord(1,1); mpObj->normal(vNorm);
	mpObj->position(vPos + vSpan -vDir);   mpObj->textureCoord(0,1);
	mpObj->position(vPos + vSpan +vDir);	mpObj->textureCoord(0,0);
	mpObj->position(vPos - vSpan +vDir);	mpObj->textureCoord(1,0);
	mpObj->end();
}

void CommonShapes::DrawPipeArrow( const ARCPath3&path, double dwidth,double dArrowHeader )
{
	if(path.size()<2)
		return;
	ARCPipe pipe(path,dwidth);
	double dLength = path.GetLength();
	double dArrowPos = dLength - dArrowHeader;
	dArrowPos = MAX(0,dArrowPos);
	float dArrowPosIndex = path.GetDistIndex(dArrowPos);

	mpObj->begin(msMat.GetString(),RenderOperation::OT_TRIANGLE_STRIP);

	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);

	Vector3 perNorm = Vector3::ZERO;
	
	for(size_t i=1;i<dArrowPosIndex;++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();
		
		Vector3 destNorm = (norm+perNorm).normalisedCopy();
		mpObj->position(prepos1);mpObj->normal(destNorm);mpObj->colour(mColor);
		mpObj->position(prepos2);
	

		prepos1 = pos1;
		prepos2 = pos2;
		perNorm = norm;
		
	}

	//last arrow pos
	Vector3 dPosArrow = OGREVECTOR(path.GetIndexPos(dArrowPosIndex));
	Vector3 dDirArrow = OGREVECTOR(path.GetIndexDir(dArrowPosIndex)).normalisedCopy();
	Vector3 endPos = OGREVECTOR(*path.rbegin());
	Vector3 dirSpan = dDirArrow.crossProduct(Vector3::UNIT_Z);	
	Vector3 norm = dirSpan.crossProduct(dDirArrow);

	Vector3 destNorm = (norm+perNorm).normalisedCopy();
	mpObj->position(prepos1);mpObj->normal(destNorm);
	mpObj->position(prepos2);

	mpObj->position(dPosArrow-dirSpan*dwidth*0.5);
	mpObj->position(dPosArrow+dirSpan*dwidth*0.5);
	
	mpObj->position(dPosArrow-dirSpan*dwidth);
	mpObj->position(dPosArrow+dirSpan*dwidth);


	mpObj->position(endPos);
	mpObj->position(endPos);

	mpObj->end();
}

void CommonShapes::DrawPipeArrowLine( const ARCPath3&path, double dwidth, double dArrowHeader )
{
	if(path.size()<2)
		return;
	ARCPipe pipe(path,dwidth);
	double dLength = path.GetLength();
	double dArrowPos = dLength - dArrowHeader;
	dArrowPos = MAX(0,dArrowPos);
	float dArrowPosIndex = path.GetDistIndex(dArrowPos);

	std::vector<Vector3> vLeftPts;
	std::vector<Vector3> vRightPts;

	for(size_t i=0;i<dArrowPosIndex;++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);	

		vLeftPts.push_back(pos1);
		vRightPts.push_back(pos2);

	}
	//last arrow pos
	Vector3 dPosArrow = OGREVECTOR(path.GetIndexPos(dArrowPosIndex));
	Vector3 dDirArrow = OGREVECTOR(path.GetIndexDir(dArrowPosIndex)).normalisedCopy();
	Vector3 endPos = OGREVECTOR(*path.rbegin());
	Vector3 dirSpan = dDirArrow.crossProduct(Vector3::UNIT_Z);		

	vLeftPts.push_back(dPosArrow-dirSpan*dwidth*0.5);
	vRightPts.push_back(dPosArrow+dirSpan*dwidth*0.5);

	vLeftPts.push_back(dPosArrow-dirSpan*dwidth*0.75);
	vRightPts.push_back(dPosArrow+dirSpan*dwidth*0.75);

	mpObj->begin(msMat.GetString(),RenderOperation::OT_LINE_STRIP);

	mpObj->position(endPos);
	
	for(size_t i=0;i<vLeftPts.size();i++)
	{
		mpObj->position(vLeftPts[i]); mpObj->colour(mColor);
	}
	int nPtCount = (int)vRightPts.size();
	for(int i=0;i<nPtCount;i++)
	{
		mpObj->position(vRightPts[nPtCount-i-1]);
	}

	mpObj->position(endPos);



	mpObj->end();
}
//////////////////////////////////////////////////////////////////////////

