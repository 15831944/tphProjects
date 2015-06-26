#pragma once

class CModel;
class C3DNodeObject; 
class CModelSnapShot
{
public:
	CModelSnapShot(){ SetSize(512,512); }
	void SetSize(int zx,int zy){ dSizeX = zx; dSizeY = zy; }
	bool DoSnapeShot(CModel& model);
	
	int dSizeX;
	int dSizeY;

	void DoNode(C3DNodeObject& node, CString outfolder);
};
