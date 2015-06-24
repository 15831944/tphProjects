#pragma once

class InputAirsideDataCheck
{
public:
	InputAirsideDataCheck(int nPrjID);
	void DoCheck();

protected:
	void CheckFloorPictureAndDwg(int nLevelID);
	int m_nPrjId;
};
