#include "AirsidePassenger.h"
#include <assert.h>

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
    m_nRotateAngle = pOther->m_nRotateAngle;
    m_nLine = pOther->m_nLine;
    m_nCol = pOther->m_nCol;

    m_leftSideType = pOther->m_leftSideType;
    m_rightSideType = pOther->m_rightSideType;
    m_topSideType = pOther->m_topSideType;
    m_bottomSideType = pOther->m_bottomSideType;

    m_pLeft = pOther->m_pLeft;
    m_pRight = pOther->m_pRight;
    m_pTop = pOther->m_pTop;
    m_pBottom = pOther->m_pBottom;
}

bool ClassAirsidePassenger::CanAboveMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerSideType curTopSideType;
    if(m_nRotateAngle == 0)
    {
        curTopSideType = m_topSideType;
    }
    else if(m_nRotateAngle == 90)
    {
        curTopSideType = m_rightSideType;
    }
    else if(m_nRotateAngle == 180)
    {
        curTopSideType = m_bottomSideType;
    }
    else if(m_nRotateAngle == 270)
    {
        curTopSideType = m_leftSideType;
    }

    if(curTopSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerSideType otherCurBottomSideType;
    if(pOther->m_nRotateAngle == 0)
    {
        otherCurBottomSideType = pOther->m_bottomSideType;
    }
    else if(pOther->m_nRotateAngle == 90)
    {
        otherCurBottomSideType = pOther->m_leftSideType;
    }
    else if(pOther->m_nRotateAngle == 180)
    {
        otherCurBottomSideType = pOther->m_topSideType;
    }
    else if(pOther->m_nRotateAngle == 270)
    {
        otherCurBottomSideType = pOther->m_rightSideType;
    }

    if(otherCurBottomSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    if((curTopSideType == AirsidePassengerSideType_Convex && otherCurBottomSideType == AirsidePassengerSideType_Concave) ||
        (curTopSideType == AirsidePassengerSideType_Concave && otherCurBottomSideType == AirsidePassengerSideType_Convex))
    {
            return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanBelowMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerSideType curBottomSideType;
    if(m_nRotateAngle == 0)
    {
        curBottomSideType = m_bottomSideType;
    }
    else if(m_nRotateAngle == 90)
    {
        curBottomSideType = m_leftSideType;
    }
    else if(m_nRotateAngle == 180)
    {
        curBottomSideType = m_topSideType;
    }
    else if(m_nRotateAngle == 270)
    {
        curBottomSideType = m_rightSideType;
    }

    if(curBottomSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerSideType otherCurTopSideType;
    if(pOther->m_nRotateAngle == 0)
    {
        otherCurTopSideType = pOther->m_topSideType;
    }
    else if(pOther->m_nRotateAngle == 90)
    {
        otherCurTopSideType = pOther->m_rightSideType;
    }
    else if(pOther->m_nRotateAngle == 180)
    {
        otherCurTopSideType = pOther->m_bottomSideType;
    }
    else if(pOther->m_nRotateAngle == 270)
    {
        otherCurTopSideType = pOther->m_leftSideType;
    }

    if(otherCurTopSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    if((curBottomSideType == AirsidePassengerSideType_Convex && otherCurTopSideType == AirsidePassengerSideType_Concave) ||
        (curBottomSideType == AirsidePassengerSideType_Concave && otherCurTopSideType == AirsidePassengerSideType_Convex))
    {
        // Convex and concave side can be combined.
        return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanLeftsideMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerSideType curLeftSideType;
    if(m_nRotateAngle == 0)
    {
        curLeftSideType = m_leftSideType;
    }
    else if(m_nRotateAngle == 90)
    {
        curLeftSideType = m_topSideType;
    }
    else if(m_nRotateAngle == 180)
    {
        curLeftSideType = m_rightSideType;
    }
    else if(m_nRotateAngle == 270)
    {
        curLeftSideType = m_bottomSideType;
    }

    if(curLeftSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerSideType otherCurRightSideType;
    if(pOther->m_nRotateAngle == 0)
    {
        otherCurRightSideType = pOther->m_rightSideType;
    }
    else if(pOther->m_nRotateAngle == 90)
    {
        otherCurRightSideType = pOther->m_bottomSideType;
    }
    else if(pOther->m_nRotateAngle == 180)
    {
        otherCurRightSideType = pOther->m_leftSideType;
    }
    else if(pOther->m_nRotateAngle == 270)
    {
        otherCurRightSideType = pOther->m_topSideType;
    }

    if(otherCurRightSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    if((curLeftSideType == AirsidePassengerSideType_Convex && otherCurRightSideType == AirsidePassengerSideType_Concave) ||
        (curLeftSideType == AirsidePassengerSideType_Concave && otherCurRightSideType == AirsidePassengerSideType_Convex))
    {
        // Convex and concave side can be combined.
        return true;
    }
    else
    {
        return false;
    }
}

bool ClassAirsidePassenger::CanRightsideMe(ClassAirsidePassenger* pOther)
{
    AirsidePassengerSideType curRightSideType;
    if(m_nRotateAngle == 0)
    {
        curRightSideType = m_rightSideType;
    }
    else if(m_nRotateAngle == 90)
    {
        curRightSideType = m_bottomSideType;
    }
    else if(m_nRotateAngle == 180)
    {
        curRightSideType = m_leftSideType;
    }
    else if(m_nRotateAngle == 270)
    {
        curRightSideType = m_topSideType;
    }

    if(curRightSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    AirsidePassengerSideType otherCurLeftSideType;
    if(pOther->m_nRotateAngle == 0)
    {
        otherCurLeftSideType = pOther->m_leftSideType;
    }
    else if(pOther->m_nRotateAngle == 90)
    {
        otherCurLeftSideType = pOther->m_topSideType;
    }
    else if(pOther->m_nRotateAngle == 180)
    {
        otherCurLeftSideType = pOther->m_rightSideType;
    }
    else if(pOther->m_nRotateAngle == 270)
    {
        otherCurLeftSideType = pOther->m_bottomSideType;
    }

    if(otherCurLeftSideType == AirsidePassengerSideType_Flat)
    {
        // Flat side can not be combined with other AirsidePassenger.
        return false;
    }

    if((curRightSideType == AirsidePassengerSideType_Convex && otherCurLeftSideType == AirsidePassengerSideType_Concave) ||
        (curRightSideType == AirsidePassengerSideType_Concave && otherCurLeftSideType == AirsidePassengerSideType_Convex))
    {
        // The convex side must be combined with concave side.
        return true;
    }
    else
    {
        return false;
    }
}

void ClassAirsidePassenger::SetRotateAngle(short val)
{
    // the rotation of this piece must be 0, 90, 180 or 270.
    assert((val%90) == 0);
    m_nRotateAngle = val;
    m_nRotateAngle = m_nRotateAngle%360;
}

void ClassAirsidePassenger::IncreaseRotateAngle(short val)
{
    // the rotation of this piece must be 0, 90, 180 or 270.
    assert((val%90) == 0);
    m_nRotateAngle += val;
    m_nRotateAngle = m_nRotateAngle%360;
}

