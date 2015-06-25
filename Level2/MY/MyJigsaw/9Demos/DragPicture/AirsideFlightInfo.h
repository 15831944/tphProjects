#pragma once
#include <vector>

typedef struct 
{
    float m_x; // x coordinate of top-left point.
    float m_y; // y coordinate of top-left point.
    PointFXY& operator=(const PointFXY& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
    }
} PointFXY;

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
    void AddPassenger();
    PointFXY GetRotationCenter();
    bool IsThisPointFXYInsideMe(const PointFXY& pt);
    bool CanJoinToMe(int& errorCode, ClassAirsideFlight* pOther);
    ClassAirsideFlight* JoinToMe(int& errorCode, ClassAirsideFlight* pOther);

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

