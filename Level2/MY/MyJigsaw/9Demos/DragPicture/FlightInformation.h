#pragma once
#include <vector>

class Piece
{
protected:
    short m_rotation;
    float m_x; // x coordinate of center point.
    float m_y; // y coordinate of center point.
};

class ClassA
{
public:
    ClassA();
    ~ClassA();
protected:
    float m_x; // x coordinate of center point.
    float m_y; // y coordinate of center point.
    float m_rotation; // the rotation of this 
    std::vector<Piece*> m_vPieces; // list of pieces
public:
    // return the radius of all pieces
    float GetRadiusOfThePieces();
    // add a piece to this module
    void AddPiece(Piece* pPiece);
    // 
};

