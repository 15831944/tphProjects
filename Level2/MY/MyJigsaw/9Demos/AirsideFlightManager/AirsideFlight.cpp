#include "stdafx.h"
#include "Macros.h"
#include "AirsideFlight.h"
#include "CommonOperations.h"
#include <assert.h>

ClassAirsideFlight::ClassAirsideFlight():
m_rotateAngle(0),
    m_nLineCount(-1),
    m_nColCount(-1),
    m_pointTopLeft(-1, -1)
{
    m_vAirsidePassengers.clear();
}

ClassAirsideFlight::~ClassAirsideFlight()
{

}

void ClassAirsideFlight::RemoveAllAirsideFlight()
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

// Is the point in this AirsideFlight?
bool ClassAirsideFlight::IsPointInMe(const PointFXY& pt)
{
    PointFXY ptOri = CommonOperations::GetRotatedPointFXY(pt, GetRotationCenter(), -m_rotateAngle);
    float ptOriToTopLeft_X = (ptOri-m_pointTopLeft).GetX();
    float ptOriToTopLeft_Y = (ptOri-m_pointTopLeft).GetY();
    short nLine = (short)(ptOriToTopLeft_X / AIRSIDEPASSENGER_WIDTH) + 1;
    short nCol = (short)(ptOriToTopLeft_Y / AIRSIDEPASSENGER_WIDTH) + 1;

    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(nLine == m_vAirsidePassengers.at(i)->GetLineInAirsideFlight() &&
            nCol == m_vAirsidePassengers.at(i)->GetColumnInAirsideFlight())
        {
            // Find one Piece with the same Line and Column, return true.
            return true;
        }
    }

    // No Piece is found with the same Line and Column, return false.
    return false;
}

// Find the index of the AirsidePassenger that the point located, return -1 if not found.
int ClassAirsideFlight::FindAirsidePassengerIndexByPointFXY(const PointFXY& pt)
{
    PointFXY ptOri = CommonOperations::GetRotatedPointFXY(pt, GetRotationCenter(), -m_rotateAngle);
    float ptOriToTopLeft_X = (ptOri-m_pointTopLeft).GetX();
    float ptOriToTopLeft_Y = (ptOri-m_pointTopLeft).GetY();
    short nLine = (short)(ptOriToTopLeft_X / AIRSIDEPASSENGER_WIDTH) + 1;
    short nCol = (short)(ptOriToTopLeft_Y / AIRSIDEPASSENGER_WIDTH) + 1;

    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(nLine == m_vAirsidePassengers.at(i)->GetLineInAirsideFlight() &&
            nCol == m_vAirsidePassengers.at(i)->GetColumnInAirsideFlight())
        {
            // Find one Piece with the same Line and Column, return it.
            return (int)i;
        }
    }

    // No Piece is found with the same Line and Column, return -1.
    return -1;
}

// Find the AirsidePassenger that the point located, return 0 if not found.
ClassAirsidePassenger* ClassAirsideFlight::FindAirsidePassengerByPointFXY(const PointFXY& pt)
{
    int nIndex = FindAirsidePassengerIndexByPointFXY(pt);
    if(nIndex>=0)
        return m_vAirsidePassengers.at(nIndex);
    else
        return 0;
}

// Can this AirsideFlight be combined to me?
bool ClassAirsideFlight::CanCombinedToMe(int& errorCode, ClassAirsideFlight* pOther)
{
    // Condition 1:
    // The rotation difference between the two AirsideFlight must be not greater than MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE.
    if((m_rotateAngle - pOther->GetRotateAngle()) > MAX_AIRSIDEFLIGHT_ROTATION_DIFFERENCE)
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

            short myNewLineInAirsideFlight = pAirsidePassenger->GetLineInAirsideFlight() - static_cast<short>(meToNewXOffset/AIRSIDEPASSENGER_WIDTH);
            short myNewColumnInAirsideFlight = pAirsidePassenger->GetColumnInAirsideFlight() - static_cast<short>(meToNewYOffset/AIRSIDEPASSENGER_WIDTH);
            short otherNewLineInAirsideFlight = pOtherAirsidePassenger->GetLineInAirsideFlight() - static_cast<short>(otherToNewXOffset/AIRSIDEPASSENGER_WIDTH);
            short otherNewColumnInAirsideFlight = pOtherAirsidePassenger->GetColumnInAirsideFlight() - static_cast<short>(otherToNewYOffset/AIRSIDEPASSENGER_WIDTH);

            if(myNewLineInAirsideFlight == otherNewLineInAirsideFlight && myNewColumnInAirsideFlight == otherNewColumnInAirsideFlight)
            {
                // Conflict: two AirsidePassengers overlapped, these two AirsideFlight can not be combined.
                return false;
            }
            else if(myNewLineInAirsideFlight == otherNewLineInAirsideFlight &&
                    myNewColumnInAirsideFlight - otherNewColumnInAirsideFlight == 1)
            {
                // Conflict: There are two AirsidePassengers be adjacent to each other, 
                // but the side is not fit, so these two AirsideFlight can not be combined.
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanAboveMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewLineInAirsideFlight == otherNewLineInAirsideFlight &&
                    myNewColumnInAirsideFlight - otherNewColumnInAirsideFlight == -1)
            {
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanBelowMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewColumnInAirsideFlight == otherNewColumnInAirsideFlight &&
                    myNewLineInAirsideFlight - otherNewLineInAirsideFlight == 1)
            {
                bAirsidePassengerAdjacent = true;
                if(!pAirsidePassenger->CanRightsideMe(pOtherAirsidePassenger))
                    return false;
            }
            else if(myNewColumnInAirsideFlight == otherNewColumnInAirsideFlight &&
                    myNewLineInAirsideFlight - otherNewLineInAirsideFlight == -1)
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

        short myNewLineInAirsideFlight = pAirsidePassenger->GetLineInAirsideFlight() - static_cast<short>(meToNewXOffset/AIRSIDEPASSENGER_WIDTH);
        pAirsidePassenger->SetLineInAirsideFlight(myNewLineInAirsideFlight);
        if(myNewLineInAirsideFlight>m_nLineCount)
        {
            m_nLineCount = myNewLineInAirsideFlight;
        }

        short myNewColumnInAirsideFlight = pAirsidePassenger->GetColumnInAirsideFlight() - static_cast<short>(meToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pAirsidePassenger->SetColumnInAirsideFlight(myNewColumnInAirsideFlight);
        if(myNewColumnInAirsideFlight>m_nColCount)
        {
            m_nColCount = myNewColumnInAirsideFlight;
        }
    }

    size_t nOtherCount = pOther->GetAirsidePassengerCount();
    for(size_t i=0; i<nOtherCount; i++)
    {
        ClassAirsidePassenger* pOtherAirsidePassenger = pOther->m_vAirsidePassengers.at(i);

        short otherNewLineInAirsideFlight = pOtherAirsidePassenger->GetLineInAirsideFlight() - static_cast<short>(otherToNewXOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetLineInAirsideFlight(otherNewLineInAirsideFlight);
        if(otherNewLineInAirsideFlight>m_nLineCount)
        {
            m_nLineCount = otherNewLineInAirsideFlight;
        }

        short otherNewColumnInAirsideFlight = pOtherAirsidePassenger->GetColumnInAirsideFlight() - static_cast<short>(otherToNewYOffset/AIRSIDEPASSENGER_WIDTH);
        pOtherAirsidePassenger->SetColumnInAirsideFlight(otherNewColumnInAirsideFlight);
        if(otherNewColumnInAirsideFlight>m_nColCount)
        {
            m_nColCount = otherNewColumnInAirsideFlight;
        }
    }
    m_vAirsidePassengers.insert(m_vAirsidePassengers.begin(), pOther->m_vAirsidePassengers.begin(), pOther->m_vAirsidePassengers.end());
    pOther->RemoveAllAirsideFlight();
    return;
}

// Pick up the piece that pt located, and then rebuild AirsideFlight if necessary.
std::vector<ClassAirsideFlight*> ClassAirsideFlight::BreakMe(int& errorCode, const PointFXY& pt)
{
    // This AirsideFlight will be broken into several new AirsideFlights.
    std::vector<ClassAirsideFlight*> vNewAirsideFlights;
    vNewAirsideFlights.clear();
    if(!IsPointInMe(pt))
    {
        errorCode = 2;
        return vNewAirsideFlights;
    }

    ClassAirsidePassenger* pSelectedAirsidePassenger = FindAirsidePassengerByPointFXY(pt);
    assert(pSelectedAirsidePassenger != 0);

    ClassAirsidePassenger* pLeft = pSelectedAirsidePassenger->GetLeft();
    ClassAirsidePassenger* pTop = pSelectedAirsidePassenger->GetTop();
    ClassAirsidePassenger* pRight = pSelectedAirsidePassenger->GetRight();
    ClassAirsidePassenger* pBottom = pSelectedAirsidePassenger->GetBottom();

    if(pLeft == 0 && pTop == 0 && pRight == 0 && pBottom == 0)
    {
        // This AirsideFlight contains only one AirsidePassenger, can't be broken.
        errorCode = 3;
        return vNewAirsideFlights;
    }
    else
    {
        // Tell the neighbors that I'm going, and they shouldn't keep the information of me anymore.
        if(pLeft != 0)
        {
            pLeft->SetRight(0);
            pSelectedAirsidePassenger->SetLeft(0);
        }
        if(pTop != 0)
        {
            pTop->SetBottom(0);
            pSelectedAirsidePassenger->SetTop(0);
        }
        if(pRight != 0)
        {
            pRight->SetLeft(0);
            pSelectedAirsidePassenger->SetRight(0);
        }
        if(pBottom != 0)
        {
            pBottom->SetTop(0);
            pSelectedAirsidePassenger->SetBottom(0);
        }
    }

    // Create a new AirsideFlight, it contains the selected AirsidePassenger only.
    ClassAirsideFlight* pNewAirsideFlight = new ClassAirsideFlight;
    pNewAirsideFlight->AddAirsidePassenger(pSelectedAirsidePassenger);
    int iErr = 0;
    RemoveAirsidePassenger(iErr, pSelectedAirsidePassenger);
    pNewAirsideFlight->ReCalculateLineCount();
    pNewAirsideFlight->ReCalculateColumtCount();
    vNewAirsideFlights.push_back(pNewAirsideFlight);

    MarkAllAirsidePassengersTraversed(false);
    if(pLeft != 0)
    {
        // Create a new AirsideFlight, it contains the AirsidePassengers that connected to pLeft.
        ClassAirsideFlight* pLeftAirsideFlight = new ClassAirsideFlight;
        TraverseAirsidePassengers(pLeftAirsideFlight, pLeft);
        pLeftAirsideFlight->ReCalculateLineCount();
        pLeftAirsideFlight->ReCalculateColumtCount();
        vNewAirsideFlights.push_back(pLeftAirsideFlight);
    }
    if(pTop != 0)
    {
        // Create a new AirsideFlight, it contains the AirsidePassengers that connected to pTop.
        ClassAirsideFlight* pTopAirsideFlight = new ClassAirsideFlight;
        TraverseAirsidePassengers(pTopAirsideFlight, pTop);
        if(pTopAirsideFlight->GetAirsidePassengerCount() == 0)
        {
            delete pTopAirsideFlight;
        }
        else
        {
            pTopAirsideFlight->ReCalculateLineCount();
            pTopAirsideFlight->ReCalculateColumtCount();
            vNewAirsideFlights.push_back(pTopAirsideFlight);
        }
    }
    if(pRight != 0)
    {
        // Create a new AirsideFlight, it contains the AirsidePassengers that connected to pRight.
        ClassAirsideFlight* pRightAirsideFlight = new ClassAirsideFlight;
        TraverseAirsidePassengers(pRightAirsideFlight, pRight);
        if(pRightAirsideFlight->GetAirsidePassengerCount() == 0)
        {
            delete pRightAirsideFlight;
        }
        else
        {
            pRightAirsideFlight->ReCalculateLineCount();
            pRightAirsideFlight->ReCalculateColumtCount();
            vNewAirsideFlights.push_back(pRightAirsideFlight);
        }
    }
    if(pBottom != 0)
    {
        // Create a new AirsideFlight, it contains the AirsidePassengers that connected to pBottom.
        ClassAirsideFlight* pBottomAirsideFlight = new ClassAirsideFlight;
        TraverseAirsidePassengers(pBottomAirsideFlight, pBottom);
        if(pBottomAirsideFlight->GetAirsidePassengerCount() == 0)
        {
            delete pBottomAirsideFlight;
        }
        else
        {
            pBottomAirsideFlight->ReCalculateLineCount();
            pBottomAirsideFlight->ReCalculateColumtCount();
            vNewAirsideFlights.push_back(pBottomAirsideFlight);
        }
    }
    assert(GetAirsidePassengerCount() == 0);
    return vNewAirsideFlights;
}

// Rotate this AirsideFlight.
// If its rotate angle overloads 90 degree, make it decreased 90 degree,
// and make all AirsidePassneger's angle increased by 90,
// and change all AirsidePassenger's Line and Column.
void ClassAirsideFlight::Rotate(int& errorCode, float fAngle)
{
    m_rotateAngle += fAngle;
    short nCountOf90 = (short)(m_rotateAngle/90.0f);
    m_rotateAngle = m_rotateAngle - nCountOf90*90.0f;
    
    short nCount = nCountOf90 % 4;
    if(nCount<0)
        nCount+=4;

    for(int i=0; i<nCount; i++)
    {
        size_t nAirsidePassenger = m_vAirsidePassengers.size();
        for(size_t i=0; i<nAirsidePassenger; i++)
        {
            ClassAirsidePassenger* pAirsidePassneger = m_vAirsidePassengers.at(i);

            // Make the AirsidePassenger's angle increased by 90.
            pAirsidePassneger->IncreaseRotateAngle(pAirsidePassneger->GetRotateAngle() + 90);

            // Change Line and Column in AirsideFlight of this AirsidePassenger.
            // newLine = -oldColumn
            // newColumn = oldLine.
            short nOriCol = pAirsidePassneger->GetColumnInAirsideFlight();
            pAirsidePassneger->SetColumnInAirsideFlight(pAirsidePassneger->GetLineInAirsideFlight());
            pAirsidePassneger->SetLineInAirsideFlight(-nOriCol);
        }
    }
    return;
}

void ClassAirsideFlight::ReCalculateLineCount()
{
    m_nLineCount = -1;
    std::vector<ClassAirsidePassenger*>::iterator itor = m_vAirsidePassengers.begin();
    short nMaxLine = 0;
    short nMinLine = 0x7fff;
    for(; itor!=m_vAirsidePassengers.end(); itor++)
    {
        int nLine = (*itor)->GetColumnInAirsideFlight();
        if(nLine > nMaxLine)
            nMaxLine = nLine;
        if(nLine < nMinLine)
            nMinLine = nLine;
    }

    m_nLineCount = nMaxLine - nMinLine + 1;
    itor = m_vAirsidePassengers.begin();
    for(; itor!=m_vAirsidePassengers.end(); itor++)
    {
        (*itor)->SetLineInAirsideFlight((*itor)->GetLineInAirsideFlight() - nMinLine);
    }
}

void ClassAirsideFlight::ReCalculateColumtCount()
{
    m_nColCount = -1;
    std::vector<ClassAirsidePassenger*>::iterator itor = m_vAirsidePassengers.begin();
    short nMaxColumn = 0;
    short nMinColumn = 0x7fff;
    for(; itor!=m_vAirsidePassengers.end(); itor++)
    {
        int nCoulumn = (*itor)->GetColumnInAirsideFlight();
        if(nCoulumn > nMaxColumn)
            nMaxColumn = nCoulumn;
        if(nCoulumn < nMinColumn)
            nMinColumn = nCoulumn;
    }

    m_nLineCount = nMaxColumn - nMinColumn + 1;
    itor = m_vAirsidePassengers.begin();
    for(; itor!=m_vAirsidePassengers.end(); itor++)
    {
        (*itor)->SetColumnInAirsideFlight((*itor)->GetColumnInAirsideFlight() - nMinColumn);
    }
}

void ClassAirsideFlight::MarkAllAirsidePassengersTraversed(bool isTraversed)
{
    size_t nCount = m_vAirsidePassengers.size();
    for(size_t i=0; i<m_vAirsidePassengers.size(); i++)
    {
        m_vAirsidePassengers.at(i)->SetIsTraversed(isTraversed);
    }
}

void ClassAirsideFlight::AddAirsidePassenger(ClassAirsidePassenger* pAirsidePassenger)
{
    m_vAirsidePassengers.push_back(pAirsidePassenger);
}

void ClassAirsideFlight::TraverseAirsidePassengers(ClassAirsideFlight* pAirsideFlight, ClassAirsidePassenger* pAirsidePassenger)
{
    if(pAirsidePassenger == 0 || pAirsidePassenger->GetIsTraversed())
        return;

    pAirsidePassenger->SetIsTraversed(true);
    pAirsideFlight->AddAirsidePassenger(pAirsidePassenger);
    int iErrorCode = 0;
    RemoveAirsidePassenger(iErrorCode, pAirsidePassenger);

    TraverseAirsidePassengers(pAirsideFlight, pAirsidePassenger->GetLeft());
    TraverseAirsidePassengers(pAirsideFlight, pAirsidePassenger->GetTop());
    TraverseAirsidePassengers(pAirsideFlight, pAirsidePassenger->GetRight());
    TraverseAirsidePassengers(pAirsideFlight, pAirsidePassenger->GetBottom());
}

void ClassAirsideFlight::RemoveAirsidePassenger(int& errorCode, ClassAirsidePassenger* pAirsidePassenger)
{
    std::vector<ClassAirsidePassenger*>::iterator itor = m_vAirsidePassengers.begin();
    for(; itor!= m_vAirsidePassengers.end(); ++itor)
    {
        if(*itor == pAirsidePassenger)
        {
            m_vAirsidePassengers.erase(itor);
            break;
        }
    }
}

