#include "StdAfx.h"
#include ".\grid3d.h"
#include "OgreConvert.h"
#include "3DScene.h"
#include <common\Grid.h>
#include <common/util.h>
#include "ogreutil.h"
#include <boost\tuple\tuple.hpp>
#include <inputonboard/ComponentModel.h>


using namespace Ogre;

#define STR_LINE_MAT _T("GridLine")

void CGrid3D::Update(const CGrid& griddata,emExtent extent,bool bShowZ)
{
	SceneManager* pScene = GetScene();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject( GetIDName(),pScene);
	Build( griddata,pObj,extent,bShowZ);	
	AddObject(pObj);
}

void CGrid3D::Build( const CGrid& griddata, Ogre::ManualObject* pGridObj,emExtent extType, bool bShowZ /*= false*/ )
{
	double alpha = 64.0f/255.0f;	
	int nSubdivs = griddata.nSubdivs;
	DWORD cSubdivsColor = griddata.cSubdivsColor;
	DWORD cAxesColor = griddata.cAxesColor;
	DWORD cLinesColor = griddata.cLinesColor;
	double dLinesEvery = griddata.dLinesEvery;

	Ogre::Vector3 planX = Vector3::UNIT_X;
	Ogre::Vector3 planY = Vector3::UNIT_Y;
	Ogre::Vector3 planZOffset = Vector3::ZERO;

	double planXSize = griddata.dSizeX;
	double planYSize = griddata.dSizeY;
	double planZSize = griddata.dSizeZ;


	if(extType == CGrid3D::YZExtent)
	{
		planX = Vector3::UNIT_Y;
		planY = Vector3::UNIT_Z;
		planXSize = griddata.dSizeY;
		planYSize = griddata.dSizeZ;
		planZOffset = -Vector3::UNIT_X * 0.5 * griddata.dSizeX;
	}

	if(extType == CGrid3D::XZExtent)
	{
		planX = Vector3::UNIT_X;
		planY = Vector3::UNIT_Z;
		planXSize = griddata.dSizeX;
		planYSize = griddata.dSizeZ;
		planZOffset = -Vector3::UNIT_Y * 0.5 * griddata.dSizeY;
	}


	double dMinX = -planXSize;
	double dMaxX = planXSize;
	double dMinY = -planYSize;
	double dMaxY = planYSize;

	if(!(dMinX<dMaxX && dMinY<dMaxY && dLinesEvery>0))
	{
		return;
	}

	pGridObj->clear();
	if (false == griddata.bVisibility)
		return;

	pGridObj->begin( STR_LINE_MAT ,RenderOperation::OT_LINE_LIST);

	//sub dive lines
	if( nSubdivs > 1)
	{			
		pGridObj->colour( OgreConvert::GetColor(cSubdivsColor,alpha) );
		double step =  dLinesEvery/nSubdivs;
		for(double i=step; i<=dMaxX; i+=step) {
			pGridObj->position(i*planX+dMaxY*planY + planZOffset);
			pGridObj->position(i*planX+dMinY*planY + planZOffset);
		}
		for(double i=-step; i>=dMinX; i-=step) {
			pGridObj->position(i*planX+dMaxY*planY + planZOffset);
			pGridObj->position(i*planX+dMinY*planY + planZOffset);
		}
		for(double i=step; i<=dMaxY; i+=step) {
			pGridObj->position(dMaxX*planX+i*planY + planZOffset);
			pGridObj->position(dMinX*planX+i*planY + planZOffset);
		}
		for(double i=-step; i>=dMinY; i-=step){
			pGridObj->position(dMaxX*planX+i*planY + planZOffset);
			pGridObj->position(dMinX*planX+i*planY + planZOffset);
		}
	}	

	//create grid lines
	pGridObj->colour(OgreConvert::GetColor(cLinesColor,alpha));
	for(double i=dLinesEvery; i<=dMaxX; i+=dLinesEvery) 
	{
		pGridObj->position(i*planX+ dMaxY*planY + planZOffset);
		pGridObj->position(i*planX+ dMinY*planY + planZOffset);		
	}
	for(double i=-dLinesEvery; i>=dMinX; i-=dLinesEvery)
	{
		pGridObj->position(i*planX+ dMaxY*planY + planZOffset);
		pGridObj->position(i*planX+ dMinY*planY + planZOffset);
	}
	//
	for(double i=dLinesEvery; i<=dMaxY; i+=dLinesEvery)
	{
		pGridObj->position(dMaxX*planX+ i*planY + planZOffset);
		pGridObj->position(dMinX*planX+ i*planY + planZOffset);		
	}	
	for(double i=-dLinesEvery; i>= dMinY; i-= dLinesEvery)
	{
		pGridObj->position(dMaxX*planX+ i*planY + planZOffset);
		pGridObj->position(dMinX*planX+ i*planY + planZOffset);
	}

	pGridObj->end();


	//create center line	
	{
		pGridObj->begin( STR_LINE_MAT ,RenderOperation::OT_LINE_LIST);
		pGridObj->position( dMaxY*planY + planZOffset);
		pGridObj->colour(OgreConvert::GetColor(cAxesColor));
		pGridObj->position( dMinY*planY + planZOffset);
		pGridObj->position(dMaxX*planX + planZOffset);
		pGridObj->position(dMinX*planX + planZOffset);		
		pGridObj->end();	


		/*if(bShowZ)
		{
		pGridObj->begin( STR_CENTER_LINE_MAT ,RenderOperation::OT_LINE_LIST);
		double dMinZ = -0.5*griddata.dSizeZ;
		double dMaxZ = 0.5*griddata.dSizeZ;
		pGridObj->position(dMaxX,0,dMinZ);
		pGridObj->colour(Ogre::ColourValue::Blue);
		pGridObj->position(dMaxX,0,dMaxZ);
		pGridObj->end();
		}	*/
	}

	//create invisible plane
	/*pGridObj->begin("Invisible",RenderOperation::OT_TRIANGLE_FAN);
	{
	pGridObj->position(dMinX,dMinY,0);
	pGridObj->colour(1,1,1,0);		
	pGridObj->position(dMaxX,dMinY,0);
	pGridObj->position(dMaxX,dMaxY,0);
	pGridObj->position(dMinX,dMaxY,0);
	}
	pGridObj->end();*/
}


void CGrid3D::Build( const CGrid& griddata, Ogre::ManualObject* pGridObj,BOOL bActive )
{
	double alpha = 64.0f/255.0f;	
	int nSubdivs = griddata.nSubdivs;
	DWORD cSubdivsColor = griddata.cSubdivsColor;
	DWORD cAxesColor = griddata.cAxesColor;
	DWORD cLinesColor = griddata.cLinesColor;
	double dLinesEvery = griddata.dLinesEvery;

	Ogre::Vector3 planX = Vector3::UNIT_X;
	Ogre::Vector3 planY = Vector3::UNIT_Y;
	Ogre::Vector3 planZOffset = Vector3::ZERO;

	double planXSize = griddata.dSizeX;
	double planYSize = griddata.dSizeY;
	double planZSize = griddata.dSizeZ;

	double dMinX = -planXSize;
	double dMaxX = planXSize;
	double dMinY = -planYSize;
	double dMaxY = planYSize;

	if(!(dMinX<dMaxX && dMinY<dMaxY && dLinesEvery>0))
	{
		return;
	}

	pGridObj->clear();
	if (false == griddata.bVisibility)
		return;

	pGridObj->begin( STR_LINE_MAT ,RenderOperation::OT_LINE_LIST);

	//sub dive lines
	if( nSubdivs > 1)
	{			
		pGridObj->colour( OgreConvert::GetColor(cSubdivsColor,alpha) );
		double step =  dLinesEvery/nSubdivs;
		for(double i=step; i<=dMaxX; i+=step) {
			pGridObj->position(i*planX+dMaxY*planY + planZOffset);
			pGridObj->position(i*planX+dMinY*planY + planZOffset);
		}
		for(double i=-step; i>=dMinX; i-=step) {
			pGridObj->position(i*planX+dMaxY*planY + planZOffset);
			pGridObj->position(i*planX+dMinY*planY + planZOffset);
		}
		for(double i=step; i<=dMaxY; i+=step) {
			pGridObj->position(dMaxX*planX+i*planY + planZOffset);
			pGridObj->position(dMinX*planX+i*planY + planZOffset);
		}
		for(double i=-step; i>=dMinY; i-=step){
			pGridObj->position(dMaxX*planX+i*planY + planZOffset);
			pGridObj->position(dMinX*planX+i*planY + planZOffset);
		}
	}	

	//create grid lines
	pGridObj->colour(OgreConvert::GetColor(cLinesColor,alpha));
	for(double i=dLinesEvery; i<=dMaxX; i+=dLinesEvery) 
	{
		pGridObj->position(i*planX+ dMaxY*planY + planZOffset);
		pGridObj->position(i*planX+ dMinY*planY + planZOffset);		
	}
	for(double i=-dLinesEvery; i>=dMinX; i-=dLinesEvery)
	{
		pGridObj->position(i*planX+ dMaxY*planY + planZOffset);
		pGridObj->position(i*planX+ dMinY*planY + planZOffset);
	}
	//
	for(double i=dLinesEvery; i<=dMaxY; i+=dLinesEvery)
	{
		pGridObj->position(dMaxX*planX+ i*planY + planZOffset);
		pGridObj->position(dMinX*planX+ i*planY + planZOffset);		
	}	
	for(double i=-dLinesEvery; i>= dMinY; i-= dLinesEvery)
	{
		pGridObj->position(dMaxX*planX+ i*planY + planZOffset);
		pGridObj->position(dMinX*planX+ i*planY + planZOffset);
	}

	pGridObj->end();

	if(bActive){
		cAxesColor = RGB(0,0,75);
	}
	//create center line	
	{
		pGridObj->begin( STR_LINE_MAT ,RenderOperation::OT_LINE_LIST);
		pGridObj->position( dMaxY*planY + planZOffset);
		pGridObj->colour(OgreConvert::GetColor(cAxesColor));
		pGridObj->position( dMinY*planY + planZOffset);
		pGridObj->position(dMaxX*planX + planZOffset);
		pGridObj->position(dMinX*planX + planZOffset);		
		pGridObj->end();

	}

}


