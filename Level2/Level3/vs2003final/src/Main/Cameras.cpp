// Cameras.cpp: implementation of the CCameras class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cameras.h"
#include "../Common/ARCBoundingSphere.h"

#include <Common/3DViewCameras.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



void CPaneInfo::Convert( C3DViewPaneInfo& paneInfo ) const
{
	paneInfo.nTopHeight = nHeightRow0;
	if (nNumRows>1)
		paneInfo.nBottomHeight = nHeightRow0;
	paneInfo.nLeftWidth = nWidthCol0;
	if (nNumCols>1)
		paneInfo.nRightWidth = nWidthCol0;
}

void CViewDesc::Convert( C3DViewCameraDesc& camDesc ) const
{
	camDesc.SetName(name);
	C3DViewPaneInfo& paneInfo = camDesc.GetPaneInfo();
	paneinfo.Convert(paneInfo);
	for(int i=0;i<paneInfo.GetRowCount();i++)
	{
		for(int j=0;j<paneInfo.GetColCount();j++)
		{
			camDesc.GetCameraData(i, j) = *panecameras[i*2+j];
		}
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameras::CCameras()
	: DataSet( CamerasFile, 2.4f)
{
	m_userView2D.name = "USER VIEW 2D";
	m_userView3D.name = "USER VIEW 3D";
	m_userView2D.panecameras.reserve(4);
	m_userView3D.panecameras.reserve(4);
	for(int i=0; i<4; i++) {
		m_userView2D.panecameras.push_back(new C3DCamera(C3DCamera::parallel));
		m_userView3D.panecameras.push_back(new C3DCamera(C3DCamera::perspective));
	}
	m_userView2D.paneinfo.nNumRows = m_userView2D.paneinfo.nNumCols = 1;
	m_userView3D.paneinfo.nNumRows = m_userView3D.paneinfo.nNumCols = 1;
	//m_userViewType = C3DCamera::perspective; //3d

	m_defaultView2D.name = "DEFAULT VIEW 2D";
	m_defaultView3D.name = "DEFAULT VIEW 3D";
	m_defaultView2D.paneinfo.nNumRows = m_defaultView2D.paneinfo.nNumCols = 1;
	m_defaultView3D.paneinfo.nNumRows = m_defaultView3D.paneinfo.nNumCols = 1;
	m_defaultView2D.panecameras.resize(4);
	m_defaultView3D.panecameras.resize(4);
	m_defaultView2D.panecameras[0] = new C3DCamera(C3DCamera::parallel);
	m_defaultView3D.panecameras[0] = new C3DCamera(C3DCamera::perspective);
	//share single 3d camera for all panes
	for(i=1; i<4; i++) {
		m_defaultView2D.panecameras[i] = m_defaultView2D.panecameras[0];
		m_defaultView3D.panecameras[i] = m_defaultView3D.panecameras[0];
	}
	
}

CCameras::CCameras( int szType)
:DataSet( szType, 2.5f )
{
	m_userView2D.name = "USER VIEW 2D";
	m_userView3D.name = "USER VIEW 3D";
	m_userView2D.panecameras.reserve(4);
	m_userView3D.panecameras.reserve(4);
	for(int i=0; i<4; i++) {
		m_userView2D.panecameras.push_back(new C3DCamera(C3DCamera::parallel));
		m_userView3D.panecameras.push_back(new C3DCamera(C3DCamera::perspective));
	}
	m_userView2D.paneinfo.nNumRows = m_userView2D.paneinfo.nNumCols = 1;
	m_userView3D.paneinfo.nNumRows = m_userView3D.paneinfo.nNumCols = 1;
	//m_userViewType = C3DCamera::perspective; //3d

	m_defaultView2D.name = "DEFAULT VIEW 2D";
	m_defaultView3D.name = "DEFAULT VIEW 3D";
	m_defaultView2D.paneinfo.nNumRows = m_defaultView2D.paneinfo.nNumCols = 1;
	m_defaultView3D.paneinfo.nNumRows = m_defaultView3D.paneinfo.nNumCols = 1;
	m_defaultView2D.panecameras.resize(4);
	m_defaultView3D.panecameras.resize(4);
	m_defaultView2D.panecameras[0] = new C3DCamera(C3DCamera::parallel);
	m_defaultView3D.panecameras[0] = new C3DCamera(C3DCamera::perspective);
	//share single 3d camera for all panes

	for(i=1; i<4; i++) 
	{
		m_defaultView2D.panecameras[i] = m_defaultView2D.panecameras[0];
		m_defaultView3D.panecameras[i] = m_defaultView3D.panecameras[0];
	}
}

CCameras::~CCameras()
{
	
	int nCount2 = m_userView2D.panecameras.size();
	int nCount3 = m_userView3D.panecameras.size();
	for(int i=0; i<nCount2; i++ )
		delete m_userView2D.panecameras[i];
	for(i=0; i<nCount3; i++ )
		delete m_userView3D.panecameras[i];

	m_userView2D.panecameras.clear();
	m_userView3D.panecameras.clear();

	delete m_defaultView2D.panecameras[0];
	delete m_defaultView3D.panecameras[0];
	m_defaultView2D.panecameras.clear();
	m_defaultView3D.panecameras.clear();
	clear();
}

void CCameras::clear()
{
	int nSVCount = m_savedViews.size();
	for( int i=0; i<nSVCount; i++ ) {
		int nCamCount = m_savedViews[i].panecameras.size();
		for(int j=0; j<nCamCount; j++) {
			delete m_savedViews[i].panecameras[j];
		}
		m_savedViews[i].panecameras.clear();
	}
	m_savedViews.clear();
}

void CCameras::initDefaultValues()
{
	clear();

	/*
	m_savedViews.resize(2);
	for( int i=0; i<2; i++ )
	{
		m_savedViews[i].paneinfo.nNumRows =1;
		m_savedViews[i].paneinfo.nNumCols =1;
		m_savedViews[i].paneinfo.nHeightRow0 =0;
		m_savedViews[i].paneinfo.nWidthCol0 =0;
		m_savedViews[i].panecameras.resize(4);
		for(int j=0; j<4; j++) {
			m_savedViews[i].panecameras[j]= new C3DCamera(i%2?C3DCamera::parallel:C3DCamera::perspective);
		}
		m_savedViews[i].name = i%2?_T("DEFAULT 2D VIEW"):_T("DEFAULT 3D VIEW");
	}
	*/
}
/*
void CCameras::readData(ArctermFile& p_file)
{
	int nCount;
	p_file.getLine();
	p_file.getInteger(nCount);
	m_vBookmarkedCameras.reserve(nCount);
	for( int i=0; i<nCount; i++ ) //for each camera
	{
		p_file.getLine();
		C3DCamera* pCam = new C3DCamera();
		m_vBookmarkedCameras.push_back(pCam);
		pCam->ReadCamera(&p_file);
	}
	p_file.getLine();
	p_file.getInteger(nCount);
	ASSERT(nCount == 4);
	for(i=0; i<nCount; i++ ) //for each camera
	{
		p_file.getLine();
		m_userView.panecameras[i]->ReadCamera(&p_file);
	}

	p_file.getLine();
	p_file.getInteger(m_userView.paneinfo.nNumRows);
	for(i=0; i<2; i++)
		p_file.getInteger(m_userView.paneinfo.nHeightRow0);
	p_file.getLine();
	p_file.getInteger(m_userView.paneinfo.nNumCols);
	for(i=0; i<2; i++)
		p_file.getInteger(m_userView.paneinfo.nWidthCol0);
}

void CCameras::readObsoleteData (ArctermFile& p_file)
{
	int nCount;
	p_file.getLine();
	p_file.getInteger(nCount);
	for( int i=0; i<nCount; i++ ) //for each camera
	{
		p_file.getLine();
		C3DCamera* pCam = new C3DCamera();
		m_vBookmarkedCameras.push_back(pCam);
		pCam->ReadCamera(&p_file);
	}
}
*/

void CCameras::readData(ArctermFile& p_file)
{
	char buf[256];
	
	//data:
	//user view
	//saved view count
	//saved view 1
	//saved view 2
	//...
	//saved view n

	//each view data contains:
	//pane info
	//camera count
	//cam 1
	//..
	//cam n

	//user view 2d
	p_file.getLine();
	p_file.getField(buf,255);
	m_userView2D.name = buf;
	p_file.getLine();
	p_file.getInteger(m_userView2D.paneinfo.nNumRows);
	p_file.getInteger(m_userView2D.paneinfo.nHeightRow0);
	p_file.getInteger(m_userView2D.paneinfo.nNumCols);
	p_file.getInteger(m_userView2D.paneinfo.nWidthCol0);
	p_file.getLine();
	int nCount;
	p_file.getInteger(nCount); //must be 4
	ASSERT(nCount == 4);
	m_userView2D.panecameras.reserve(nCount);
	for(int i=0; i<nCount; i++ ) { //for each camera
		p_file.getLine();
		m_userView2D.panecameras[i]->ReadCamera(&p_file);
	}

	//user view 3d
	p_file.getLine();
	p_file.getField(buf,255);
	m_userView3D.name = buf;
	p_file.getLine();
	p_file.getInteger(m_userView3D.paneinfo.nNumRows);
	p_file.getInteger(m_userView3D.paneinfo.nHeightRow0);
	p_file.getInteger(m_userView3D.paneinfo.nNumCols);
	p_file.getInteger(m_userView3D.paneinfo.nWidthCol0);
	p_file.getLine();
	p_file.getInteger(nCount); //must be 4
	ASSERT(nCount == 4);
	m_userView3D.panecameras.reserve(nCount);
	for(i=0; i<nCount; i++ ) { //for each camera
		p_file.getLine();
		m_userView3D.panecameras[i]->ReadCamera(&p_file);
	}

	//saved view count
	int nSVCount;
	p_file.getLine();
	p_file.getInteger(nSVCount);
	m_savedViews.resize(nSVCount);
	for(i=0; i<nSVCount; i++) {
		p_file.getLine();
		p_file.getField(buf,255);
		m_savedViews[i].name = buf;
		p_file.getLine();
		p_file.getInteger(m_savedViews[i].paneinfo.nNumRows);
		p_file.getInteger(m_savedViews[i].paneinfo.nHeightRow0);
		p_file.getInteger(m_savedViews[i].paneinfo.nNumCols);
		p_file.getInteger(m_savedViews[i].paneinfo.nWidthCol0);
		p_file.getLine();
		p_file.getInteger(nCount); //must be 4
		ASSERT(nCount == 4);
		m_savedViews[i].panecameras.reserve(nCount);
		for(int j=0; j<nCount; j++) {
			p_file.getLine();
			C3DCamera* pCam = new C3DCamera();
			pCam->ReadCamera(&p_file);
			m_savedViews[i].panecameras.push_back(pCam);
		}
	}
}

void CCameras::readObsoleteData (ArctermFile& p_file)
{
	char buf[256];
	
	if( p_file.getVersion() < 2.3f ) {
		int nCount;
		p_file.getLine();
		p_file.getInteger(nCount);
		/*
		m_savedViews.resize(nCount+2);
		for( int i=0; i<2; i++ )
		{
			m_savedViews[i].paneinfo.nNumRows =1;
			m_savedViews[i].paneinfo.nNumCols =1;
			m_savedViews[i].paneinfo.nHeightRow0 =0;
			m_savedViews[i].paneinfo.nWidthCol0 =0;
			m_savedViews[i].panecameras.reserve(4);
			for(int j=0; j<4; j++) {
				m_savedViews[i].panecameras.push_back(new C3DCamera(i%2?C3DCamera::parallel:C3DCamera::perspective));
			}
			m_savedViews[i].name = i%2?_T("DEFAULT 2D VIEW"):_T("DEFAULT 3D VIEW");
		}
		*/
		m_savedViews.resize(nCount);
		for( int i=0; i<nCount; i++ ) //for each camera
		{
			m_savedViews[i].paneinfo.nNumRows =1;
			m_savedViews[i].paneinfo.nNumCols =1;
			m_savedViews[i].paneinfo.nHeightRow0 =0;
			m_savedViews[i].paneinfo.nWidthCol0 =0;
			m_savedViews[i].panecameras.reserve(4);
			p_file.getLine();
			C3DCamera* pCam = new C3DCamera();
			pCam->ReadCamera(&p_file);
			m_savedViews[i].panecameras.push_back(pCam);
			m_savedViews[i].name = _T("Saved ") + pCam->GetName();
			for(int j=1; j<4; j++) {
				m_savedViews[i].panecameras.push_back(new C3DCamera(*pCam));
			}
		}
	}
	else { //2.3
		p_file.getLine();
		p_file.getField(buf,255);
		m_userView3D.name = buf;
		p_file.getLine();
		p_file.getInteger(m_userView3D.paneinfo.nNumRows);
		p_file.getInteger(m_userView3D.paneinfo.nHeightRow0);
		p_file.getInteger(m_userView3D.paneinfo.nNumCols);
		p_file.getInteger(m_userView3D.paneinfo.nWidthCol0);
		p_file.getLine();
		int nCount;
		p_file.getInteger(nCount); //must be 4
		ASSERT(nCount == 4);
		m_userView3D.panecameras.reserve(nCount);
		for(int i=0; i<nCount; i++ ) { //for each camera
			p_file.getLine();
			m_userView3D.panecameras[i]->ReadCamera(&p_file);
		}

		//saved view count
		int nSVCount;
		p_file.getLine();
		p_file.getInteger(nSVCount);
		//m_savedViews.resize(nSVCount+2);
		m_savedViews.resize(nSVCount);
		/*
		for( i=0; i<2; i++ ) 
		{
			m_savedViews[i].paneinfo.nNumRows =1;
			m_savedViews[i].paneinfo.nNumCols =1;
			m_savedViews[i].paneinfo.nHeightRow0 =0;
			m_savedViews[i].paneinfo.nWidthCol0 =0;
			m_savedViews[i].panecameras.reserve(4);
			m_savedViews[i].panecameras.push_back(new C3DCamera(i%2?C3DCamera::parallel:C3DCamera::perspective));
			m_savedViews[i].name = i%2?_T("DEFAULT 2D VIEW"):_T("DEFAULT 3D VIEW");
			for(int j=1; j<4; j++) {
				m_savedViews[i].panecameras.push_back(new C3DCamera(i%2?C3DCamera::parallel:C3DCamera::perspective));
			}
		}
		*/
		for(i=0; i<nSVCount; i++) {//for each camera
			p_file.getLine();
			p_file.getField(buf,255);
			m_savedViews[i].name = buf;
			p_file.getLine();
			p_file.getInteger(m_savedViews[i].paneinfo.nNumRows);
			p_file.getInteger(m_savedViews[i].paneinfo.nHeightRow0);
			p_file.getInteger(m_savedViews[i].paneinfo.nNumCols);
			p_file.getInteger(m_savedViews[i].paneinfo.nWidthCol0);
			p_file.getLine();
			p_file.getInteger(nCount); //must be 4 or less
			ASSERT(nCount == m_savedViews[i].paneinfo.nNumRows*m_savedViews[i].paneinfo.nNumCols);
			ASSERT(nCount <= 4);
			m_savedViews[i].panecameras.reserve(4);
			for(int j=0; j<nCount; j++) {
				p_file.getLine();
				C3DCamera* pCam = new C3DCamera();
				pCam->ReadCamera(&p_file);
				m_savedViews[i].panecameras.push_back(pCam);
			}
			for(j=nCount; j<4; j++) {
				m_savedViews[i].panecameras.push_back(new C3DCamera(*(m_savedViews[i].panecameras[nCount-1])));
			}
		}

	}
}

void CCameras::writeData(ArctermFile& p_file) const
{
	p_file.writeField(m_userView2D.name);
	p_file.writeLine();
	p_file.writeInt(m_userView2D.paneinfo.nNumRows);
	p_file.writeInt(m_userView2D.paneinfo.nHeightRow0);
	p_file.writeInt(m_userView2D.paneinfo.nNumCols);
	p_file.writeInt(m_userView2D.paneinfo.nWidthCol0);
	p_file.writeLine();
	int nCount = m_userView2D.panecameras.size();
	ASSERT(nCount == 4);
	p_file.writeInt(nCount);
	p_file.writeLine();
	for(int i=0; i<nCount; i++ ) //for each camera
	{
		ASSERT(m_userView2D.panecameras[i]!=NULL);
		m_userView2D.panecameras[i]->WriteCamera(&p_file);
		p_file.writeLine();
	}

	p_file.writeField(m_userView3D.name);
	p_file.writeLine();
	p_file.writeInt(m_userView3D.paneinfo.nNumRows);
	p_file.writeInt(m_userView3D.paneinfo.nHeightRow0);
	p_file.writeInt(m_userView3D.paneinfo.nNumCols);
	p_file.writeInt(m_userView3D.paneinfo.nWidthCol0);
	p_file.writeLine();
	nCount = m_userView3D.panecameras.size();
	ASSERT(nCount == 4);
	p_file.writeInt(nCount);
	p_file.writeLine();
	for(i=0; i<nCount; i++ ) //for each camera
	{
		ASSERT(m_userView3D.panecameras[i]!=NULL);
		m_userView3D.panecameras[i]->WriteCamera(&p_file);
		p_file.writeLine();
	}

	int nSVCount = m_savedViews.size();
	p_file.writeInt(nSVCount);
	p_file.writeLine();
	for(i=0; i<nSVCount; i++) {
		p_file.writeField(m_savedViews[i].name);
		p_file.writeLine();
		p_file.writeInt(m_savedViews[i].paneinfo.nNumRows);
		p_file.writeInt(m_savedViews[i].paneinfo.nHeightRow0);
		p_file.writeInt(m_savedViews[i].paneinfo.nNumCols);
		p_file.writeInt(m_savedViews[i].paneinfo.nWidthCol0);
		p_file.writeLine();
		nCount = m_savedViews[i].panecameras.size();
		p_file.writeInt(nCount);
		p_file.writeLine();
		for(int j=0; j<nCount; j++) {
			ASSERT(m_savedViews[i].panecameras[j] != NULL);
			m_savedViews[i].panecameras[j]->WriteCamera(&p_file);
			p_file.writeLine();
		}
	}	
}

void CCameras::InitDefault( const ARCBoundingSphere& bs )
{
	C3DCamera * pCamera2D  = m_defaultView2D.panecameras[0];

	if( pCamera2D )
	{
		ARCVector3 vLook = -ARCVector3(0,0,1);
		ARCVector3 vEye = bs.GetLocation() -  ARCVector3(0,0,bs.GetRadius());
		
		pCamera2D->SetCamera(vEye,vLook,ARCVector3(0,1,0));
		for(int i=1; i<4; i++) 
		{
			m_defaultView2D.panecameras[i] = m_defaultView2D.panecameras[0];			
		}
	}

	C3DCamera * pCamera3D = m_defaultView3D.panecameras[0];
	if(pCamera3D)
	{
		ARCVector3 vLook = ARCVector3(0,1,-0.6);
		ARCVector3 vEye = bs.GetLocation() + ARCVector3(0,-1,0.6) * bs.GetRadius();
		pCamera3D->SetCamera(vEye,vLook,ARCVector3(0,1,0));
		for(int i=1; i<4; i++) 
		{
			m_defaultView3D.panecameras[i] = m_defaultView3D.panecameras[0];			
		}
	}

	for(int i=0; i<4; i++) {
		*m_userView2D.panecameras[i] = * pCamera2D;
		*m_userView3D.panecameras[i] = * pCamera3D; 
	}

	
}

void CCameras::Convert( C3DViewCameras& cams ) const
{
	// cams.m_b3DMode = true; // ignored
	m_userView2D.Convert(cams.m_userCamera2D);
	m_userView3D.Convert(cams.m_userCamera3D);
	size_t nCount = m_savedViews.size();
	cams.m_savedCameras.resize(nCount);
	for(size_t i=0;i<nCount;i++)
	{
		m_savedViews[i].Convert(cams.m_savedCameras[i]);
	}
}
/*
int CCameras::AddCameraCopy(const C3DCamera& _cam, const CString& _sName)
{
	C3DCamera* pCamCopy = new C3DCamera(_cam);
	pCamCopy->SetName(_sName);
	m_vCameras.push_back(pCamCopy);
	return m_vCameras.size()-1;
}
*/
