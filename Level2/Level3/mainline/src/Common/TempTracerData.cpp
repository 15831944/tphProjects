// TempTracerData.cpp: implementation of the CTempTracerData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "termplan.h"
#include "TempTracerData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempTracerData::CTempTracerData()
{

}

CTempTracerData::~CTempTracerData()
{
	clear();
}

void CTempTracerData::clear()
{
	//m_vDispPropIdx.clear();
	//m_vTracks.clear();
	m_vItems.clear();
}

int CTempTracerData::GetTrackCount()
{
	return m_vItems.size();
}

int CTempTracerData::GetDispPropIdx(int nTrackIdx)
{
	return m_vItems[nTrackIdx].iDispPropIdx;
}

std::vector<CTrackerVector3>& CTempTracerData::GetTrack(int nTrackIdx)
{	
	return m_vItems[nTrackIdx].vTracks;
}

const std::vector<CTrackerVector3>& CTempTracerData::GetTrack( int nTrackIdx ) const
{
	return m_vItems[nTrackIdx].vTracks;
}

const COLORREF CTempTracerData::GetColor(int nTrackIdx ) const
{
	return m_vItems[nTrackIdx].mColor;
}

const int CTempTracerData::GetLineType(int nTrackIdx ) const
{
	return m_vItems[nTrackIdx].mLineType;
}


int CTempTracerData::AddTrack(int nHits, int nDPIdx, int nPaxId)
{
	int nCount = m_vItems.size();
	/*m_vTracks.resize(nCount+1);
	m_vDispPropIdx.push_back(nDPIdx);
	ASSERT(m_vTracks.size()==m_vDispPropIdx.size());
	ASSERT(m_vTracks[nCount].size() == 0);
	m_vTracks[nCount].resize(nHits);
	ASSERT(m_vTracks[nCount].size() == nHits);*/
	m_vItems.resize(nCount+1);
	m_vItems[nCount].vTracks.resize(nHits);
	m_vItems[nCount].iDispPropIdx = nDPIdx;
	m_vItems[nCount].nPaxId= nPaxId;
	return nCount;
}

int CTempTracerData::AddTrack(int nHits, int nDPIdx, int nPaxId,COLORREF nColor,int nLineType)
{
	int nCount = m_vItems.size();
	/*m_vTracks.resize(nCount+1);
	m_vDispPropIdx.push_back(nDPIdx);
	ASSERT(m_vTracks.size()==m_vDispPropIdx.size());
	ASSERT(m_vTracks[nCount].size() == 0);
	m_vTracks[nCount].resize(nHits);
	ASSERT(m_vTracks[nCount].size() == nHits);*/
	m_vItems.resize(nCount+1);
	m_vItems[nCount].vTracks.resize(nHits);
	m_vItems[nCount].iDispPropIdx = nDPIdx;
	m_vItems[nCount].nPaxId= nPaxId;
	m_vItems[nCount].mColor=nColor;
	m_vItems[nCount].mLineType=nLineType;
	return nCount;
}

int CTempTracerData::GetPaxID( int nTrackIdx )
{
	return m_vItems[nTrackIdx].nPaxId;
}

DistanceUnit CTrackerVector3::DistanceTo( const CTrackerVector3& _rhs ) const
{
	DistanceUnit length;
	length = sqrt((this->x-_rhs.x)*(this->x-_rhs.x)+(this->y-_rhs.y)*(this->y-_rhs.y)+(this->z-_rhs.z)*(this->z-_rhs.z));
	return length;
}
