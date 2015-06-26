#pragma once
#include <list>
#include <string>
#include "AirsideFlight.h"

class AirsideGUIParameter
{
public:
    AirsideGUIParameter();
    ~AirsideGUIParameter();
protected:
    short m_nLineCount;
    short m_nColCount;
    std::string m_sImgPath;
public:
    short GetLineCount() const { return m_nLineCount; }
    void SetLineCount(short val) { m_nLineCount = val; }
    short GetColCount() const { return m_nColCount; }
    void SetColCount(short val) { m_nColCount = val; }
    std::string GetImgPath() const { return m_sImgPath; }
    bool SetImgPath(std::string val);
};

class AirsideFlightGenerator
{
private:
    AirsideFlightGenerator();
    ~AirsideFlightGenerator();
public:
    static std::list<ClassAirsideFlight*> GenerateFlight(const AirsideGUIParameter& param);
};

