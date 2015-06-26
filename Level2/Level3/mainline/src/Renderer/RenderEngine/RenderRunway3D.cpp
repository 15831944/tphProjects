#include "StdAfx.h"
#include ".\renderrunway3d.h"

#include "RenderAirport3D.h"
#include "SelectionManager.h"

#include <minmax.h>
#include <Common/ARCPath.h>
#include "CustomizedRenderQueue.h"
#include "ShapeBuilder.h"

using namespace Ogre;

static void InsertSimpleClip(TextureClips& _vClips, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dClipLength, std::string _textureName)
{
	size_t idx = _vClips.size();
	_vClips.resize(idx+1);
	_vClips[idx].texture_name = _textureName;
	std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
	std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
	vertex_coords.resize(4);
	texture_coords.resize(4);
	vertex_coords[0] = _corner1;
	vertex_coords[3] = _corner2;
	vertex_coords[1] = _corner1 + _dClipLength * _dir;
	vertex_coords[2] = _corner2 + _dClipLength * _dir;
	texture_coords[0] = ARCVector2(1.0,0.0);
	texture_coords[3] = ARCVector2(0.0,0.0);
	texture_coords[1] = ARCVector2(1.0,1.0);
	texture_coords[2] = ARCVector2(0.0,1.0);
	_corner1 = vertex_coords[1];
	_corner2 = vertex_coords[2];
}

static void InsertThresholdClip(TextureClips& _vClips, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dClipLength, std::string _textureName, bool bEndLine)
{
	size_t idx = _vClips.size();
	const static double dLineWidth = 3;
	//insert begin line clip
	{
		_vClips.resize(idx+1);
		_vClips[idx].texture_name = "Runway/rwline";
		std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
		std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
		vertex_coords.resize(4);
		texture_coords.resize(4);
		vertex_coords[0] = _corner1;
		vertex_coords[3] = _corner2;
		vertex_coords[1] = _corner1 + dLineWidth * _dir;
		vertex_coords[2] = _corner2 + dLineWidth * _dir;
		texture_coords[0] = ARCVector2(1.0,0.0);
		texture_coords[3] = ARCVector2(0.0,0.0);
		texture_coords[1] = ARCVector2(1.0,1.0);
		texture_coords[2] = ARCVector2(0.0,1.0);
		_corner1 = vertex_coords[1];
		_corner2 = vertex_coords[2];
	}

	double iTex = int(_dClipLength / 20.0 + 0.5 );

	//
	{
		idx = _vClips.size();
		_vClips.resize(idx+1);
		std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
		std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
		_vClips[idx].texture_name = _textureName;
		vertex_coords.resize(4);
		texture_coords.resize(4);
		vertex_coords[0] = _corner1;
		vertex_coords[3] = _corner2;
		vertex_coords[1] = _corner1 + _dClipLength * _dir;
		vertex_coords[2] = _corner2 + _dClipLength * _dir;

		texture_coords[0] = ARCVector2(1.0,0.0);
		texture_coords[3] = ARCVector2(0.0,0.0);
		texture_coords[1] = ARCVector2(1.0,iTex);
		texture_coords[2] = ARCVector2(0.0,iTex);
		_corner1 = vertex_coords[1];
		_corner2 = vertex_coords[2];
	}


	//
	if (!bEndLine) return;
	if (!(iTex>0)) return;

	{
		idx = _vClips.size();
		_vClips.resize(idx+1);
		std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
		std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
		_vClips[idx].texture_name = "Runway/rwline";
		vertex_coords.resize(4);
		texture_coords.resize(4);
		vertex_coords[0] = _corner1;
		vertex_coords[3] = _corner2;
		vertex_coords[1] = _corner1 + dLineWidth * _dir;
		vertex_coords[2] = _corner2 + dLineWidth * _dir;
		texture_coords[0] = ARCVector2(1.0,0.0);
		texture_coords[3] = ARCVector2(0.0,0.0);
		texture_coords[1] = ARCVector2(1.0,1.0);
		texture_coords[2] = ARCVector2(0.0,1.0);
		_corner1 = vertex_coords[1];
		_corner2 = vertex_coords[2]; 
	}

}

static int InsertMarkingsClips(TextureClips& _vClips, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dClipLength, const std::string& _marking)
{
	if (_marking.empty())
		return 0;

	char szMarking[255];
	strcpy(szMarking,_marking.c_str());

	char* p = szMarking;
	int nRwyDir = atoi(szMarking);
	while(isdigit(*p)) {
		++p;
	}
	bool bHasLetter = (p != NULL && (*p)!='\0');

	int nClipLevelsAdded = 0;

	size_t idx = _vClips.size();

	if (bHasLetter)
	{
		//add letter clip
		_vClips.resize(idx+1);
		std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
		std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
		_vClips[idx].texture_name = std::string("Runway/") + p;
		vertex_coords.resize(4);
		texture_coords.resize(4);
		vertex_coords[0] = _corner1;
		vertex_coords[3] = _corner2;
		vertex_coords[1] = _corner1 + _dClipLength * _dir;
		vertex_coords[2] = _corner2 + _dClipLength * _dir;
		texture_coords[2] = ARCVector2(0.0,0.0);
		texture_coords[1] = ARCVector2(1.0,0.0);
		texture_coords[3] = ARCVector2(0.0,1.0);
		texture_coords[0] = ARCVector2(1.0,1.0);
		_corner1 = vertex_coords[1];
		_corner2 = vertex_coords[2];
		idx++;

		nClipLevelsAdded++;
		p[0] = 0; //clear letter
	}
	if (nRwyDir<0) return nClipLevelsAdded;

	//add single digit clips
	if(nRwyDir <10 && strlen(szMarking)==1)
	{
		char RwDigitTex[] = "Rw0C";
		RwDigitTex[2] = *szMarking;

		_vClips.resize(idx +1);
		std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
		std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
		_vClips[idx].texture_name = std::string("Runway/") + RwDigitTex;
		vertex_coords.resize(4);
		texture_coords.resize(4);
		vertex_coords[0] = _corner1;
		vertex_coords[3] = _corner2;
		vertex_coords[1] = _corner1 + _dClipLength * _dir;
		vertex_coords[2] = _corner2 + _dClipLength * _dir;
		texture_coords[2] = ARCVector2(0.0,0.0);
		texture_coords[1] = ARCVector2(1.0,0.0);
		texture_coords[3] = ARCVector2(0.0,1.0);
		texture_coords[0] = ARCVector2(1.0,1.0);
		idx++;

	}
	else if(strlen(szMarking) ==2)  //add double digit clips
	{
		//add double digit clips
		ARCVector2 vRwyCenter = (_corner1 + _corner2)/2;

		char RwDigitTex[] = "Rw0L"; 
		RwDigitTex[2] = *szMarking; 

		{
			_vClips.resize(idx+1);
			std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
			std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
			_vClips[idx].texture_name = std::string("Runway/") + RwDigitTex;
			vertex_coords.resize(4);
			texture_coords.resize(4);
			vertex_coords[0] = vRwyCenter;
			vertex_coords[3] = _corner2;
			vertex_coords[1] = vRwyCenter + _dClipLength * _dir;
			vertex_coords[2] = _corner2 + _dClipLength * _dir;
			texture_coords[2] = ARCVector2(0.0,0.0);
			texture_coords[1] = ARCVector2(1.0,0.0);
			texture_coords[3] = ARCVector2(0.0,1.0);
			texture_coords[0] = ARCVector2(1.0,1.0);
			idx++;
		}

		{
			_vClips.resize(idx+1);  RwDigitTex[2] = *(szMarking +1); RwDigitTex[3] = 'R';
			std::vector<ARCVector2>& vertex_coords = _vClips[idx].vertex_coords;
			std::vector<ARCVector2>& texture_coords = _vClips[idx].texture_coords;
			_vClips[idx].texture_name = std::string("Runway/") + RwDigitTex;
			vertex_coords.resize(4);
			texture_coords.resize(4);
			vertex_coords[0] = _corner1;
			vertex_coords[3] = vRwyCenter;
			vertex_coords[1] = _corner1 + _dClipLength * _dir;
			vertex_coords[2] = vRwyCenter + _dClipLength * _dir;
			texture_coords[2] = ARCVector2(0.0,0.0);
			texture_coords[1] = ARCVector2(1.0,0.0);
			texture_coords[3] = ARCVector2(0.0,1.0);
			texture_coords[0] = ARCVector2(1.0,1.0);
		}
	}
	_corner1 = _corner1 + _dClipLength * _dir;
	_corner2 = _corner2 + _dClipLength * _dir;
	nClipLevelsAdded++;


	return nClipLevelsAdded;
}

void CRenderRunway3D::GenTextureClips(Runway* pRunway)
{
	int c = pRunway->GetPath().getCount();

	if (c<2) return;
	const CPath2008& path = pRunway->GetPath();
	CPoint2008 p0 = path.getPoint(0);
	CPoint2008 p1 = path.getPoint(1);
	double width = pRunway->GetWidth();

	//get perpendicular vector
	ARCVector2 vPerp( p1.getY()-p0.getY() , -(p1.getX()-p0.getX()));
	vPerp.Normalize();
	vPerp*=(width/2.0);

	//get runway direction vector
	ARCVector2 vRwy(p1.getX()-p0.getX(), p1.getY()-p0.getY());
	double dRwyLength = vRwy.Magnitude()/100.0; //in meters
	vRwy.Normalize();
	vRwy *= 100.0; // vRwy is 1 meter in magnitude

	//clear clips
	m_vClips.clear();
	m_vClips.reserve(10);
	DistanceUnit dClipLength = 0.0;
	DistanceUnit dLengthRemaining = dRwyLength;

	ARCVector2 vRwyCorner1, vRwyCorner2, vRwyCenter;


	//// End of the runway at point p0
	vRwyCorner1 = ARCVector2(p0.getX()+vPerp[VX], p0.getY()+vPerp[VY]);
	vRwyCorner2 = ARCVector2(p0.getX()-vPerp[VX], p0.getY()-vPerp[VY]);
	vRwyCenter = ARCVector2(p0.getX(), p0.getY());


	//clip is threshold 1 (50 meters)
	//takeoff threshold
	{
		DistanceUnit dTakeoffThres = min( pRunway->GetMark1LandingThreshold()/SCALE_FACTOR, pRunway->GetMark1TakeOffThreshold()/SCALE_FACTOR );
		InsertSimpleClip(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, dTakeoffThres, "Runway/RunwayReview");
		dLengthRemaining -= dTakeoffThres;
	}


	//bool bShowlandingThreshold = TRUE;
	DistanceUnit disPlacedThreshold = 0;
	disPlacedThreshold = pRunway->GetMark1LandingThreshold()/SCALE_FACTOR - pRunway->GetMark1TakeOffThreshold() /SCALE_FACTOR;
	if(disPlacedThreshold > 0) 
	{
		InsertThresholdClip(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, disPlacedThreshold, "Runway/Threshold", true);
		dLengthRemaining -= disPlacedThreshold;
	}
	//landing Threshold 50
	{
		InsertSimpleClip(m_vClips,vRwyCorner1,vRwyCorner2,vRwy,50.0,"Runway/RwThreshold");
		dLengthRemaining -= 50.0;
	}

	int nClipLevelsAdded = InsertMarkingsClips(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, 30.0, pRunway->GetMarking1());
	dLengthRemaining -= (30.0 * nClipLevelsAdded);

	ARCVector2 vRwyCorner1a = vRwyCorner1;
	ARCVector2 vRwyCorner2a = vRwyCorner2;

	// End of the runway at point p1
	vRwyCorner1 = ARCVector2(p1.getX()-vPerp[VX], p1.getY()-vPerp[VY]);
	vRwyCorner2 = ARCVector2(p1.getX()+vPerp[VX], p1.getY()+vPerp[VY]);
	vRwyCenter = ARCVector2(p1.getX(), p1.getY());
	vRwy = -vRwy;

	//next clip is threshold 2 (50 meters)
	{
		DistanceUnit dTakeoffThres  = min(pRunway->GetMark2TakeOffThreshold() / SCALE_FACTOR, pRunway->GetMark2LandingThreshold()/SCALE_FACTOR);
		InsertSimpleClip(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, dTakeoffThres, "Runway/RunwayReview");
		dLengthRemaining -= dTakeoffThres;
	}


	{
		DistanceUnit displaceThreshold = pRunway->GetMark2LandingThreshold()/SCALE_FACTOR-pRunway->GetMark2TakeOffThreshold()/SCALE_FACTOR ;
		if(displaceThreshold > 0)
		{
			InsertThresholdClip(m_vClips,vRwyCorner1,vRwyCorner2,vRwy,displaceThreshold,"Runway/Threshold",true);
			dLengthRemaining -= displaceThreshold;
		}
	}	
	{
		//DistanceUnit dLandThreshold = pRunway->GetMark2TakeOffThreshold() / SCALE_FACTOR;
		InsertSimpleClip(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, 50.0, "Runway/RwThreshold" );
		dLengthRemaining -= 50.0;
	}
	//next n = [1..3] clips are for designation 2

	nClipLevelsAdded = InsertMarkingsClips(m_vClips, vRwyCorner1, vRwyCorner2, vRwy, 30.0, pRunway->GetMarking2());
	dLengthRemaining -= (30.0 * nClipLevelsAdded);

	//last clip is centerline

	dClipLength = dLengthRemaining;


	size_t idx = m_vClips.size();
	m_vClips.resize(idx+1);
	std::vector<ARCVector2>& vertex_coords = m_vClips[idx].vertex_coords;
	std::vector<ARCVector2>& texture_coords = m_vClips[idx].texture_coords;
	m_vClips[idx].texture_name = "Runway/RwCenter";
	vertex_coords.resize(4);
	texture_coords.resize(4);
	vertex_coords[0] = vRwyCorner1;
	vertex_coords[3] = vRwyCorner2;
	vertex_coords[1] = vRwyCorner2a;
	vertex_coords[2] = vRwyCorner1a;
	texture_coords[0] = ARCVector2(0.0,0.0);
	texture_coords[3] = ARCVector2(1.0,0.0);
	texture_coords[1] = ARCVector2(0.0,dClipLength/50.0);
	texture_coords[2] = ARCVector2(1.0,dClipLength/50.0);
}

void CRenderRunway3D::Build(Runway* pRunway)
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	pObj->clear();
	pObj->setRenderQueueGroup(RenderObject_Surface);
	GenTextureClips(pRunway);

	Ogre::ColourValue clrTransparent(1.0, 1.0, 1.0, 0.0);

	ARCPath3 drawPath;
	const CPath2008& path2008 = pRunway->GetPath();
	drawPath.push_back(path2008.getPoint(0));
	drawPath.push_back(path2008.getPoint(1));
	ProcessorBuilder::DrawTexturePath(pObj, _T("Runway/RunwayBaseLine"), clrTransparent, drawPath, pRunway->GetWidth(), 0.0);

	ManualObject* pMarkObj = _GetOrCreateManualObject(GetName() + _T("-Marks"));//OgreUtil::createOrRetrieveManualObject(GetIDName() + _T("-Marks"), GetScene());
	pMarkObj->setRenderQueueGroup(RenderObject_Mark);
	ProcessorBuilder::DrawTextureClips(pMarkObj, m_vClips, clrTransparent, 0.1);
	//pMarkObj->setVisibilityFlags(NormalVisibleFlag);
// 	REG_SELECT(pLineObj);
	AddObject(pMarkObj);
	//OgreUtil::AttachMovableObject(pMarkObj,mpNode);
	AddObject(pObj);

}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderRunway3D, Runway);


void CRenderRunway3D::Update3D( ALTObjectDisplayProp* pDispObj )
{
	
}

void CRenderRunway3D::Update3D( ALTObject* pBaseObj )
{
	Build((Runway*)pBaseObj);
}
