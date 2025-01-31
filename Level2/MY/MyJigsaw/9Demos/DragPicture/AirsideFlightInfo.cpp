#include "stdafx.h"
#include "ErrorHandling.h"
#include "AirsideFlightInfo.h"
#include <assert.h>

ClassAirsideFlightManager::ClassAirsideFlightManager():
m_pFocusAirsideFlight(0),
m_pFloatingAirsideFlight(0)
{
    m_listAirsideFlight.clear();
}

ClassAirsideFlightManager::~ClassAirsideFlightManager()
{

}

bool ClassAirsideFlightManager::WorkIsDone()
{
    return true;
}

void ClassAirsideFlightManager::TopAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    if(pAirsideFlight == *itor) // The input AirsideFlight is the top one, nothing to change.
    {
        return;
    }
    ++itor;
    while(itor != m_listAirsideFlight.end())
    {
        if(*itor == pAirsideFlight)
        {
            // An AirsideFlight fits the input one is found, and top it.
            m_listAirsideFlight.erase(itor);
            m_listAirsideFlight.push_front(pAirsideFlight);
            return ;
        }
        ++itor;
    }
}

void ClassAirsideFlightManager::FocusOnAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    TopAirsideFlight(errorCode, pAirsideFlight);
    if(errorCode != Error_Success)
        return;
    m_pFocusAirsideFlight = pAirsideFlight;
    return;
}

void ClassAirsideFlightManager::CancelFocus()
{
    m_pFocusAirsideFlight = 0;
}

void ClassAirsideFlightManager::AddAirsideFlight(ClassAirsideFlight* pAirsideFlight)
{
    m_listAirsideFlight.push_front(pAirsideFlight);
}

void ClassAirsideFlightManager::RemoveAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for( ; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if(*itor == pAirsideFlight)
        {
            m_listAirsideFlight.erase(itor);
            errorCode = Error_Success;
            return;
        }
    }
    errorCode = Error_CanntFindAirsideFlight;
    return;
}

void ClassAirsideFlightManager::DeleteAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for( ; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if(*itor == pAirsideFlight)
        {
            m_listAirsideFlight.erase(itor);
            delete pAirsideFlight;
            errorCode = Error_Success;
            return;
        }
    }
    errorCode = Error_CanntFindAirsideFlight;
    return;
}

void ClassAirsideFlightManager::InsertAirsideFlightAfter(int& errorCode, ClassAirsideFlight* pAirsideFlight, ClassAirsideFlight* pAfter)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for( ; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if(*itor == pAfter)
        {
            m_listAirsideFlight.insert(itor, pAirsideFlight);
            errorCode = Error_Success;
            return;
        }
    }
    errorCode = Error_CanntFindAirsideFlight;
    return;
}

void ClassAirsideFlightManager::MouseLButtonDoubleClick(int& errorCode, PointFXY pt)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for( ; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if((*itor)->IsPointInMe(pt))
            break;
    }
    if(itor == m_listAirsideFlight.end())
    {
        errorCode = Warn_NoAirsideFlightFitPointFXY;
        return;
    }

    std::vector<ClassAirsideFlight*> vNewAirsideFlight = (*itor)->BreakMe(errorCode, pt);
    if(errorCode != Error_Success)
        return;
    size_t nCount = vNewAirsideFlight.size();
    for(size_t i=0; i<nCount; i++)
    {
        AddAirsideFlight(vNewAirsideFlight.at(i));
    }
    assert((*itor)->GetAirsidePassengerCount() == 0);
    DeleteAirsideFlight(errorCode, (*itor));
    FocusOnAirsideFlight(errorCode, *vNewAirsideFlight.begin());
    if(errorCode != Error_Success)
        return;
}

void ClassAirsideFlightManager::MouseLButtonDown(int& errorCode, PointFXY pt)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for( ; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if((*itor)->IsPointInMe(pt))
            break;
    }
    if(itor == m_listAirsideFlight.end())
    {
        errorCode = Warn_NoAirsideFlightFitPointFXY;
        return;
    }

    FocusOnAirsideFlight(errorCode, (*itor));
    m_pFloatingAirsideFlight = *itor;
    if(errorCode != Error_Success)
        return;
}

void ClassAirsideFlightManager::MouseLButtonUp(int& errorCode, PointFXY pt)
{
    if(m_pFloatingAirsideFlight)
         m_pFloatingAirsideFlight = 0;
}

void ClassAirsideFlightManager::MouseMove(int& errorCode, PointFXY ptVector)
{
    if(m_pFloatingAirsideFlight)
        m_pFloatingAirsideFlight->MoveToPointFXY(ptVector);
}

