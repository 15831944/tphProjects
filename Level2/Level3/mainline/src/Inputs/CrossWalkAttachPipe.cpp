#include "StdAfx.h"
#include ".\crosswalkattachpipe.h"
#include "..\Landside\InputLandside.h"
#include "..\Landside\LandsideCrosswalk.h"
#include "../Main/TermPlanDoc.h"

CrossWalkAttachPipe::CrossWalkAttachPipe(InputLandside* pInputLandside)
{
	m_groupIndex.start = -1;
	m_groupIndex.end = -1;

	m_pCrossWalk = NULL;
	m_pInputLanside = pInputLandside;

}

CrossWalkAttachPipe::~CrossWalkAttachPipe(void)
{
}


void CrossWalkAttachPipe::SetGroupIndex( const GroupIndex& groupIndex )
{
	m_groupIndex = groupIndex;
}

const GroupIndex& CrossWalkAttachPipe::GetGroupIndex() const
{
	return m_groupIndex;
}

void CrossWalkAttachPipe::AttachCrossWalk( LandsideCrosswalk* pCrossWalk )
{
	ASSERT(pCrossWalk);
	m_pCrossWalk = pCrossWalk;
}

LandsideCrosswalk* CrossWalkAttachPipe::GetAttachWalk() const
{
	return m_pCrossWalk;
}

void CrossWalkAttachPipe::ReadData( ArctermFile& p_file )
{
	//retrieve groupindex
	p_file.getInteger(m_groupIndex.start);
	p_file.getInteger(m_groupIndex.end);

	//retrieve landside crosswalk
	int nObjectID = -1;
	p_file.getInteger(nObjectID);
	ASSERT(m_pInputLanside);
	if (m_pInputLanside)
	{
		m_pCrossWalk = (LandsideCrosswalk*)(m_pInputLanside->getObjectList().getObjectByID(nObjectID));
	}
}

void CrossWalkAttachPipe::WriteData( ArctermFile& p_file ) const
{
	//write groupindex
	p_file.writeInt(m_groupIndex.start);
	p_file.writeInt(m_groupIndex.end);

	//write crosswalk id
	p_file.writeInt(m_pCrossWalk->getID());
}

bool CrossWalkAttachPipe::operator==( const CrossWalkAttachPipe& crossPipe ) const
{
	if (m_pCrossWalk == crossPipe.m_pCrossWalk)
	{
		return true;
	}
	return false;
}

bool CrossWalkAttachPipe::OverlapCrosswalk( double dIdx,double dDest ) const
{
	double dMIn = MIN(dIdx,dDest);
	double dMax = MAX(dIdx,dDest);

	int iCrossStart = MIN(m_groupIndex.start,m_groupIndex.end);
	int iCrossEnd = MAX(m_groupIndex.start,m_groupIndex.end);

	if (dMIn > iCrossEnd || dMax < iCrossStart)
	{
		return false;
	}

	return true;
}

bool CrossWalkAttachPipe::InCrossWalk( double dIdx ) const
{
	int iCrossStart = MIN(m_groupIndex.start,m_groupIndex.end);
	int iCrossEnd = MAX(m_groupIndex.start,m_groupIndex.end);

	if (dIdx > iCrossEnd || dIdx < iCrossStart)
	{
		return false;
	}

	return true;
}
