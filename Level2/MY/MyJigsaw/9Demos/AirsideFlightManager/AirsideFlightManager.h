#pragma once
#include <list>
#include "AirsideFlight.h"

#ifdef AIRSIDEFLIGHTMANAGER_EXPORT
class __declspec(dllexport) ClassAirsideFlightManager
#else
class __declspec(dllimport) ClassAirsideFlightManager
#endif
{
public:
    ClassAirsideFlightManager();
    ~ClassAirsideFlightManager();
protected:
    ClassAirsideFlight* m_pFocusAirsideFlight;
    ClassAirsideFlight* m_pFloatingAirsideFlight;
    PointFXY m_vectorAirsideFlightToMouse;
    std::list<ClassAirsideFlight*> m_listAirsideFlight;
public:
    void InitializeAirsideFlightManager(std::vector<ClassAirsidePassenger*> vecAirsidePassenger);
    void MouseLButtonDown(int& errorCode, PointFXY pt);
    void MouseLButtonUp(int& errorCode, PointFXY pt);
    void MouseLButtonDoubleClick(int& errorCode, PointFXY pt);
    void MouseMove(int& errorCode, PointFXY pt);
    void MouseWheel(int& errorCode, int nOffset);
protected:
    void SetFocusAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void CancelFocus();
    void TopAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void AddAirsideFlight(ClassAirsideFlight* pAirsideFlight);
    void RemoveAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void DeleteAirsideFlight(int& errorCode, ClassAirsideFlight* pAirsideFlight);
    void InsertAirsideFlightAfter(int& errorCode, ClassAirsideFlight* pAirsideFlight, ClassAirsideFlight* pAfter);
    bool WorkIsDone();
};

