#include "AirsideFlightGenerator.h"

AirsideFlightGenerator::AirsideFlightGenerator()
{
}


AirsideFlightGenerator::~AirsideFlightGenerator()
{
}

std::list<ClassAirsideFlight*> AirsideFlightGenerator::GenerateFlight(const AirsideGUIParameter& param)
{
    std::list<ClassAirsideFlight*> listResult;
    return listResult;
}

bool AirsideGUIParameter::SetImgPath(std::string str)
{
    // length of file path string must be less than 255.
    if(str.length() > 255)
        return false;
    m_sImgPath = str;
    return true;
}
