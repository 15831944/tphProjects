#pragma once
#include "RenderScene3DNode.h"
#include "AnimaMob3DList.h"
class RENDERENGINE_API CLandsideVehicle3D : public C3DNodeObject
{
public:
	SCENENODE_CONVERSION_DEF(CLandsideVehicle3D, C3DNodeObject)
	//PROTYPE_DECLARE(CLandsideVehicle3D);
	CLandsideVehicle3D(int nPaxid, Ogre::SceneNode* pNode);

	static CString GetNodeName(int nID);

	void SetShape(const CString& strAcType, double dlen, double dwidth,double dheight);	
	void SetColor( COLORREF color );

	//show the bound box 
	void UpdateDebugShape(double dlen, double dwidth, double dSpeed);
protected:
	void SetQueryData();

};

	
class RENDERENGINE_API CAnimaLandsideVehicle3DList: public  CAnimaMob3DList<CLandsideVehicle3D>
{

};

