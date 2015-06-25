#include "stdafx.h"
#include "Macros.h"
#include "AirsideFlightInfo.h"
#include "CommonOperations.h"

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
    m_nLine = pOther->m_nLine;
    m_nCol = pOther->m_nCol;

    m_leftEdgeType = pOther->m_leftEdgeType;
    m_rightEdgeType = pOther->m_rightEdgeType;
    m_topEdgeType = pOther->m_topEdgeType;
    m_bottomEdgeType = pOther->m_bottomEdgeType;

    m_pLeft = pOther->m_pLeft;
    m_pRight = pOther->m_pRight;
    m_pTop = pOther->m_pTop;
    m_pBottom = pOther->m_pBottom;
}

bool ClassAirsidePassenger::CanAboveMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerEdgeType curTopEdgeType;
    if(m_rotation == 0)
    {
        curTopEdgeType = m_topEdgeType;
    }
    else if(m_rotation == 90)
    {
        curTopEdgeType = m_rightEdgeType;
    }
    else if(m_rotation == 180)
    {
        curTopEdgeType = m_bottomEdgeType;
    }
    else if(m_rotation == 270)
    {
        curTopEdgeType = m_leftEdgeType;
    }

    if(curTopEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerEdgeType otherCurBottomEdgeType;
    if(pOther->m_rotation == 0)
    {
        otherCurBottomEdgeType = pOther->m_bottomEdgeType;
    }
    else if(pOther->m_rotation == 90)
    {
        otherCurBottomEdgeType = pOther->m_leftEdgeType;
    }
    else if(pOther->m_rotation == 180)
    {
        otherCurBottomEdgeType = pOther->m_topEdgeType;
    }
    else if(pOther->m_rotation == 270)
    {
        otherCurBottomEdgeType = pOther->m_rightEdgeType;
    }

    if(otherCurBottomEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    if((curTopEdgeType == AirsidePassengerEdgeType_Convex && otherCurBottomEdgeType == AirsidePassengerEdgeType_Concave) ||
        (curTopEdgeType == AirsidePassengerEdgeType_Concave && otherCurBottomEdgeType == AirsidePassengerEdgeType_Convex))
    {
            return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanBelowMe( ClassAirsidePassenger* pOther )
{
    AirsidePassengerEdgeType curBottomEdgeType;
    if(m_rotation == 0)
    {
        curBottomEdgeType = m_bottomEdgeType;
    }
    else if(m_rotation == 90)
    {
        curBottomEdgeType = m_leftEdgeType;
    }
    else if(m_rotation == 180)
    {
        curBottomEdgeType = m_topEdgeType;
    }
    else if(m_rotation == 270)
    {
        curBottomEdgeType = m_rightEdgeType;
    }

    if(curBottomEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerEdgeType otherCurTopEdgeType;
    if(pOther->m_rotation == 0)
    {
        otherCurTopEdgeType = pOther->m_topEdgeType;
    }
    else if(pOther->m_rotation == 90)
    {
        otherCurTopEdgeType = pOther->m_rightEdgeType;
    }
    else if(pOther->m_rotation == 180)
    {
        otherCurTopEdgeType = pOther->m_bottomEdgeType;
    }
    else if(pOther->m_rotation == 270)
    {
        otherCurTopEdgeType = pOther->m_leftEdgeType;
    }

    if(otherCurTopEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    if((curBottomEdgeType == AirsidePassengerEdgeType_Convex && otherCurTopEdgeType == AirsidePassengerEdgeType_Concave) ||
        (curBottomEdgeType == AirsidePassengerEdgeType_Concave && otherCurTopEdgeType == AirsidePassengerEdgeType_Convex))
    {
        // Convex and concave edge can be combined.
        return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanLeftsideMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerEdgeType curLeftEdgeType;
    if(m_rotation == 0)
    {
        curLeftEdgeType = m_leftEdgeType;
    }
    else if(m_rotation == 90)
    {
        curLeftEdgeType = m_topEdgeType;
    }
    else if(m_rotation == 180)
    {
        curLeftEdgeType = m_rightEdgeType;
    }
    else if(m_rotation == 270)
    {
        curLeftEdgeType = m_bottomEdgeType;
    }

    if(curLeftEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerEdgeType otherCurRightEdgeType;
    if(pOther->m_rotation == 0)
    {
        otherCurRightEdgeType = pOther->m_rightEdgeType;
    }
    else if(pOther->m_rotation == 90)
    {
        otherCurRightEdgeType = pOther->m_bottomEdgeType;
    }
    else if(pOther->m_rotation == 180)
    {
        otherCurRightEdgeType = pOther->m_leftEdgeType;
    }
    else if(pOther->m_rotation == 270)
    {
        otherCurRightEdgeType = pOther->m_topEdgeType;
    }

    if(otherCurRightEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    if((curLeftEdgeType == AirsidePassengerEdgeType_Convex && otherCurRightEdgeType == AirsidePassengerEdgeType_Concave) ||
        (curLeftEdgeType == AirsidePassengerEdgeType_Concave && otherCurRightEdgeType == AirsidePassengerEdgeType_Convex))
    {
        // Convex and concave edge can be combined.
        return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanRightsideMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerEdgeType curRightEdgeType;
    if(m_rotation == 0)
    {
        curRightEdgeType = m_rightEdgeType;
    }
    else if(m_rotation == 90)
    {
        curRightEdgeType = m_bottomEdgeType;
    }
    else if(m_rotation == 180)
    {
        curRightEdgeType = m_leftEdgeType;
    }
    else if(m_rotation == 270)
    {
        curRightEdgeType = m_topEdgeType;
    }

    if(curRightEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerEdgeType otherCurLeftEdgeType;
    if(pOther->m_rotation == 0)
    {
        otherCurLeftEdgeType = pOther->m_leftEdgeType;
    }
    else if(pOther->m_rotation == 90)
    {
        otherCurLeftEdgeType = pOther->m_topEdgeType;
    }
    else if(pOther->m_rotation == 180)
    {
        otherCurLeftEdgeType = pOther->m_rightEdgeType;
    }
    else if(pOther->m_rotation == 270)
    {
        otherCurLeftEdgeType = pOther->m_bottomEdgeType;
    }

    if(otherCurLeftEdgeType == AirsidePassengerEdgeType_Flat)
    {
        // Flat edge can not be combined with other AirsidePassenger.
        return false;
    }

    if((curRightEdgeType == AirsidePassengerEdgeType_Convex && otherCurLeftEdgeType == AirsidePassengerEdgeType_Concave) ||
        (curRightEdgeType == AirsidePassengerEdgeType_Concave && otherCurLeftEdgeType == AirsidePassengerEdgeType_Convex))
    {
        // Convex and concave edge can be combined.
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

ClassAirsideFlight::ClassAirsideFlight():
m_rotation(0),
    m_nLineCount(0),
    m_nColCount(0),
    m_pPrevAirsideFlight(NULL),
    m_pNextAirsideFlight(NULL),
    m_pointTopLeft(0, 0)
{
    m_vAirsidePassengers.clear();
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
    float fX = static_cast<float>(m_nLineCount)/2*AIRSIDEPASSENGER_WIDTH;
    float fY = static_cast<float>(m_nColCount)/2*AIRSIDEPASSENGER_WIDTH;
    PointFXY pt(m_pointTopLeft.GetX()+fX, m_pointTopLeft.GetY()+fY);
    return pt;
}

// Is the point in any one AirsidePassengers of this AirsideFlight?
bool ClassAirsideFlight::IsThisPointFXYInsideMe(const PointFXY& pt)
{
    PointFXY ptOri = CommonOperations::GetRotatedPointFXY(pt, GetRotationCenter(), -m_rotation);
    float ptOriToTopLeft_X = (ptOri-m_pointTopLeft).GetX();
    float ptOriToTopLeft_Y = (ptOri-m_pointTopLeft).GetY();
    short nLine = (ptOriToTopLeft_X / AIRSIDEPASSENGER_WIDTH) + 1;
    short nCol = (ptOriToTopLeft_Y / AIRSIDEPASSENGER_WIDTH) + 1;

    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(nLine == m_vAirsidePassengers.at(i)->GetXInAirsideFlight() &&
            nCol == m_vAirsidePassengers.at(i)->GetYInAirsideFlight())
        {
            // Find one Piece with the same Line and Column, return true.
            return true;
        }
    }

    // No Piece is found with the same Line and Column, return false.
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
    // Offset's threshold is MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE
    float xOffset = m_pointTopLeft.GetX() - pOther->m_pointTopLeft.GetX();
    int n = (int)(xOffset/AIRSIDEPASSENGER_WIDTH);
    float xPieceOffset = xOffset - AIRSIDEPASSENGER_WIDTH*n;
    if(xPieceOffset > MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE)
    {
        return false;
    }

    // Condition 3:
    // The y coordinate difference between the two AirsideFlight must be multiple of piece's length.
    // Offset's threshold is MAX_AIRSIDEFLIGHT_LOCATION_DIFFERENCE
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
        if(myNewXInAirsideFlight>m_nLineCount)
        {
            m_nLineCount = myNewXInAirsideFlight;
        }

        short myNewYInAirsideFlight = pAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(meToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pAirsidePassenger->SetYInAirsideFlight(myNewYInAirsideFlight);
        if(myNewYInAirsideFlight>m_nColCount)
        {
            m_nColCount = myNewYInAirsideFlight;
        }
    }

    size_t nOtherCount = pOther->GetAirsidePassengerCount();
    for(size_t i=0; i<nOtherCount; i++)
    {
        ClassAirsidePassenger* pOtherAirsidePassenger = pOther->m_vAirsidePassengers.at(i);

        short otherNewXInAirsideFlight = pOtherAirsidePassenger->GetXInAirsideFlight() - static_cast<short>(otherToNewXOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetXInAirsideFlight(otherNewXInAirsideFlight);
        if(otherNewXInAirsideFlight>m_nLineCount)
        {
            m_nLineCount = otherNewXInAirsideFlight;
        }

        short otherNewYInAirsideFlight = pOtherAirsidePassenger->GetYInAirsideFlight() - static_cast<short>(otherToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetYInAirsideFlight(otherNewYInAirsideFlight);
        if(otherNewYInAirsideFlight>m_nColCount)
        {
            m_nColCount = otherNewYInAirsideFlight;
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
