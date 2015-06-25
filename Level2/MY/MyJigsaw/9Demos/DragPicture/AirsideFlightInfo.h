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

    PointFXY operator-(const PointFXY& other)
    {
        return PointFXY(m_x-other.m_x, m_y-other.m_y);
    }

    PointFXY operator+(const PointFXY& other)
    {
        return PointFXY(m_x+other.m_x, m_y+other.m_y);
    }
};

typedef enum
{
    AirsidePassengerEdgeType_Flat, // Flat edge
    AirsidePassengerEdgeType_Convex, //  Convex edge
    AirsidePassengerEdgeType_Concave, //  Concave edge
} AirsidePassengerEdgeType;

class ClassAirsidePassenger
{
public:
    ClassAirsidePassenger();
    ~ClassAirsidePassenger();
    void PartialCopyFrom(ClassAirsidePassenger* pOther);
protected:
    // One image will be split into M x N segments.
    // M and N signs it's position in the jigsaw image.
    short m_M;
    short m_N;
    PointFXY m_pointTopLeft;
    short m_rotation; // the rotation of this piece itself.
    short m_nLine; // Line number in AirsideFlight.
    short m_nCol; // Column number in AirsideFlight.
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
    bool CanAboveMe(ClassAirsidePassenger* pOther);
    bool CanBelowMe(ClassAirsidePassenger* pOther);
    bool CanLeftsideMe(ClassAirsidePassenger* pOther);
    bool CanRightsideMe(ClassAirsidePassenger* pOther);

    short GetXInAirsideFlight(){ return m_nLine; }
    void SetXInAirsideFlight(short val) { m_nLine = val; }

    short GetYInAirsideFlight(){ return m_nCol; }
    void SetYInAirsideFlight(short val) { m_nCol = val; }

    short GetRotation() const { return m_rotation; }
    void SetRotation(short val) { m_rotation = val; }
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

    short m_nLineCount;
    short m_nColCount;

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

