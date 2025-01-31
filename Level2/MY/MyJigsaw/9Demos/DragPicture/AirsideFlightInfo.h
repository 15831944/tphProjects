#pragma once
#include <list>
#include "AirsideFlightInfo1.h"

class ClassAirsideFlightManager
{
public:
    ClassAirsideFlightManager();
    ~ClassAirsideFlightManager();
protected:
    ClassAirsideFlight* m_pFocusAirsideFlight;
    ClassAirsideFlight* m_pFloatingAirsideFlight;
    std::list<ClassAirsideFlight*> m_listAirsideFlight;
public:
    void MouseLButtonDown(int& errorCode, PointFXY pt);
    void MouseLButtonUp(int& errorCode, PointFXY pt);
    void MouseLButtonDoubleClick(int& errorCode, PointFXY pt);
    void MouseMove(int& errorCode, PointFXY pt);

protected:
    void FocusOnAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void CancelFocus();
    void TopAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void AddAirsideFlight(ClassAirsideFlight* pAirsideFlight);
    void RemoveAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void DeleteAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void InsertAirsideFlightAfter(int& errorCode, ClassAirsideFlight* pAirsideFlight, ClassAirsideFlight* pAfter);
    bool WorkIsDone();
};

