#pragma once
#include <vector>

class PointFXY
{
public:
    PointFXY(){}
    PointFXY(float x, float y);
    ~PointFXY(){}

private:
    float m_x; // x coordinate of top-left point.
    float m_y; // y coordinate of top-left point.
public:
    float GetX() const { return m_x; }
    void SetX(float val) { m_x = val; }

    float GetY() const { return m_y; }
    void SetY(float val) { m_y = val; }
};

typedef enum
{
    AirsidePassengerEdgeType_M, //  M
    AirsidePassengerEdgeType_W, //  W
} AirsidePassengerEdgeType;

class ClassAirsidePassenger
{
public:
    ClassAirsidePassenger();
    ~ClassAirsidePassenger();
    void PartialCopyFrom(ClassAirsidePassenger* pOther);
protected:
    PointFXY m_pointTopLeft;
    float m_rotation; // the rotation of this piece it self.
    short m_xInAirsideFlight;
    short m_yInAirsideFlight;
    AirsidePassengerEdgeType m_leftEdgeType;
    AirsidePassengerEdgeType m_rightEdgeType;
    AirsidePassengerEdgeType m_topEdgeType;
    AirsidePassengerEdgeType m_bottomEdgeType;

    ClassAirsidePassenger* m_pLeft;
    ClassAirsidePassenger* m_pRight;
    ClassAirsidePassenger* m_pTop;
    ClassAirsidePassenger* m_pBottom;

    //ID2D1BitmapBrush *m_pBitmapBrush;
public:
    bool IsThisPointFXYInsideMe(const PointFXY& pt);
    bool CanAboveMe(ClassAirsidePassenger* pOther);
    bool CanBelowMe(ClassAirsidePassenger* pOther);
    bool CanLeftsideMe(ClassAirsidePassenger* pOther);
    bool CanRightsideMe(ClassAirsidePassenger* pOther);

    short GetXInAirsideFlight(){ return m_xInAirsideFlight; }
    void SetXInAirsideFlight(short val) { m_xInAirsideFlight = val; }

    short GetYInAirsideFlight(){ return m_yInAirsideFlight; }
    void SetYInAirsideFlight(short val) { m_yInAirsideFlight = val; }

    float Rotation() const { return m_rotation; }
    void Rotation(float val) { m_rotation = val; }
};

class ClassAirsideFlight
{
public:
    ClassAirsideFlight();
    ~ClassAirsideFlight();
protected:
    PointFXY m_pointTopLeft;
    float m_rotation; // the rotation of this ClassAirsideFlight
    std::vector<ClassAirsidePassenger*> m_vAirsidePassengers; // list of pieces

    short m_xLength;
    short m_yLength;

    ClassAirsideFlight* m_pPrevAirsideFlight;
    ClassAirsideFlight* m_pNextAirsideFlight;
public:
    void Clear();
    void AddAirsidePassenger();
    size_t GetAirsidePassengerCount(){ return m_vAirsidePassengers.size(); }
    PointFXY GetRotationCenter();
    bool IsThisPointFXYInsideMe(const PointFXY& pt);
    bool CanCombinedToMe(int& errorCode, ClassAirsideFlight* pOther);
    void CombineToMe(int& errorCode, ClassAirsideFlight* pOther);

    float GetRotation(){ return m_rotation; }
    void SetRotation(float val) { m_rotation = val; }

    ClassAirsideFlight* GetPrevAirsideFlight(){ return m_pPrevAirsideFlight; }
    void SetPrevAirsideFlight(ClassAirsideFlight* val){ m_pPrevAirsideFlight = val; }

    ClassAirsideFlight* GetNextAirsideFlight(){ return m_pNextAirsideFlight; }
    void SetNextAirsideFlight(ClassAirsideFlight* val){ m_pNextAirsideFlight = val; }
};

class ClassAirsideFlightManager
{
public:
    ClassAirsideFlightManager();
    ~ClassAirsideFlightManager();
protected:
    ClassAirsideFlight* m_pTopAirsideFlight;
    ClassAirsideFlight* m_pFocusAirsideFlight;
public:
    bool WorkIsDone();
private:
    void TopThisAirsideFlight(long& errorCode, ClassAirsideFlight* pAirsideFlight);
};

