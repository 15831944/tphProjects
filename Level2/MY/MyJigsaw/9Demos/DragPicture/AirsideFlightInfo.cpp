#include "stdafx.h"
#include "Macros.h"
#include "AirsideFlightInfo.h"

////////////////////////////////////////////////////////////////////////////////

PointFXY::PointFXY( float x, float y ): m_x(x), m_y(y)
{

}

////////////////////////////////////////////////////////////////////////////////
ClassAirsidePassenger::ClassAirsidePassenger()
{

}

ClassAirsidePassenger::~ClassAirsidePassenger()
{

}

// Copy information from another AirsidePassenger excluding its D2D info.
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
    return true;
}

bool ClassAirsidePassenger::CanAboveMe( ClassAirsidePassenger* pOther )
{
    return true;
}

bool ClassAirsidePassenger::CanBelowMe( ClassAirsidePassenger* pOther )
{
    return true;
}

bool ClassAirsidePassenger::CanLeftsideMe( ClassAirsidePassenger* pOther )
{
    return true;
}

bool ClassAirsidePassenger::CanRightsideMe( ClassAirsidePassenger* pOther )
{
    return true;
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
    m_vAirsidePassengers.clear();
}

PointFXY ClassAirsideFlight::GetRotationCenter()
{
    float fX = static_cast<float>(m_xLength)/2*AIRSIDEPASSENGER_WIDTH;
    float fY = static_cast<float>(m_yLength)/2*AIRSIDEPASSENGER_WIDTH;
    PointFXY pt(m_pointTopLeft.GetX()+fX, m_pointTopLeft.GetY()+fY);
    return pt;
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

// Can this AirsideFlight be combined to me?
bool ClassAirsideFlight::CanCombinedToMe(int& errorCode, ClassAirsideFlight* pOther)
{
    // Condition 1:
    // The rotation difference between the two AirsideFlight must be not greater than MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE.
    if((m_rotation - pOther->GetRotation()) > MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 2:
    // The x coordinate difference between the two AirsideFlight must be multiple of piece's length.
    float xOffset = m_pointTopLeft.GetX() - pOther->m_pointTopLeft.GetX();
    int n = (int)(xOffset/AIRSIDEPASSENGER_WIDTH);
    float xPieceOffset = xOffset - AIRSIDEPASSENGER_WIDTH*n;
    if(xPieceOffset > MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 3:
    // The y coordinate difference between the two AirsideFlight must be multiple of piece's length.
    float yOffset = m_pointTopLeft.GetY() - pOther->m_pointTopLeft.GetY();
    n = (int)(yOffset/AIRSIDEPASSENGER_WIDTH);
    float yPieceOffset = yOffset - AIRSIDEPASSENGER_WIDTH*n;
    if(yPieceOffset > MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 4:
    // There is no AirsidePassengers in conflict after two AirsideFlight are combined.
    ClassAirsideFlight airsideFlight;
    float meToNewXOffset = m_pointTopLeft.GetX() - MIN(m_pointTopLeft.GetX(), pOther->m_pointTopLeft.GetX());
    float meToNewYOffset = m_pointTopLeft.GetY() - MIN(m_pointTopLeft.GetY(), pOther->m_pointTopLeft.GetY());
    float otherToNewXOffset = pOther->m_pointTopLeft.GetX() - MIN(m_pointTopLeft.GetX(), pOther->m_pointTopLeft.GetX());
    float otherToNewYOffset = pOther->m_pointTopLeft.GetY() - MIN(m_pointTopLeft.GetY(), pOther->m_pointTopLeft.GetY());

    size_t nCount = m_vAirsidePassengers.size();
    size_t nOtherCount = pOther->GetAirsidePassengerCount();
    
    bool bAirsidePassengerAdjacent = false;
    for(size_t i=0; i<nCount; i++)
    {
        for(size_t j=0; j<nOtherCount; j++)
        {
            ClassAirsidePassenger* pAirsidePassenger = m_vAirsidePassengers.at(i);
            ClassAirsidePassenger* pOtherAirsidePassenger = pOther->m_vAirsidePassengers.at(j);

            short myNewXInAirsideFlight = pAirsidePassenger->GetXInAirsideFlight() - static_cast<short>(meToNewXOffset/AIRSIDEPASSENGER_WIDTH);
            short myNewYInAirsideFlight = pAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(meToNewYOffset/AIRSIDEPASSENGER_WIDTH);
            short otherNewXInAirsideFlight = pOtherAirsidePassenger->GetXInAirsideFlight() - static_cast<short>(otherToNewXOffset/AIRSIDEPASSENGER_WIDTH);
            short otherNewYInAirsideFlight = pOtherAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(otherToNewYOffset/AIRSIDEPASSENGER_WIDTH);

            if(myNewXInAirsideFlight == otherNewXInAirsideFlight && myNewYInAirsideFlight == otherNewYInAirsideFlight)
            {
                // Conflict: two AirsidePassengers overlapped, these two AirsideFlight can not be combined.
                return false;
            }
            else if(myNewXInAirsideFlight == otherNewXInAirsideFlight &&
                    myNewYInAirsideFlight - otherNewYInAirsideFlight == 1)
            {
                // Conflict: There are two AirsidePassengers be adjacent to each other, 
                // but the edge is not fit, so these two AirsideFlight can not be combined.
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanAboveMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewXInAirsideFlight == otherNewXInAirsideFlight &&
                    myNewYInAirsideFlight - otherNewYInAirsideFlight == -1)
            {
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanBelowMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewYInAirsideFlight == otherNewYInAirsideFlight &&
                    myNewXInAirsideFlight - otherNewXInAirsideFlight == 1)
            {
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanRightsideMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewYInAirsideFlight == otherNewYInAirsideFlight &&
                    myNewXInAirsideFlight - otherNewXInAirsideFlight == -1)
            {
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanLeftsideMe(pOtherAirsidePassenger))
                    return false;
            }
        }
    }

    // Condition 5:
    // No piece be adjacent to another, can't be combined.
    if(!bAirsidePassengerAdjacent)
    {
        return false;
    }

    // Well, all OK, can be combined, return true.
    return true;
}

// Combine an other AirsideFlight to this one.
void ClassAirsideFlight::CombineToMe(int& errorCode, ClassAirsideFlight* pOther)
{
    float meToNewXOffset = m_pointTopLeft.GetX() - MIN(m_pointTopLeft.GetX(), pOther->m_pointTopLeft.GetX());
    float meToNewYOffset = m_pointTopLeft.GetY() - MIN(m_pointTopLeft.GetY(), pOther->m_pointTopLeft.GetY());
    float otherToNewXOffset = pOther->m_pointTopLeft.GetX() - MIN(m_pointTopLeft.GetX(), pOther->m_pointTopLeft.GetX());
    float otherToNewYOffset = pOther->m_pointTopLeft.GetY() - MIN(m_pointTopLeft.GetY(), pOther->m_pointTopLeft.GetY());

    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        ClassAirsidePassenger* pAirsidePassenger = m_vAirsidePassengers.at(i);

        short myNewXInAirsideFlight = pAirsidePassenger->GetXInAirsideFlight() - static_cast<short>(meToNewXOffset/AIRSIDEPASSENGER_WIDTH);
        pAirsidePassenger->SetXInAirsideFlight(myNewXInAirsideFlight);
        if(myNewXInAirsideFlight>m_xLength)
        {
            m_xLength = myNewXInAirsideFlight;
        }

        short myNewYInAirsideFlight = pAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(meToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pAirsidePassenger->SetYInAirsideFlight(myNewYInAirsideFlight);
        if(myNewYInAirsideFlight>m_yLength)
        {
            m_yLength = myNewYInAirsideFlight;
        }
    }

    size_t nOtherCount = pOther->GetAirsidePassengerCount();
    for(size_t i=0; i<nOtherCount; i++)
    {
        ClassAirsidePassenger* pOtherAirsidePassenger = pOther->m_vAirsidePassengers.at(i);

        short otherNewXInAirsideFlight = pOtherAirsidePassenger->GetXInAirsideFlight() - static_cast<short>(otherToNewXOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetXInAirsideFlight(otherNewXInAirsideFlight);
        if(otherNewXInAirsideFlight>m_xLength)
        {
            m_xLength = otherNewXInAirsideFlight;
        }

        short otherNewYInAirsideFlight = pOtherAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(otherToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetYInAirsideFlight(otherNewYInAirsideFlight);
        if(otherNewYInAirsideFlight>m_yLength)
        {
            m_yLength = otherNewYInAirsideFlight;
        }
    }
    m_vAirsidePassengers.insert(m_vAirsidePassengers.begin(), pOther->m_vAirsidePassengers.begin(), pOther->m_vAirsidePassengers.end());
    pOther->Clear();
    return;
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
    return true;
}

void ClassAirsideFlightManager::TopThisAirsideFlight(long& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    ClassAirsideFlight* pIterator = m_pTopAirsideFlight;
    if(pAirsideFlight == pIterator) // The input AirsideFlight is the top one, nothing to change.
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
            {
                errorCode = 1; // No AirsideFlight fits the input one is found.
                return;
            }
            if(pIterator == pAirsideFlight)
            {
                // An AirsideFlight fits the input one is found, and top it.
                pIterator->GetPrevAirsideFlight()->SetNextAirsideFlight(pIterator->GetNextAirsideFlight());
                pIterator->SetNextAirsideFlight(m_pTopAirsideFlight);
                m_pTopAirsideFlight = pIterator;
                errorCode = 0;
                return ;
            }
        }
    }
}
