#include "StdAfx.h"
#include "Clearance.h"
#include <limits>

void Clearance::ClearItems( int idx/*=0*/ )
{
	m_vItems.erase(m_vItems.begin()+idx,m_vItems.end());
}

void Clearance::ClearAllItems()
{
	m_vItems.clear();
}

void Clearance::RemoveItem(int idx)
{
	m_vItems.erase(m_vItems.begin()+idx);
}

DistanceUnit Clearance::GetLength( int ibegin, int iend ) const
{	
	ClearanceItem lastItem = ItemAt(ibegin);
	DistanceUnit dDist = 0;
	for(int i = ibegin+1;i<iend;i++)
	{		
		const ClearanceItem& thisItem = ItemAt(i);

		dDist += lastItem.DistanceTo(thisItem);

		lastItem = thisItem;
	}

	return dDist;
}

void Clearance::AddClearance( const Clearance& nextclearance )
{
	m_vItems.insert(m_vItems.end(),nextclearance.m_vItems.begin(), nextclearance.m_vItems.end());
}


std::ostream& operator<<( std::ostream& s, const Clearance& clearance )
{
	for(int i=0;i< clearance.GetItemCount();i++)
	{
		s << clearance.ItemAt(i);
		s << std::endl;
	}
	s << std::endl;
	return s;
}


ClearanceItem::ClearanceItem( AirsideResource * pRes, AirsideMobileElementMode fltmode , DistanceUnit dist/*=0 */ )
{
	m_pResource = pRes;
	m_nMode = fltmode;
	m_dDist = dist;
	m_dAlt = 0;
	m_vSpeed = 0;
	m_tDelayReseaon = FltDelayReason_Unknown;

	m_acc = 0.0;
	m_offsetAngle = 0;
	m_nDelayId = 0;
	m_bWirteLog = true;
	m_bPushback = false;
}

CPoint2008 ClearanceItem::GetPosition() const
{
	CPoint2008 reslt;

	if(m_pResource && (m_pResource->GetType() == AirsideResource::ResType_AirRouteSegment 
		|| m_pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment))
		reslt = m_pos;
	else if(m_pResource && m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
		reslt = m_pos;	
	else if(m_pResource)
		reslt = m_pResource->GetDistancePoint(m_dDist);

	//reslt.setZ(m_dAlt);
	return reslt;
}

DistanceUnit ClearanceItem::DistanceTo( const ClearanceItem& nextItem ) const
{
	if(m_pResource)
	{
		if(nextItem.GetResource() == m_pResource)
		{
			return nextItem.m_dDist - m_dDist;
		}
		else if(nextItem.GetResource())
		{
			return nextItem.GetPosition().distance( GetPosition());
		}
	}
	else
	{
		if(nextItem.GetResource())
		{
			return nextItem.m_dDist;
		}
	}
	return 0;	
}

AirsideResource * ClearanceItem::GetResource() const
{
	if(GetMode() == OnTerminate)
		return NULL;

	return (AirsideResource*)m_pResource;
}

void ClearanceItem::WriteLog( bool b )
{
	m_bWirteLog = b;
}

std::ostream& operator<<( std::ostream& os, const ClearanceItem& cItem )
{
	os<<AIRSIDEMOBILE_MODE_NAME[cItem.GetMode()]<<"	";
	if(cItem.GetResource())
	{
		os<<cItem.GetResource()->GetTypeName();
		os<<"	";
		os<<cItem.GetResource()->PrintResource();
		os<<"	";
	}
	
	os << cItem.m_dDist <<"	 ";

	os << cItem.GetPosition() << "	";

	os<< cItem.m_dAlt <<"	";
	long timeL = (long)cItem.GetTime();
	os<<cItem.GetTime()<<"("<< long(cItem.GetTime())<<")" <<"	";

	os<< cItem.m_vSpeed << "	";

	os<< cItem.GetOffsetAngle() << "	";
	
	return os;
}
