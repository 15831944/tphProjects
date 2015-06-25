#include "stdafx.h"
#include "AirsideFlightInfo.h"


////////////////////////////////////////////////////////////////////////////////

ClassAirsidePassenger::ClassAirsidePassenger()
{

}

ClassAirsidePassenger::~ClassAirsidePassenger()
{

}

void ClassAirsidePassenger::PartialCopyFrom(ClassAirsidePassenger* pOther)
{
    m_pointTopLeft = pOther->m_pointTopLeft;
    m_rotation = pOther->m_rotation;
    m_xInAirsideFlight = pOther->m_xInAirsideFlight;
    m_yInAirsideFlight = pOther->m_yInAirsideFlight;

    m_leftEdgeType = pOther->m_leftEdgeType;
    m_rightEdgeType = pOther->m_rightEdgeType;
    m_topEdgeType = pOther->m_topEdgeType;
    m_bottomEdgeType = pOther->m_bottomEdgeType;

    m_pLeft = pOther->m_pLeft;
    m_pRight = pOther->m_pRight;
    m_pTop = pOther->m_pTop;
    m_pBottom = pOther->m_pBottom;
}

bool ClassAirsidePassenger::IsThisPointFXYInsideMe( const PointFXY& pt )
{

}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

ClassAirsideFlight::ClassAirsideFlight()
{

}

ClassAirsideFlight::~ClassAirsideFlight()
{

}

void ClassAirsideFlight::Clear()
{
    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=nCount-1; i>0; i--)
    {
        delete m_vAirsidePassengers.back();
        m_vAirsidePassengers.pop_back();
    }
}

PointFXY ClassAirsideFlight::GetRotationCenter()
{

}

// Is the point in any one AirsidePassengers of this AirsideFlight?
bool ClassAirsideFlight::IsThisPointFXYInsideMe(const PointFXY& pt)
{
    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(m_vAirsidePassengers.at(i)->IsThisPointFXYInsideMe(pt))
            return true;
    }
    return false;
}

// Can this AirsideFlight join to me?
bool ClassAirsideFlight::CanJoinToMe(int& errorCode, ClassAirsideFlight* pOther)
{
    // Condition 1:
    // The rotation difference between the two AirsideFlight is 
    // not greater than MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE.
    if((m_rotation - pOther->GetRotation()) > MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 2:
    // The x and y coordinate difference between the two AirsideFlight is 
    // not greater than MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE
    float xOffset = m_pointTopLeft.m_x - pOther->m_pointTopLeft.m_x;
    float yOffset = m_pointTopLeft.m_y - pOther->m_pointTopLeft.m_y;
    if(xOffset > MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE ||
       yOffset > MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 3:
    // There is not any AirsidePassengers in conflict after Two AirsideFlight joined.
    ClassAirsideFlight airsideFlight;
    ClassAirsidePassenger airsidePax;
    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        airsidePax.PartialCopyFrom(m_vAirsidePassengers.at(i));
        airsideFlight.
    }
}

// Join an other AirsideFlight to this one.
// !!Dangerous!! this operation will delete 'pOther'.
ClassAirsideFlight* ClassAirsideFlight::JoinToMe(int& errorCode, ClassAirsideFlight* pOther)
{

}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

ClassAirsideFlightManager::ClassAirsideFlightManager()
{

}

ClassAirsideFlightManager::~ClassAirsideFlightManager()
{

}

bool ClassAirsideFlightManager::WorkIsDone()
{

}

void ClassAirsideFlightManager::TopThisAirsideFlight(long& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    ClassAirsideFlight* pIterator = m_pTopAirsideFlight;
    if(pAirsideFlight == pIterator) // This AirsideFlight is the top one, nothing to change.
    {
        errorCode = 0;
        return;
    }
    else
    {
        while(true)
        {
            pIterator = pIterator->GetNextAirsideFlight();
            if(pIterator == NULL)
                errorCode = 1; // No AirsideFlight fits the input one is found.
            if(pIterator == pAirsideFlight)
            {
                // An AirsideFlight fits the input one is found, and top it.
                pIterator->GetPrevAirsideFlight()->SetNextAirsideFlight(pIterator->GetNextAirsideFlight());
                pIterator->SetNextAirsideFlight(m_pTopAirsideFlight);
                m_pTopAirsideFlight = pIterator;
                return 0;
            }
        }
    }
}
