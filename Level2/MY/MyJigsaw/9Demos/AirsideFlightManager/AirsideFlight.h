#pragma once
#include <vector>
#include "AirsidePassenger.h"

class ClassAirsideFlight
{
public:
    ClassAirsideFlight();
    ~ClassAirsideFlight();
protected:
    PointFXY m_pointTopLeft;
    float m_rotateAngle; // the rotation of this ClassAirsideFlight
    std::vector<ClassAirsidePassenger*> m_vAirsidePassengers; // list of pieces

    short m_nLineCount;
    short m_nColCount;
public:
    void RemoveAllAirsideFlight();
    void AddAirsidePassenger(ClassAirsidePassenger* pAirsidePassenger);
    size_t GetAirsidePassengerCount(){ return m_vAirsidePassengers.size(); }
    PointFXY GetRotationCenter();
    bool IsPointInMe(const PointFXY& pt);
    int FindAirsidePassengerIndexByPointFXY(const PointFXY& pt);
    ClassAirsidePassenger* FindAirsidePassengerByPointFXY(const PointFXY& pt);
    bool CanCombinedToMe(int& errorCode, ClassAirsideFlight* pOther);
    void CombineToMe(int& errorCode, ClassAirsideFlight* pOther);
    std::vector<ClassAirsideFlight*> BreakMe(int& errorCode, const PointFXY& pt);
    void TraverseAirsidePassengers(ClassAirsideFlight* pLeftAirsideFlight, ClassAirsidePassenger* pLeft);
    void Rotate(int& errorCode, float fAngle);
    void ReCalculateLineCount();
    void ReCalculateColumtCount();
    void MarkAllAirsidePassengersTraversed(bool isTraversed);
    void RemoveAirsidePassenger(int& errorCode, ClassAirsidePassenger* pAirsidePassenger);

    PointFXY GetPointTopLeft() const { return m_pointTopLeft; }
    void SetPointTopLeft(PointFXY val) { m_pointTopLeft = val; }

    float GetRotateAngle(){ return m_rotateAngle; }
    void SetRotateAngle(float val) { m_rotateAngle = val; }
};

