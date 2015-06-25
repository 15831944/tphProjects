#pragma once

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

    void operator+=(const PointFXY& other)
    {
        m_x+=other.m_x;
        m_y+=other.m_y;
        return ;
    }
};

typedef enum
{
    AirsidePassengerSideType_Flat, // Flat side
    AirsidePassengerSideType_Convex, //  Convex side
    AirsidePassengerSideType_Concave, //  Concave side
} AirsidePassengerSideType;

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
    short m_nRotateAngle; // the rotation of this piece itself, it must be 0, 90, 180 or 270.
    short m_nLine; // Line number in AirsideFlight.
    short m_nCol; // Column number in AirsideFlight.
    AirsidePassengerSideType m_leftSideType;
    AirsidePassengerSideType m_rightSideType;
    AirsidePassengerSideType m_topSideType;
    AirsidePassengerSideType m_bottomSideType;

    ClassAirsidePassenger* m_pLeft;
    ClassAirsidePassenger* m_pRight;
    ClassAirsidePassenger* m_pTop;
    ClassAirsidePassenger* m_pBottom;

    bool m_bIsTraversed; // use for traversing the maze and get subsets.
    //ID2D1BitmapBrush *m_pBitmapBrush;
public:
    bool CanAboveMe(ClassAirsidePassenger* pOther);
    bool CanBelowMe(ClassAirsidePassenger* pOther);
    bool CanLeftsideMe(ClassAirsidePassenger* pOther);
    bool CanRightsideMe(ClassAirsidePassenger* pOther);

    short GetLineInAirsideFlight(){ return m_nLine; }
    void SetLineInAirsideFlight(short val) { m_nLine = val; }

    short GetColumnInAirsideFlight(){ return m_nCol; }
    void SetColumnInAirsideFlight(short val) { m_nCol = val; }

    short GetRotateAngle() const { return m_nRotateAngle; }
    void IncreaseRotateAngle(short val);

    ClassAirsidePassenger* GetLeft() const { return m_pLeft; }
    void SetLeft(ClassAirsidePassenger* val) { m_pLeft = val; }

    ClassAirsidePassenger* GetRight() const { return m_pRight; }
    void SetRight(ClassAirsidePassenger* val) { m_pRight = val; }

    ClassAirsidePassenger* GetTop() const { return m_pTop; }
    void SetTop(ClassAirsidePassenger* val) { m_pTop = val; }

    ClassAirsidePassenger* GetBottom() const { return m_pBottom; }
    void SetBottom(ClassAirsidePassenger* val) { m_pBottom = val; }

    bool GetIsTraversed(){ return m_bIsTraversed; }
    void SetIsTraversed(bool val) { m_bIsTraversed = val; }
};

