#pragma once
#include "../Common/template.h"
class InputLandside;
class LandsideCrosswalk;

class CrossWalkAttachPipe
{
public:
	CrossWalkAttachPipe(InputLandside* pInputLandside);
	~CrossWalkAttachPipe(void);

	void SetGroupIndex(const GroupIndex& groupIndex);
	const GroupIndex& GetGroupIndex()const;

	void AttachCrossWalk(LandsideCrosswalk* pCrossWalk);
	LandsideCrosswalk* GetAttachWalk()const;

	void ReadData(ArctermFile& p_file);
	void WriteData(ArctermFile& p_file)const;

	bool operator == (const CrossWalkAttachPipe& crossPipe)const;


	bool OverlapCrosswalk(double dIdx,double dDest)const;
	bool InCrossWalk(double dIdx)const;
private:
	GroupIndex m_groupIndex;
	LandsideCrosswalk* m_pCrossWalk;
	InputLandside* m_pInputLanside;
};
