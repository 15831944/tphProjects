#include "ErrorHandling.h"
#include "AirsideFlightManager.h"
#include <assert.h>

ClassAirsideFlightManager::ClassAirsideFlightManager():
m_pFocusAirsideFlight(0),
    m_pFloatingAirsideFlight(0),
    m_vectorAirsideFlightToMouse(0, 0)
{
    m_listAirsideFlight.clear();
}

ClassAirsideFlightManager::~ClassAirsideFlightManager()
{

}

bool ClassAirsideFlightManager::WorkIsDone()
{
    if(m_listAirsideFlight.size() != 1)
        return false;

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

// This function will top the AirsideFLight and then set the m_pFocusAirsideFlight.
void ClassAirsideFlightManager::SetFocusAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
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

void ClassAirsideFlightManager::AddAirsideFlights(std::list<ClassAirsideFlight*> listAirsideFlight)
{
    std::list<ClassAirsideFlight*>::iterator itor = listAirsideFlight.begin();
    for(; itor!=listAirsideFlight.end(); ++itor)
    {
        m_listAirsideFlight.push_back(*itor);
    }
}

void ClassAirsideFlightManager::RemoveAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for(; itor!=m_listAirsideFlight.end(); ++itor)
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
    for(; itor!=m_listAirsideFlight.end(); ++itor)
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
    for(; itor!=m_listAirsideFlight.end(); ++itor)
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

void ClassAirsideFlightManager::InitializeAirsideFlightManager(std::vector<ClassAirsidePassenger*> vecAirsidePassenger)
{
    std::vector<ClassAirsidePassenger*>::iterator itor = vecAirsidePassenger.begin();
    for(;itor!=vecAirsidePassenger.end(); ++itor)
    {
        ClassAirsideFlight* pNewAirsideFlight = new ClassAirsideFlight;
        (*itor)->SetLineInAirsideFlight(0);
        (*itor)->SetColumnInAirsideFlight(0);
        (*itor)->SetRotateAngle(CommonOperations::GetRandom(4)*90);
        pNewAirsideFlight->AddAirsidePassenger(*itor);

        pNewAirsideFlight->SetPointTopLeft(CommonOperations::GetRandomPointFXY(PointFXY(300, 300)));
        pNewAirsideFlight->SetRotateAngle((float)CommonOperations::GetRandom(90));
        AddAirsideFlight(pNewAirsideFlight);
    }
}

void ClassAirsideFlightManager::MouseLButtonDoubleClick(int& errorCode, PointFXY pt)
{
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for(; itor!=m_listAirsideFlight.end(); ++itor)
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

    // vNewAirsideFlight.begin() is the new AirsideFlight which contains the selected AirsidePassenger.
    SetFocusAirsideFlight(errorCode, *vNewAirsideFlight.begin());
    if(errorCode != Error_Success)
        return;
}

void ClassAirsideFlightManager::MouseLButtonDown(int& errorCode, PointFXY pt)
{
    if(m_pFloatingAirsideFlight)
        m_pFloatingAirsideFlight = 0;
    m_vectorAirsideFlightToMouse.SetZero();

    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for(; itor!=m_listAirsideFlight.end(); ++itor)
    {
        if((*itor)->IsPointInMe(pt))
            break;
    }
    if(itor == m_listAirsideFlight.end())
    {
        errorCode = Warn_NoAirsideFlightFitPointFXY;
        return;
    }

    SetFocusAirsideFlight(errorCode, (*itor));
    m_pFloatingAirsideFlight = *itor;
    if(errorCode != Error_Success)
        return;
    m_vectorAirsideFlightToMouse = pt - m_pFloatingAirsideFlight->GetPointTopLeft();
}

void ClassAirsideFlightManager::MouseLButtonUp(int& errorCode, PointFXY pt)
{
    if(m_pFloatingAirsideFlight)
        m_pFloatingAirsideFlight = 0;
    m_vectorAirsideFlightToMouse.SetZero();

    int iErr = -1;
    std::list<ClassAirsideFlight*>::iterator itor = m_listAirsideFlight.begin();
    for(;itor!= m_listAirsideFlight.end(); ++itor)
    {
        if(m_pFocusAirsideFlight->CanCombinedToMe(iErr, *itor))
        {
            m_pFocusAirsideFlight->CombineToMe(iErr, *itor);
            if(iErr != 0)
                return;
            assert((*itor)->GetAirsidePassengerCount() == 0);
            DeleteAirsideFlight(iErr, *itor);
            if(iErr != 0)
                return;
            break;
        }
    }
    if(WorkIsDone())
    {
        //TODO:
    }
}

void ClassAirsideFlightManager::MouseMove(int& errorCode, PointFXY newPt)
{
    if(m_pFloatingAirsideFlight)
        m_pFloatingAirsideFlight->SetPointTopLeft(newPt - m_vectorAirsideFlightToMouse);
}

void ClassAirsideFlightManager::MouseWheel(int& errorCode, int nOffset)
{
    m_pFocusAirsideFlight->Rotate(errorCode, (float)nOffset);
}

