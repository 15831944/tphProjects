#include "StdAfx.h"
#include "..\InputAirside\Runway.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"
#include ".\runway3d.h"
#include ".\3DView.h"
#include "../Common/ARCPipe.h"
#include ".\glrender\glTextureResource.h"

CRunway3D::CRunway3D(int id):ALTObject3D(id)
{
	m_pObj = new Runway;
	//m_pDisplayProp = new RunwayDisplayProp;
	m_bInEdit = false;
}

CRunway3D::~CRunway3D(void)
{
}

void CRunway3D::DrawOGL(C3DView * pView)
{
	if(IsNeedSync()){
		DoSync();
	}
	glEnable(GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	RenderBaseWithSideLine(pView);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-4.0,-4.0);

	glDepthMask(GL_FALSE);	
	//
	RenderBaseWithSideLine(pView);	//get data	
	//RenderBase(pView);
	//
	RenderMarkings(pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

	
}

void InsertSimpleBlock( std::vector<RunwayBlock>& _vBlocks, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dBlockLength, std::string _textureName)
{
	int idx = _vBlocks.size();
	_vBlocks.resize(idx+1);
	_vBlocks[idx].texture_name = _textureName;
	_vBlocks[idx].vertex_coords.resize(4);
	_vBlocks[idx].texture_coords.resize(4);
	_vBlocks[idx].vertex_coords[0] = _corner1;
	_vBlocks[idx].vertex_coords[3] = _corner2;
	_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
	_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
	_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
	_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
	_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
	_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
	_corner1 = _vBlocks[idx].vertex_coords[1];
	_corner2 = _vBlocks[idx].vertex_coords[2];
}

void InsertThresholdBlock(std::vector<RunwayBlock>& _vBlocks, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dBlockLength, std::string _textureName, bool bEndLine)
{
	int idx = _vBlocks.size();
	const static double dLineWidth = 3;
	//insert begin line block
	_vBlocks.resize(idx+1);
	_vBlocks[idx].texture_name = "rwline";
	_vBlocks[idx].vertex_coords.resize(4);
	_vBlocks[idx].texture_coords.resize(4);
	_vBlocks[idx].vertex_coords[0] = _corner1;
	_vBlocks[idx].vertex_coords[3] = _corner2;
	_vBlocks[idx].vertex_coords[1] = _corner1 + dLineWidth * _dir;
	_vBlocks[idx].vertex_coords[2] = _corner2 + dLineWidth * _dir;
	_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
	_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
	_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
	_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
	_corner1 = _vBlocks[idx].vertex_coords[1];
	_corner2 = _vBlocks[idx].vertex_coords[2];

	
	//
    idx = _vBlocks.size();
	_vBlocks.resize(idx+1);
	_vBlocks[idx].texture_name = _textureName;
	_vBlocks[idx].vertex_coords.resize(4);
	_vBlocks[idx].texture_coords.resize(4);
	_vBlocks[idx].vertex_coords[0] = _corner1;
	_vBlocks[idx].vertex_coords[3] = _corner2;
	_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
	_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
	
    double iTex = int(_dBlockLength / 20.0 + 0.5 );

	_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
	_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
	_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,iTex);
	_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,iTex);
	_corner1 = _vBlocks[idx].vertex_coords[1];
	_corner2 = _vBlocks[idx].vertex_coords[2];
	

	//
	if(!bEndLine ) return ;
	if(!(iTex>0)) return;
	idx = _vBlocks.size();
	_vBlocks.resize(idx+1);
	_vBlocks[idx].texture_name = "rwline";
	_vBlocks[idx].vertex_coords.resize(4);
	_vBlocks[idx].texture_coords.resize(4);
	_vBlocks[idx].vertex_coords[0] = _corner1;
	_vBlocks[idx].vertex_coords[3] = _corner2;
	_vBlocks[idx].vertex_coords[1] = _corner1 + dLineWidth * _dir;
	_vBlocks[idx].vertex_coords[2] = _corner2 + dLineWidth * _dir;
	_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
	_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
	_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
	_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
	_corner1 = _vBlocks[idx].vertex_coords[1];
	_corner2 = _vBlocks[idx].vertex_coords[2]; 

}


int InsertMarkingsBlocks( std::vector<RunwayBlock>& _vBlocks, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dBlockLength, const std::string& _marking)
{
	if(_marking.empty())return 0;
	char szMarking[255];
	strcpy(szMarking,_marking.c_str());
	
	char* p = szMarking;
	int nRwyDir = atoi(szMarking);
	while(isdigit(*p)) {
		++p;
	}
	bool bHasLetter = (p != NULL && (*p)!='\0');

	int nBlockLevelsAdded = 0;

	int idx = _vBlocks.size();

	if(bHasLetter) {
		
		//add letter block
		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_name = p;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		_corner1 = _vBlocks[idx].vertex_coords[1];
		_corner2 = _vBlocks[idx].vertex_coords[2];
		idx++;

		nBlockLevelsAdded++;
		p[0] = 0; //clear letter
	}
	if(nRwyDir<0)return nBlockLevelsAdded;

	//add single digit blocks
	if(nRwyDir <10 && strlen(szMarking)==1)
	{
		char RwDigitTex[] = "Rw0C";
		RwDigitTex[2] = *szMarking;
		
		_vBlocks.resize(idx +1);
		_vBlocks[idx].texture_name = RwDigitTex;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		idx++;

	}
	else if(strlen(szMarking) ==2)  //add double digit blocks
	{
		//add double digit blocks
		ARCVector2 vRwyCenter = (_corner1 + _corner2)/2;

		char RwDigitTex[] = "Rw0L"; 
		RwDigitTex[2] = *szMarking; 

		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_name = RwDigitTex;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = vRwyCenter;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = vRwyCenter + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		idx++;

		_vBlocks.resize(idx+1);  RwDigitTex[2] = *(szMarking +1); RwDigitTex[3] = 'R';
		_vBlocks[idx].texture_name = RwDigitTex;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = vRwyCenter;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = vRwyCenter + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);


	}
	_corner1 = _corner1 + _dBlockLength * _dir;
	_corner2 = _corner2 + _dBlockLength * _dir;
	nBlockLevelsAdded++;


	return nBlockLevelsAdded;
}
void CRunway3D::GenBlocks()
{
	Runway * m_Runway = GetRunway();

	
	int c = m_Runway->GetPath().getCount();
	
	if(c<2 )return;

	CPoint2008 p0 = m_Runway->GetPath().getPoint(0);
	CPoint2008 p1 = m_Runway->GetPath().getPoint(1);
	double width = m_Runway->GetWidth();

	//get perpedicular vector
	ARCVector2 vPerp( p1.getY()-p0.getY() , -(p1.getX()-p0.getX()));
	vPerp.Normalize();
	vPerp*=(width/2.0);

	m_vOutline.clear();
	m_vOutline.push_back(ARCVector2(p0) + vPerp);
	m_vOutline.push_back(ARCVector2(p0)- vPerp);
	m_vOutline.push_back(ARCVector2(p1) - vPerp);
	m_vOutline.push_back(ARCVector2(p1)+vPerp);


	//get runway direction vector
	ARCVector2 vRwy(p1.getX()-p0.getX(), p1.getY()-p0.getY());
	double dRwyLength = vRwy.Magnitude()/100.0; //in meters
	vRwy.Normalize();
	vRwy *= 100.0; // vRwy is 1 meter in magnitude

	//clear blocks
	m_vBlocks.clear();
	m_vBlocks.reserve(10);
	DistanceUnit dBlockLength = 0.0;
	DistanceUnit dLengthRemaining = dRwyLength;

	ARCVector2 vRwyCorner1, vRwyCorner2, vRwyCenter;


	//// End of the runway at point p0
	vRwyCorner1 = ARCVector2(p0.getX()+vPerp[VX], p0.getY()+vPerp[VY]);
	vRwyCorner2 = ARCVector2(p0.getX()-vPerp[VX], p0.getY()-vPerp[VY]);
	vRwyCenter = ARCVector2(p0.getX(), p0.getY());


	//block is threshold 1 (50 meters)
	//takeoff threshold
	{
		DistanceUnit dTakeoffThres = min( GetRunway()->GetMark1LandingThreshold()/SCALE_FACTOR, GetRunway()->GetMark1TakeOffThreshold()/SCALE_FACTOR );
		InsertSimpleBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, dTakeoffThres, "RunwayReview");
		dLengthRemaining -= dTakeoffThres;
	}


	//bool bShowlandingThreshold = TRUE;
	DistanceUnit disPlacedThreshold = 0;
	disPlacedThreshold = GetRunway()->GetMark1LandingThreshold()/SCALE_FACTOR - GetRunway()->GetMark1TakeOffThreshold() /SCALE_FACTOR;
	if(disPlacedThreshold > 0) 
	{
		InsertThresholdBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, disPlacedThreshold, "Threshold", true);
		dLengthRemaining -= disPlacedThreshold;
	}
	//landing Threshold 50
	{
		InsertSimpleBlock(m_vBlocks,vRwyCorner1,vRwyCorner2,vRwy,50.0,"RwThreshold");
		dLengthRemaining -= 50.0;
	}

	int nBlockLevelsAdded = InsertMarkingsBlocks(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, 30.0, m_Runway->GetMarking1());
	dLengthRemaining -= (30.0 * nBlockLevelsAdded);

	ARCVector2 vRwyCorner1a = vRwyCorner1;
	ARCVector2 vRwyCorner2a = vRwyCorner2;

	// End of the runway at point p1
	vRwyCorner1 = ARCVector2(p1.getX()-vPerp[VX], p1.getY()-vPerp[VY]);
	vRwyCorner2 = ARCVector2(p1.getX()+vPerp[VX], p1.getY()+vPerp[VY]);
	vRwyCenter = ARCVector2(p1.getX(), p1.getY());
	vRwy = -vRwy;

	//next block is threshold 2 (50 meters)
	{
		DistanceUnit dTakeoffThres  = min(GetRunway()->GetMark2TakeOffThreshold() / SCALE_FACTOR, GetRunway()->GetMark2LandingThreshold()/SCALE_FACTOR);
		InsertSimpleBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, dTakeoffThres, "RunwayReview");
		dLengthRemaining -= dTakeoffThres;
	}

		
	{
		DistanceUnit displaceThreshold = GetRunway()->GetMark2LandingThreshold()/SCALE_FACTOR-GetRunway()->GetMark2TakeOffThreshold()/SCALE_FACTOR ;
		if(displaceThreshold > 0)
		{
			InsertThresholdBlock(m_vBlocks,vRwyCorner1,vRwyCorner2,vRwy,displaceThreshold,"Threshold",true);
			dLengthRemaining -= displaceThreshold;
		}
	}	
	{
		//DistanceUnit dLandThreshold = GetRunway()->GetMark2TakeOffThreshold() / SCALE_FACTOR;
		InsertSimpleBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, 50.0, "RwThreshold" );
		dLengthRemaining -= 50.0;
	}
	//next n = [1..3] blocks are for designation 2

	nBlockLevelsAdded = InsertMarkingsBlocks(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, 30.0, m_Runway->GetMarking2());
	dLengthRemaining -= (30.0 * nBlockLevelsAdded);

	//last block is centerline

	dBlockLength = dLengthRemaining;


	int idx = m_vBlocks.size();
	m_vBlocks.resize(idx+1);
	m_vBlocks[idx].texture_name = "RwCenter";
	m_vBlocks[idx].vertex_coords.resize(4);
	m_vBlocks[idx].texture_coords.resize(4);
	m_vBlocks[idx].vertex_coords[0] = vRwyCorner1;
	m_vBlocks[idx].vertex_coords[3] = vRwyCorner2;
	m_vBlocks[idx].vertex_coords[1] = vRwyCorner2a;
	m_vBlocks[idx].vertex_coords[2] = vRwyCorner1a;
	m_vBlocks[idx].texture_coords[0] = ARCVector2(0.0,0.0);
	m_vBlocks[idx].texture_coords[3] = ARCVector2(1.0,0.0);
	m_vBlocks[idx].texture_coords[1] = ARCVector2(0.0,dBlockLength/50.0);
	m_vBlocks[idx].texture_coords[2] = ARCVector2(1.0,dBlockLength/50.0);
}

Runway * CRunway3D::GetRunway() const
{
	return (Runway *)m_pObj.get();
}

RunwayDisplayProp * CRunway3D::GetRunwayDisplay() const
{
	return (RunwayDisplayProp*) GetDisplayProp();
}

void CRunway3D::DrawSelect( C3DView * pView )
{
	//DrawOGL(pView);
	if(IsNeedSync()){
		DoSync();
	}
	
	//render base
	ARCPipe pipepath (GetRunway()->GetPath(),GetRunway()->GetWidth());
	int npippathcnt  = (int)pipepath.m_centerPath.size();
	glBegin(GL_QUAD_STRIP);
	ARCVector3 prePoint = pipepath.m_centerPath[0];	
	for(int i=0;i<npippathcnt;i ++ )
	{	
		glVertex3dv(pipepath.m_sidePath1[i]);	
		glVertex3dv(pipepath.m_sidePath2[i]);
	}
	glEnd();	
	//
	if(IsInEdit())
	{
		RenderEditPoint(pView,true);
	}

	for(int i=0;i<(int)m_vAllHoldPositions3D.size();i++){
		m_vAllHoldPositions3D.at(i)->DrawSelect(pView);
	}
}

void CRunway3D::RenderEditPoint( C3DView * pView,bool bSelectMode  )
{
	if(!IsInEdit()) return;
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		if(bSelectMode)
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CRunway3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetRunway()->getPath().DoOffset(dx,dy,dz);
	GenBlocks();	
}

ARCPoint3 CRunway3D::GetLocation( void ) const
{
	Runway * m_Runway = (Runway*)m_pObj.get();	
	
	if(m_Runway->GetPath().getCount() <2) return ARCPoint3(0,0,0);

	CPoint2008 p0 = m_Runway->GetPath().getPoint(0);
	CPoint2008 p1 = m_Runway->GetPath().getPoint(1);

	return ARCPoint3(  p0 + p1 ) * 0.5;
}

void CRunway3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetRunway()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetRunway()->getPath(),i) );
	}

	GenBlocks();
	SyncDateType::DoSync();
}


void CRunway3D::FlushChange()
{
	try
	{
		GetRunway()->UpdatePath();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

//void CRunway3D::UpdateHoldPositions( const ALTObject3DList& otherObjList )
//{
//	
//	ALTObjectList vAllObject = otherObjList.GetObjectList();
//	GetRunway()->UpdateHoldPositions(vAllObject);
//	m_vAllHoldPositions3D.clear();
//
//	for(int i =0 ;i<(int)GetRunway()->m_vAllHoldPositions.size();i++)
//	{
//		const HoldPosition& hold = GetRunway()->m_vAllHoldPositions[i];
//		if(hold.GetIntersectNode().GetOtherObj()->GetType() == ALT_TAXIWAY)
//		{
//			m_vAllHoldPositions3D.push_back(new HoldPositionRunwayToTaxiway3D(GetRunway(),i));
//		}
//		if(hold.GetIntersectNode().GetOtherObj()->GetType() == ALT_RUNWAY)
//		{
//			m_vAllHoldPositions3D.push_back(new HoldPositionRunwayToRunway3D(GetRunway(),i));
//		}		
//	}	
//	
//}
//
void CRunway3D::RenderHoldPosition( C3DView * pView )
{
	for(int i=0;i<(int)m_vAllHoldPositions3D.size();i++){
		m_vAllHoldPositions3D.at(i)->Draw(pView);
	}
}

void CRunway3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	GetRunway()->getPath().DoOffset(-center[VX],-center[VY],-center[VZ]);
	GetRunway()->getPath().Rotate(dx);
	GetRunway()->getPath().DoOffset(center[VX],center[VY],center[VZ]);
	GenBlocks();
}

void CRunway3D::RenderBase( C3DView * pView )
{
	if(!IsInEdit())
		glDisable(GL_BLEND);

	if(GetRunway()->GetPath().getCount()<1)return;

	ARCPipe pipepath (GetRunway()->GetPath(),GetRunway()->GetWidth());
	int npippathcnt  = (int)pipepath.m_centerPath.size();

	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
	if(pTexture)
		pTexture->Apply();

		glColor4d(1.0,1.0,1.0,0.4);
	double dLen =0;
	glBegin(GL_QUAD_STRIP);
	ARCVector3 prePoint = pipepath.m_centerPath[0];
	glNormal3i(0,0,1);
	for(int i=0;i<npippathcnt;i ++ )
	{
		dLen += prePoint.DistanceTo(pipepath.m_centerPath[i]);
		double dtexcord = dLen * 0.0002;
		glTexCoord2d(0,dtexcord);
		glVertex3dv(pipepath.m_sidePath1[i]);
		glTexCoord2d(1,dtexcord);
		glVertex3dv(pipepath.m_sidePath2[i]);
		
	}
	glEnd();
	if(!IsInEdit())
		glEnable(GL_BLEND);
}

void CRunway3D::RenderMarkings( C3DView * pView )
{
	if (!glIsEnabled(GL_BLEND))
		glEnable(GL_BLEND);
	GenBlocks();
	glColor3ub(255,255,255);
	//draw	
	for(size_t i=0;i<m_vBlocks.size();i++){
		CTexture * pTex = pView->getTextureResource()->getTexture(m_vBlocks[i].texture_name.c_str());
		if(pTex)pTex->Apply();
		glNormal3i(0,0,1);
		glBegin(GL_QUADS);
		for(size_t j=0;j<m_vBlocks[i].vertex_coords.size();j++){
			glTexCoord2dv(m_vBlocks[i].texture_coords[j]);
			glVertex2dv(m_vBlocks[i].vertex_coords[j]);
		}
		glEnd();
	}
}

void CRunway3D::RenderBaseWithSideLine( C3DView *pView )
{
	if(!IsInEdit())
		glDisable(GL_BLEND);
	if(GetRunway()->GetPath().getCount()<1)return;

	ARCPipe pipepath (GetRunway()->GetPath(),GetRunway()->GetWidth()*1.1);
	int npippathcnt  = (int)pipepath.m_centerPath.size();
	
	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBaseLine");

	if(pTexture)
		pTexture->Apply();


	double dLen =0;
	glBegin(GL_QUAD_STRIP);
	ARCVector3 prePoint = pipepath.m_centerPath[0];
	glColor4d(1.0,1.0,1.0,0.4);
	glNormal3i(0,0,1);
	for(int i=0;i<npippathcnt;i ++ )
	{
		dLen += prePoint.DistanceTo(pipepath.m_centerPath[i]);
		double dtexcord = dLen * 0.0002;
		glTexCoord2d(0,dtexcord);
		glVertex3dv(pipepath.m_sidePath1[i]);
		glTexCoord2d(1,dtexcord);
		glVertex3dv(pipepath.m_sidePath2[i]);

	}
	glEnd();
	if(!IsInEdit())
		glEnable(GL_BLEND);
}

//void RunwayHoldPosition3D::UpdateLine()
//{
//	//GetRunway()->GetHoldPositionLine(m_nid,m_line);
//}
//
//HoldPosition & RunwayHoldPosition3D::GetHoldPosition()
//{
//	return GetRunway()->m_vAllHoldPositions[m_nid];
//}

